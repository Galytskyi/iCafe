#ifndef ORDERSTATESOCKET_H
#define ORDERSTATESOCKET_H

#include "../lib/UdpSocket.h"
#include "../lib/Order.h"

// This class is designed to set and receive orders from OrderServer

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
	void            replyUdpOption(const Udp::Request& request);				// CLIENT_GET_CUSTOMER_UDP_OPTION

	void            requestCreateOrder(const Order::Item& order);				// CLIENT_CREATE_ORDER
	void            replyCreateOrder(const Udp::Request& request);				// CLIENT_CREATE_ORDER

	void            requestGetOrderState();										// CLIENT_GET_ORDER_STATE
	void            replyGetOrderState(const Udp::Request& request);			// CLIENT_GET_ORDER_STATE

	void            requestRemoveOrder(const Order::Item& order);				// CLIENT_REMOVE_ORDER
	void            replyRemoveOrder(const Udp::Request& request);				// CLIENT_REMOVE_ORDER

signals:

	void			appendOrderToBase(const Order::Item& order);
	void			orderChangedState(const Order::Item& order);

	void            failConnection();

public slots:

	void			timeout();

	void			processReply(const Udp::Request& request);
	void			failReply(const Udp::Request& request);
};

// ==============================================================================================

#endif // ORDERSTATESOCKET_H
