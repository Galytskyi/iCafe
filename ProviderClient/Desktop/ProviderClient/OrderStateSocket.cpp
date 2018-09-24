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
	qDebug() << "OrderStateSocket::OrderStateSocket" << serverAddress << port;
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

	m_requestGetOrderStateTimer.start(theOptions.providerData().requestProviderTime());
}

// -------------------------------------------------------------------------------------------------------------------

void OrderStateSocket::onSocketThreadFinished()
{
	m_requestGetOrderStateTimer.stop();
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

	if (m_getOrderStateIndex >= list.count())
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

	switch(wo.state)
	{
		case Order::STATE_ORDER_NOT_FOUND:
			{
				qDebug() << "OrderStateSocket::replyGetOrderState - Order::STATE_ORDER_NOT_FOUND";

				bool result = theOrderBase.remove(wo.orderID);
				if (result == true)
				{
					emit orderRemoved(wo);
				}

				qDebug() << "OrderStateSocket::replyGetOrderState - theOrderBase.remove(wo.order_id()) - " << (result == true ? "Ok" : "Fail");
			}
			break;

		case Order::STATE_ORDER_CANCEL:
			{
				qDebug() << "OrderStateSocket::replyGetOrderState - Order::STATE_ORDER_CANCEL";

				wo.state = Order::STATE_CUSTOMER_REMOVING_ORDER;
				requestRemoveOrder(wo);
			}
			break;

		default:
			Order::Item order(wo);
			qDebug() << "OrderStateSocket::replyGetOrderState - ID : " << order.handle().ID << ", state: " << order.state();
			break;
	}
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
	sio_OrderWrap wo = *(sio_OrderWrap*) const_cast<const Udp::Request&>(request).data();

	bool result = wo.isValid();
	if (result == false)
	{
		qDebug() << "OrderStateSocket::replyRemoveOrder - incorrect orderWrap" << wo.state;
		assert(0);
		return;
	}

	switch(wo.state)
	{
		case Order::STATE_ORDER_NOT_FOUND:
			qDebug() << "OrderStateSocket::replyRemoveOrder - Order::STATE_ORDER_NOT_FOUND";
			break;

		case Order::STATE_SERVER_REMOVED_ORDER:
			{
				qDebug() << "OrderStateSocket::replyRemoveOrder - Order::STATE_SERVER_REMOVED_ORDER";

				bool result = theOrderBase.remove(wo.orderID);
				if (result == true)
				{
					qDebug() << "OrderStateSocket::replyRemoveOrder - theOrderBase.remove(w1->orderID)";

					emit orderRemoved(wo);
				}
			}
			break;

		default:
			qDebug() << "OrderStateSocket::replyRemoveOrder - state:" << wo.state;
			break;
	}
}

// -------------------------------------------------------------------------------------------------------------------

void OrderStateSocket::failReply(const Udp::Request& request)
{
	switch(request.ID())
	{
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


