#ifndef ORDERSTATESOCKET_H
#define ORDERSTATESOCKET_H

#include "../lib/UdpSocket.h"
#include "../lib/Order.h"

// ==============================================================================================

class OrderStateSocket : public Udp::ClientSocket
{

	Q_OBJECT

public:

	OrderStateSocket(const QHostAddress& serverAddress, quint16 port);
	virtual ~OrderStateSocket();

private:

	virtual void	onSocketThreadStarted();
	virtual void	onSocketThreadFinished();

	QTimer			m_requestGetOrderStateTimer;

	int				m_getOrderStateIndex = 0;

public:

	// functions: Request - Reply
	//
	void            replyGetOrderState(const Udp::Request& request);
	void            replyRemoveOrder(const Udp::Request& request);

signals:

	void			removeOrderFromBase(const Order::Item& order);

	void            failConnection();

public slots:

	void            requestGetOrderState();										// CLIENT_GET_ORDER_STATE
	void            requestRemoveOrder(const Order::Item& order);				// CLIENT_REMOVE_ORDER

	void			processReply(const Udp::Request& request);
	void			failReply(const Udp::Request& request);
};

// ==============================================================================================

#endif // ORDERSTATESOCKET_H
