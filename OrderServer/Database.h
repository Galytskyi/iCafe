#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QtSql>
#include <QMutex>

// ==============================================================================================

#define					DATABASE_NAME				"icafe"
#define					DATABASE_VERSION			1

// ==============================================================================================

#define					DATABASE_USER_ADMIN			"bra"
#define					DATABASE_USER_ADMIN_PASSWD	"br20082018"

#define					DATABASE_USER				"bru"

// ==============================================================================================
//
// Is a list of fields SQL tables.
//
class SqlFieldBase : public QSqlRecord
{
public:

	SqlFieldBase();
	virtual ~SqlFieldBase();

public:

	int					init(int objectType, int version);

	void				append(const QSqlField& field);
	void				append(QString name, QVariant::Type type = QVariant::Invalid, int length = 0);

	QString				extFieldName(int index);
};

// ==============================================================================================

const char* const		SqlTabletName[] =
{
						QT_TRANSLATE_NOOP("Database.h", "DatabaseInfo"),
						QT_TRANSLATE_NOOP("Database.h", "History"),

						QT_TRANSLATE_NOOP("Database.h", "Provider"),
						QT_TRANSLATE_NOOP("Database.h", "ProviderType"),
						QT_TRANSLATE_NOOP("Database.h", "ProviderWorkTime"),
						QT_TRANSLATE_NOOP("Database.h", "ProviderDinnerTime"),

						QT_TRANSLATE_NOOP("Database.h", "Customer"),

						QT_TRANSLATE_NOOP("Database.h", "OrderHistory"),
};

const int				SQL_TABLE_COUNT							= sizeof(SqlTabletName)/sizeof(SqlTabletName[0]);

const int				SQL_TABLE_UNKNONW						= -1,
						SQL_TABLE_DATABASE_INFO					= 0,
						SQL_TABLE_HISTORY						= 1,
						SQL_TABLE_PROVIDER						= 2,
						SQL_TABLE_PROVIDER_TYPE					= 3,
						SQL_TABLE_PROVIDER_WORK_TIME			= 4,
						SQL_TABLE_PROVIDER_DINNER_TIME			= 5,
						SQL_TABLE_CUSTOMER						= 6,
						SQL_TABLE_ORDER_HISTORY					= 7;

// ==============================================================================================

const int				SQL_TABLE_VER_UNKNONW = -1;

// ----------------------------------------------------------------------------------------------
//
// current versions of tables or views
//
const int				SqlTableVersion[SQL_TABLE_COUNT] =
{
						DATABASE_VERSION,	//	SQL_TABLE_DATABASE_INFO
						1,					//	SQL_TABLE_HISTORY

						1,					//	SQL_TABLE_PROVIDER
						1,					//	SQL_TABLE_PROVIDER_TYPE
						1,					//	SQL_TABLE_PROVIDER_WORK_TIME
						1,					//	SQL_TABLE_PROVIDER_DINNER_TIME

						1,					//	SQL_TABLE_CUSTOMER

						1,					//	SQL_TABLE_ORDER_HISTORY
};

// ==============================================================================================

const int				SQL_OBJECT_ID_UNKNONW = -1;

// ----------------------------------------------------------------------------------------------
//
// unique object ID in the database
// x00 - providers, x10 - points, x20 - signals, , x30 - racks
//
const int				SqlObjectID[SQL_TABLE_COUNT] =
{
						0,			//	SQL_TABLE_DATABASE_INFO
						1,			//	SQL_TABLE_HISTORY

						100,		//	SQL_TABLE_PROVIDER
						101,		//	SQL_TABLE_PROVIDER_TYPE
						102,		//	SQL_TABLE_PROVIDER_WORK_TIME
						103,		//	SQL_TABLE_PROVIDER_DINNER_TIME

						200,		//	SQL_TABLE_CUSTOMER

						300,		//	SQL_TABLE_ORDER_HISTORY

};

// ==============================================================================================

const int				SQL_FIELD_OBJECT_ID = 0;			// zero column is unique identifier of the table (or other object) in the database
const int				SQL_FIELD_KEY		= 1;			// first column is key in the table, for example:: RecordID, PointID, SignalID, ReportID и т.д.

// ==============================================================================================

const int				SQL_INVALID_INDEX	= -1;
const int				SQL_INVALID_KEY		= -1;
const int				SQL_INVALID_RECORD	= -1;

// ==============================================================================================
//
// Represents the structure determines the version of the object (tables, databases, etc.) in the database
//
class SqlObjectInfo
{
public:

	SqlObjectInfo();
	virtual ~SqlObjectInfo();

private:

	int					m_objectType = SQL_TABLE_UNKNONW;			// type of table
	int					m_objectID = SQL_OBJECT_ID_UNKNONW;			// unique identifier of table in the database
	QString				m_caption;									// caption of table
	int					m_version = SQL_TABLE_VER_UNKNONW;			// table version, is read when the database initialization

public:

	bool				init(int objectType);
	void				clear();

	int					objectType() const { return m_objectType; }
	void				setObjectType(int type) { m_objectType = type; }

	int					objectID() const { return m_objectID; }
	void				setObjectID(int objectID) { m_objectID = objectID; }

	QString				caption() const { return m_caption; }
	void				setCaption(const QString& caption) { m_caption = caption; }

	int					version() const { return m_version; }
	void				setVersion(int verison) { m_version = verison; }

	SqlObjectInfo&		operator=(SqlObjectInfo& from);
};

// ==============================================================================================

class SqlHistoryDatabase
{

public:

	SqlHistoryDatabase();
	SqlHistoryDatabase(int objectID, int version, const QString& event, const QString& time);
	virtual ~SqlHistoryDatabase();

private:

	int					m_objectID = SQL_OBJECT_ID_UNKNONW;
	int					m_version = SQL_TABLE_VER_UNKNONW;
	QString				m_event;
	QString				m_time;

public:

	int					objectID() const { return m_objectID; }
	void				setObjectID(int objectID) { m_objectID = objectID; }

	int					version() const { return m_version; }
	void				setVersion(int verison) { m_version = verison; }

	QString				event() const { return m_event; }
	void				setEvent(const QString& event) { m_event = event; }

	QString				time() const { return m_time; }
	void				setTime(const QString& time) { m_time = time; }

	SqlHistoryDatabase& operator=(SqlHistoryDatabase& from);
};

// ==============================================================================================

const int				REMOVE_TRANSACTION_RECORD_COUNT = 500;

// ----------------------------------------------------------------------------------------------

class SqlTable
{
public:

	SqlTable();
	virtual ~SqlTable();

private:

	QSqlDatabase*		m_pDatabase;
	SqlObjectInfo		m_info;
	SqlFieldBase		m_fieldBase;

public:

	SqlObjectInfo&		info() { return m_info; }
	void				setInfo(SqlObjectInfo info) { m_info = info; }

	bool				isEmpty() { return recordCount() == 0; }
	int					recordCount() const;
	int					lastKey() const;

	bool				init(int objectType, QSqlDatabase* pDatabase);

	bool				isExist() const;
	bool				isOpen() const { return m_fieldBase.count() != 0; }
	bool				open();
	void				close();

	bool				create();
	bool				drop();
	bool				clear();

	int					read(void* pRecord, int key) { return read(pRecord, &key, 1); }
	int					read(void* pRecord, int* key = nullptr, int keyCount = 0);			// read record form table, if key == nullptr in the array pRecord will be record all records of table

	int					write(void* pRecord) { return write(pRecord, 1); }
	int					write(void* pRecord, int count, int key) { return write(pRecord, count, &key); }
	int					write(void* pRecord, int recordCount, int* key = nullptr);			// insert or update records (depend from key) in a table, pRecord - array of record, count - amount records

	int					remove(int key) { return remove(&key, 1); }							// remove records by key
	int					remove(const int* key, int keyCount) const;

	SqlTable&			operator=(SqlTable& from);
};

// ==============================================================================================

class Database : public QObject
{
	Q_OBJECT

public:

	explicit Database(QObject* parent = 0);
	virtual ~Database();

private:

	QSqlDatabase		m_database;
	SqlTable			m_table[SQL_TABLE_COUNT];

	static SqlHistoryDatabase m_history[DATABASE_VERSION + 1];

	void				initVersion();
	void				createTables();

public:

	bool				isOpen() const { return m_database.isOpen(); }
	bool				open();
	void				close();

	SqlTable*			openTable(int objectType);

	bool				appendRecord(void* pRecord);
	bool				removeRecord(const QVector<int>& keyList);
};

// ==============================================================================================

extern Database*		thePtrDB;

// ==============================================================================================

#endif // DATABASE_H
