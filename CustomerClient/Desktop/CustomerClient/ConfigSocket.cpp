#include "ConfigSocket.h"

#include <QFile>
#include <assert.h>

#include "Options.h"

#include "../../../lib/Provider.h"

// -------------------------------------------------------------------------------------------------------------------

ConfigSocket::ConfigSocket(const QHostAddress &serverAddress, quint16 port)
	:	Udp::ClientSocket(serverAddress, port)
	,	m_requestGetXmlInfoTimer(this)
{
	qDebug() << serverAddress;
}

// -------------------------------------------------------------------------------------------------------------------

ConfigSocket::~ConfigSocket()
{
}

// -------------------------------------------------------------------------------------------------------------------

void ConfigSocket::onSocketThreadStarted()
{
	qDebug() << "ConfigSocket::onSocketThreadStarted()";

	connect(this, &Udp::ClientSocket::ackReceived, this, &ConfigSocket::processReply, Qt::QueuedConnection);
	connect(this, &Udp::ClientSocket::ackTimeout, this, &ConfigSocket::failReply, Qt::QueuedConnection);

	connect(&m_requestGetXmlInfoTimer, &QTimer::timeout, this, &ConfigSocket::requestGetConfigXmlCrc, Qt::QueuedConnection);

	connect(this, &ConfigSocket::cfgXmlReceived, &theProviderBase, &Provider::Base::readFromXml, Qt::QueuedConnection);
	connect(this, &ConfigSocket::cfgXmlReceived, &theProviderTypeBase, &Provider::TypeBase::readFromXml, Qt::QueuedConnection);

	m_cfgXmlData.clear();

	m_rcxi.clear();
	m_rcxi.crc32 = getConfigFileCrc();

	requestGetConfigXmlCrc();

	m_requestGetXmlInfoTimer.start(theOptions.customerData().requestConfigTime());
}

// -------------------------------------------------------------------------------------------------------------------

void ConfigSocket::onSocketThreadFinished()
{
	m_requestGetXmlInfoTimer.stop();

	disconnect(this, &ConfigSocket::cfgXmlReceived, &theProviderBase, &Provider::Base::readFromXml);
	disconnect(this, &ConfigSocket::cfgXmlReceived, &theProviderTypeBase, &Provider::TypeBase::readFromXml);
}

// -------------------------------------------------------------------------------------------------------------------

quint32 ConfigSocket::getConfigFileCrc()
{
	m_cfgXmlData.clear();

	// read form file
	//
	QFile file(xmlConfigFileName);
	if (file.open(QIODevice::ReadOnly) == false)
	{
		return 0xFFFFFFFF;
	}

	m_cfgXmlData = file.readAll();
	file.close();

	qDebug() << "ConfigSocket::getConfigFileCrc - config file size: " << m_cfgXmlData.size();

	if (m_cfgXmlData.size() == 0)
	{
		return 0xFFFFFFFF;
	}

	return CalcCRC32(m_cfgXmlData, m_cfgXmlData.size());
}


// -------------------------------------------------------------------------------------------------------------------

void ConfigSocket::processReply(const Udp::Request& request)
{
	if (request.errorCode() != SIO_ERROR_NONE)
	{
		qDebug() << "OrderStateSocket::processReply - Request has error : " << request.errorCode();
		assert(false);
	}


	switch(request.ID())
	{
		case CLIENT_GET_CONFIG_XML_CRC:
			replyGetConfigXmlCrc(request);
			break;

		case CLIENT_GET_CONFIG_XML_INFO:
			replyGetConfigXmlInfo(request);
			break;

		case CLIENT_GET_CONFIG_XML:
			replyGetConfigXml(request);
			break;

		default:
			qDebug() << "ConfigSocket::processReply - Unknown request.ID() : " << request.ID();
			assert(false);
			break;
	}
}

// -------------------------------------------------------------------------------------------------------------------
// CLIENT_GET_CONFIG_XML_CRC

void ConfigSocket::requestGetConfigXmlCrc()
{
	if (isReadyToSend() == false)
	{
		return;
	}

	if (m_rcxi.dataSize != 0)
	{
		return;
	}

	sendRequest(CLIENT_GET_CONFIG_XML_CRC);
}

// -------------------------------------------------------------------------------------------------------------------

void ConfigSocket::replyGetConfigXmlCrc(const Udp::Request& request)
{
	//const char * pBuffer = const_cast<const UdpRequest&>(udpRequest).data();

	quint32 cfgXmlCrc32 = *(quint32*) request.data();
	if (cfgXmlCrc32 == 0xFFFFFFFF)
	{
		assert(0);

		m_rcxi.clear();
		m_rcxi.crc32 = getConfigFileCrc();

	   // timer call requestGetConfigXmlCrc()

		return;
	}

	if (cfgXmlCrc32 == m_rcxi.crc32)
	{
		m_rcxi.dataSize = m_cfgXmlData.size();

		emit cfgXmlReceived(m_cfgXmlData, xmlTagProviderLastVersion);
		m_cfgXmlData.clear();

		qDebug() << "ConfigSocket::replyGetConfigXmlCrc - loaded from file by CRC";

		return;
	}

	requestGetConfigXmlInfo();
}

// -------------------------------------------------------------------------------------------------------------------
// CLIENT_GET_CONFIG_XML_INFO

void ConfigSocket::requestGetConfigXmlInfo()
{
	if (m_rcxi.dataSize != 0)
	{
		return;
	}

	sendRequest(CLIENT_GET_CONFIG_XML_INFO);
}

// -------------------------------------------------------------------------------------------------------------------

void ConfigSocket::replyGetConfigXmlInfo(const Udp::Request& request)
{
	quint32 version = *(quint32*) request.data();

	switch(version)
	{
		case 1:

			m_rcxi = *(sio_ReplyCfgXmlInfo*) request.data();

			break;

		default:
			break;
	}

	m_cfgXmlData.clear();

	if (m_rcxi.dataSize == 0 || m_rcxi.partCount == 0)
	 {
		 // we have empty data
		 //
		 assert(m_rcxi.dataSize == 0);
		 assert(m_rcxi.partCount == 0);

		 m_rcxi.clear();
		 m_rcxi.crc32 = getConfigFileCrc();

		// timer call requestGetConfigXmlCrc()

		 return;
	 }

	requestGetConfigXml(0);
}

// -------------------------------------------------------------------------------------------------------------------
// CLIENT_GET_CONFIG_XML

void ConfigSocket::requestGetConfigXml(quint32 partIndex)
{
	 if (partIndex >= m_rcxi.partCount)
	 {
		 // all parts were requested, then begin to require params
		 //
		 if ( m_rcxi.dataSize != m_cfgXmlData.count())
		 {
			 assert(m_rcxi.dataSize != m_cfgXmlData.count());
		 }


		 QFile file(xmlConfigFileName);
		 if (file.open(QIODevice::WriteOnly) == true)
		 {
			 file.write(m_cfgXmlData);
			 file.close();
		 }

		emit cfgXmlReceived(m_cfgXmlData, xmlTagProviderLastVersion);
		m_cfgXmlData.clear();

		qDebug() << "ConfigSocket::requestGetConfigXml - received bytes : " << m_rcxi.dataSize;

		 return;
	 }

	 sendRequest(CLIENT_GET_CONFIG_XML, (const char*) &partIndex, sizeof(partIndex));
}

// -------------------------------------------------------------------------------------------------------------------

void ConfigSocket::replyGetConfigXml(const Udp::Request& request)
{
	//const char * pBuffer = const_cast<const UdpRequest&>(udpRequest).data();

	sio_ReplyCfgXml* rcx = (sio_ReplyCfgXml*) request.data();

	if (rcx->partIndex < 0 || rcx->partIndex >= m_rcxi.partCount)
	{
		m_cfgXmlData.clear();

		m_rcxi.clear();
		m_rcxi.crc32 = getConfigFileCrc();

		// timer call requestGetConfigXmlCrc()
		//
		return;
	}

	m_cfgXmlData.append(rcx->data, rcx->dataSize);

	requestGetConfigXml(rcx->partIndex + 1);
}

// -------------------------------------------------------------------------------------------------------------------

void ConfigSocket::failReply(const Udp::Request& request)
{
	switch(request.ID())
	{
		case CLIENT_GET_CONFIG_XML_CRC:
			emit failConnection();
			break;

		case CLIENT_GET_CONFIG_XML_INFO:
			emit failConnection();
			break;

		case CLIENT_GET_CONFIG_XML:
			emit failConnection();
			break;

		default:
			assert(false);
			qDebug() << "ConfigSocket::failAck - Unknown request.ID() : " << request.ID();
			break;
	}
}

// -------------------------------------------------------------------------------------------------------------------
