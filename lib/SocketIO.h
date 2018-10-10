#pragma once

#include <QObject>

// ==============================================================================================

const int		MAX_UDP_DATAGRAM_SIZE = 4096;

const int		UDP_REQUEST_HEADER_VERSION = 1;

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
// Errors
//

const int		SIO_ERROR_NONE = 0,
				SIO_ERROR_INCCORECT_CRC32 = 1,
				SIO_ERROR_INCCORECT_REQUEST_ID = 2,
				SIO_ERROR_INCCORECT_PART_NUMBER = 3;

const int		SIO_ERROR_COUNT = 3;

// ==============================================================================================
// Limiters
//

const int		MAX_FAIL_ACK_COUNT				= 5;

const int		MAX_CUSTOMER_ORDER_COUNT		= 5;

const int		MAX_SECONDS_AUTO_REMOVE_ORDER	= 10 * 60 * 1000;	// 10 min
const int		MAX_SECONDS_ORDER_LIVE			= 30 * 60;			// 30 min in seconds
const int		MAX_SECONDS_ORDER_LIVE_AFTER_OK	= 20 * 60;			// 20 min in seconds

// ==============================================================================================
// request struct
//
// ----------------------------------------------------------------------------------------------
// CLIENT_GET_CONFIG_XML_INFO
//
const quint32	REPLY_CONFIG_XML_INFO_VERSION = 1;

#pragma pack(push, 1)

struct sio_ReplyCfgXmlInfo
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

struct sio_ReplyCfgXml
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

struct sio_OrderWrap
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

struct sio_RequestGetOrder
{
	quint32 version = REQUEST_GET_ORDER_VERSION;

	quint32 providerID = -1;
	quint32 wrapVersion = 0;
};

#pragma pack(pop)

// ==============================================================================================

const char* const	NO_CONNECTION_STR = QT_TRANSLATE_NOOP("OrderReceiveSocket.h", "  Ожидание соединения ...");

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

const char* const	ArchTypeStr[] =
{
					"Error",
					"Warning",
					"Event",
					"Order",
};

const int			ARCH_MSG_TYPE_COUNT						= sizeof(ArchTypeStr)/sizeof(ArchTypeStr[0]);

const int			ARCH_MSG_TYPE_ERROR						= 0,
					ARCH_MSG_TYPE_WARNING					= 1,
					ARCH_MSG_TYPE_EVENT						= 2,
					ARCH_MSG_TYPE_ORDER						= 3;

// ----------------------------------------------------------------------------------------------

const char* const	ArchFileName = "arch.csv";

// ----------------------------------------------------------------------------------------------

const char* const	ArchFileDivider = ";";
const char* const	ArchFileTerminator = "\r\n";

// ==============================================================================================

