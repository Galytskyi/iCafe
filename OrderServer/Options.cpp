#include "Options.h"

#include <QSettings>

#include "Database.h"

#include "../lib/SocketIO.h"

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

	thePtrDB = new Database;
	if (thePtrDB == nullptr)
	{
		return false;
	}

	if (thePtrDB->open() == false)
	{
		return false;
	}

	return true;
}

// -------------------------------------------------------------------------------------------------------------------

void DatabaseOption::remove()
{
	if (thePtrDB != nullptr)
	{
		thePtrDB->close();
		delete thePtrDB;
	}
}

// -------------------------------------------------------------------------------------------------------------------

void DatabaseOption::load()
{
	QSettings s;

	m_path = s.value(QString("%1Path").arg(DATABASE_OPTIONS_REG_KEY), qApp->applicationDirPath()).toString();
	m_type = s.value(QString("%1Type").arg(DATABASE_OPTIONS_REG_KEY), DATABASE_TYPE_PSQL).toInt();
}

// -------------------------------------------------------------------------------------------------------------------

void DatabaseOption::save()
{
	QSettings s;

	s.setValue(QString("%1Path").arg(DATABASE_OPTIONS_REG_KEY), m_path);
	s.setValue(QString("%1Type").arg(DATABASE_OPTIONS_REG_KEY), m_type);
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
// UdpOption class implementation
//
// -------------------------------------------------------------------------------------------------------------------

UdpOption::UdpOption(QObject *parent) :
	QObject(parent)
{
}

// -------------------------------------------------------------------------------------------------------------------

UdpOption::UdpOption(const UdpOption& from, QObject *parent) :
	QObject(parent)
{
	*this = from;
}

// -------------------------------------------------------------------------------------------------------------------

UdpOption::~UdpOption()
{
}

// -------------------------------------------------------------------------------------------------------------------

void UdpOption::load()
{
	QSettings s;

	m_requestCustomerTime = s.value(QString("%1RequestCustomerTime").arg(UDP_OPTIONS_REG_KEY), REQUEST_CUSTOMER_TIMEOUT).toInt();
	m_waitReplyCustomerTime = s.value(QString("%1WaitReplyCustomerTime").arg(UDP_OPTIONS_REG_KEY), WAIT_REPLY_CUSTOMER_TIMEOUT).toInt();

	m_requestProviderTime = s.value(QString("%1RequestProviderTime").arg(UDP_OPTIONS_REG_KEY), REQUEST_PROVIDER_TIMEOUT).toInt();
	m_waitReplyProviderTime = s.value(QString("%1WaitReplyProviderTime").arg(UDP_OPTIONS_REG_KEY), WAIT_REPLY_PROVIDER_TIMEOUT).toInt();
}

// -------------------------------------------------------------------------------------------------------------------

void UdpOption::save()
{
	QSettings s;

	s.setValue(QString("%1RequestCustomerTime").arg(UDP_OPTIONS_REG_KEY), m_requestCustomerTime);
	s.setValue(QString("%1WaitReplyCustomerTime").arg(UDP_OPTIONS_REG_KEY), m_waitReplyCustomerTime);

	s.setValue(QString("%1RequestProviderTime").arg(UDP_OPTIONS_REG_KEY), m_requestProviderTime);
	s.setValue(QString("%1WaitReplyProviderTime").arg(UDP_OPTIONS_REG_KEY), m_waitReplyProviderTime);
}

// -------------------------------------------------------------------------------------------------------------------

UdpOption& UdpOption::operator=(const UdpOption& from)
{
	m_requestCustomerTime = from.m_requestCustomerTime;
	m_waitReplyCustomerTime = from.m_waitReplyCustomerTime;

	m_requestProviderTime = from.m_requestProviderTime;
	m_waitReplyProviderTime = from.m_waitReplyProviderTime;

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
	m_database.load();
	m_database.create();
}

// -------------------------------------------------------------------------------------------------------------------

void Options::save()
{
	m_database.save();
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

	m_appType = from.m_appType;
	m_database = from.m_database;
	m_udp = from.m_udp;

	return *this;
}

// -------------------------------------------------------------------------------------------------------------------
