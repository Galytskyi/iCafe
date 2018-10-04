#include "Options.h"

#include <QSettings>

// -------------------------------------------------------------------------------------------------------------------

Options theOptions;

// -------------------------------------------------------------------------------------------------------------------
//
// DatabaseOption class implementation
//
// -------------------------------------------------------------------------------------------------------------------

DatabaseOption::DatabaseOption(QObject *parent) :
	QObject(parent)
{
}

// -------------------------------------------------------------------------------------------------------------------

DatabaseOption::DatabaseOption(const DatabaseOption& from, QObject *parent) :
	QObject(parent)
{
	*this = from;
}

// -------------------------------------------------------------------------------------------------------------------

DatabaseOption::~DatabaseOption()
{
}

// -------------------------------------------------------------------------------------------------------------------

bool DatabaseOption::create()
{
	remove();

//	thePtrDB = new Database;
//	if (thePtrDB == nullptr)
//	{
//		return false;
//	}

//	if (thePtrDB->open() == false)
//	{
//		return false;
//	}

	return true;
}

// -------------------------------------------------------------------------------------------------------------------

void DatabaseOption::remove()
{
//	if (thePtrDB != nullptr)
//	{
//		thePtrDB->close();
//		delete thePtrDB;
//	}
}

// -------------------------------------------------------------------------------------------------------------------

void DatabaseOption::load()
{
	QSettings s;

//	m_path = s.value(QString("%1Path").arg(DATABASE_OPTIONS_REG_KEY), QDir::currentPath()).toString();
//	m_type = s.value(QString("%1Type").arg(DATABASE_OPTIONS_REG_KEY), DATABASE_TYPE_SQLITE).toInt();
}

// -------------------------------------------------------------------------------------------------------------------

void DatabaseOption::save()
{
	QSettings s;

//	s.setValue(QString("%1Path").arg(DATABASE_OPTIONS_REG_KEY), m_path);
//	s.setValue(QString("%1Type").arg(DATABASE_OPTIONS_REG_KEY), m_type);
}

// -------------------------------------------------------------------------------------------------------------------

DatabaseOption& DatabaseOption::operator=(const DatabaseOption& from)
{
	m_path = from.m_path;
	m_type = from.m_type;

	return *this;
}

// -------------------------------------------------------------------------------------------------------------------
//
// ConnectionOption class implementation
//
// -------------------------------------------------------------------------------------------------------------------

ConnectionOption::ConnectionOption(QObject *parent) :
	QObject(parent)
{
}

// -------------------------------------------------------------------------------------------------------------------

ConnectionOption::ConnectionOption(const ConnectionOption& from, QObject *parent) :
	QObject(parent)
{
	*this = from;
}

// -------------------------------------------------------------------------------------------------------------------

ConnectionOption::~ConnectionOption()
{
}

// -------------------------------------------------------------------------------------------------------------------

void ConnectionOption::load()
{
	QSettings s;

	m_serverIP = s.value(QString("%1ServerIP").arg(CONNECTION_REG_KEY), "127.0.0.1").toString();
	m_serverCfgPort = s.value(QString("%1ServerCfgPort").arg(CONNECTION_REG_KEY), PORT_CONFIG_XML_REQUEST).toUInt();
	m_serverDataPort = s.value(QString("%1ServerDataPort").arg(CONNECTION_REG_KEY), PORT_PROVIDER_ORDER_REQUEST).toUInt();
}

// -------------------------------------------------------------------------------------------------------------------

void ConnectionOption::save()
{
	QSettings s;

	s.setValue(QString("%1ServerIP").arg(CONNECTION_REG_KEY), m_serverIP);
	s.setValue(QString("%1ServerCfgPort").arg(CONNECTION_REG_KEY), m_serverCfgPort);
	s.setValue(QString("%1ServerDataPort").arg(CONNECTION_REG_KEY), m_serverDataPort);
}

// -------------------------------------------------------------------------------------------------------------------

ConnectionOption& ConnectionOption::operator=(const ConnectionOption& from)
{
	m_serverIP = from.m_serverIP;
	m_serverCfgPort = from.m_serverCfgPort;
	m_serverDataPort = from.m_serverDataPort;

	return *this;
}

// -------------------------------------------------------------------------------------------------------------------
//
// CustomerDataOption class implementation
//
// -------------------------------------------------------------------------------------------------------------------

CustomerDataOption::CustomerDataOption(QObject *parent) :
	QObject(parent)
{
}

// -------------------------------------------------------------------------------------------------------------------

CustomerDataOption::CustomerDataOption(const CustomerDataOption& from, QObject *parent) :
	QObject(parent)
{
	*this = from;
}

// -------------------------------------------------------------------------------------------------------------------

CustomerDataOption::~CustomerDataOption()
{
}

// -------------------------------------------------------------------------------------------------------------------

void CustomerDataOption::load()
{
	QSettings s;

	m_serverIP = s.value(QString("%1ServerIP").arg(CUSTOMER_DATA_REG_KEY), "192.168.76.123").toString();
	m_serverConfigPort = s.value(QString("%1ServerConfigPort").arg(CUSTOMER_DATA_REG_KEY), PORT_CONFIG_XML_REQUEST).toUInt();
	m_serverCustomerPort = s.value(QString("%1ServerCustomerPort").arg(CUSTOMER_DATA_REG_KEY), PORT_CUSTOMER_ORDER_REQUEST).toUInt();

	m_phone = s.value(QString("%1Phone").arg(CUSTOMER_DATA_REG_KEY), 11).toULongLong();
	m_orderTime = s.value(QString("%1OrderTime").arg(CUSTOMER_DATA_REG_KEY), 49152).toUInt();
	m_people = s.value(QString("%1People").arg(CUSTOMER_DATA_REG_KEY), 2).toUInt();

	m_requestConfigTime = s.value(QString("%1RequestConfigTime").arg(CUSTOMER_DATA_REG_KEY), 1000).toUInt();
	m_requestCustomerTime = s.value(QString("%1RequestCustomerTime").arg(CUSTOMER_DATA_REG_KEY), 1000).toUInt();

	m_fontName = s.value(QString("%1FontName").arg(CUSTOMER_DATA_REG_KEY), "Arial").toString();
	m_fontSize = s.value(QString("%1FontSize").arg(CUSTOMER_DATA_REG_KEY), 12).toUInt();
}

// -------------------------------------------------------------------------------------------------------------------

void CustomerDataOption::save()
{
	QSettings s;

	s.setValue(QString("%1ServerIP").arg(CUSTOMER_DATA_REG_KEY), m_serverIP);
	s.setValue(QString("%1ServerConfigPort").arg(CUSTOMER_DATA_REG_KEY), m_serverConfigPort);
	s.setValue(QString("%1ServerCustomerPort").arg(CUSTOMER_DATA_REG_KEY), m_serverCustomerPort);

	s.setValue(QString("%1Phone").arg(CUSTOMER_DATA_REG_KEY), m_phone);
	s.setValue(QString("%1OrderTime").arg(CUSTOMER_DATA_REG_KEY), m_orderTime);
	s.setValue(QString("%1People").arg(CUSTOMER_DATA_REG_KEY), m_people);

	s.setValue(QString("%1RequestConfigTime").arg(CUSTOMER_DATA_REG_KEY), m_requestConfigTime);
	s.setValue(QString("%1RequestCustomerTime").arg(CUSTOMER_DATA_REG_KEY), m_requestCustomerTime);

	s.setValue(QString("%1FontName").arg(CUSTOMER_DATA_REG_KEY), m_fontName);
	s.setValue(QString("%1FontSize").arg(CUSTOMER_DATA_REG_KEY), m_fontSize);
}

// -------------------------------------------------------------------------------------------------------------------

CustomerDataOption& CustomerDataOption::operator=(const CustomerDataOption& from)
{
	m_serverIP = from.m_serverIP;
	m_serverConfigPort = from.m_serverConfigPort;
	m_serverCustomerPort = from.m_serverCustomerPort;

	m_orderType = from.m_orderType;
	m_providerID = from.m_providerID;
	m_phone = from.m_phone;
	m_orderTime = from.m_orderTime;
	m_people = from.m_people;

	m_requestConfigTime = from.m_requestConfigTime;
	m_requestCustomerTime = from.m_requestCustomerTime;

	m_fontName = from.m_fontName;
	m_fontSize = from.m_fontSize;

	return *this;
}

// -------------------------------------------------------------------------------------------------------------------
//
// Options class implementation
//
// -------------------------------------------------------------------------------------------------------------------

Options::Options(QObject *parent) :
	QObject(parent)
{
}

// -------------------------------------------------------------------------------------------------------------------

Options::Options(const Options& from, QObject *parent) :
	QObject(parent)
{
	*this = from;
}

// -------------------------------------------------------------------------------------------------------------------

Options::~Options()
{
}

// -------------------------------------------------------------------------------------------------------------------

void Options::load()
{
	//m_database.load();
	//m_database.create();

	m_connection.load();

	m_customerData.load();
}

// -------------------------------------------------------------------------------------------------------------------

void Options::save()
{
	m_customerData.save();

	m_connection.save();

	//m_database.save();
}

// -------------------------------------------------------------------------------------------------------------------

void Options::unload()
{
}

// -------------------------------------------------------------------------------------------------------------------

bool Options::readFromXml()
{
	bool result = false;

	return result;
}

// -------------------------------------------------------------------------------------------------------------------

Options& Options::operator=(const Options& from)
{
	QMutexLocker locker(&m_mutex);

	m_database = from.m_database;
	m_connection = from.m_connection;
	m_customerData = from.m_customerData;

	return *this;
}

// -------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------
