#include "Options.h"

#include <QSettings>

// -------------------------------------------------------------------------------------------------------------------

Options theOptions;

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

	m_serverIP = s.value(QString("%1ServerIP").arg(CONNECTION_REG_KEY), "192.168.76.123").toString();
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
// ProviderDataOption class implementation
//
// -------------------------------------------------------------------------------------------------------------------

ProviderDataOption::ProviderDataOption(QObject *parent) :
	QObject(parent)
{
}

// -------------------------------------------------------------------------------------------------------------------

ProviderDataOption::ProviderDataOption(const ProviderDataOption& from, QObject *parent) :
	QObject(parent)
{
	*this = from;
}

// -------------------------------------------------------------------------------------------------------------------

ProviderDataOption::~ProviderDataOption()
{
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderDataOption::load()
{
	QSettings s;

	m_providerID = s.value(QString("%1ProviderID").arg(PROVIDER_DATA_REG_KEY), -1).toUInt();
	m_requestProviderTime = s.value(QString("%1RequestProviderTime").arg(PROVIDER_DATA_REG_KEY), 1000).toInt();
	m_requestCustomerTime = s.value(QString("%1RequestCustomerTime").arg(PROVIDER_DATA_REG_KEY), 1000).toInt();

	m_serverIP = s.value(QString("%1ServerIP").arg(PROVIDER_DATA_REG_KEY), "192.168.76.123").toString();
	m_serverProviderPort = s.value(QString("%1ServerProviderPort").arg(PROVIDER_DATA_REG_KEY), PORT_PROVIDER_ORDER_REQUEST).toInt();
	m_serverCustomerPort = s.value(QString("%1ServerCustomerPort").arg(PROVIDER_DATA_REG_KEY), PORT_CUSTOMER_ORDER_REQUEST).toInt();

	m_showKeyboard = s.value(QString("%1ShowKeyboard").arg(PROVIDER_DATA_REG_KEY), true).toBool();
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderDataOption::save()
{
	QSettings s;

	s.setValue(QString("%1ProviderID").arg(PROVIDER_DATA_REG_KEY), m_providerID);
	s.setValue(QString("%1RequestProviderTime").arg(PROVIDER_DATA_REG_KEY), m_requestProviderTime);
	s.setValue(QString("%1RequestCustomerTime").arg(PROVIDER_DATA_REG_KEY), m_requestCustomerTime);

	s.setValue(QString("%1ServerIP").arg(PROVIDER_DATA_REG_KEY), m_serverIP);
	s.setValue(QString("%1ServerProviderPort").arg(PROVIDER_DATA_REG_KEY), m_serverProviderPort);
	s.setValue(QString("%1ServerCustomerPort").arg(PROVIDER_DATA_REG_KEY), m_serverCustomerPort);

	s.setValue(QString("%1ShowKeyboard").arg(PROVIDER_DATA_REG_KEY), m_showKeyboard);
}

// -------------------------------------------------------------------------------------------------------------------

ProviderDataOption& ProviderDataOption::operator=(const ProviderDataOption& from)
{
	m_providerID = from.m_providerID;
	m_requestProviderTime = from.m_requestProviderTime;
	m_requestCustomerTime = from.m_requestCustomerTime;

	m_serverIP = from.m_serverIP;
	m_serverProviderPort = from.m_serverProviderPort;
	m_serverCustomerPort = from.m_serverCustomerPort;

	m_showKeyboard = from.m_showKeyboard;

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

	//m_connection.load();

	m_providerData.load();
}

// -------------------------------------------------------------------------------------------------------------------

void Options::save()
{
	m_providerData.save();

	//m_connection.save();

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

	m_connection = from.m_connection;
	m_providerData = from.m_providerData;

	return *this;
}

// -------------------------------------------------------------------------------------------------------------------
