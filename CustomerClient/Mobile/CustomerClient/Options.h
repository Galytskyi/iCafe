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
	quint16				m_serverDataPort = PORT_CUSTOMER_ORDER_REQUEST;

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

#define					CUSTOMER_DATA_REG_KEY		"Options/CustomerData/"

// ----------------------------------------------------------------------------------------------

class CustomerDataOption : public QObject
{
	Q_OBJECT

public:

	explicit CustomerDataOption(QObject *parent = 0);
	CustomerDataOption(const CustomerDataOption& from, QObject *parent = 0);
	virtual ~CustomerDataOption();

private:

	QString				m_serverIP;
	int					m_serverConfigPort = PORT_CONFIG_XML_REQUEST;
	int					m_serverCustomerPort = PORT_CUSTOMER_ORDER_REQUEST;

	int					m_orderType = 0;
	quint32				m_providerID = -1;
	quint32				m_phone = 0;
	quint32				m_orderTime = 49152;
	int					m_people = 2;

	int					m_requestConfigTime = 1000;
	int					m_requestCustomerTime = 1000;

	QString				m_fontName;
	int					m_fontSize = 12;

public:

	int					w;
	int					h;

	QString				serverIP() const { return m_serverIP; }
	void				setServerIP(const QString& ip) { m_serverIP = ip; }

	int					serverConfigPort() const { return m_serverConfigPort; }
	void				setServerConfigPort(int port) { m_serverConfigPort = port; }

	int					serverCustomerPort() const { return m_serverCustomerPort; }
	void				setServerCustomerPort(int port) { m_serverCustomerPort = port; }


	int					orderType() const { return m_orderType; }
	void				setOrderType(int type) { m_orderType = type; }

	quint32				providerID() const { return m_providerID; }
	void				setProviderID(quint32 id) { m_providerID = id; }

	quint32				phone() const { return m_phone; }
	void				setPhone(quint32 phone) { m_phone = phone; }

	quint32				orderTime() const { return m_orderTime; }
	void				setOrderTime(quint32 time) { m_orderTime = time; }

	int					people() const { return m_people; }
	void				setPeople(int count) { m_people = count; }

	int					requestConfigTime() const { return m_requestConfigTime; }
	void				setRequestConfigTime(int  time) { m_requestConfigTime = time; }

	int					requestCustomerTime() const { return m_requestCustomerTime; }
	void				setRequestCustomerTime(int  time) { m_requestCustomerTime = time; }

	QString				fontName() const { return m_fontName; }
	void				setFontName(const QString& name) { m_fontName = name; }

	int					fontSize() const { return m_fontSize; }
	void				setFontSize(int size) { m_fontSize = size; }


	void				load();
	void				save();

	CustomerDataOption&	operator=(const CustomerDataOption& from);
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

	CustomerDataOption	m_customerData;

public:

	DatabaseOption&		database() { return m_database; }
	void				setDatabase(const DatabaseOption& database) { m_database = database; }

	ConnectionOption&	connection() { return m_connection; }
	void				setConnection(const ConnectionOption& connection) { m_connection = connection; }

	CustomerDataOption&	customerData() { return m_customerData; }
	void				setCustomerData(const CustomerDataOption& data) { m_customerData = data; }

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