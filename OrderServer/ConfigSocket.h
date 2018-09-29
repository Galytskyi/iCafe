#ifndef CONFIGSOCKET_H
#define CONFIGSOCKET_H

#include "../lib/UdpSocket.h"
#include "../lib/Order.h"

// ==============================================================================================

class ConfigSocket : public Udp::ServerSocket
{
	Q_OBJECT

public:

	ConfigSocket(const QHostAddress& serverAddress, quint16 port);
	virtual ~ConfigSocket();

private:

	sio_ReplyCfgXmlInfo m_rcxi;
	QByteArray		m_cfgXmlData;

	virtual void	onSocketThreadStarted();
	virtual void	onSocketThreadFinished();

	bool			createCfgXml();
	void			updateCfgXmlInfo();

public:

	void			replyGetConfigXmlCrc(Udp::Request request);
	void			replyGetConfigXmlInfo(Udp::Request request);
	void			replyGetConfigXml(Udp::Request request);

signals:

	void			msgBox(const QString &title, const QString& text);
	void			appendMessageToArch(int type, const QString& func, const QString& text, const Order::Item& order);

private slots:

	void			processRequest(Udp::Request request);
};

// ==============================================================================================

#endif // CONFIGSOCKET_H
