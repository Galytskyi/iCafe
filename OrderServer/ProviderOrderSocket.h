#ifndef ORDERROVIDERSOCKET_H
#define ORDERPROVIDERSOCKET_H

#include "../lib/UdpSocket.h"
#include "../lib/Order.h"

// ==============================================================================================

class ProviderOrderSocket : public Udp::ServerSocket
{
	Q_OBJECT

public:

	ProviderOrderSocket(const QHostAddress& serverAddress, quint16 port);
	virtual ~ProviderOrderSocket();

private:

	virtual void	onSocketThreadStarted();
	virtual void	onSocketThreadFinished();

public:

	void			replyGetOrder(const Udp::Request& request);
	void			replySetOrderState(const Udp::Request& request);

signals:

	void			setProviderConnected(quint32 providerID, quint32 wrapVersion);

private slots:

	void			processRequest(Udp::Request request);
};

// ==============================================================================================

#endif // ORDERROVIDERSOCKET_H
