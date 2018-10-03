#ifndef ORDER_H
#define ORDER_H

#include <QHostAddress>
#include <QMutex>
#include <QDateTime>
#include <QTimer>
#include <QDebug>

#include "../lib/SocketIO.h"
#include "../lib/XmlHelper.h"

namespace Order
{
	// ==============================================================================================

	const int			STATE_UNDEFINED						= 0,
						STATE_ORDER_OK						= 1,
						STATE_ORDER_CANCEL					= 2,
						STATE_ORDER_PROCESSING				= 3,
						STATE_ORDER_NOT_FOUND				= 4,
						STATE_ORDER_ALREADY_EXIST			= 5,
						STATE_ORDER_NOT_REMOVED				= 6,
						STATE_INCORRECT_REQUEST				= 7,
						STATE_INCORRECT_PARSE_ORDERWRAP		= 8,
						STATE_INCORRECT_PARSE_PROTOWRAP		= 9,
						STATE_PROVIDER_IS_NOT_CONNECTED		= 10,
						STATE_CUSTOMER_CREATING_ORDER		= 11,
						STATE_CUSTOMER_REMOVING_ORDER		= 12,
						STATE_SERVER_CREATED_ORDER			= 13,
						STATE_SERVER_REMOVED_ORDER			= 14,
						STATE_SERVER_SEND_TO_PROVIDER		= 15;

	const int			STATE_COUNT							= 16;

	// ----------------------------------------------------------------------------------------------

	const int			TYPE_UNDEFINED						= 0,
						TYPE_TABLE							= 1,
						TYPE_DINNER							= 2;

	const int			TYPE_COUNT							= 3;

	// ==============================================================================================

	#pragma pack(push, 1)

	union Time32
	{
		Time32(quint32 t = 0)
		{
			time = t;
		}

		quint32 time = 0;

		struct
		{
			quint32 second : 6;
			quint32 minute : 6;
			quint32 hour : 5;

			quint32 day : 5;
			quint32 month : 4;
			quint32 year : 6;
		};

		void fillCurrentDateTime()
		{
			QDateTime ct = QDateTime::currentDateTime();

			year = ct.date().year() - 2000;
			month = ct.date().month();
			day = ct.date().day();

			hour = ct.time().hour();
			minute = ct.time().minute();
			second = ct.time().second();
		}

		void fillCurrentDate()
		{
			QDateTime ct = QDateTime::currentDateTime();

			year = ct.date().year() - 2000;
			month = ct.date().month();
			day = ct.date().day();
		}

		QString str()
		{
			return QString().sprintf("%04d-%02d-%02d %02d:%02d:%02d", year + 2000, month, day, hour, minute, second);
		}
	};

	#pragma pack(pop)


	// ==============================================================================================

	const quint64		EMPTY_ORDER_HANDLE					= 0xFFFFFFFF;

	// ----------------------------------------------------------------------------------------------

	#pragma pack(push, 1)

	union Handle
	{
		Handle(quint64 handle = EMPTY_ORDER_HANDLE)
		{
			ID = handle;
		}

		quint64 ID = EMPTY_ORDER_HANDLE;

		struct
		{
			quint64 providerID : 32;
			quint64 phone : 32;
		};
	};

	#pragma pack(pop)

	// ==============================================================================================

	class Item : public QObject
	{
		Q_OBJECT

	public:

		explicit Item(QObject *parent = 0);
		Item(const Item& order, QObject *parent = 0);
		Item(const sio_OrderWrap& wo, QObject *parent = 0);
		virtual ~Item();

	private:

		QHostAddress		m_address;
		int					m_port = 0;

		int					m_state = STATE_UNDEFINED;
		int					m_type = TYPE_UNDEFINED;

		Handle				m_handle = EMPTY_ORDER_HANDLE;

		Time32				m_orderTime = 0;
		QDateTime			m_removeTime;
		int					m_people = 0;

		int					m_cancelCode = 0;

	public:

		bool				isEmpty() const;
		bool				isValid() const;
		void				clear();

		QHostAddress		address() const { return m_address; }
		void				setAddress(const QHostAddress& address) { m_address = address; }

		int					port() const { return m_port; }
		void				setPort(int port) { m_port = port; }

		int					state() const;
		bool				setState(int state);

		int					type() const;
		QString				typeStr() const;
		bool				setType(int type);

		Handle				handle() const { return m_handle; }
		void				setHandle(const Handle& handle) { m_handle = handle; }

		quint32				providerID() const { return m_handle.providerID; }
		void				setProviderID(quint32 id) { m_handle.providerID = id; }

		quint32				phone() const { return m_handle.phone; }
		void				setPhone(quint32 phone) { m_handle.phone = phone; }

		Time32				orderTime() const { return m_orderTime.time; }
		void				setOrderTime(Time32 time) { m_orderTime = time; }
		void				setOrderTime(quint32 time) { m_orderTime.time = time; }

		QDateTime			removeTime() const { return m_removeTime; }
		void				setRemoveTime(const QDateTime& removeTime) { m_removeTime = removeTime; }
		void				setRemoveTime() { m_removeTime = QDateTime::currentDateTime().addSecs(MAX_SECONDS_ORDER_LIVE); }

		int					people() const { return m_people; }
		void				setPeople(int count) { m_people = count; }

		int					cancelCode() const { return m_cancelCode; }
		void				setCancelCode(int code) { m_cancelCode = code; }
		int					createCancelCode();

		sio_OrderWrap		toWrap() const;
		void				toWrap(sio_OrderWrap* pWo);
		void				fromWrap(const sio_OrderWrap& wo);
		void				fromWrap(sio_OrderWrap* pWo);

		bool				readFromXml(XmlReadHelper& xml, int version);
		void				writeToXml(XmlWriteHelper& xml, int version);

		Item&				operator=(const Item& from);
	};

	// ==============================================================================================

	class Base : public QObject
	{
		Q_OBJECT

	public:

		explicit Base(QObject *parent = 0);
		virtual ~Base();

	private:

		mutable QMutex		m_mutex;

		QMap<quint64, Item>	m_orderMap;
		QVector<Item>		m_orderForProviderList;

	public:

		void				clear();
		int					count() const;

		bool				isExist(quint64 orderID) const;

		bool				append(const Item &order);
		bool				remove(quint64 orderID);

		Item				order(quint64 orderID) const;
		Item*				orderPtr(quint64 orderID);
		QList<Item>			orderList() const;

		int					orderState(quint64 orderID) const;

		bool				readFromXml(const QString& fileName);
		void				writeToXml(const QString& fileName);

		Base&				operator=(const Base& from);

		Item				hasOrderForProvider(quint32 providerID);

	signals:

		void				signal_stateChanged(const Item& order);

	public slots:

		void				slot_appendOrder(const Item& order);
		void				slot_changeState(const Item& order);
		void				slot_removeOrder(const Item& order);
	};
}

// ==============================================================================================

Q_DECLARE_METATYPE(Order::Item)	// for type QVariant

// ==============================================================================================

extern Order::Base theOrderBase;

// ==============================================================================================

#endif // ORDER_H
