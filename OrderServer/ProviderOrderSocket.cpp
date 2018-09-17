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
	switch(request.ID())
	{
		case CLIENT_GET_ORDER:
			replyGetOrder(request);
			break;

		case CLIENT_SET_ORDER_STATE:
			replySetOrderState(request);
			break;

		default:
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

	quint32 providerID = *(quint32*) const_cast<const Udp::Request&>(request).data();

	emit setProviderConnected(providerID);

	Order::Item order = theOrderBase.hasOrderForProvider(providerID);

	sendReply(request, order.toProtoWrap());
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderOrderSocket::replySetOrderState(const Udp::Request& request)
{
	Order::wrapOrder wo;

	bool result = wo.ParseFromArray(reinterpret_cast<const void*>(request.data()), request.dataSize());
	if (result == false)
	{

		wo.set_state(Order::STATE_INCORRECT_PARSE_PROTOWRAP);
		sendReply(request, wo);

		assert(0);
		return;
	}

	Order::Item* pOrder = theOrderBase.orderPtr(wo.order_id());
	if (pOrder == nullptr)
	{
		wo.set_state(Order::STATE_ORDER_NOT_FOUND);
		sendReply(request, pOrder->toProtoWrap());
	}

	qDebug() << "ProviderOrderSocket::replySetOrderState : " << wo.state();

	pOrder->setState(wo.state());
	sendReply(request, pOrder->toProtoWrap());
}

// -------------------------------------------------------------------------------------------------------------------


