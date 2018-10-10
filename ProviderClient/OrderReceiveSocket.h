#ifndef ORDERRECEIVESOCKET_H
#define ORDERRECEIVESOCKET_H

#include "../lib/UdpSocket.h"
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

	sio_RequestGetOrder m_rgo;

	// functions: Request - Reply
	//
	void            replyGetOrder(const Udp::Request& request);					// PROVIDER_GET_ORDER
	void            replySetOrderState(const Udp::Request& request);			// PROVIDER_SET_ORDER_STATE

signals:

	void			appendOrderToBase(const Order::Item& order);

	void            failConnection();

public slots:

	void            requestGetOrder();											// PROVIDER_GET_ORDER
	void            requestSetOrderState(const Order::Item& order);				// PROVIDER_SET_ORDER_STATE

	void			processReply(const Udp::Request& request);
	void			failReply(const Udp::Request& request);
};

// ==============================================================================================

#endif // ORDERRECEIVESOCKET_H
