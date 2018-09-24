#include "OrderStateSocket.h"

#include <QDebug>
#include <assert.h>

#include "../../../lib/SocketIO.h"

#include "Options.h"

// -------------------------------------------------------------------------------------------------------------------

OrderStateSocket::OrderStateSocket(const QHostAddress &serverAddress, quint16 port)
	: Udp::ClientSocket(serverAddress, port)
	, m_requestGetOrderStateTimer(this)
	, m_getOrderStateIndex(0)
	, m_connect(false)
	, m_failAckCount(0)
{
}

// -------------------------------------------------------------------------------------------------------------------

OrderStateSocket::~OrderStateSocket()
{
}

// -------------------------------------------------------------------------------------------------------------------

void OrderStateSocket::onSocketThreadStarted()
{
	qDebug() << "OrderStateSocket::onSocketThreadStarted()";

	connect(this, &Udp::ClientSocket::ackReceived, this, &OrderStateSocket::processReply, Qt::QueuedConnection);
	connect(this, &Udp::ClientSocket::ackTimeout, this, &OrderStateSocket::failReply, Qt::QueuedConnection);

	connect(&m_requestGetOrderStateTimer, &QTimer::timeout, this, &OrderStateSocket::requestGetOrderState, Qt::QueuedConnection);

	connect(this, &OrderStateSocket::appendOrderToBase, &theOrderBase, &Order::Base::slot_appendOrder, Qt::QueuedConnection);
	connect(this, &OrderStateSocket::orderChangedState, &theOrderBase, &Order::Base::slot_changeState, Qt::QueuedConnection);

	m_requestGetOrderStateTimer.start(theOptions.customerData().requestCustomerTime());
}

// -------------------------------------------------------------------------------------------------------------------

void OrderStateSocket::onSocketThreadFinished()
{
	m_requestGetOrderStateTimer.stop();

	disconnect(this, &OrderStateSocket::appendOrderToBase, &theOrderBase, &Order::Base::slot_appendOrder);
	disconnect(this, &OrderStateSocket::orderChangedState, &theOrderBase, &Order::Base::slot_changeState);
}

// -------------------------------------------------------------------------------------------------------------------

void OrderStateSocket::setConnectState(bool connect)
{
	if (connect == true)
	{
		m_failAckCount = 0;

		if (m_connect == false) // if old state == false
		{
			emit socketConnection(true);

			qDebug() << "OrderStateSocket::setConnectState - Connected";
		}
	}
	else
	{
		if (m_connect == true) // if old state == true
		{
			emit socketConnection(false);

			qDebug() << "OrderStateSocket::setConnectState - Discconnected";
		}
	}

	m_connect = connect;
}

// -------------------------------------------------------------------------------------------------------------------

void OrderStateSocket::processReply(const Udp::Request& request)
{
	if (request.errorCode() != SIO_ERROR_NONE)
	{
		qDebug() << "OrderStateSocket::processReply - Request has error : " << request.errorCode();
		assert(false);
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
			qDebug() << "OrderStateSocket::processReply - Unknown request.ID() : " << request.ID();
			assert(false);
			break;
	}

	setConnectState(true);
}

// -------------------------------------------------------------------------------------------------------------------
// CLIENT_CREATE_ORDER

void OrderStateSocket::requestCreateOrder(const Order::Item& order)
{
	if (order.state() != Order::STATE_CUSTOMER_CREATING_ORDER)
	{
		assert(0);
		return;
	}

	sio_OrderWrap wo = order.toWrap();
	sendRequest(CLIENT_CREATE_ORDER, wo);
}

// -------------------------------------------------------------------------------------------------------------------

void OrderStateSocket::replyCreateOrder(const Udp::Request& request)
{
	sio_OrderWrap wo = *(sio_OrderWrap*) const_cast<const Udp::Request&>(request).data();

	bool result = wo.isValid();
	if (result == false)
	{
		qDebug() << "OrderStateSocket::replyCreateOrder - incorrect orderWrap" << wo.state;
		assert(0);
		return;
	}

	switch(wo.state)
	{
		case Order::STATE_PROVIDER_IS_NOT_CONNECTED:	qDebug() << "OrderStateSocket::replyCreateOrder - Order::STATE_PROVIDER_IS_NOT_CONNECTED";		break;
		case Order::STATE_ORDER_ALREADY_EXIST:			qDebug() << "OrderStateSocket::replyCreateOrder - Order::STATE_ORDER_ALREADY_EXIST";			break;
		case Order::STATE_SERVER_CREATED_ORDER:			qDebug() << "OrderStateSocket::replyCreateOrder - Order::STATE_SERVER_CREATED_ORDER";			break;
		default:										qDebug() << "OrderStateSocket::replyCreateOrder - Order::???" << wo.state;						break;
	}

	emit appendOrderToBase(wo);
}

// -------------------------------------------------------------------------------------------------------------------
// CLIENT_GET_ORDER_STATE

void OrderStateSocket::requestGetOrderState()
{
	if (isReadyToSend() == false)
	{
		return;
	}

	if (theOrderBase.count() == 0)
	{
		return;
	}

	QList<Order::Item> list = theOrderBase.orderList();

	int orderCount = list.count();
	if (m_getOrderStateIndex >= orderCount)
	{
		m_getOrderStateIndex = 0;
	}

	Order::Item order = list[m_getOrderStateIndex];
	if (order.isValid() == false)
	{
		return;
	}

	m_getOrderStateIndex++;

	sio_OrderWrap wo = order.toWrap();
	sendRequest(CLIENT_GET_ORDER_STATE, wo);
}

// -------------------------------------------------------------------------------------------------------------------

void OrderStateSocket::replyGetOrderState(const Udp::Request& request)
{
	sio_OrderWrap wo = *(sio_OrderWrap*) const_cast<const Udp::Request&>(request).data();

	bool result = wo.isValid();
	if (result == false)
	{
		qDebug() << "OrderStateSocket::replyGetOrderState - incorrect orderWrap" << wo.state;
		assert(0);
		return;
	}

	if(wo.state == Order::STATE_ORDER_NOT_FOUND)
	{
		qDebug() << "OrderStateSocket::replyGetOrderState - Order::STATE_ORDER_NOT_FOUND";

		theOrderBase.remove(wo.orderID);
		return;
	}

	emit orderChangedState(wo);

	if(wo.state == Order::STATE_ORDER_CANCEL)
	{
		qDebug() << "OrderStateSocket::replyGetOrderState - Order::STATE_ORDER_CANCEL";

		wo.state = Order::STATE_CUSTOMER_REMOVING_ORDER;
		requestRemoveOrder(wo);
	}

	Order::Item order(wo);
	qDebug() << "OrderStateSocket::replyGetOrderState - ID : " << order.handle().ID << ", state: " << order.state();
}

// -------------------------------------------------------------------------------------------------------------------
// CLIENT_REMOVE_ORDER

void OrderStateSocket::requestRemoveOrder(const Order::Item& order)
{
	if (order.state() != Order::STATE_CUSTOMER_REMOVING_ORDER)
	{
		assert(0);
		return;
	}

	sio_OrderWrap wo = order.toWrap();
	sendRequest(CLIENT_REMOVE_ORDER, wo);
}

// -------------------------------------------------------------------------------------------------------------------

void OrderStateSocket::replyRemoveOrder(const Udp::Request& request)
{
	//const char * pBuffer = const_cast<const UdpRequest&>(udpRequest).data();

	sio_OrderWrap wo = *(sio_OrderWrap*) const_cast<const Udp::Request&>(request).data();

	bool result = wo.isValid();
	if (result == false)
	{
		qDebug() << "OrderStateSocket::replyRemoveOrder - incorrect orderWrap" << wo.state;
		assert(0);
		return;
	}

	qDebug() << "OrderStateSocket::replyRemoveOrder - state:" << wo.state;

	bool removed = theOrderBase.remove(wo.orderID);
	qDebug() << "OrderStateSocket::replyRemoveOrder - Order::STATE_SERVER_REMOVED_ORDER - " << (removed == true ? "Ok" : "fail");
}

// -------------------------------------------------------------------------------------------------------------------

void OrderStateSocket::failReply(const Udp::Request& request)
{
	switch(request.ID())
	{
		case CLIENT_CREATE_ORDER:
			emit failConnection();
			break;

		case CLIENT_GET_ORDER_STATE:
			emit failConnection();
			break;

		case CLIENT_REMOVE_ORDER:
			emit failConnection();
			break;

		default:
			qDebug() << "OrderStateSocket::failAck - Unknown request.ID() : " << request.ID();
			assert(false);
			break;
	}

	m_failAckCount++;

	if (m_failAckCount >= MAX_FAIL_ACK_COUNT)
	{
		setConnectState(false);
	}
}

// -------------------------------------------------------------------------------------------------------------------


