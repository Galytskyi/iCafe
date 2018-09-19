#ifndef CONFIGSOCKET_H
#define CONFIGSOCKET_H

#include <QTimer>

#include "../../../lib/UdpSocket.h"
#include "../../../lib/Crc32.h"

// This class is designed to receive config file from OrderServer
//
// Algorithm:
//
// onConnection()
//              |
//              CLIENT_GET_CONFIG_XML_CRC
//              |
//              CLIENT_GET_CONFIG_XML_INFO
//              |
//              CLIENT_GET_CONFIG_XML

// ==============================================================================================

class ConfigSocket : public Udp::ClientSocket
{

	Q_OBJECT

public:

	ConfigSocket(const QHostAddress& serverAddress, quint16 port);
	virtual ~ConfigSocket();

private:

	//CRC32			m_cfgXmlCrc32 = 0xFFFFFFFF;
	QByteArray		m_cfgXmlData;
	replyCfgXmlInfo	m_rcxi;

	virtual void	onSocketThreadStarted();
	virtual void	onSocketThreadFinished();

	CRC32			getConfigFileCrc();

	QTimer			m_requestGetXmlInfoTimer;

public:

	// functions: Request - Reply
	//
	void            replyGetConfigXmlCrc(const Udp::Request& request);					// CLIENT_GET_CONFIG_XML_CRC
	void            replyGetConfigXmlInfo(const Udp::Request& request);					// CLIENT_GET_CONFIG_XML_INFO
	void            replyGetConfigXml(const Udp::Request& request);						// CLIENT_GET_CONFIG_XML

	// timer
	//
	void			startTimerGetXml() { m_requestGetXmlInfoTimer.start(1000); }	// theOptions.providerData().requestTime()
	void			stopTimerGetXml() { m_requestGetXmlInfoTimer.stop(); }

signals:

	void            cfgXmlReceived(const QByteArray& cfgXmlData, int version);
	void            failConnection();

private slots:

	void            requestGetConfigXmlCrc();											// CLIENT_GET_CONFIG_XML_CRC
	void            requestGetConfigXmlInfo();											// CLIENT_GET_CONFIG_XML_INFO
	void            requestGetConfigXml(quint32 partIndex);								// CLIENT_GET_CONFIG_XML

	void			processAck(const Udp::Request& request);
	void			failAck(const Udp::Request& request);

};

// ==============================================================================================

#endif // CONFIGSOCKET_H
