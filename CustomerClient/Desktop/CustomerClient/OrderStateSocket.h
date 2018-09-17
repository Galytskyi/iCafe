#ifndef ORDERSTATESOCKET_H
#define ORDERSTATESOCKET_H

#include "../../../lib/UdpSocket.h"
#include "../../../lib/Order.h"

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

	QTimer			m_requestGetOrderStateTimer;

	int				m_getOrderStateIndex = 0;

	bool			m_connect = false;
	int				m_failAckCount  = 0;

public:

	// functions: Request - Reply
	//
	void            replyCreateOrder(const Udp::Request& request);
	void            replyGetOrderState(const Udp::Request& request);
	void            replyRemoveOrder(const Udp::Request& request);

	// net connection
	//
	bool			isConnected() const { return m_connect; }
	void			setConnectState(bool connect);

signals:

	void			socketConnection(bool connect);

	void			appendOrderToBase(const Order::Item& order);
	void			orderChangedState(const Order::Item& order);
	void            failConnection();

public slots:

	void            requestCreateOrder(const Order::Item& order);				// CLIENT_CREATE_ORDER
	void            requestGetOrderState();										// CLIENT_GET_ORDER_STATE
	void            requestRemoveOrder(const Order::Item& order);				// CLIENT_REMOVE_ORDER

	void			processAck(const Udp::Request& request);
	void			failAck(const Udp::Request& request);
};

// ==============================================================================================

#endif // ORDERSTATESOCKET_H