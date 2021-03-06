#include "OrderStateSocket.h"

#include <QDebug>

#include "Options.h"

#include "../lib/wassert.h"
#include "../lib/SocketIO.h"

// -------------------------------------------------------------------------------------------------------------------

OrderStateSocket::OrderStateSocket(const QHostAddress &serverAddress, quint16 port)
	: Udp::ClientSocket(serverAddress, port)
	, m_optionReceived(false)
	, m_requestGetOrderStateTimer(this)
{
	qDebug() << "OrderStateSocket::OrderStateSocket" << serverAddress << port;

	setMaxFailAckCount(MAX_FAIL_ACK_COUNT);
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

	connect(&m_requestGetOrderStateTimer, &QTimer::timeout, this, &OrderStateSocket::timeout, Qt::QueuedConnection);

	requestUdpOption();

	m_requestGetOrderStateTimer.start(theOptions.providerData().requestCustomerTime());
}

// -------------------------------------------------------------------------------------------------------------------

void OrderStateSocket::onSocketThreadFinished()
{
	m_requestGetOrderStateTimer.stop();
}

// -------------------------------------------------------------------------------------------------------------------

void OrderStateSocket::timeout()
{
	if (m_optionReceived == false)
	{
		requestUdpOption();
	}
	else
	{
		requestGetOrderState();
	}
}

// -------------------------------------------------------------------------------------------------------------------

void OrderStateSocket::processReply(const Udp::Request& request)
{
	if (request.errorCode() != SIO_ERROR_NONE)
	{
		qDebug() << "OrderStateSocket::processReply - Request has error : " << request.errorCode();
		wassert(false);
	}

	switch(request.ID())
	{
		case CLIENT_GET_CUSTOMER_UDP_OPTION:
			replyUdpOption(request);
			break;

		case CLIENT_GET_ORDER_STATE:
			replyGetOrderState(request);
			break;

		case CLIENT_REMOVE_ORDER:
			replyRemoveOrder(request);
			break;

		default:
			qDebug() << "OrderStateSocket::processReply - Unknown request.ID() : " << request.ID();
			wassert(false);
			break;
	}
}

// -------------------------------------------------------------------------------------------------------------------
// CLIENT_GET_CUSTOMER_UDP_OPTION

void OrderStateSocket::requestUdpOption()
{
	if (isReadyToSend() == false)
	{
		return;
	}

	if (m_optionReceived == true)
	{
		return;
	}

	sendRequest(CLIENT_GET_CUSTOMER_UDP_OPTION);
}

// -------------------------------------------------------------------------------------------------------------------

void OrderStateSocket::replyUdpOption(const Udp::Request& request)
{
	sio_UdpOption* ptr_uo = (sio_UdpOption*) const_cast<const Udp::Request&>(request).data();

	if (ptr_uo->version < 1 || ptr_uo->version > REPLY_UDP_OPTION_VERSION)
	{
		return;
	}

	m_optionReceived = true;

	m_requestGetOrderStateTimer.stop();

	theOptions.providerData().setRequestCustomerTime(ptr_uo->requestTime);
	setWaitAckTimeout(ptr_uo->waitReplyTime);

	m_requestGetOrderStateTimer.start(theOptions.providerData().requestCustomerTime());

	requestGetOrderState();
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

	sio_RequestGetOrderState rgos;

	rgos.version = REQUEST_GET_ORDER_STATE_VERSION;
	rgos.count = 0;

	QList<Order::Item> list = theOrderBase.orderList();

	int orderCount = list.count();
	for(int i = 0; i < orderCount; i++)
	{
		Order::Item order = list[i];

		if (order.isEmpty() == true)
		{
			continue;
		}

		if (i > MAX_GET_ORDER_STATE)
		{
			break;
		}

		rgos.orderState[rgos.count].orderID = order.handle().ID;
		rgos.orderState[rgos.count].state = order.state();

		rgos.count++;
	}

	sendRequest(CLIENT_GET_ORDER_STATE, (const char*) &rgos, sizeof(sio_RequestGetOrderState));
}

// -------------------------------------------------------------------------------------------------------------------

void OrderStateSocket::replyGetOrderState(const Udp::Request& request)
{
	sio_RequestGetOrderState* ptr_rgos = (sio_RequestGetOrderState*) const_cast<const Udp::Request&>(request).data();

	if (ptr_rgos->version < 1 || ptr_rgos->version > REQUEST_GET_ORDER_STATE_VERSION)
	{
		return;
	}

	int orderCount = ptr_rgos->count;
	for(int i  = 0; i < orderCount; i++)
	{
		sio_OrderState os = ptr_rgos->orderState[i];

		theOrderBase.slot_changeState(os.orderID, os.state);

		switch(os.state)
		{
				case Order::STATE_ORDER_NOT_FOUND:
					{
						qDebug() << "OrderStateSocket::replyGetOrderState - Order::STATE_ORDER_NOT_FOUND";
						emit removeOrderFromBase(os.orderID);
					}
					break;

				case Order::STATE_ORDER_CANCEL:
					{
						qDebug() << "OrderStateSocket::replyGetOrderState - Order::STATE_ORDER_CANCEL";

						Order::Item order = theOrderBase.order(os.orderID);
						order.setState(Order::STATE_CUSTOMER_REMOVING_ORDER);
						requestRemoveOrder(order);
					}
					break;

				default:
					qDebug() << "OrderStateSocket::replyGetOrderState - ID : " << os.orderID << ", state: " << os.state;
					break;
			}
	}
}

// -------------------------------------------------------------------------------------------------------------------
// CLIENT_REMOVE_ORDER

void OrderStateSocket::requestRemoveOrder(const Order::Item& order)
{
	if (order.state() != Order::STATE_CUSTOMER_REMOVING_ORDER)
	{
		wassert(0);
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
		wassert(0);
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
				emit removeOrderFromBase(wo.orderID);
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
		case CLIENT_GET_CUSTOMER_UDP_OPTION:
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
			wassert(false);
			break;
	}
}

// -------------------------------------------------------------------------------------------------------------------


