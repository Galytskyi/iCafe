#ifndef ORDERRECEIVESOCKET_H
#define ORDERRECEIVESOCKET_H

#include "../lib/UdpSocket.h"
#include "../lib/Provider.h"
#include "../lib/Order.h"

// This class is designed to set and receive orders from OrderServer

// ==============================================================================================

class OrderReceiveSocket : public Udp::ClientSocket
{

	Q_OBJECT

public:

	OrderReceiveSocket(const QHostAddress& serverAddress, quint16 port);
	virtual ~OrderReceiveSocket();

private:

	virtual void	onSocketThreadStarted();
	virtual void	onSocketThreadFinished();

	QTimer			m_requestGetOrderTimer;

public:

	// functions: Request - Reply
	//
	void            replyGetOrder(const Udp::Request& request);					// CLIENT_GET_ORDER
	void            replySetOrderState(const Udp::Request& request);			// CLIENT_SET_ORDER_STATE
	void            replyGetProviderState(const Udp::Request& request);			// CLIENT_GET_PROVIDER_STATE
	void            replySetProviderState(const Udp::Request& request);			// CLIENT_SET_PROVIDER_STATE

signals:

	void			appendOrderToBase(const Order::Item& order);
	void			providerStateChanged(quint32 state);

	void            failConnection();

public slots:

	void            requestGetOrder();											// CLIENT_GET_ORDER
	void            requestSetOrderState(const Order::Item& order);				// CLIENT_SET_ORDER_STATE
	void            requestGetProviderState();									// CLIENT_GET_PROVIDER_STATE
	void            requestSetProviderState(quint32 state);						// CLIENT_SET_PROVIDER_STATE

	void			processReply(const Udp::Request& request);
	void			failReply(const Udp::Request& request);
};

// ==============================================================================================

#endif // ORDERRECEIVESOCKET_H
