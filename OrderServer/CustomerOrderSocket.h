#ifndef ORDERCUSTOMERSOCKET_H
#define ORDERCUSTOMERSOCKET_H

#include "../lib/UdpSocket.h"
#include "../lib/Order.h"

// ==============================================================================================

class CustomerOrderSocket : public Udp::ServerSocket
{
	Q_OBJECT

public:

	CustomerOrderSocket(const QHostAddress& serverAddress, quint16 port);
	virtual ~CustomerOrderSocket();

private:

	virtual void	onSocketThreadStarted();
	virtual void	onSocketThreadFinished();

public:

	void			replyCreateOrder(const Udp::Request& request);
	void			replyGetOrderState(const Udp::Request& request);
	void			replyRemoveOrder(const Udp::Request& request);

signals:

	void			appendOrderToBase(const Order::Item& order);
	void			removeOrderFromBase(const Order::Item& order);

	void			msgBox(const QString &title, const QString& text);
	void			appendMessageToArch(int type, const QString& func, const QString& text);

private slots:

	void			processRequest(Udp::Request request);
};

// ==============================================================================================

#endif // ORDERCUSTOMERSOCKET_H
