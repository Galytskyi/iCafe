#include "ConfigSocket.h"

#include <assert.h>

#include "../lib/Provider.h"

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

	createCfgXml();

	connect(this, &Udp::ServerSocket::requestReceived, this, &ConfigSocket::processRequest, Qt::QueuedConnection);
}

// -------------------------------------------------------------------------------------------------------------------

void ConfigSocket::onSocketThreadFinished()
{

}

// -------------------------------------------------------------------------------------------------------------------

bool ConfigSocket::createCfgXml()
{
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

			theProviderTypeBase.writeToXml(xml, xmlTagProviderTypeLastVersion); // Writing provider types
		}
		xml.writeEndElement();	// <ConfigFile>
	}
	xml.writeEndDocument();

	m_cfgXmlCrc32 = CalcCRC32(m_cfgXmlData, m_cfgXmlData.size());

	updateCfgXmlInfo();

	return true;
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

			qDebug() << tr("ConfigSocket::updateCfgXmlInfo - Undefined version");
			assert(0);

			break;
	}
}

// -------------------------------------------------------------------------------------------------------------------

void ConfigSocket::processRequest(Udp::Request request)
{
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
			assert(false);
			qDebug() << "ConfigSocket::processRequest - Unknown request.ID() : " << request.ID();
			break;
	}
}

// -------------------------------------------------------------------------------------------------------------------

void ConfigSocket::replyGetConfigXmlCrc(Udp::Request request)
{
	request.initWrite();
	request.writeData((const char*) &m_cfgXmlCrc32, sizeof(m_cfgXmlCrc32));

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
		return;
	}

	int offer = partIndex * m_rcxi.bytesPerPart;
	int dataSize = m_rcxi.bytesPerPart;

	if (partIndex == (int) m_rcxi.partCount - 1)
	{
		dataSize = m_rcxi.dataSize - offer;
	}

	request.initWrite();
	request.writeDword(REPLY_CONFIG_XML_VERSION);
	request.writeDword(partIndex);
	request.writeDword(dataSize);
	request.writeData(m_cfgXmlData.data() + offer, dataSize);

	sendAck(request);
}

// -------------------------------------------------------------------------------------------------------------------
