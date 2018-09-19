#include "ProviderOrderSocket.h"

#include <assert.h>

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
	qDebug() << "ProviderOrderSocket::onSocketThreadStarted()";

	connect(this, &Udp::ServerSocket::requestReceived, this, &ProviderOrderSocket::processRequest, Qt::QueuedConnection);

	connect(this, &ProviderOrderSocket::setProviderConnected, &theProviderBase, &Provider::Base::setProviderConnected, Qt::QueuedConnection);
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderOrderSocket::onSocketThreadFinished()
{
	disconnect(this, &ProviderOrderSocket::setProviderConnected, &theProviderBase, &Provider::Base::setProviderConnected);
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderOrderSocket::processRequest(Udp::Request request)
{
	if(request.headerCrcOk() == false)
	{
		request.setErrorCode(SIO_ERROR_INCCORECT_CRC32);
		sendAck(request);
	}

	switch(request.ID())
	{
		case CLIENT_GET_ORDER:
			replyGetOrder(request);
			break;

		case CLIENT_SET_ORDER_STATE:
			replySetOrderState(request);
			break;

		default:

			request.setErrorCode(SIO_ERROR_INCCORECT_REQUEST_ID);
			sendAck(request);

			qDebug() << "ProviderOrderSocket::processRequest - Unknown request.ID() : " << request.ID();
			assert(false);

			break;
	}
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderOrderSocket::replyGetOrder(const Udp::Request& request)
{
	//const char * pBuffer = const_cast<const Udp::Request&>(request).data();
	//OrderItem* order = (OrderItem*) const_cast<const Udp::Request&>(request).data();

	sio_RequestGetOrder rgo = *(sio_RequestGetOrder*) const_cast<const Udp::Request&>(request).data();

	if (rgo.version != 1)
	{
		assert(0);
	}

	emit setProviderConnected(rgo.providerID, rgo.wrapVersion);

	Order::Item order = theOrderBase.hasOrderForProvider(rgo.providerID);

	sio_OrderWrap wo = order.toWrap();
	sendReply(request, wo);
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

		assert(0);
		return;
	}

	Order::Item* pOrder = theOrderBase.orderPtr(wo.orderID);
	if (pOrder == nullptr)
	{
		wo.state = Order::STATE_ORDER_NOT_FOUND;
		sendReply(request, wo);
	}

	qDebug() << "ProviderOrderSocket::replySetOrderState : " << wo.state;

	pOrder->setState(wo.state);
	sendReply(request, wo);
}

// -------------------------------------------------------------------------------------------------------------------


