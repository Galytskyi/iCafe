#ifndef OPTIONS_H
#define OPTIONS_H

#include <QObject>
#include <QMutex>

#include "../../../lib/SocketIO.h"

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
};

const int				DATABASE_TYPE_COUNT		= sizeof(DatabaseType)/sizeof(DatabaseType[0]);

const int				DATABASE_TYPE_SQLITE	= 0;


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

#define					CONNECTION_REG_KEY		"Options/Connection/"

// ----------------------------------------------------------------------------------------------

class ConnectionOption : public QObject
{
	Q_OBJECT

public:

	explicit ConnectionOption(QObject *parent = 0);
	ConnectionOption(const ConnectionOption& from, QObject *parent = 0);
	virtual ~ConnectionOption();

private:

	QString				m_serverIP;
	quint16				m_serverCfgPort = PORT_CONFIG_XML_REQUEST;
	quint16				m_serverDataPort = PORT_PROVIDER_ORDER_REQUEST;
public:

	QString				serverIP() const { return m_serverIP; }
	void				setServerIP(const QString& ip) { m_serverIP = ip; }

	quint16				serverCfgPort() const { return m_serverCfgPort; }
	void				setServerCfgPort(quint16 port) { m_serverCfgPort = port; }

	quint16				serverDataPort() const { return m_serverDataPort; }
	void				setServerDataPort(quint16 port) { m_serverDataPort = port; }

	void				load();
	void				save();

	ConnectionOption&	operator=(const ConnectionOption& from);
};

// ==============================================================================================

#define					PROVIDER_DATA_REG_KEY		"Options/ProviderData/"

// ----------------------------------------------------------------------------------------------

class ProviderDataOption : public QObject
{
	Q_OBJECT

public:

	explicit ProviderDataOption(QObject *parent = 0);
	ProviderDataOption(const ProviderDataOption& from, QObject *parent = 0);
	virtual ~ProviderDataOption();

private:

	QString				m_serverIP;
	quint16				m_serverProviderPort = PORT_PROVIDER_ORDER_REQUEST;
	quint16				m_serverCustomerPort = PORT_CUSTOMER_ORDER_REQUEST;

	quint16				m_providerID = -1;
	quint16				m_requestProviderTime = 1000;
	quint16				m_requestCustomerTime = 1000;

public:

	QString				serverIP() const { return m_serverIP; }
	void				setServerIP(const QString& ip) { m_serverIP = ip; }

	quint16				serveProviderPort() const { return m_serverProviderPort; }
	void				setServerProviderPort(quint16 port) { m_serverProviderPort = port; }

	quint16				serverCustomerPort() const { return m_serverCustomerPort; }
	void				setServerCustomerPort(quint16 port) { m_serverCustomerPort = port; }

	quint16				providerID() const { return m_providerID; }
	void				setProviderID(quint16 id) { m_providerID = id; }

	quint16				requestProviderTime() const { return m_requestProviderTime; }
	void				setRequestProviderTime(quint16  time) { m_requestProviderTime = time; }

	quint16				requestCustomerTime() const { return m_requestCustomerTime; }
	void				setRequestCustomerTime(quint16  time) { m_requestCustomerTime = time; }

	void				load();
	void				save();

	ProviderDataOption&	operator=(const ProviderDataOption& from);
};

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

	DatabaseOption		m_database;

	ConnectionOption	m_connection;

	ProviderDataOption	m_providerData;

public:

	DatabaseOption&		database() { return m_database; }
	void				setDatabase(const DatabaseOption& database) { m_database = database; }

	ConnectionOption&	connection() { return m_connection; }
	void				setConnection(const ConnectionOption& connection) { m_connection = connection; }

	ProviderDataOption&	providerData() { return m_providerData; }
	void				setProviderData(const ProviderDataOption& data) { m_providerData = data; }

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
