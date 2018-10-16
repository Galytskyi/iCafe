#include "OrderReceiveSocket.h"

#include <QTime>

#include "Options.h"

#include "../lib/wassert.h"

// -------------------------------------------------------------------------------------------------------------------

OrderReceiveSocket::OrderReceiveSocket(const QHostAddress &serverAddress, quint16 port)
	: Udp::ClientSocket(serverAddress, port)
	, m_optionReceived(false)
	, m_requestGetOrderTimer(this)
{
	qDebug() << "OrderReceiveSocket::OrderReceiveSocket" << serverAddress << port;

	setMaxFailAckCount(MAX_FAIL_ACK_COUNT);
}

// -------------------------------------------------------------------------------------------------------------------

OrderReceiveSocket::~OrderReceiveSocket()
{
}

// -------------------------------------------------------------------------------------------------------------------

void OrderReceiveSocket::onSocketThreadStarted()
{
	qDebug() << "OrderReceiveSocket::onSocketThreadStarted()";

	connect(this, &Udp::ClientSocket::ackReceived, this, &OrderReceiveSocket::processReply, Qt::QueuedConnection);
	connect(this, &Udp::ClientSocket::ackTimeout, this, &OrderReceiveSocket::failReply, Qt::QueuedConnection);

	connect(&m_requestGetOrderTimer, &QTimer::timeout, this, &OrderReceiveSocket::timeout, Qt::QueuedConnection);

	m_requestGetOrderTimer.start(theOptions.providerData().requestProviderTime());
}

// -------------------------------------------------------------------------------------------------------------------

void OrderReceiveSocket::onSocketThreadFinished()
{
	m_requestGetOrderTimer.stop();
}

// -------------------------------------------------------------------------------------------------------------------

void OrderReceiveSocket::timeout()
{
	if (m_optionReceived == false)
	{
		requestUdpOption();
	}
	else
	{
		requestGetOrder();
	}
}

// -------------------------------------------------------------------------------------------------------------------

void OrderReceiveSocket::processReply(const Udp::Request& request)
{
	if (request.errorCode() != SIO_ERROR_NONE)
	{
		qDebug() << "OrderStateSocket::processReply - Request has error : " << request.errorCode();
		wassert(false);
	}

	switch(request.ID())
	{
		case CLIENT_GET_PROVIDER_UDP_OPTION:
			replyUdpOption(request);
			break;

		case CLIENT_GET_ORDER:
			replyGetOrder(request);
			break;

		case CLIENT_SET_ORDER_STATE:
			replySetOrderState(request);
			break;

		case CLIENT_GET_PROVIDER_STATE:
			replyGetProviderState(request);
			break;

		case CLIENT_SET_PROVIDER_STATE:
			replySetProviderState(request);
			break;

		default:
			qDebug() << "OrderReceiveSocket::processReply - Unknown request.ID() : " << request.ID();
			wassert(false);
			break;
	}
}

// -------------------------------------------------------------------------------------------------------------------
// CLIENT_GET_PROVIDER_UDP_OPTION

void OrderReceiveSocket::requestUdpOption()
{
	if (isReadyToSend() == false)
	{
		return;
	}

	if (m_optionReceived == true)
	{
		return;
	}

	sendRequest(CLIENT_GET_PROVIDER_UDP_OPTION);
}

// -------------------------------------------------------------------------------------------------------------------

void OrderReceiveSocket::replyUdpOption(const Udp::Request& request)
{
	sio_UdpOption* ptr_uo = (sio_UdpOption*) const_cast<const Udp::Request&>(request).data();

	if (ptr_uo->version < 1 || ptr_uo->version > REPLY_UDP_OPTION_VERSION)
	{
		return;
	}

	m_optionReceived = true;

	m_requestGetOrderTimer.stop();

	theOptions.providerData().setRequestProviderTime(ptr_uo->requestTime);
	setWaitAckTimeout(ptr_uo->waitReplyTime);

	m_requestGetOrderTimer.start(theOptions.providerData().requestProviderTime());
}

// -------------------------------------------------------------------------------------------------------------------
// PROVIDER_GET_ORDER

void OrderReceiveSocket::requestGetOrder()
{
	if (isReadyToSend() == false)
	{
		return;
	}

	sio_RequestGetOrder rgo;

	rgo.version = REQUEST_GET_ORDER_VERSION;
	rgo.providerID = theOptions.providerData().providerID();
	rgo.wrapVersion = ORDER_WRAP_VERSION;

	sendRequest(CLIENT_GET_ORDER, (const char*) &rgo, sizeof(sio_RequestGetOrder));
}

// -------------------------------------------------------------------------------------------------------------------

void OrderReceiveSocket::replyGetOrder(const Udp::Request& request)
{
	sio_OrderWrap wo = *(sio_OrderWrap*) const_cast<const Udp::Request&>(request).data();

	bool result = wo.isValid();
	if (result == false)
	{
		qDebug() << "OrderReceiveSocket::replyGetOrder - incorrect orderWrap" << wo.state;
		wassert(0);
		return;
	}

	Order::Item order(wo);
	if (order.isEmpty() == true || order.isValid() == false)
	{
		return;
	}

	qDebug() << "OrderReceiveSocket::replyGetOrder " << wo.state;

	if (wo.state != Order::STATE_SERVER_SEND_TO_PROVIDER)
	{
		qDebug() << "OrderReceiveSocket::replyGetOrder - wo.state() != Order::STATE_SERVER_SEND_TO_PROVIDER";
		wassert(0);
		return;
	}

	order.setState(Order::STATE_ORDER_PROCESSING);

	requestSetOrderState(order);

	emit appendOrderToBase(order);

	QTime time = QTime::currentTime();
	qDebug() << time.hour() << ":" << time.minute() << " - " << "appendOrderToBase";
}

// -------------------------------------------------------------------------------------------------------------------
// CLIENT_SET_ORDER_STATE

void OrderReceiveSocket::requestSetOrderState(const Order::Item& order)
{
	if (isReadyToSend() == false)
	{
		return;
	}

	sio_OrderWrap wo = order.toWrap();
	sendRequest(CLIENT_SET_ORDER_STATE, wo);
}

// -------------------------------------------------------------------------------------------------------------------

void OrderReceiveSocket::replySetOrderState(const Udp::Request& request)
{
	Q_UNUSED(request);

	qDebug() << "OrderReceiveSocket::replySetOrderState";
}

// -------------------------------------------------------------------------------------------------------------------
// CLIENT_GET_PROVIDER_STATE

void OrderReceiveSocket::requestGetProviderState()
{
	if (isReadyToSend() == false)
	{
		return;
	}

	sio_RequestProviderState rsps;

	rsps.version = REQUEST_SET_PROVIDER_STATE_VERSION;
	rsps.providerID = theOptions.providerData().providerID();
	rsps.state = theOptions.providerData().state();

	sendRequest(CLIENT_GET_PROVIDER_STATE, (const char*) &rsps, sizeof(sio_RequestProviderState));
}

// -------------------------------------------------------------------------------------------------------------------

void OrderReceiveSocket::replyGetProviderState(const Udp::Request& request)
{
	sio_RequestProviderState* ptr_rps = (sio_RequestProviderState*) const_cast<const Udp::Request&>(request).data();

	if (ptr_rps->version < 1 || ptr_rps->version > REQUEST_SET_PROVIDER_STATE_VERSION)
	{
		return;
	}

	if (ptr_rps->providerID != theOptions.providerData().providerID())
	{
		return;
	}

	emit providerStateChanged(ptr_rps->state);

	qDebug() << "OrderReceiveSocket::replyGetProviderState";
}

// -------------------------------------------------------------------------------------------------------------------
// CLIENT_SET_PROVIDER_STATE

void OrderReceiveSocket::requestSetProviderState(quint32 state)
{
	if (isReadyToSend() == false)
	{
		return;
	}

	sio_RequestProviderState rsps;

	rsps.version = REQUEST_SET_PROVIDER_STATE_VERSION;
	rsps.providerID = theOptions.providerData().providerID();
	rsps.state = state;

	sendRequest(CLIENT_SET_PROVIDER_STATE, (const char*) &rsps, sizeof(sio_RequestProviderState));
}

// -------------------------------------------------------------------------------------------------------------------

void OrderReceiveSocket::replySetProviderState(const Udp::Request& request)
{
	sio_RequestProviderState* ptr_rps = (sio_RequestProviderState*) const_cast<const Udp::Request&>(request).data();

	if (ptr_rps->version < 1 || ptr_rps->version > REQUEST_SET_PROVIDER_STATE_VERSION)
	{
		return;
	}

	if (ptr_rps->providerID != theOptions.providerData().providerID())
	{
		return;
	}

	emit providerStateChanged(ptr_rps->state);

	qDebug() << "OrderReceiveSocket::replySetProviderState";
}

// -------------------------------------------------------------------------------------------------------------------

void OrderReceiveSocket::failReply(const Udp::Request& request)
{
	switch(request.ID())
	{
		case CLIENT_GET_PROVIDER_UDP_OPTION:
			emit failConnection();
			break;

		case CLIENT_GET_ORDER:
			emit failConnection();
			break;

		case CLIENT_SET_ORDER_STATE:
			emit failConnection();
			break;

		case CLIENT_GET_PROVIDER_STATE:
			emit failConnection();
			break;

		case CLIENT_SET_PROVIDER_STATE:
			emit failConnection();
			break;

		default:
			qDebug() << "ConfigSocket::processReply - Unknown request.ID() : " << request.ID();
			wassert(false);
			break;
	}
}

// -------------------------------------------------------------------------------------------------------------------
