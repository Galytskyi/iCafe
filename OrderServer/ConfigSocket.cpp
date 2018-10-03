#include "ConfigSocket.h"

#include <assert.h>

#include "../lib/Provider.h"
#include "../lib/Crc32.h"

// -------------------------------------------------------------------------------------------------------------------
//
// ConfigSocket class implementation
//
// -------------------------------------------------------------------------------------------------------------------


ConfigSocket::ConfigSocket(const QHostAddress &serverAddress, quint16 port) :
	Udp::ServerSocket(serverAddress, port)
{

}

// -------------------------------------------------------------------------------------------------------------------

ConfigSocket::~ConfigSocket()
{
}

// -------------------------------------------------------------------------------------------------------------------

void ConfigSocket::onSocketThreadStarted()
{
	qDebug() << "ConfigSocket::onSocketThreadStarted()";
	emit appendMessageToArch(ARCH_MSG_TYPE_EVENT, __FUNCTION__, "started", Order::Item());

	createCfgXml();

	connect(this, &Udp::ServerSocket::requestReceived, this, &ConfigSocket::processRequest, Qt::QueuedConnection);
}

// -------------------------------------------------------------------------------------------------------------------

void ConfigSocket::onSocketThreadFinished()
{
}

// -------------------------------------------------------------------------------------------------------------------

void ConfigSocket::createCfgXml()
{
	QMutexLocker locker(&m_mutex);

	m_cfgXmlData.clear();
	XmlWriteHelper xml(&m_cfgXmlData);

	xml.setAutoFormatting(true);
	xml.writeStartDocument();
	{
		xml.writeStartElement(xmlTagConfigFile);
		{
			//xml.writeIntAttribute("buildID", m_buildResultWriter->buildInfo().id);

			xml.writeIntAttribute("Version", xmlTagConfigFileLastVersion ); // version of ConfigFile file

			theProviderBase.writeToXml(xml, xmlTagProviderLastVersion);	// Writing providers

			//theProviderTypeBase.writeToXml(xml, xmlTagProviderTypeLastVersion); // Writing provider types
		}
		xml.writeEndElement();	// <ConfigFile>
	}
	xml.writeEndDocument();

	updateCfgXmlInfo();
}

// -------------------------------------------------------------------------------------------------------------------

void ConfigSocket::updateCfgXmlInfo()
{
	m_rcxi.version = REPLY_CONFIG_XML_INFO_VERSION;

	switch(m_rcxi.version)
	{
		case 1:

			m_rcxi.dataSize = m_cfgXmlData.size();

			m_rcxi.bytesPerPart = (MAX_UDP_DATAGRAM_SIZE - (sizeof(Udp::RequestHeader) + sizeof(quint32) + sizeof(quint32) + sizeof(quint32))); //  version partIndex dataSize

			m_rcxi.partCount = m_rcxi.dataSize / m_rcxi.bytesPerPart;
			if (m_rcxi.dataSize % m_rcxi.bytesPerPart  != 0)
			{
				m_rcxi.partCount ++;
			}

			break;

		default:

			emit appendMessageToArch(ARCH_MSG_TYPE_ERROR, __FUNCTION__, QString("Undefined version: %1").arg(m_rcxi.version), Order::Item());
			qDebug() << tr("ConfigSocket::updateCfgXmlInfo - Undefined version");
			assert(0);

			break;
	}

	m_rcxi.crc32 = CalcCRC32(m_cfgXmlData, m_cfgXmlData.size());
}

// -------------------------------------------------------------------------------------------------------------------

void ConfigSocket::processRequest(Udp::Request request)
{
	if(request.headerCrcOk() == false)
	{
		emit appendMessageToArch(ARCH_MSG_TYPE_ERROR, __FUNCTION__, "SIO_ERROR_INCCORECT_CRC32", Order::Item());
		request.setErrorCode(SIO_ERROR_INCCORECT_CRC32);
		sendAck(request);
		return;
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

			request.setErrorCode(SIO_ERROR_INCCORECT_REQUEST_ID);
			sendAck(request);

			emit appendMessageToArch(ARCH_MSG_TYPE_ERROR, __FUNCTION__, QString("Unknown request.ID(): %1").arg(request.ID()), Order::Item());
			qDebug() << "ConfigSocket::processRequest - Unknown request.ID(): " << request.ID();
			assert(false);

			break;
	}
}

// -------------------------------------------------------------------------------------------------------------------

void ConfigSocket::replyGetConfigXmlCrc(Udp::Request request)
{
	request.initWrite();
	request.writeData((const char*) &m_rcxi.crc32, sizeof(m_rcxi.crc32));

	sendAck(request);
}

// -------------------------------------------------------------------------------------------------------------------

void ConfigSocket::replyGetConfigXmlInfo(Udp::Request request)
{
	request.initWrite();
	request.writeData((const char*) &m_rcxi, sizeof(m_rcxi));

	sendAck(request);
}

// -------------------------------------------------------------------------------------------------------------------

void ConfigSocket::replyGetConfigXml(Udp::Request request)
{
	int partIndex = (int) request.readDword();
	if (partIndex < 0 || partIndex >= (int) m_rcxi.partCount)
	{
		// error part index

		emit appendMessageToArch(ARCH_MSG_TYPE_ERROR, __FUNCTION__, QString("SIO_ERROR_INCCORECT_PART_NUMBER: %1").arg(partIndex), Order::Item());
		request.setErrorCode(SIO_ERROR_INCCORECT_PART_NUMBER);
		sendAck(request);

		return;
	}

	int offer = partIndex * m_rcxi.bytesPerPart;
	int dataSize = m_rcxi.bytesPerPart;

	if (partIndex == (int) m_rcxi.partCount - 1)
	{
		dataSize = m_rcxi.dataSize - offer;
	}

	request.initWrite();
	request.writeData((const char*) &REPLY_CONFIG_XML_VERSION, sizeof(quint32));
	request.writeData((const char*) &partIndex, sizeof(quint32));
	request.writeData((const char*) &dataSize, sizeof(quint32));
	request.writeData(m_cfgXmlData.data() + offer, dataSize);

	sendAck(request);
}

// -------------------------------------------------------------------------------------------------------------------
