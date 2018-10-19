#ifndef OPTIONS_H
#define OPTIONS_H

#include <QObject>
#include <QMutex>

#include "../lib/SocketIO.h"

// ==============================================================================================

#define					DATABASE_OPTIONS_REG_KEY		"Options/Database/"

// ----------------------------------------------------------------------------------------------

const char* const		DatabaseParam[] =
{
						QT_TRANSLATE_NOOP("Options.h", "Path"),
						QT_TRANSLATE_NOOP("Options.h", "Type"),
};

const int				DBO_PARAM_COUNT	= sizeof(DatabaseParam)/sizeof(DatabaseParam[0]);

const int				DBO_PARAM_PATH	= 0,
						DBO_PARAM_TYPE	= 1;

// ----------------------------------------------------------------------------------------------

const char* const		DatabaseType[] =
{
						QT_TRANSLATE_NOOP("Options.h", "SQLite"),
						QT_TRANSLATE_NOOP("Options.h", "PSQL"),
};

const int				DATABASE_TYPE_COUNT		= sizeof(DatabaseType)/sizeof(DatabaseType[0]);

const int				DATABASE_TYPE_SQLITE	= 0,
						DATABASE_TYPE_PSQL		= 1;

// ----------------------------------------------------------------------------------------------

class DatabaseOption : public QObject
{
	Q_OBJECT

public:

	explicit DatabaseOption(QObject *parent = 0);
	explicit DatabaseOption(const DatabaseOption& from, QObject *parent = 0);
	virtual ~DatabaseOption();

private:

	QString				m_path;
	int					m_type;

public:

	QString				path() const { return m_path; }
	void				setPath(const QString& path) { m_path = path; }

	int					type() const { return m_type; }
	void				setType(int type) { m_type = type; }


	bool				create();
	void				remove();

	void				load();
	void				save();

	DatabaseOption&		operator=(const DatabaseOption& from);
};

// ==============================================================================================

#define					UDP_OPTIONS_REG_KEY		"Options/Udp/"

// ----------------------------------------------------------------------------------------------

class UdpOption : public QObject
{
	Q_OBJECT

public:

	explicit UdpOption(QObject *parent = 0);
	explicit UdpOption(const UdpOption& from, QObject *parent = 0);
	virtual ~UdpOption();

private:

	quint32				m_requestCustomerTime = REQUEST_CUSTOMER_TIMEOUT;
	quint32				m_waitReplyCustomerTime = WAIT_REPLY_CUSTOMER_TIMEOUT;

	quint32				m_requestProviderTime = REQUEST_PROVIDER_TIMEOUT;
	quint32				m_waitReplyProviderTime = WAIT_REPLY_PROVIDER_TIMEOUT;

public:

	quint32				requestCustomerTime() const { return m_requestCustomerTime; }
	void				setRequestCustomerTime(quint32 ms) { m_requestCustomerTime = ms; }

	quint32				waitReplyCustomerTime() const { return m_waitReplyCustomerTime; }
	void				setWaitReplyCustomerTime(quint32 ms) { m_waitReplyCustomerTime = ms; }

	quint32				requestProviderTime() const { return m_requestProviderTime; }
	void				setRequestProviderTime(quint32 ms) { m_requestProviderTime = ms; }

	quint32				waitReplyProviderTime() const { return m_waitReplyProviderTime; }
	void				setWaitReplyProviderTime(quint32 ms) { m_waitReplyProviderTime = ms; }

	void				load();
	void				save();

	UdpOption&			operator=(const UdpOption& from);
};


// ==============================================================================================

const int			PLATFORM_TYPE_DESKTOP	= 0,
					PLATFORM_TYPE_ANDROID	= 1;

const int			PLATFORM_TYPE_COUNT		= 2;

// ==============================================================================================

class Options : public QObject
{
	Q_OBJECT

public:

	explicit	Options(QObject *parent = 0);
				Options(const Options& from, QObject *parent = 0);
	virtual		~Options();

private:

	QMutex				m_mutex;

	int					m_appType = PLATFORM_TYPE_DESKTOP;

	DatabaseOption		m_database;

	UdpOption			m_udp;

public:

	int					platformType() const { return m_appType; }
	void				setPlatformType(int type) { m_appType = type; }

	DatabaseOption&		database() { return m_database; }
	void				setDatabase(const DatabaseOption& database) { m_database = database; }

	UdpOption&			udp() { return m_udp; }
	void				setUdp(const UdpOption& udp) { m_udp = udp; }

	void				load();
	void				save();
	void				unload();

	bool				readFromXml();

	Options&			operator=(const Options& from);
};

// ==============================================================================================

extern Options			theOptions;

// ==============================================================================================

#endif // OPTIONS_H
