#ifndef OPTIONS_H
#define OPTIONS_H

#include <QObject>
#include <QMutex>

#include "../../../lib/SocketIO.h"

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
	int					m_serverCfgPort = PORT_CONFIG_XML_REQUEST;
	int					m_serverDataPort = PORT_PROVIDER_ORDER_REQUEST;


public:

	QString				serverIP() const { return m_serverIP; }
	void				setServerIP(const QString& ip) { m_serverIP = ip; }

	int					serverCfgPort() const { return m_serverCfgPort; }
	void				setServerCfgPort(int port) { m_serverCfgPort = port; }

	int					serverDataPort() const { return m_serverDataPort; }
	void				setServerDataPort(int port) { m_serverDataPort = port; }

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
	int					m_serverProviderPort = PORT_PROVIDER_ORDER_REQUEST;
	int					m_serverCustomerPort = PORT_CUSTOMER_ORDER_REQUEST;

	quint32				m_providerID = -1;
	int					m_requestProviderTime = 1000;
	int					m_requestCustomerTime = 1000;

	bool				m_showKeyboard = true;

public:

	QString				serverIP() const { return m_serverIP; }
	void				setServerIP(const QString& ip) { m_serverIP = ip; }

	int					serveProviderPort() const { return m_serverProviderPort; }
	void				setServerProviderPort(int port) { m_serverProviderPort = port; }

	int					serverCustomerPort() const { return m_serverCustomerPort; }
	void				setServerCustomerPort(int port) { m_serverCustomerPort = port; }

	quint32				providerID() const { return m_providerID; }
	void				setProviderID(quint32 id) { m_providerID = id; }

	int					requestProviderTime() const { return m_requestProviderTime; }
	void				setRequestProviderTime(int time) { m_requestProviderTime = time; }

	int					requestCustomerTime() const { return m_requestCustomerTime; }
	void				setRequestCustomerTime(int time) { m_requestCustomerTime = time; }

	bool				showKeyboard() const { return m_showKeyboard; }
	void				setShowKeyboard(bool show) { m_showKeyboard = show; }


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

	ConnectionOption	m_connection;

	ProviderDataOption	m_providerData;

public:

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
