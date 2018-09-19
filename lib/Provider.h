#ifndef PROVIDER_H
#define PROVIDER_H

#include <QMutex>
#include <QTimer>
#include <QDebug>

#include "../lib/XmlHelper.h"

class XmlWriteHelper;
class XmlReadHelper;

namespace Provider
{
	// ==============================================================================================

	const quint32			INVALID_ID	= -1;

	// ==============================================================================================

	const int				TIMER_WAIT_CONNECT = 5 * 1000; // 5 sec

	// ==============================================================================================

	class Item : public QObject
	{
		Q_OBJECT

	public:

		explicit Item(QObject* parent = 0);
		Item(const Item& provider, QObject *parent = 0);
		virtual ~Item();

	private:

		quint32				m_id = INVALID_ID;

		bool				m_active = false;
		int					m_type = 0;
		QString				m_name;

		QString				m_address;
		QString				m_phone;
		QString				m_mail;
		QString				m_website;

		bool				m_connect = false;

		QTimer				m_timer;

	public:

		bool				isEmpty() const;
		void				clear();

		quint32				providerID() const { return m_id; }
		void				setProviderID(quint32 id) { m_id = id; }

		bool				active() const { return m_active; }
		void				setActive(bool active) { m_active = active; }

		int					type() const { return m_type; }
		void				setType(int type) { m_type = type; }

		QString				name() const { return m_name; }
		void				setName(const QString& name) { m_name = name; }

		QString				address() const { return m_address; }
		void				setAddress(const QString& address) { m_address = address; }

		QString				phone() const { return m_phone; }
		void				setPhone(const QString& phone) { m_phone = phone; }

		QString				mail() const { return m_mail; }
		void				setMail(const QString& mail) { m_mail = mail; }

		QString				website() const { return m_website; }
		void				setWebsite(const QString& website) { m_website = website; }

		bool				isConnected() const { return m_connect; }
		void				setConnectState(bool connect);

		bool				readFromXml(XmlReadHelper& xml, int version);
		void				writeToXml(XmlWriteHelper& xml, int version);

		Item&				operator=(const Item& from);

	signals:

		void				providerConnected(quint32 providerID);
		void				providerDisconnected(quint32 providerID);

	private slots:

		void				onTimeoutConnect() { setConnectState(false); }
	};

	// ==============================================================================================

	class Base : public QObject
	{
		Q_OBJECT

	public:

		explicit Base(QObject *parent = 0);
		virtual ~Base();

	private:

		mutable QMutex			m_mutex;
		QVector<Item>			m_providerList;
		QMap<quint16, int>		m_providerIndexMap;

	public:

		void					clear();
		int						count() const;

		bool					isExist(quint32 providerID) const;

		int						append(const Item &provider);
		void					append(const QVector<Item>& list);
		void					remove(int index);

		Item					provider(int index) const;
		Item*					providerPtr(int index);

		Item					provider(quint16 providerID) const;
		Item*					providerPtr(quint16 providerID);

		void					setProvider(int index, const Item& provider);

		Item*					data() const;

		Base&					operator=(const Base& from);

	signals:

		void					cfgXmlDataLoaded();

	public slots:

		bool					readFromXml(const QByteArray& cfgXmlData, int version);
		void					writeToXml(XmlWriteHelper& xml, int version);

		void					setProviderConnected(quint32 providerID, quint32 wrapVersion);
	};

	// ==============================================================================================

	class Type : public QObject
	{
		Q_OBJECT

	public:

		explicit Type(QObject *parent = 0);
		Type(const Type& type, QObject *parent = 0);
		virtual ~Type();

	private:

		quint32				m_id = INVALID_ID;

		QString				m_name;

	public:

		bool				isEmpty() const;
		void				clear();

		int					typeID() const { return m_id; }
		void				setTypeID(int id) { m_id = id; }

		QString				name() const { return m_name; }
		void				setName(const QString& name) { m_name = name; }

		bool				readFromXml(XmlReadHelper& xml, int version);
		void				writeToXml(XmlWriteHelper& xml, int version);

		Type&				operator=(const Type& from);
	};

	// ==============================================================================================

	class TypeBase : public QObject
	{
		Q_OBJECT

	public:

		explicit TypeBase(QObject *parent = 0);
		virtual ~TypeBase();

	private:

		mutable QMutex			m_mutex;
		QVector<Type>			m_typeList;

	public:

		void					clear();
		int						count() const;

		int						append(const Type &type);
		void					append(const QVector<Type>& list);
		void					remove(int index);

		Type					type(int index) const;
		Type*					typePtr(int index);
		void					setType(int index, const Type& type);

		Type*					data() const;

		TypeBase&				operator=(const TypeBase& from);

	signals:

		void					cfgXmlDataLoaded();

	public slots:

		bool					readFromXml(const QByteArray& cfgXmlData, int version);
		void					writeToXml(XmlWriteHelper& xml, int version);
	};
}

// ==============================================================================================

Q_DECLARE_METATYPE(Provider::Item)	// for type QVariant
Q_DECLARE_METATYPE(Provider::Type)	// for type QVariant

// ==============================================================================================

extern Provider::Base theProviderBase;
extern Provider::TypeBase theProviderTypeBase;

// ==============================================================================================

#endif // PROVIDER_H
