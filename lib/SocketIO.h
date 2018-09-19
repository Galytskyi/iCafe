#pragma once

#include <QString>

// ==============================================================================================

const int		MAX_UDP_DATAGRAM_SIZE = 4096;

// ==============================================================================================
// ports
//

const int		PORT_CONFIG_XML_REQUEST = 15100;
const int		PORT_CUSTOMER_ORDER_REQUEST = 15200;
const int		PORT_PROVIDER_ORDER_REQUEST = 15300;

// ==============================================================================================
// request id
//
				// PORT_CONFIG_XML_REQUEST = 15100
				//
const quint32	CLIENT_GET_CONFIG_XML_CRC = 1100;
const quint32	CLIENT_GET_CONFIG_XML_INFO = 1101;
const quint32	CLIENT_GET_CONFIG_XML = 1102;

				// PORT_CUSTOMER_ORDER_REQUEST = 15200
				//
const quint32	CLIENT_CREATE_ORDER = 1200;
const quint32	CLIENT_GET_ORDER_STATE = 1201;
const quint32	CLIENT_REMOVE_ORDER = 1202;

				// PORT_PROVIDER_ORDER_REQUEST = 15300
				//
const quint32	CLIENT_GET_ORDER = 1300;
const quint32	CLIENT_SET_ORDER_STATE = 1301;

// ==============================================================================================
// Limiters
//

const int		MAX_FAIL_ACK_COUNT = 5;

const int		MAX_CUSTOMER_ORDER_COUNT = 5;

const int		MAX_SECONDS_ORDER_LIVE = 60 * 60; // in seconds

const int		CUSTOMER_GET_ORDER_STATE_MAX = 10;

// ==============================================================================================
// request struct
//
// ----------------------------------------------------------------------------------------------
// CLIENT_GET_CONFIG_XML_INFO
//
const quint32	REPLY_CONFIG_XML_INFO_VERSION = 1;

#pragma pack(push, 1)

struct replyCfgXmlInfo
{
	quint32 version = REPLY_CONFIG_XML_INFO_VERSION;

	quint32 dataSize = 0;
	quint32 bytesPerPart = 0;
	quint32 partCount = 0;

	quint32	crc32 = 0xFFFFFFFF;

	void clear()
	{
		version = REPLY_CONFIG_XML_INFO_VERSION;

		dataSize = 0;
		bytesPerPart = 0;
		partCount = 0;

		crc32 = 0xFFFFFFFF;
	}
};

#pragma pack(pop)

// ----------------------------------------------------------------------------------------------
// CLIENT_GET_CONFIG_XML
//
const quint32	REPLY_CONFIG_XML_VERSION = 1;

#pragma pack(push, 1)

struct replyCfgXml
{
	quint32 version = REPLY_CONFIG_XML_VERSION;

	quint32 partIndex = 0;

	quint32 dataSize = 0;
	char data[MAX_UDP_DATAGRAM_SIZE];
};

#pragma pack(pop)

// ----------------------------------------------------------------------------------------------
// ORDER_WRAP
//
const quint32	ORDER_WRAP_VERSION = 1;

#pragma pack(push, 1)

struct orderWrap
{
	quint32 version = ORDER_WRAP_VERSION;

	quint32 state = 0;
	quint32 type = 0;

	quint64 orderID = 0;

	quint32 orderTime = 0;
	quint32 people = 0;

	quint32 cancelCode = 0;

	bool isValid() const { return true; }
};

#pragma pack(pop)

// ----------------------------------------------------------------------------------------------
// CLIENT_GET_ORDER
//
const quint32	REQUEST_GET_ORDER_VERSION = 1;

#pragma pack(push, 1)

struct requestGetOrder
{
	quint32 version = REQUEST_GET_ORDER_VERSION;

	quint32 providerID = -1;
	quint32 wrapVersion = 0;
};

#pragma pack(pop)

// ==============================================================================================
// xml tags
//

const char* const	xmlConfigFileName = "config.xml";
const char* const	xmlTagConfigFile = "ConfigFile";
const int			xmlTagConfigFileLastVersion = 1;

const char* const	xmlTagProviders = "Providers";
const char* const	xmlTagProvider = "Provider";
const int			xmlTagProviderLastVersion = 1;

const char* const	xmlTagProviderTypes = "ProviderTypes";
const char* const	xmlTagProviderType = "ProviderType";
const int			xmlTagProviderTypeLastVersion = 1;

const char* const	xmlOrderFileName = "order.xml";
const char* const	xmlTagOrderFile = "OrderFile";
const int			xmlTagOrderFileLastVersion = 1;

const char* const	xmlTagOrders = "Orders";
const char* const	xmlTagOrder = "Order";
const int			xmlTagOrderLastVersion = 1;

// ==============================================================================================

