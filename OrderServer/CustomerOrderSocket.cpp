#include "CustomerOrderSocket.h"

#include <assert.h>

#include "../lib/SocketIO.h"
#include "../lib/Provider.h"

// -------------------------------------------------------------------------------------------------------------------
//
// OrderCustomerSocket class implementation
//
// -------------------------------------------------------------------------------------------------------------------

CustomerOrderSocket::CustomerOrderSocket(const QHostAddress &serverAddress, quint16 port) :
	Udp::ServerSocket(serverAddress, port)
{

}

// -------------------------------------------------------------------------------------------------------------------

CustomerOrderSocket::~CustomerOrderSocket()
{
}

// -------------------------------------------------------------------------------------------------------------------

void CustomerOrderSocket::onSocketThreadStarted()
{
	qDebug() << "CustomerOrderSocket::onSocketThreadStarted()";

	connect(this, &Udp::ServerSocket::requestReceived, this, &CustomerOrderSocket::processRequest, Qt::QueuedConnection);

	connect(this, &CustomerOrderSocket::appendOrderToBase, &theOrderBase, &Order::Base::slot_appendOrder, Qt::QueuedConnection);
}

// -------------------------------------------------------------------------------------------------------------------

void CustomerOrderSocket::onSocketThreadFinished()
{
	disconnect(this, &CustomerOrderSocket::appendOrderToBase, &theOrderBase, &Order::Base::slot_appendOrder);
}

// -------------------------------------------------------------------------------------------------------------------

void CustomerOrderSocket::processRequest(Udp::Request request)
{
	switch(request.ID())
	{
		case CLIENT_CREATE_ORDER:
			replyCreateOrder(request);
			break;

		case CLIENT_GET_ORDER_STATE:
			replyGetOrderState(request);
			break;

		case CLIENT_REMOVE_ORDER:
			replyRemoveOrder(request);
			break;

		default:
			qDebug() << "CustomerOrderSocket::processRequest - Unknown request.ID() : " << request.ID();
			assert(false);
			break;
	}
}

// -------------------------------------------------------------------------------------------------------------------

void CustomerOrderSocket::replyCreateOrder(const Udp::Request& request)
{
	//char* pData = (char*) const_cast<const Udp::Request&>(request).data();

	Order::wrapOrder wo;

	bool result = wo.ParseFromArray(reinterpret_cast<const void*>(request.data()), request.dataSize());
	if (result == false)
	{
		wo.set_state(Order::STATE_INCORRECT_PARSE_PROTOWRAP);
		sendReply(request, wo);

		assert(0);
		return;
	}

	if (wo.state() != Order::STATE_CUSTOMER_CREATING_ORDER)
	{
		qDebug() << "CustomerOrderSocket::replyCreateOrder - Order::STATE_REQUEST_INCORRECT";

		wo.set_state(Order::STATE_INCORRECT_REQUEST);
		sendReply(request, wo);

		return;
	}

	if (theOrderBase.isExist(wo.order_id()) == true)
	{
		qDebug() << "CustomerOrderSocket::replyCreateOrder - Order::STATE_ORDER_ALREADY_EXIST";

		wo.set_state(Order::STATE_ORDER_ALREADY_EXIST);
		sendReply(request, wo);

		return;
	}

// ---------------------------------

//	Order::Item order(wo);

//	QTime t;
//	t.start();

//	for(int i = 0; i < 1000; i++)
//	{
//		order.setProviderID(i);

//		order.setAddress(request.address());
//		order.setPort(request.port());
//		order.setState(Order::STATE_SERVER_CREATED_ORDER);
//		order.setRemoveTime();
//		order.createCancelCode();

//		emit appendOrderToBase(order);
//	}

//	qDebug("CustomerOrderSocket::Time elapsed: %d ms", t.elapsed());
// ---------------------------------

	Order::Item order(wo);

	order.setAddress(request.address());
	order.setPort(request.port());
	order.setState(Order::STATE_SERVER_CREATED_ORDER);
	order.setRemoveTime();
	order.createCancelCode();

	emit appendOrderToBase(order);
	qDebug() << "CustomerOrderSocket::replyCreateOrder - appendOrderToBase";

	sendReply(request, order.toProtoWrap());
}

// -------------------------------------------------------------------------------------------------------------------

void CustomerOrderSocket::replyGetOrderState(const Udp::Request& request)
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

	if (theOrderBase.isExist(wo.order_id()) == false)
	{
		wo.set_state(Order::STATE_ORDER_NOT_FOUND);
		sendReply(request, wo);

		qDebug() << "CustomerOrderSocket::replyCreateOrder - Order::STATE_ORDER_NOT_FOUND";
		return;
	}

	int state = theOrderBase.orderState(wo.order_id());

	wo.set_state(state);
	sendReply(request, wo);
}

// -------------------------------------------------------------------------------------------------------------------

void CustomerOrderSocket::replyRemoveOrder(const Udp::Request& request)
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

	if (wo.state() != Order::STATE_CUSTOMER_REMOVING_ORDER)
	{
		qDebug() << "CustomerOrderSocket::replyRemoveOrder - Order::STATE_REQUEST_INCORRECT";

		wo.set_state(Order::STATE_INCORRECT_REQUEST);
		sendReply(request, wo);

		return;
	}

	if (theOrderBase.isExist(wo.order_id()) == false)
	{
		qDebug() << "CustomerOrderSocket::replyRemoveOrder - Order::STATE_ORDER_ALREADY_EXIST";

		wo.set_state(Order::STATE_ORDER_ALREADY_EXIST);
		sendReply(request, wo);

		return;
	}

	if (theOrderBase.remove(wo.order_id()) == false)
	{
		qDebug() << "CustomerOrderSocket::replyRemoveOrder - Order::STATE_ORDER_NOT_REMOVED";

		wo.set_state(Order::STATE_ORDER_NOT_REMOVED);
		sendReply(request, wo);

		return;
	}

	qDebug() << "CustomerOrderSocket::replyRemoveOrder - removeOrderFromBase";

	wo.set_state(Order::STATE_SERVER_REMOVED_ORDER);
	sendReply(request, wo);
}

// -------------------------------------------------------------------------------------------------------------------

