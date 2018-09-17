#include "Options.h"

#include <QSettings>

#include "Database.h"

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

	m_path = s.value(QString("%1Path").arg(DATABASE_OPTIONS_REG_KEY), QDir::currentPath()).toString();
	m_type = s.value(QString("%1Type").arg(DATABASE_OPTIONS_REG_KEY), DATABASE_TYPE_SQLITE).toInt();
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
	m_mutex.lock();

		m_database = from.m_database;

	m_mutex.unlock();

	return *this;
}

// -------------------------------------------------------------------------------------------------------------------
