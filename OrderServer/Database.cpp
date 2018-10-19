#include "Database.h"

#include <QMessageBox>

#include "Options.h"

#include "../lib/wassert.h"
#include "../lib/Provider.h"

// -------------------------------------------------------------------------------------------------------------------

Database* thePtrDB = nullptr;

// -------------------------------------------------------------------------------------------------------------------
//
// SqlFieldBase class implementation
//
// -------------------------------------------------------------------------------------------------------------------

SqlFieldBase::SqlFieldBase()
{
}

// -------------------------------------------------------------------------------------------------------------------

SqlFieldBase::~SqlFieldBase()
{
}

// -------------------------------------------------------------------------------------------------------------------

int SqlFieldBase::init(int objectType, int)
{
	switch(objectType)
	{
		case SQL_TABLE_DATABASE_INFO:

			append("ID",							QVariant::Int);
			append("ObjectID",						QVariant::Int);
			append("Name",							QVariant::String, 256);
			append("Version",						QVariant::Int);

			break;

		case SQL_TABLE_HISTORY:

			append("ObjectID",						QVariant::Int);
			append("Version",						QVariant::Int);
			append("Event",							QVariant::String, 256);
			append("Time",							QVariant::String, 64);

			break;

		case SQL_TABLE_PROVIDER:

			append("ObjectID",						QVariant::Int);
			append("ProviderID",					QVariant::Int);
			append("GoogleID",						QVariant::String, 32);

			append("State",							QVariant::Int);
			append("ActiveTime",					QVariant::String, 32);

			append("Rank",							QVariant::Int);
			append("TypeID",						QVariant::Int);

			append("Name",							QVariant::String, 64);
			append("Address",						QVariant::String, 256);
			append("Phone",							QVariant::String, 16);

			append("Lat",							QVariant::Double);
			append("Lng",							QVariant::Double);

			break;

		case SQL_TABLE_PROVIDER_TYPE:

			append("ObjectID",						QVariant::Int);
			append("TypeID",						QVariant::Int);

			append("Name",							QVariant::String, 64);

			break;

		case SQL_TABLE_PROVIDER_WORK_TIME:

			append("ObjectID",						QVariant::Int);
			append("ProviderID",					QVariant::Int);

			append("Day0_From",						QVariant::Int);
			append("Day0_To",						QVariant::Int);

			append("Day1_From",						QVariant::Int);
			append("Day1_To",						QVariant::Int);

			append("Day2_From",						QVariant::Int);
			append("Day2_To",						QVariant::Int);

			append("Day3_From",						QVariant::Int);
			append("Day3_To",						QVariant::Int);

			append("Day4_From",						QVariant::Int);
			append("Day4_To",						QVariant::Int);

			append("Day5_From",						QVariant::Int);
			append("Day5_To",						QVariant::Int);

			append("Day6_From",						QVariant::Int);
			append("Day6_To",						QVariant::Int);

			break;

		case SQL_TABLE_PROVIDER_DINNER_TIME:

			append("ObjectID",						QVariant::Int);
			append("ProviderID",					QVariant::Int);

			append("Day0_From",						QVariant::Int);
			append("Day0_To",						QVariant::Int);

			append("Day1_From",						QVariant::Int);
			append("Day1_To",						QVariant::Int);

			append("Day2_From",						QVariant::Int);
			append("Day2_To",						QVariant::Int);

			append("Day3_From",						QVariant::Int);
			append("Day3_To",						QVariant::Int);

			append("Day4_From",						QVariant::Int);
			append("Day4_To",						QVariant::Int);

			append("Day5_From",						QVariant::Int);
			append("Day5_To",						QVariant::Int);

			append("Day6_From",						QVariant::Int);
			append("Day6_To",						QVariant::Int);

			break;

		case SQL_TABLE_CUSTOMER:

			append("ObjectID",						QVariant::Int);
			append("CustomerID",					QVariant::Int);

			append("Name",							QVariant::String, 64);
			append("Phone",							QVariant::String, 16);

			break;

		case SQL_TABLE_ORDER_HISTORY:

			append("ObjectID",						QVariant::Int);
			append("OrderID",						QVariant::Int);

			append("RegTime",						QVariant::String, 64);
			append("ProviderID",					QVariant::Int); // (ProviderID or Customer phone)
			append("CustomerPhone",					QVariant::Int);	// (ProviderID or Customer phone)

			append("OrderType",						QVariant::Int);
			append("OrderTime",						QVariant::String, 64);
			append("OrderPeople",					QVariant::Int);

			break;

		default:
			wassert(0);
			break;
	}

	int fieldCount = count();
	wassert(fieldCount);

	return fieldCount;
}

// -------------------------------------------------------------------------------------------------------------------


void SqlFieldBase::append(const QSqlField& field)
{
	QSqlRecord::append(field);
}

// -------------------------------------------------------------------------------------------------------------------

void SqlFieldBase::append(QString name, QVariant::Type type, int length)
{
	if (name.isEmpty() == true)
	{
		return;
	}

	if (type == QVariant::Invalid)
	{
		return;
	}

	QSqlField field(name, type);

	if (type == QVariant::Double)
	{
		field.setPrecision(6);
	}

	if (type == QVariant::String)
	{
		field.setLength(length);
	}

	append(field);
}

// -------------------------------------------------------------------------------------------------------------------

QString SqlFieldBase::extFieldName(int index)
{
	if (index < 0 || index >= count())
	{
		return QString();
	}

	QSqlField f = field(index);

	QString result;

	switch(f.type())
	{
		case QVariant::Bool:	result = QString("%1 BOOL").arg(f.name());									break;
		case QVariant::Int:		result = QString("%1 INTEGER").arg(f.name());								break;
		case QVariant::Double:	result = QString("%1 DECIMAL(10, %2)").arg(f.name()).arg(f.precision());	break;
		case QVariant::String:	result = QString("%1 VARCHAR(%2)").arg(f.name()).arg(f.length());			break;
		default:				result.clear();
	}

	return result;
}

// -------------------------------------------------------------------------------------------------------------------
//
// SqlObjectInfo class implementation
//
// -------------------------------------------------------------------------------------------------------------------

SqlObjectInfo::SqlObjectInfo()
{
}

// -------------------------------------------------------------------------------------------------------------------

SqlObjectInfo::~SqlObjectInfo()
{
}

// -------------------------------------------------------------------------------------------------------------------

bool SqlObjectInfo::init(int objectType)
{
	if (objectType < 0 || objectType >= SQL_TABLE_COUNT)
	{
		return false;
	}

	m_objectType = objectType;
	m_objectID = SqlObjectID[objectType];
	m_caption = SqlTabletName[objectType];
	m_version = SqlTableVersion[objectType];

	return true;
}

// -------------------------------------------------------------------------------------------------------------------

void SqlObjectInfo::clear()
{
	m_objectType = SQL_TABLE_UNKNONW;
	m_objectID = SQL_OBJECT_ID_UNKNONW;
	m_caption.clear();
	m_version = SQL_TABLE_VER_UNKNONW;
}

// -------------------------------------------------------------------------------------------------------------------

SqlObjectInfo& SqlObjectInfo::operator=(SqlObjectInfo& from)
{
	m_objectType = from.m_objectType;
	m_objectID = from.m_objectID;
	m_caption = from.m_caption;
	m_version = from.m_version;

	return *this;
}

// -------------------------------------------------------------------------------------------------------------------
//
// SqlHistoryDatabase class implementation
//
// -------------------------------------------------------------------------------------------------------------------

SqlHistoryDatabase::SqlHistoryDatabase()
{
}

// -------------------------------------------------------------------------------------------------------------------

SqlHistoryDatabase::~SqlHistoryDatabase()
{
}

// -------------------------------------------------------------------------------------------------------------------

SqlHistoryDatabase::SqlHistoryDatabase(int objectID, int version, const QString& event,  const QString& time)
{
	m_objectID = objectID;
	m_version = version;
	m_event = event;
	m_time = time;
}

// -------------------------------------------------------------------------------------------------------------------

SqlHistoryDatabase& SqlHistoryDatabase::operator=(SqlHistoryDatabase& from)
{
	m_objectID = from.m_objectID;
	m_version = from.m_version;
	m_event = from.m_event;
	m_time = from.m_time;

	return *this;
}

// -------------------------------------------------------------------------------------------------------------------
//
// SqlTable class implementation
//
// -------------------------------------------------------------------------------------------------------------------

SqlTable::SqlTable()
{
}

// -------------------------------------------------------------------------------------------------------------------

SqlTable::~SqlTable()
{
}

// -------------------------------------------------------------------------------------------------------------------

int SqlTable::recordCount() const
{
	if (isOpen() == false)
	{
		return 0;
	}

	int type = m_info.objectType();
	if (type < 0 || type >= SQL_TABLE_COUNT)
	{
		return 0;
	}

	QSqlQuery query(QString("SELECT count(*) FROM %1").arg(m_info.caption()));
	if (query.next() == false)
	{
		return 0;
	}

	return query.value(0).toInt();
}

// -------------------------------------------------------------------------------------------------------------------

int SqlTable::lastKey() const
{
	if (isOpen() == false)
	{
		return SQL_INVALID_KEY;
	}

	int type = m_info.objectType();
	if (type < 0 || type >= SQL_TABLE_COUNT)
	{
		return SQL_INVALID_KEY;
	}

	QSqlQuery query(QString("SELECT max(%1) FROM %2").arg(m_fieldBase.field(SQL_FIELD_KEY).name()).arg(m_info.caption()));
	if (query.next() == false)
	{
		return SQL_INVALID_KEY;
	}

	return query.value(0).toInt();
}

// -------------------------------------------------------------------------------------------------------------------

bool SqlTable::init(int objectType, QSqlDatabase* pDatabase)
{
	if (objectType < 0 || objectType >= SQL_TABLE_COUNT)
	{
		return false;
	}

	if (pDatabase == nullptr)
	{
		return false;
	}

	if (m_info.init(objectType) == false)
	{
		return false;
	}

	m_pDatabase = pDatabase;

	return true;
}

// -------------------------------------------------------------------------------------------------------------------

bool SqlTable::isExist() const
{
	if (m_pDatabase == nullptr)
	{
		return false;
	}

	if (m_pDatabase->isOpen() == false)
	{
		return false;
	}

	int type = m_info.objectType();
	if (type < 0 || type >= SQL_TABLE_COUNT)
	{
		return false;
	}

	bool tableIsExist = false;

	int existTableCount = m_pDatabase->tables().count();
	for(int et = 0; et < existTableCount; et++)
	{
		if (m_pDatabase->tables().at(et).compare(SqlTabletName[type], Qt::CaseInsensitive) == 0)
		{
			tableIsExist = true;
			break;
		}
	}

	return tableIsExist;
}


// -------------------------------------------------------------------------------------------------------------------

bool SqlTable::open()
{
	if (isExist() == false)
	{
		return false;
	}

	if (m_fieldBase.init(m_info.objectType(), m_info.version()) == 0)
	{
		return false;
	}

	return true;
}

// -------------------------------------------------------------------------------------------------------------------

void SqlTable::close()
{
	m_fieldBase.clear();
}

// -------------------------------------------------------------------------------------------------------------------

bool SqlTable::create()
{
	if (isExist() == true)
	{
		return false;
	}

	if (m_fieldBase.init(m_info.objectType(), m_info.version()) == 0)
	{
		return false;
	}

	QSqlQuery query;

	QString request = QString("CREATE TABLE if not exists %1 (").arg(m_info.caption());

	int filedCount = m_fieldBase.count();
	for(int field = 0; field < filedCount; field++)
	{
		request.append(m_fieldBase.extFieldName(field));

		if (field == SQL_FIELD_KEY)
		{
			request.append(" PRIMARY KEY NOT NULL");
		}

		if (field != filedCount - 1)
		{
			request.append(", ");
		}
	}

	// create links
	//
	switch(m_info.objectType())
	{
		case SQL_TABLE_PROVIDER_WORK_TIME:
		case SQL_TABLE_PROVIDER_DINNER_TIME:

			request.append(QString(", FOREIGN KEY (ProviderID) REFERENCES %1(ProviderID) ON DELETE CASCADE").arg(SqlTabletName[SQL_TABLE_PROVIDER]));

			break;
	}

	request.append(");");

	qDebug() << request;

	return query.exec(request);
}

// -------------------------------------------------------------------------------------------------------------------

bool SqlTable::drop()
{
	QSqlQuery query;
	if (query.exec(QString("DROP TABLE %1").arg(m_info.caption())) == false)
	{
		return false;
	}

	close();

	return true;
}

// -------------------------------------------------------------------------------------------------------------------

bool SqlTable::clear()
{
	if (isOpen() == false)
	{
		return false;
	}

	QSqlQuery query;

	if (query.exec("BEGIN TRANSACTION") == false)
	{
		return false;
	}

	if (query.exec(QString("DELETE FROM %1").arg(m_info.caption())) == false)
	{
		return false;
	}

	if (query.exec("COMMIT") == false)
	{
		return false;
	}

	return true;
}

// -------------------------------------------------------------------------------------------------------------------

int SqlTable::read(void* pRecord, int* key, int keyCount)
{
	if (isOpen() == false)
	{
		return 0;
	}

	if (pRecord == nullptr)
	{
		return 0;
	}

	// create request
	//
	QString request = QString("SELECT * FROM %1").arg(m_info.caption());

	if (key != nullptr && keyCount != 0)
	{
		request.append(" WHERE ");
		QString keyFieldName = m_fieldBase.field(SQL_FIELD_KEY).name();

		for(int k = 0; k < keyCount; k++)
		{
			request.append(QString("%1=%2").arg(keyFieldName).arg(key[k]));

			if (k != keyCount - 1)
			{
				request.append(" OR ");
			}
		}
	}

	request.append(QString(" ORDER By %1").arg(m_fieldBase.field(SQL_FIELD_KEY).name()));

	qDebug() << request;

	// exec select
	//
	QSqlQuery query;
	if (query.exec(request) == false)
	{
		return 0;
	}

	int field = 0;
	int objectID = SQL_OBJECT_ID_UNKNONW;
	int readedCount = 0;

	// read data
	//
	while (query.next() == true)
	{
		field = 0;

		// check unique ID of table or view, zero field always is ObjectID
		//
		objectID = query.value(field++).toInt();
		if (objectID != m_info.objectID())
		{
			continue;
		}

		// read field's data
		//
		switch(m_info.objectType())
		{
			case SQL_TABLE_DATABASE_INFO:
				{
					SqlObjectInfo* info = static_cast<SqlObjectInfo*> (pRecord) + readedCount;

					info->setObjectID(query.value(field++).toInt());
					info->setCaption(query.value(field++).toString());
					info->setVersion(query.value(field++).toInt());
				}
				break;

			case SQL_TABLE_HISTORY:
				{
					SqlHistoryDatabase* history = static_cast<SqlHistoryDatabase*> (pRecord) + readedCount;

					history->setObjectID(objectID);
					history->setVersion(query.value(field++).toInt());
					history->setEvent(query.value(field++).toString());
					history->setTime(query.value(field++).toString());
				}
				break;

			case SQL_TABLE_PROVIDER:
				{
					Provider::Item* provider = static_cast<Provider::Item*> (pRecord) + readedCount;

					provider->setProviderID(query.value(field++).toInt());
					provider->setGoogleID(query.value(field++).toString());

					provider->setState(query.value(field++).toInt());
					provider->setActiveTime(query.value(field++).toString());

					provider->setRank(query.value(field++).toInt());
					provider->setType(query.value(field++).toInt());

					provider->setName(query.value(field++).toString());
					provider->setAddress(query.value(field++).toString());
					provider->setPhone(query.value(field++).toString());

					provider->setGeoLat(query.value(field++).toDouble());
					provider->setGeoLng(query.value(field++).toDouble());
				}
				break;

			case SQL_TABLE_PROVIDER_TYPE:
				{
					Provider::Type* type = static_cast<Provider::Type*> (pRecord) + readedCount;

					type->setTypeID(query.value(field++).toInt());
					type->setName(query.value(field++).toString());
				}
				break;

			default:
				wassert(0);
				break;
		}

		readedCount ++;
	}

	return readedCount;
}

// -------------------------------------------------------------------------------------------------------------------

int SqlTable::write(void* pRecord, int count, int* key)
{
	if (isOpen() == false)
	{
		return 0;
	}

	if (pRecord == nullptr)
	{
		return 0;
	}

	if (count == 0)
	{
		return 0;
	}

	// create request
	//

	QString request;

	if (key == nullptr)
	{
		request = QString("INSERT INTO %1 (").arg(m_info.caption());

		int filedCount = m_fieldBase.count();
		for(int f = 0; f < filedCount; f++)
		{
			request.append(m_fieldBase.field(f).name());

			if (f != filedCount - 1)
			{
				request.append(", ");
			}
		}

		request.append(") VALUES (");

		for(int f = 0; f < filedCount; f++)
		{
			request.append("?");

			if (f != filedCount - 1)
			{
				request.append(", ");
			}
		}

		request.append(");");
	}
	else
	{
		request = QString("UPDATE %1 SET ").arg(m_info.caption());

		int filedCount = m_fieldBase.count();
		for(int f = 0; f < filedCount; f++)
		{
			request.append(QString("%1=?").arg(m_fieldBase.field(f).name()));

			if (f != filedCount - 1)
			{
				request.append(", ");
			}
		}

		request.append(QString(" WHERE %1=").arg(m_fieldBase.field(SQL_FIELD_KEY).name()));
	}

	int field = 0;
	int writedCount = 0;

	QSqlQuery query;

	for (int r = 0; r < count; r++)
	{
		if (query.prepare(key == nullptr ? request :  request + QString("%1").arg(key[r])) == false)
		{
			continue;
		}

		field = 0;

		query.bindValue(field++, m_info.objectID());

		switch(m_info.objectType())
		{
			case SQL_TABLE_DATABASE_INFO:
				{
					SqlObjectInfo* info = static_cast<SqlObjectInfo*> (pRecord) + r;

					query.bindValue(field++, info->objectID());
					query.bindValue(field++, info->caption());
					query.bindValue(field++, info->version());
				}
				break;

			case SQL_TABLE_HISTORY:
				{
					SqlHistoryDatabase* history = static_cast<SqlHistoryDatabase*> (pRecord) + r;

					query.bindValue(field++, history->version());
					query.bindValue(field++, history->event());
					query.bindValue(field++, history->time());
				}
				break;

			case SQL_TABLE_PROVIDER:
				{
					Provider::Item* provider = static_cast<Provider::Item*> (pRecord) + r;

					if (key == nullptr) // for insert
					{
						provider->setProviderID(lastKey() + 1);
					}

					query.bindValue(field++, provider->providerID());
					query.bindValue(field++, provider->googleID());

					query.bindValue(field++, provider->state());
					query.bindValue(field++, provider->activeTime());

					query.bindValue(field++, provider->rank());
					query.bindValue(field++, provider->type());

					query.bindValue(field++, provider->name());
					query.bindValue(field++, provider->address());
					query.bindValue(field++, provider->phone());

					query.bindValue(field++, provider->geoLat());
					query.bindValue(field++, provider->geoLng());
				}
				break;

			case SQL_TABLE_PROVIDER_TYPE:
				{
					Provider::Type* type = static_cast<Provider::Type*> (pRecord) + r;

					if (key == nullptr)	// for insert
					{
						type->setTypeID(lastKey() + 1);
					}

					query.bindValue(field++, type->typeID());
					query.bindValue(field++, type->name());
				}
				break;

			default:
				wassert(0);
				break;
		}

		if (query.exec() == false)
		{
			continue;
		}

		writedCount ++;
	}

	return writedCount;
}

// -------------------------------------------------------------------------------------------------------------------

int SqlTable::remove(const int* key, int keyCount) const
{
	if (isOpen() == false)
	{
		return 0;
	}

	if (key == nullptr || keyCount == 0)
	{
		return 0;
	}

	QString keyFieldName = m_fieldBase.field(SQL_FIELD_KEY).name();
	if (keyFieldName.isEmpty() == true)
	{
		return 0;
	}

	int count = recordCount();
	if (count == 0)
	{
		return 0;
	}

	int transactionCount = keyCount / REMOVE_TRANSACTION_RECORD_COUNT;

	if (keyCount % REMOVE_TRANSACTION_RECORD_COUNT != 0)
	{
		transactionCount++;
	}

	int record = 0;

	for (int t = 0; t < transactionCount; t++)
	{
		QString request = QString("DELETE FROM %1 WHERE ").arg(m_info.caption());

		for (int k = 0; k < REMOVE_TRANSACTION_RECORD_COUNT; k++)
		{
			request.append(QString("%1=%2").arg(keyFieldName).arg(key[record++]));

			if (record >= keyCount )
			{
				break;
			}

			if (k != REMOVE_TRANSACTION_RECORD_COUNT - 1)
			{
				request.append(" OR ");
			}
		}

		QSqlQuery query;

		if (query.exec("BEGIN TRANSACTION") == false)
		{
			return 0;
		}

		if (query.exec(request) == false)
		{
			query.exec("END TRANSACTION");

			return 0;
		}

		if (query.exec("COMMIT") == false)
		{
			return 0;
		}
	}

	return count - recordCount();
}

// -------------------------------------------------------------------------------------------------------------------

SqlTable& SqlTable::operator=(SqlTable& from)
{
	m_pDatabase = from.m_pDatabase;
	m_info = from.m_info;
	m_fieldBase = from.m_fieldBase;

	return *this;
}

// -------------------------------------------------------------------------------------------------------------------
//
// Database class implementation
//
// -------------------------------------------------------------------------------------------------------------------

SqlHistoryDatabase Database::m_history[] =
{
	SqlHistoryDatabase(SQL_TABLE_HISTORY, 1, "Create database", "01-09-2018 08:00:00"),
};

// -------------------------------------------------------------------------------------------------------------------

Database::Database(QObject* parent) :
	QObject(parent)
{
	for(int type = 0; type < SQL_TABLE_COUNT; type++)
	{
		m_table[type].init(type, &m_database);
	}
}

// -------------------------------------------------------------------------------------------------------------------

Database::~Database()
{
	close();
}

// -------------------------------------------------------------------------------------------------------------------

bool Database::open()
{
	QString path = theOptions.database().path();
	if (path.isEmpty() == true)
	{
		QMessageBox::critical(nullptr, tr("Database"), tr("Invalid path!"));
		return false;
	}

	switch(theOptions.database().type())
	{
		case DATABASE_TYPE_SQLITE:
			{
				m_database = QSqlDatabase::addDatabase("QSQLITE");
				if (m_database.lastError().isValid() == true)
				{
					return false;
				}

				m_database.setDatabaseName(path + QDir::separator() + DATABASE_NAME + ".db");


				if (m_database.open() == false)
				{
					QMessageBox::critical(nullptr, tr("Database"), tr("Cannot open database"));
					return false;
				}

				QSqlQuery query;

				if (query.exec("PRAGMA foreign_keys=on") == false)
				{
					QMessageBox::critical(nullptr, tr("Database"), tr("Error set option of database: [foreign keys=on]"));
				}

				if (query.exec("PRAGMA synchronous=normal") == false)
				{
					QMessageBox::critical(nullptr, tr("Database"), tr("Error set option of database: [synchronous=normal]"));
				}
			}
			break;

		case DATABASE_TYPE_PSQL:
			{
				m_database = QSqlDatabase::addDatabase("QPSQL");
				if (m_database.lastError().isValid() == true)
				{
					return false;
				}

				m_database.setHostName("127.0.0.1");
				m_database.setPort(5432);
				m_database.setDatabaseName(DATABASE_NAME);
				m_database.setUserName(DATABASE_USER_ADMIN);
				m_database.setPassword(DATABASE_USER_ADMIN_PASSWD);

				if (m_database.open() == false)
				{
					QMessageBox::critical(nullptr, tr("Database"), tr("Cannot open database"));
					return false;
				}
			}

			break;

		default:
			wassert(0);
			break;
	}



	initVersion();
	createTables();

	return true;
}

// -------------------------------------------------------------------------------------------------------------------

void Database::close()
{
	for(int type = 0; type < SQL_TABLE_COUNT; type++)
	{
		if (m_table[type].isOpen() == true)
		{
			m_table[type].close();
		}

		m_table[type].info().clear();
	}

	if (m_database.isOpen() == true)
	{
		m_database.close();
	}
}

// -------------------------------------------------------------------------------------------------------------------

SqlTable* Database::openTable(int objectType)
{
	if (objectType < 0 || objectType >= SQL_TABLE_COUNT)
	{
		return nullptr;
	}

	if (m_table[objectType].isOpen() == true)
	{
		return nullptr;
	}

	if (m_table[objectType].open() == false)
	{
		return nullptr;
	}

	return &m_table[objectType];
}

// -------------------------------------------------------------------------------------------------------------------

void Database::initVersion()
{
	SqlTable table;
	if (table.init(SQL_TABLE_DATABASE_INFO, &m_database) == false)
	{
		return;
	}

	QVector<SqlObjectInfo> info;

	if (table.isExist() == false)
	{
		if (table.create() == true)
		{
			info.resize(SQL_TABLE_COUNT);

			for(int t = 0; t < SQL_TABLE_COUNT; t++)
			{
				info[t] = m_table[t].info();
			}

			table.write(info.data(), info.count());
		}
	}
	else
	{
		if (table.open() == true)
		{
			info.resize(table.recordCount());

			int count = table.read(info.data());
			for (int i = 0; i < count; i++)
			{
				for(int t = 0; t < SQL_TABLE_COUNT; t++)
				{
					if (m_table[t].info().objectID() == info[i].objectID())
					{
						m_table[t].info().setVersion(info[i].version());
						break;
					}
				}
			}
		}
	}

	table.close();
}

// -------------------------------------------------------------------------------------------------------------------

void Database::createTables()
{
	// find table in database, if table is not exist, then create it
	//
	for(int type = 0; type < SQL_TABLE_COUNT; type++)
	{
		SqlTable table;

		if (table.init(type, &m_database) == true)
		{
			if (table.isExist() == false)
			{
				if (table.create() == false)
				{
					QMessageBox::critical(nullptr, tr("Database"), tr("Cannot create table: %1").arg(table.info().caption()));
				}
			}
		}
	}
}

// -------------------------------------------------------------------------------------------------------------------

bool Database::appendRecord(void* pRecord)
{
	if (pRecord == nullptr)
	{
		return false;
	}

	bool result = false;

	for (int type = 0; type < SQL_TABLE_COUNT; type++)
	{
		SqlTable& table = m_table[type];

		if (table.open() == true)
		{
			if (table.write(pRecord) == 1)
			{
				result = true;
			}

			table.close();
		}
	}

	return result;
}

// -------------------------------------------------------------------------------------------------------------------

bool Database::removeRecord(const QVector<int>& keyList)
{
	bool result = false;

	for (int type = 0; type < SQL_TABLE_COUNT; type++)
	{
		SqlTable& table = m_table[type];

		if (table.open() == true)
		{
			if (table.remove(keyList.data(), keyList.count()) == keyList.count())
			{
				result = true;
			}

			table.close();
		}

		break;
	}

	return result;
}

// -------------------------------------------------------------------------------------------------------------------


