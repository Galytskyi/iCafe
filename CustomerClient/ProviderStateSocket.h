#ifndef PROVIDERSTATESOCKET_H
#define PROVIDERSTATESOCKET_H

#include "../lib/UdpSocket.h"

// This class is designed to set and receive providers state from OrderServer

// ==============================================================================================

class ProviderStateSocket : public Udp::ClientSocket
{

	Q_OBJECT

public:

	ProviderStateSocket(const QHostAddress& serverAddress, quint16 port);
	virtual ~ProviderStateSocket();

private:

	virtual void	onSocketThreadStarted();
	virtual void	onSocketThreadFinished();

	QTimer			m_requestGetProviderStateTimer;

	int				m_getProviderStateIndex = 0;

public:

	// functions: Request - Reply
	//
	void			replyGetProviderInitState(const Udp::Request& request);		// CLIENT_GET_PROVIDER_STATE
	void            replyGetProviderState(const Udp::Request& request);			// CLIENT_GET_PROVIDER_STATE

signals:

	void			providerInitStateReceived();
	void			providerStateChanged(quint32 providerID, quint32 state);

	void            failConnection();

public slots:

	void			requestGetProviderInitState(int index);						// CLIENT_GET_PROVIDER_STATE
	void            requestGetProviderState();									// CLIENT_GET_PROVIDER_STATE

	void			processReply(const Udp::Request& request);
	void			failReply(const Udp::Request& request);
};

// ==============================================================================================

#endif // PROVIDERSTATESOCKET_H
