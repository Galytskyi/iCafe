#include "ProviderOrderSocket.h"

#include "Options.h"

#include "../lib/wassert.h"
#include "../lib/SocketIO.h"
#include "../lib/Provider.h"

// -------------------------------------------------------------------------------------------------------------------
//
// ProviderOrderSocket class implementation
//
// -------------------------------------------------------------------------------------------------------------------


ProviderOrderSocket::ProviderOrderSocket(const QHostAddress &serverAddress, quint16 port) :
	Udp::ServerSocket(serverAddress, port)
{

}

// -------------------------------------------------------------------------------------------------------------------

ProviderOrderSocket::~ProviderOrderSocket()
{
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderOrderSocket::onSocketThreadStarted()
{
	emit appendMessageToArch(ARCH_MSG_TYPE_EVENT, __FUNCTION__, "started");

	connect(this, &Udp::ServerSocket::requestReceived, this, &ProviderOrderSocket::processRequest, Qt::QueuedConnection);

	connect(this, &ProviderOrderSocket::setProviderConnected, &theProviderBase, &Provider::Base::setProviderConnected, Qt::QueuedConnection);
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderOrderSocket::onSocketThreadFinished()
{
	disconnect(this, &ProviderOrderSocket::setProviderConnected, &theProviderBase, &Provider::Base::setProviderConnected);

	emit appendMessageToArch(ARCH_MSG_TYPE_EVENT, __FUNCTION__, "finished");
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderOrderSocket::processRequest(Udp::Request request)
{
	if(request.headerCrcOk() == false)
	{
		emit appendMessageToArch(ARCH_MSG_TYPE_ERROR, __FUNCTION__, "SIO_ERROR_INCCORECT_CRC32");
		request.setErrorCode(SIO_ERROR_INCCORECT_CRC32);
		sendAck(request);
		return;
	}

	switch(request.ID())
	{
		case CLIENT_GET_PROVIDER_UDP_OPTION:
			replyProviderUdpOption(request);
			break;

		case CLIENT_GET_ORDER:
			replyGetOrder(request);
			break;

		case CLIENT_SET_ORDER_STATE:
			replySetOrderState(request);
			break;

		case CLIENT_GET_PROVIDER_INIT_STATE:
		case CLIENT_GET_PROVIDER_STATE:
			replyGetProviderState(request);
			break;

		case CLIENT_SET_PROVIDER_STATE:
			replySetProviderState(request);
			break;

		default:
			emit appendMessageToArch(ARCH_MSG_TYPE_ERROR, __FUNCTION__, QString("Unknown request.ID(): %1").arg(request.ID()));
			request.setErrorCode(SIO_ERROR_INCCORECT_REQUEST_ID);
			sendAck(request);
			break;
	}
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderOrderSocket::replyProviderUdpOption(const Udp::Request& request)
{
	sio_UdpOption uo;

	uo.version = REPLY_UDP_OPTION_VERSION;
	uo.requestTime = theOptions.udp().requestProviderTime();
	uo.waitReplyTime = theOptions.udp().waitReplyProviderTime();

	sendReply(request, (const char*) &uo, sizeof(sio_UdpOption));
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderOrderSocket::replyGetOrder(const Udp::Request& request)
{
	//const char * pBuffer = const_cast<const Udp::Request&>(request).data();
	//OrderItem* order = (OrderItem*) const_cast<const Udp::Request&>(request).data();

	sio_RequestGetOrder* ptr_rgo = (sio_RequestGetOrder*) const_cast<const Udp::Request&>(request).data();

	if (ptr_rgo->version < 1 || ptr_rgo->version > REQUEST_GET_ORDER_VERSION)
	{
		emit appendMessageToArch(ARCH_MSG_TYPE_ERROR, __FUNCTION__, QString("Wrong reply version: %1").arg(ptr_rgo->version));
		return;
	}

	emit setProviderConnected(true, ptr_rgo->providerID, ptr_rgo->wrapVersion);

	Order::Item order = theOrderBase.hasOrderForProvider(ptr_rgo->providerID);

	switch (ptr_rgo->wrapVersion)
	{
		case 1:
			{
				sio_OrderWrap wo = order.toWrap();
				sendReply(request, wo);
			}
			break;

		default:
			wassert(0);
			emit appendMessageToArch(ARCH_MSG_TYPE_ERROR, __FUNCTION__, QString("Wrong wrap version: %1").arg(ptr_rgo->version));
			break;
	}
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderOrderSocket::replySetOrderState(const Udp::Request& request)
{
	sio_OrderWrap wo = *(sio_OrderWrap*) const_cast<const Udp::Request&>(request).data();

	bool result = wo.isValid();
	if (result == false)
	{

		wo.state = Order::STATE_INCORRECT_PARSE_ORDERWRAP;
		sendReply(request, wo);

		wassert(0);
		return;
	}


	//remove;


	Order::Item* pOrder = theOrderBase.orderPtr(wo.orderID);
	if (pOrder == nullptr)
	{
		wo.state = Order::STATE_ORDER_NOT_FOUND;
		sendReply(request, wo);

		return;
	}

	pOrder->setState(wo.state);

	if (wo.state == Order::STATE_ORDER_OK)
	{
		QDateTime rt = pOrder->removeTime();

		rt.setDate(QDate(pOrder->orderTime().year+2000, pOrder->orderTime().month, pOrder->orderTime().day) );
		rt.setTime(QTime(pOrder->orderTime().hour, pOrder->orderTime().minute, pOrder->orderTime().second));

		rt = rt.addSecs(MAX_SECONDS_ORDER_LIVE_AFTER_OK);

		pOrder->setRemoveTime(rt);

		emit removeFrendlyOrdersByPhone(pOrder->phone());
	}

	//
	//
	QString msgStr = QString("SetState OrderID: %1, Customer +380%2, ProviderID %3, set state: %4").
					 arg(pOrder->handle().ID).
					 arg(pOrder->phone()).
					 arg(pOrder->providerID()).
					 arg(pOrder->state());
	emit appendMessageToArch(ARCH_MSG_TYPE_ORDER, __FUNCTION__, msgStr);

	//
	//
	sendReply(request, wo);
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderOrderSocket::replyGetProviderState(const Udp::Request& request)
{
	sio_RequestProviderState* ptr_rps = (sio_RequestProviderState*) const_cast<const Udp::Request&>(request).data();

	if (ptr_rps->version < 1 || ptr_rps->version > REQUEST_SET_PROVIDER_STATE_VERSION)
	{
		emit appendMessageToArch(ARCH_MSG_TYPE_ERROR, __FUNCTION__, QString("Wrong reply version: %1").arg(ptr_rps->version));
		sendReply(request, (const char*) ptr_rps, sizeof(sio_RequestProviderState));
		return;
	}

	Provider::Item* pProvider = theProviderBase.providerPtr(ptr_rps->providerID);
	if (pProvider == nullptr)
	{
		sendReply(request, (const char*) ptr_rps, sizeof(sio_RequestProviderState));
		return;
	}

	ptr_rps->state = pProvider->state();

	sendReply(request, (const char*) ptr_rps, sizeof(sio_RequestProviderState));
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderOrderSocket::replySetProviderState(const Udp::Request& request)
{
	sio_RequestProviderState* ptr_rps = (sio_RequestProviderState*) const_cast<const Udp::Request&>(request).data();

	if (ptr_rps->version < 1 || ptr_rps->version > REQUEST_SET_PROVIDER_STATE_VERSION)
	{
		emit appendMessageToArch(ARCH_MSG_TYPE_ERROR, __FUNCTION__, QString("Wrong reply version: %1").arg(ptr_rps->version));
		sendReply(request, (const char*) ptr_rps, sizeof(sio_RequestProviderState));
		return;
	}

	Provider::Item* pProvider = theProviderBase.providerPtr(ptr_rps->providerID);
	if (pProvider == nullptr)
	{
		sendReply(request, (const char*) ptr_rps, sizeof(sio_RequestProviderState));
		return;
	}

	if (pProvider->isActive() == false)
	{
		sendReply(request, (const char*) ptr_rps, sizeof(sio_RequestProviderState));
		return;
	}

	if (ptr_rps->state != pProvider->state())
	{
		pProvider->setState(ptr_rps->state);
		pProvider->setActive(true);

		emit providerStateChanged(pProvider->providerID(), pProvider->state());

		if (pProvider->enableAcceptOrder() == false)
		{
			emit removeFrendlyOrdersByProviderID(pProvider->providerID());
		}
	}

	sendReply(request, (const char*) ptr_rps, sizeof(sio_RequestProviderState));
}

// -------------------------------------------------------------------------------------------------------------------


