#include "OrderReceiveSocket.h"

#include <QTime>
#include <assert.h>

#include "Options.h"

// -------------------------------------------------------------------------------------------------------------------

OrderReceiveSocket::OrderReceiveSocket(const QHostAddress &serverAddress, quint16 port)
	: Udp::ClientSocket(serverAddress, port)
	, m_requestGetOrderTimer(this)
	, m_connect(false)
	, m_failAckCount(0)
{
	qDebug() << "OrderReceiveSocket::OrderReceiveSocket" << serverAddress << port;
}

// -------------------------------------------------------------------------------------------------------------------

OrderReceiveSocket::~OrderReceiveSocket()
{
}

// -------------------------------------------------------------------------------------------------------------------

void OrderReceiveSocket::onSocketThreadStarted()
{
	qDebug() << "OrderReceiveSocket::onSocketThreadStarted()";

	connect(this, &Udp::ClientSocket::ackReceived, this, &OrderReceiveSocket::processAck, Qt::QueuedConnection);
	connect(this, &Udp::ClientSocket::ackTimeout, this, &OrderReceiveSocket::failAck, Qt::QueuedConnection);

	connect(&m_requestGetOrderTimer, &QTimer::timeout, this, &OrderReceiveSocket::requestGetOrder, Qt::QueuedConnection);

	m_requestGetOrderTimer.start(theOptions.providerData().requestProviderTime());
}

// -------------------------------------------------------------------------------------------------------------------

void OrderReceiveSocket::onSocketThreadFinished()
{
	m_requestGetOrderTimer.stop();
}

// -------------------------------------------------------------------------------------------------------------------

void OrderReceiveSocket::setConnectState(bool connect)
{
	if (connect == true)
	{
		m_failAckCount = 0;

		if (m_connect == false) // if old state == false
		{
			emit socketConnection(true);

			qDebug() << "OrderReceiveSocket::setConnectState - Connected";
		}
	}
	else
	{
		if (m_connect == true) // if old state == true
		{
			emit socketConnection(false);

			qDebug() << "OrderReceiveSocket::setConnectState - Discconnected";
		}
	}

	m_connect = connect;
}

// -------------------------------------------------------------------------------------------------------------------

void OrderReceiveSocket::processAck(const Udp::Request& request)
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
			assert(false);
			qDebug() << "OrderReceiveSocket::processAck - Unknown request.ID() : " << request.ID();
			break;
	}

	setConnectState(true);
}

// -------------------------------------------------------------------------------------------------------------------
// PROVIDER_GET_ORDER

void OrderReceiveSocket::requestGetOrder()
{
	if (isReadyToSend() == false)
	{
		return;
	}

	quint32 providerID = theOptions.providerData().providerID();

	sendRequest(CLIENT_GET_ORDER, (const char*) &providerID, sizeof(providerID));
}

// -------------------------------------------------------------------------------------------------------------------

void OrderReceiveSocket::replyGetOrder(const Udp::Request& request)
{
	Order::wrapOrder wo;

	bool result = wo.ParseFromArray(reinterpret_cast<const void*>(request.data()), request.dataSize());
	if (result == false)
	{
		qDebug() << "OrderReceiveSocket::replyGetOrder - incorrect Order::wrapOrder" << wo.state();
		assert(0);
		return;
	}

	Order::Item order(wo);
	if (order.isEmpty() == true || order.isValid() == false)
	{
		return;
	}

	qDebug() << "OrderReceiveSocket::replyGetOrder " << wo.state();

	if (wo.state() != Order::STATE_SERVER_SEND_TO_PROVIDER)
	{
		qDebug() << "OrderReceiveSocket::replyGetOrder - wo.state() != Order::STATE_SERVER_SEND_TO_PROVIDER";
		assert(0);
		return;
	}

	order.setState(Order::STATE_ORDER_PROCESSING);

	requestSetOrderState(order);

	emit appendOrderToBase(order);

	QTime time = QTime::currentTime();
	qDebug() << time.hour() << ":" << time.minute() << " - " << "appendOrderToBase";
}

// -------------------------------------------------------------------------------------------------------------------
// PROVIDER_SET_ORDER_STATE

void OrderReceiveSocket::requestSetOrderState(const Order::Item& order)
{
	sendRequest(CLIENT_SET_ORDER_STATE, order.toProtoWrap());
}

// -------------------------------------------------------------------------------------------------------------------

void OrderReceiveSocket::replySetOrderState(const Udp::Request& request)
{
	Q_UNUSED(request);

	qDebug() << "OrderReceiveSocket::replySetOrderState";
}

// -------------------------------------------------------------------------------------------------------------------

void OrderReceiveSocket::failAck(const Udp::Request& request)
{
	switch(request.ID())
	{
		case CLIENT_GET_ORDER:
			emit failConnection();
			break;

		case CLIENT_SET_ORDER_STATE:
			emit failConnection();
			break;

		default:
			assert(false);
			qDebug() << "ConfigSocket::processAck - Unknown request.ID() : " << request.ID();
			break;
	}

	m_failAckCount++;

	if (m_failAckCount >= MAX_FAIL_ACK_COUNT)
	{
		setConnectState(false);
	}
}

// -------------------------------------------------------------------------------------------------------------------
