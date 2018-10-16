#include "ProviderStateSocket.h"

#include <QDebug>

#include "../lib/wassert.h"
#include "../lib/Provider.h"
#include "../lib/SocketIO.h"

#include "Options.h"

// -------------------------------------------------------------------------------------------------------------------

ProviderStateSocket::ProviderStateSocket(const QHostAddress &serverAddress, quint16 port)
	: Udp::ClientSocket(serverAddress, port)
	, m_optionReceived(false)
	, m_requestGetProviderStateTimer(this)
	, m_getProviderStateIndex(0)
{
	setMaxFailAckCount(MAX_FAIL_ACK_COUNT);
}

// -------------------------------------------------------------------------------------------------------------------

ProviderStateSocket::~ProviderStateSocket()
{
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderStateSocket::onSocketThreadStarted()
{
	qDebug() << "ProviderStateSocket::onSocketThreadStarted()";

	connect(this, &Udp::ClientSocket::ackReceived, this, &ProviderStateSocket::processReply, Qt::QueuedConnection);
	connect(this, &Udp::ClientSocket::ackTimeout, this, &ProviderStateSocket::failReply, Qt::QueuedConnection);

	connect(&m_requestGetProviderStateTimer, &QTimer::timeout, this, &ProviderStateSocket::timeout, Qt::QueuedConnection);

	m_requestGetProviderStateTimer.start(theOptions.customerData().requestProviderTime());
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderStateSocket::onSocketThreadFinished()
{
	m_requestGetProviderStateTimer.stop();
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderStateSocket::timeout()
{
	if (m_optionReceived == false)
	{
		requestUdpOption();
	}
	else
	{
		requestGetProviderState();
	}
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderStateSocket::processReply(const Udp::Request& request)
{
	if (request.errorCode() != SIO_ERROR_NONE)
	{
		qDebug() << "ProviderStateSocket::processReply - Request has error : " << request.errorCode();
		wassert(false);
	}

	switch(request.ID())
	{
		case CLIENT_GET_PROVIDER_UDP_OPTION:
			replyUdpOption(request);
			break;

		case CLIENT_GET_PROVIDER_INIT_STATE:
			replyGetProviderInitState(request);
			break;

		case CLIENT_GET_PROVIDER_STATE:
			replyGetProviderState(request);
			break;

		default:
			qDebug() << "ProviderStateSocket::processReply - Unknown request.ID() : " << request.ID();
			wassert(false);
			break;
	}
}

// -------------------------------------------------------------------------------------------------------------------
// CLIENT_GET_PROVIDER_UDP_OPTION

void ProviderStateSocket::requestUdpOption()
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

void ProviderStateSocket::replyUdpOption(const Udp::Request& request)
{
	sio_UdpOption* ptr_uo = (sio_UdpOption*) const_cast<const Udp::Request&>(request).data();

	if (ptr_uo->version < 1 || ptr_uo->version > REPLY_UDP_OPTION_VERSION)
	{
		return;
	}

	m_optionReceived = true;

	m_requestGetProviderStateTimer.stop();

	theOptions.customerData().setRequestProviderTime(ptr_uo->requestTime);
	setWaitAckTimeout(ptr_uo->waitReplyTime);

	m_requestGetProviderStateTimer.start(theOptions.customerData().requestProviderTime());
}

// -------------------------------------------------------------------------------------------------------------------
// CLIENT_GET_PROVIDER_INIT_STATE

void ProviderStateSocket::requestGetProviderInitState(int index)
{
	if (index < 0 || index > theProviderBase.count())
	{
		requestGetProviderInitState(0);
		return;
	}

	Provider::Item provider = theProviderBase.provider(index);

	sio_RequestProviderState rsps;

	rsps.version = REQUEST_SET_PROVIDER_STATE_VERSION;
	rsps.providerID = provider.providerID();
	rsps.state = provider.state();
	m_getProviderStateIndex = index+1;

	sendRequest(CLIENT_GET_PROVIDER_INIT_STATE, (const char*) &rsps, sizeof(sio_RequestGetOrder));
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderStateSocket::replyGetProviderInitState(const Udp::Request& request)
{
	sio_RequestProviderState* ptr_rps = (sio_RequestProviderState*) const_cast<const Udp::Request&>(request).data();

	if (ptr_rps->version < 1 || ptr_rps->version > REQUEST_SET_PROVIDER_STATE_VERSION)
	{
		// error

		requestGetProviderInitState(m_getProviderStateIndex);
		return;
	}

	Provider::Item* pProvider = theProviderBase.providerPtr(ptr_rps->providerID);
	if (pProvider == nullptr)
	{
		// error

		requestGetProviderInitState(m_getProviderStateIndex);
		return;
	}

	pProvider->setState(ptr_rps->state);

	if (m_getProviderStateIndex == theProviderBase.count())
	{
		// all states of providers has been received

		emit providerInitStateReceived();

		m_requestGetProviderStateTimer.start(theOptions.customerData().requestProviderTime());

		return;
	}

	requestGetProviderInitState(m_getProviderStateIndex);
}

// -------------------------------------------------------------------------------------------------------------------
// CLIENT_GET_PROVIDER_STATE

void ProviderStateSocket::requestGetProviderState()
{
	if (isReadyToSend() == false)
	{
		return;
	}

	int providerCount = theProviderBase.count();
	if (providerCount ==  0)
	{
		return;
	}

	if (m_getProviderStateIndex >= providerCount)
	{
		m_getProviderStateIndex = 0;
	}

	Provider::Item provider = theProviderBase.provider(m_getProviderStateIndex);

	m_getProviderStateIndex++;

	//
	//
	sio_RequestProviderState rsps;

	rsps.version = REQUEST_SET_PROVIDER_STATE_VERSION;
	rsps.providerID = provider.providerID();
	rsps.state = provider.state();

	sendRequest(CLIENT_GET_PROVIDER_STATE, (const char*) &rsps, sizeof(sio_RequestGetOrder));
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderStateSocket::replyGetProviderState(const Udp::Request& request)
{
	sio_RequestProviderState* ptr_rps = (sio_RequestProviderState*) const_cast<const Udp::Request&>(request).data();

	if (ptr_rps->version < 1 || ptr_rps->version > REQUEST_SET_PROVIDER_STATE_VERSION)
	{
		return;
	}

	Provider::Item* pProvider = theProviderBase.providerPtr(ptr_rps->providerID);
	if (pProvider == nullptr)
	{
		return;
	}

	if (ptr_rps->state != pProvider->state())
	{
		pProvider->setState(ptr_rps->state);

		emit providerStateChanged(pProvider->providerID(), pProvider->state());
	}
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderStateSocket::failReply(const Udp::Request& request)
{
	switch(request.ID())
	{
		case CLIENT_GET_PROVIDER_UDP_OPTION:
			emit failConnection();
			break;

		case CLIENT_GET_PROVIDER_INIT_STATE:
			emit failConnection();
			break;

		case CLIENT_GET_PROVIDER_STATE:
			replyGetProviderState(request);
			break;

		default:
			qDebug() << "ProviderStateSocket::failAck - Unknown request.ID() : " << request.ID();
			wassert(false);
			break;
	}
}

// -------------------------------------------------------------------------------------------------------------------


