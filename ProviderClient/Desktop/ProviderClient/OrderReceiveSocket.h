#ifndef ORDERRECEIVESOCKET_H
#define ORDERRECEIVESOCKET_H

#include "../../../lib/UdpSocket.h"
#include "../../../lib/Order.h"

// This class is designed to set and receive orders from OrderServer

// ==============================================================================================

const char* const	NO_CONNECTION_STR = QT_TRANSLATE_NOOP("OrderReceiveSocket.h", "  Waiting for connection ...");

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

	bool			m_connect = false;
	int				m_failAckCount  = 0;

public:

	// functions: Request - Reply
	//
	void            replyGetOrder(const Udp::Request& request);					// PROVIDER_GET_ORDER
	void            replySetOrderState(const Udp::Request& request);			// PROVIDER_SET_ORDER_STATE

	// net connection
	//
	bool			isConnected() const { return m_connect; }
	void			setConnectState(bool connect);

signals:

	void			socketConnection(bool connect);

	void			appendOrderToBase(const Order::Item& order);
	void            failConnection();

public slots:

	void            requestGetOrder();											// PROVIDER_GET_ORDER
	void            requestSetOrderState(const Order::Item& order);				// PROVIDER_SET_ORDER_STATE

	void			processAck(const Udp::Request& request);
	void			failAck(const Udp::Request& request);
};

// ==============================================================================================

#endif // ORDERRECEIVESOCKET_H
