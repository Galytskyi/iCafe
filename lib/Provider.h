#ifndef PROVIDER_H
#define PROVIDER_H

#include <QMutex>
#include <QTimer>
#include <QDebug>

#include "../lib/XmlHelper.h"
#include "../lib/Order.h"

class XmlWriteHelper;
class XmlReadHelper;

namespace Provider
{
	// ==============================================================================================

	const quint32			INVALID_ID	= -1;

	// ==============================================================================================

	const int				TIMER_WAIT_CONNECT = 5 * 1000; // 5 sec

	// ==============================================================================================

	#pragma pack(push, 1)

	union State
	{
		State(quint32 f = 0)
		{
			flags = f;
		}

		quint32 flags = 0;

		struct
		{
			quint32 active : 1;
			quint32 acceptOrder : 1;
			quint32 enableDinner : 1;
		};
	};

	#pragma pack(pop)


	// ==============================================================================================

	class Item : public QObject
	{
		Q_OBJECT

	public:

		explicit Item(QObject* parent = 0);
		Item(const Item& provider, QObject *parent = 0);
		virtual ~Item();

	private:

		QTimer				m_timer;
		bool				m_connect = false;

		quint32				m_id = INVALID_ID;
		QString				m_googleID;

		State				m_state;

		QString				m_activeTime;

		int					m_rank = 0;
		int					m_type = 0;

		QString				m_name;
		QString				m_address;
		QString				m_phone;
		QString				m_mail;
		QString				m_website;

		double				m_geoLat = 0;
		double				m_geoLng = 0;

		quint32				m_requestTime = 0;
		quint32				m_waitReplyTime = 0;

		Order::Base			m_orderBase;

	public:

		bool				isEmpty() const;
		void				clear();

		bool				isConnected() const { return m_connect; }
		void				setConnectState(bool connect);

		quint32				providerID() const { return m_id; }
		void				setProviderID(quint32 id) { m_id = id; }

		QString				googleID() const { return m_googleID; }
		void				setGoogleID(const QString& id) { m_googleID = id; }

		quint32				state() const { return m_state.flags; }
		void				setState(quint32 state) { m_state = state; }

		bool				isActive() const { return m_state.active; }
		void				setActive(bool active) { m_state.active = active; }

		bool				enableAcceptOrder() const { return m_state.acceptOrder; }
		void				setEnableAcceptOrder(bool enable) { m_state.acceptOrder = enable; }

		bool				enableDinner() const { return m_state.enableDinner; }
		void				setEnableDinner(bool enable) { m_state.enableDinner = enable; }

		QString				activeTime() const { return m_activeTime; }
		void				setActiveTime(const QString& time) { m_activeTime = time; }

		int					rank() const { return m_rank; }
		void				setRank(int rank) { m_rank = rank; }

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

		double				geoLat() const { return m_geoLat; }
		void				setGeoLat(double lat) { m_geoLat = lat; }

		double				geoLng() const { return m_geoLng; }
		void				setGeoLng(double lng) { m_geoLng = lng; }

		quint32				requestTime() const { return m_requestTime; }
		void				setRequestTime(quint32 ms) { m_requestTime = ms; }

		quint32				waitReplyTime() const { return m_waitReplyTime; }
		void				setWaitReplyTime(quint32 ms) { m_waitReplyTime = ms; }

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
		QMap<quint32, int>		m_providerIndexMap;

	public:

		void					clear();
		int						count() const;

		bool					isExist(quint32 providerID) const;

		int						append(const Item &provider);
		void					append(const QVector<Item>& list);

		void					remove(int index);
		void					remove(quint32 providerID);

		Item					provider(int index) const;
		Item*					providerPtr(int index);

		Item					provider(quint32 providerID) const;
		Item*					providerPtr(quint32 providerID);
		int						providerIndex(quint32 providerID);

		void					setProvider(int index, const Item& provider);

		Item*					data() const;

		Base&					operator=(const Base& from);

	signals:

		void					cfgXmlDataLoaded();
		void					providerConnectChanged(quint32 providerID, bool connect);


	public slots:

		bool					readFromXml(const QByteArray& cfgXmlData, int version);
		void					writeToXml(XmlWriteHelper& xml, int version);

		void					setProviderConnected(bool connect, quint32 providerID, quint32 wrapVersion);
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
