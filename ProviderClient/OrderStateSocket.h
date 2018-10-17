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

	bool			m_optionReceived = false;
	QTimer			m_requestGetOrderStateTimer;

public:

	// functions: Request - Reply
	//
	void            requestUdpOption();											// CLIENT_GET_CUSTOMER_UDP_OPTION
	void            replyUdpOption(const Udp::Request& request);

	void            requestGetOrderState();										// CLIENT_GET_ORDER_STATE
	void            replyGetOrderState(const Udp::Request& request);

	void            requestRemoveOrder(const Order::Item& order);				// CLIENT_REMOVE_ORDER
	void            replyRemoveOrder(const Udp::Request& request);

signals:

	void			removeOrderFromBase(quint64 orderID);

	void            failConnection();

public slots:

	void			timeout();

	void			processReply(const Udp::Request& request);
	void			failReply(const Udp::Request& request);
};

// ==============================================================================================

#endif // ORDERSTATESOCKET_H
