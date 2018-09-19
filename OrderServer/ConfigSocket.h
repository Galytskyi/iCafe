#ifndef CONFIGSOCKET_H
#define CONFIGSOCKET_H

#include "../lib/UdpSocket.h"
#include "../lib/Crc32.h"

// ==============================================================================================

class ConfigSocket : public Udp::ServerSocket
{
	Q_OBJECT

public:

	ConfigSocket(const QHostAddress& serverAddress, quint16 port);
	virtual ~ConfigSocket();

private:

	QByteArray		m_cfgXmlData;
	replyCfgXmlInfo m_rcxi;

	virtual void	onSocketThreadStarted();
	virtual void	onSocketThreadFinished();

	bool			createCfgXml();
	void			updateCfgXmlInfo();

public:

	void			replyGetConfigXmlCrc(Udp::Request request);
	void			replyGetConfigXmlInfo(Udp::Request request);
	void			replyGetConfigXml(Udp::Request request);

private slots:

	void			processRequest(Udp::Request request);
};

// ==============================================================================================

#endif // CONFIGSOCKET_H
