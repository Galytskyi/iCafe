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
	emit appendMessageToArch(ARCH_MSG_TYPE_EVENT, __FUNCTION__, "started");

	connect(this, &Udp::ServerSocket::requestReceived, this, &CustomerOrderSocket::processRequest, Qt::QueuedConnection);

	connect(this, &CustomerOrderSocket::appendOrderToBase, &theOrderBase, &Order::Base::slot_appendOrder, Qt::QueuedConnection);

	QTime midnight(0,0,0);
	qsrand(midnight.secsTo(QTime::currentTime()));
}

// -------------------------------------------------------------------------------------------------------------------

void CustomerOrderSocket::onSocketThreadFinished()
{
	disconnect(this, &CustomerOrderSocket::appendOrderToBase, &theOrderBase, &Order::Base::slot_appendOrder);
}

// -------------------------------------------------------------------------------------------------------------------

void CustomerOrderSocket::processRequest(Udp::Request request)
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

			request.setErrorCode(SIO_ERROR_INCCORECT_REQUEST_ID);
			sendAck(request);

			emit appendMessageToArch(ARCH_MSG_TYPE_ERROR, __FUNCTION__, QString("Unknown request.ID(): %1").arg(request.ID()));
			qDebug() << "CustomerOrderSocket::processRequest - Unknown request.ID() : " << request.ID();
			assert(false);

			break;
	}
}

// -------------------------------------------------------------------------------------------------------------------

void CustomerOrderSocket::replyCreateOrder(const Udp::Request& request)
{
	//char* pData = (char*) const_cast<const Udp::Request&>(request).data();

	sio_OrderWrap wo = *(sio_OrderWrap*) const_cast<const Udp::Request&>(request).data();

	bool result = wo.isValid();
	if (result == false)
	{
		wo.state = Order::STATE_INCORRECT_PARSE_ORDERWRAP;
		sendReply(request, wo);

		assert(0);
		return;
	}

	if (wo.state != Order::STATE_CUSTOMER_CREATING_ORDER)
	{
		emit appendMessageToArch(ARCH_MSG_TYPE_ERROR, __FUNCTION__, "Order::STATE_REQUEST_INCORRECT");
		qDebug() << "CustomerOrderSocket::replyCreateOrder - Order::STATE_REQUEST_INCORRECT";

		wo.state = Order::STATE_INCORRECT_REQUEST;
		sendReply(request, wo);

		return;
	}

	if (theOrderBase.isExist(wo.orderID) == true)
	{
		emit appendMessageToArch(ARCH_MSG_TYPE_WARNING, __FUNCTION__, "Order::STATE_ORDER_ALREADY_EXIST");
		qDebug() << "CustomerOrderSocket::replyCreateOrder - Order::STATE_ORDER_ALREADY_EXIST";

		wo = theOrderBase.order(wo.orderID).toWrap();
		wo.state = Order::STATE_ORDER_ALREADY_EXIST;
		sendReply(request, wo);

		return;
	}

	Order::Item order(wo);

	order.setAddress(request.address());
	order.setPort(request.port());
	order.setState(Order::STATE_SERVER_CREATED_ORDER);
	order.updateOrderDate();
	order.setRemoveTime();
	order.createCancelCode();

	//
	//
	QString msgStr = QString("Created OrderID: %1, Customer +380%2, ProviderID %3, order of %4 for %5 people at the %6, cancel code %7").
					 arg(order.handle().ID).
					 arg(order.phone()).
					 arg(order.providerID()).
					 arg(order.typeStr()).
					 arg(order.people()).
					 arg(QString().sprintf("%02d:%02d", order.orderTime().hour, order.orderTime().minute)).
					 arg(order.cancelCode());
	emit appendMessageToArch(ARCH_MSG_TYPE_ORDER, __FUNCTION__, msgStr);

	//
	//
	emit appendOrderToBase(order);
	qDebug() << "CustomerOrderSocket::replyCreateOrder - appendOrderToBase";

	//
	//
	wo = order.toWrap();
	sendReply(request, wo);
}

// -------------------------------------------------------------------------------------------------------------------

void CustomerOrderSocket::replyGetOrderState(const Udp::Request& request)
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

	if (theOrderBase.isExist(wo.orderID) == false)
	{
		wo.state = Order::STATE_ORDER_NOT_FOUND;
		sendReply(request, wo);

		emit appendMessageToArch(ARCH_MSG_TYPE_WARNING, __FUNCTION__, "Order::STATE_ORDER_NOT_FOUND");
		qDebug() << "CustomerOrderSocket::replyGetOrderState - Order::STATE_ORDER_NOT_FOUND";
		return;
	}

	wo.state = theOrderBase.orderState(wo.orderID);
	sendReply(request, wo);
}

// -------------------------------------------------------------------------------------------------------------------

void CustomerOrderSocket::replyRemoveOrder(const Udp::Request& request)
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

	if (wo.state != Order::STATE_CUSTOMER_REMOVING_ORDER)
	{
		emit appendMessageToArch(ARCH_MSG_TYPE_ERROR, __FUNCTION__, "Order::STATE_REQUEST_INCORRECT");
		qDebug() << "CustomerOrderSocket::replyRemoveOrder - Order::STATE_REQUEST_INCORRECT";

		wo.state = Order::STATE_INCORRECT_REQUEST;
		sendReply(request, wo);

		return;
	}

	if (theOrderBase.isExist(wo.orderID) == false)
	{
		emit appendMessageToArch(ARCH_MSG_TYPE_ERROR, __FUNCTION__, "Order::STATE_ORDER_NOT_FOUND");
		qDebug() << "CustomerOrderSocket::replyRemoveOrder - Order::STATE_ORDER_NOT_FOUND";

		wo.state = Order::STATE_ORDER_NOT_FOUND;
		sendReply(request, wo);

		return;
	}

	if (theOrderBase.remove(wo.orderID) == false)
	{
		emit appendMessageToArch(ARCH_MSG_TYPE_ERROR, __FUNCTION__, "Order::STATE_ORDER_NOT_REMOVED");
		qDebug() << "CustomerOrderSocket::replyRemoveOrder - Order::STATE_ORDER_NOT_REMOVED";

		wo.state = Order::STATE_ORDER_NOT_REMOVED;
		sendReply(request, wo);

		return;
	}

	//
	//
	Order::Item order(wo);
	QString msgStr = QString("Removed OrderID: %1, Customer +380%2, ProviderID %3").
					 arg(order.handle().ID).
					 arg(order.phone()).
					 arg(order.providerID());
	emit appendMessageToArch(ARCH_MSG_TYPE_ORDER, __FUNCTION__, msgStr);

	//
	//
	qDebug() << "CustomerOrderSocket::replyRemoveOrder - removeOrderFromBase";

	wo.state = Order::STATE_SERVER_REMOVED_ORDER;
	sendReply(request, wo);
}

// -------------------------------------------------------------------------------------------------------------------

