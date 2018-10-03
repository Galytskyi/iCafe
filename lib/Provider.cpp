#include "Provider.h"

#include <assert.h>

#include "../lib/SocketIO.h"

// -------------------------------------------------------------------------------------------------------------------

Provider::Base theProviderBase;
Provider::TypeBase theProviderTypeBase;

// -------------------------------------------------------------------------------------------------------------------

namespace Provider
{
	// -------------------------------------------------------------------------------------------------------------------
	//
	// ProviderItem class implementation
	//
	// -------------------------------------------------------------------------------------------------------------------

	Item::Item(QObject *parent)
		: QObject(parent)
		, m_timer(this)
	{
		clear();

		m_timer.setSingleShot(true);
		connect(&m_timer, &QTimer::timeout, this, &Item::onTimeoutConnect);	// if we did not get connect
	}

	// -------------------------------------------------------------------------------------------------------------------

	Item::Item(const Item& provider, QObject *parent) :
		QObject(parent)
		, m_timer(this)
	{
		*this = provider;

		m_timer.setSingleShot(true);
		connect(&m_timer, &QTimer::timeout, this, &Item::onTimeoutConnect);	// if we did not get an connect
	}

	// -------------------------------------------------------------------------------------------------------------------

	Item::~Item()
	{
		m_timer.stop();
		disconnect(&m_timer, &QTimer::timeout, this, &Item::onTimeoutConnect);
	}

	// -------------------------------------------------------------------------------------------------------------------

	bool Item::isEmpty() const
	{
		if (m_id == INVALID_ID)
		{
			return true;
		}

		if (m_name.isEmpty() == true)
		{
			return true;
		}

		return false;
	}

	// -------------------------------------------------------------------------------------------------------------------

	void Item::clear()
	{
		m_id = INVALID_ID;
		m_googleID.clear();

		m_active = false;
		m_activeTime.clear();

		m_type = 0;

		m_name.clear();
		m_address.clear();
		m_phone.clear();
		m_mail.clear();
		m_website.clear();

		m_connect = false;
	}

	// -------------------------------------------------------------------------------------------------------------------

	void Item::setConnectState(bool connect)
	{
		if (connect == true)
		{
			m_timer.start(TIMER_WAIT_CONNECT);

			if (m_connect == false) // if old state == false
			{
				emit providerConnected(m_id);

				qDebug() << "Provider Connected : " << m_id;
			}
		}
		else
		{
			if (m_connect == true) // if old state == true
			{
				emit providerDisconnected(m_id);

				qDebug() << "Provider Disconnected : " << m_id;
			}
		}

		m_connect = connect;
	}

	// -------------------------------------------------------------------------------------------------------------------

	bool Item::readFromXml(XmlReadHelper& xml, int version)
	{
		bool result = true;

		switch(version)
		{
			case 1:

				result &= xml.readUInt32Attribute("ProviderID", &m_id);

				result &= xml.readIntAttribute("Type", &m_type);

				result &= xml.readStringAttribute("Name", &m_name);
				result &= xml.readStringAttribute("Address", &m_address);
				result &= xml.readStringAttribute("Telephone", &m_phone);

				break;

			default:

				qDebug() << "ProviderItem::readFromXml - Undefined version:" << version;
				assert(0);

				break;
		}

		return result;
	}

	// -------------------------------------------------------------------------------------------------------------------

	void Item::writeToXml(XmlWriteHelper& xml, int version)
	{
		switch(version)
		{
			case 1:

				xml.writeStartElement(xmlTagProvider);
				{
					xml.writeUInt32Attribute("ProviderID", providerID(), false);

					xml.writeIntAttribute("Type", type());

					xml.writeStringAttribute("Name", name());
					xml.writeStringAttribute("Address", address());
					xml.writeStringAttribute("Telephone", phone());
				}

				xml.writeEndElement();

			break;

		default:

			qDebug() << "ProviderItem::readFromXml - Undefined version:" << version;
			assert(0);

			break;
		}
	}

	// -------------------------------------------------------------------------------------------------------------------

	Item& Item::operator=(const Item& from)
	{
		m_id = from.m_id;
		m_googleID = from.m_googleID;

		m_active = from.m_active;
		m_activeTime = from.m_activeTime;

		m_type = from.m_type;

		m_name = from.m_name;
		m_address = from.m_address;
		m_phone = from.m_phone;
		m_mail = from.m_mail;
		m_website = from.m_website;

		m_connect = from.m_connect;

		return *this;
	}

	// -------------------------------------------------------------------------------------------------------------------
	//
	// ProviderBase class implementation
	//
	// -------------------------------------------------------------------------------------------------------------------

	Base::Base(QObject *parent) :
		QObject(parent)
	{
	}


	// -------------------------------------------------------------------------------------------------------------------

	Base::~Base()
	{
	}

	// -------------------------------------------------------------------------------------------------------------------

	void Base::clear()
	{
		QMutexLocker locker(&m_mutex);

		m_providerList.clear();
	}

	// -------------------------------------------------------------------------------------------------------------------

	int Base::count() const
	{
		QMutexLocker locker(&m_mutex);

		return m_providerList.count();
	}

	// -------------------------------------------------------------------------------------------------------------------

	bool Base::isExist(quint32 providerID) const
	{
		QMutexLocker locker(&m_mutex);

		return m_providerIndexMap.contains(providerID);
	}

	// -------------------------------------------------------------------------------------------------------------------

	int Base::append(const Item& provider)
	{
		QMutexLocker locker(&m_mutex);

		if (m_providerIndexMap.contains(provider.providerID()) == true)
		{
			return -1;
		}

		m_providerList.append(provider);
		int index = m_providerList.count() - 1;

		m_providerIndexMap.insert(provider.providerID(), index);

		return index;
	}

	// -------------------------------------------------------------------------------------------------------------------

	void Base::append(const QVector<Item>& list)
	{
		QMutexLocker locker(&m_mutex);

		int count = list.count();
		for(int index = 0; index < count; index++)
		{
			Item provider = list[index];

			if (m_providerIndexMap.contains(provider.providerID()) == true)
			{
				continue;
			}

			m_providerList.append(provider);
			m_providerIndexMap.insert(provider.providerID(), index);
		}
	}

	// -------------------------------------------------------------------------------------------------------------------

	void Base::remove(int index)
	{
		QMutexLocker locker(&m_mutex);

		if (index < 0 || index >= m_providerList.count())
		{
			return;
		}

		m_providerList.remove(index);
	}

	// -------------------------------------------------------------------------------------------------------------------

	Item Base::provider(int index) const
	{
		QMutexLocker locker(&m_mutex);

		if (index < 0 || index >= m_providerList.count())
		{
			return Item();
		}

		return m_providerList[index];
	}

	// -------------------------------------------------------------------------------------------------------------------

	Item* Base::providerPtr(int index)
	{
		QMutexLocker locker(&m_mutex);

		if (index < 0 || index >= m_providerList.count())
		{
			return nullptr;
		}

		return &m_providerList[index];
	}

	// -------------------------------------------------------------------------------------------------------------------

	Item Base::provider(quint32 providerID) const
	{
		QMutexLocker locker(&m_mutex);

		if (m_providerIndexMap.contains(providerID) == false)
		{
			return Item();
		}

		int index = m_providerIndexMap[providerID];
		if (index < 0 || index >= m_providerList.count())
		{
			return Item();
		}

		return m_providerList[index];
	}

	// -------------------------------------------------------------------------------------------------------------------

	Item* Base::providerPtr(quint32 providerID)
	{
		QMutexLocker locker(&m_mutex);

		if (m_providerIndexMap.contains(providerID) == false)
		{
			return nullptr;
		}

		int index = m_providerIndexMap[providerID];
		if (index < 0 || index >= m_providerList.count())
		{
			return nullptr;
		}

		return &m_providerList[index];
	}

	// -------------------------------------------------------------------------------------------------------------------

	int Base::providerIndex(quint32 providerID)
	{
		QMutexLocker locker(&m_mutex);

		if (m_providerIndexMap.contains(providerID) == false)
		{
			return -1;
		}

		return m_providerIndexMap[providerID];
	}


	// -------------------------------------------------------------------------------------------------------------------

	void Base::setProvider(int index, const Item &provider)
	{
		QMutexLocker locker(&m_mutex);

		if (index < 0 || index >= m_providerList.count())
		{
			return;
		}

		m_providerList[index] = provider;
	}

	// -------------------------------------------------------------------------------------------------------------------

	Item* Base::data() const
	{
		QMutexLocker locker(&m_mutex);

		return (Item*) m_providerList.data();
	}

	// -------------------------------------------------------------------------------------------------------------------

	bool Base::readFromXml(const QByteArray& cfgXmlData, int version)
	{
		bool result = true;

		XmlReadHelper xml(cfgXmlData);

		if (xml.findElement(xmlTagProviders) == false)
		{
			qDebug() << "ProviderBase::readFromXml - Providers section not found";
			return false;
		}

		Item provider;

		int providerCount = 0;
		result &= xml.readIntAttribute("Count", &providerCount);

		for(int r = 0; r < providerCount; r++)
		{
			if (xml.findElement(xmlTagProvider) == false)
			{
				result = false;
				break;
			}

			result &= provider.readFromXml(xml, version);
			if (result == false)
			{
				continue;
			}

			append(provider);
		}

		if (count() != providerCount)
		{
			qDebug() << "ProviderBase::readFromXml - Providers loading error, loaded: " << count() << " from " << providerCount;
			assert(false);
			//return false;
		}

		qDebug() << "ProviderBase::readFromXml - Providers were loaded: " << count();

		emit cfgXmlDataLoaded();

		return result;
	}

	// -------------------------------------------------------------------------------------------------------------------

	void Base::writeToXml(XmlWriteHelper& xml, int version)
	{
		// Writing providers
		//
		xml.writeStartElement(xmlTagProviders);
		{
			int providerWritenCount = 0;

			int providerCount = theProviderBase.count();
			for(int i = 0; i < providerCount; i++)
			{
				Item* provider = theProviderBase.providerPtr(i);
				if (provider == nullptr)
				{
					continue;
				}

				if (provider->isEmpty() == true)
				{
					assert(provider->isEmpty() == false);
					continue;
				}

				if (provider->isActive() == false)
				{
					continue;
				}

				providerWritenCount++;
			}

			xml.writeIntAttribute("Count", providerWritenCount);


			for(int i = 0; i < providerCount; i++)
			{
				Item* provider = theProviderBase.providerPtr(i);
				if (provider == nullptr)
				{
					continue;
				}

				if (provider->isEmpty() == true)
				{
					assert(provider->isEmpty() == false);
					continue;
				}

				if (provider->isActive() == false)
				{
					continue;
				}

				provider->writeToXml(xml, version);
			}
		}

		xml.writeEndElement();	// </Providers>
	}

	// -------------------------------------------------------------------------------------------------------------------

	void Base::setProviderConnected(quint32 providerID, quint32 wrapVersion)
	{
		Q_UNUSED(wrapVersion);

		QMutexLocker locker(&m_mutex);

		if (m_providerIndexMap.contains(providerID) == false)
		{
			return;
		}

		int index = m_providerIndexMap[providerID];
		if (index < 0 || index >= m_providerList.count())
		{
			return;
		}

		m_providerList[index].setConnectState(true);
	}

	// -------------------------------------------------------------------------------------------------------------------

	Base& Base::operator=(const Base& from)
	{
		QMutexLocker locker(&m_mutex);

		m_providerList = from.m_providerList;

		return *this;
	}

	// -------------------------------------------------------------------------------------------------------------------
	//
	// ProviderType class implementation
	//
	// -------------------------------------------------------------------------------------------------------------------

	Type::Type(QObject *parent) :
		QObject(parent)
	{
		clear();
	}

	// -------------------------------------------------------------------------------------------------------------------

	Type::Type(const Type& type, QObject *parent) :
		QObject(parent)
	{
		*this = type;
	}

	// -------------------------------------------------------------------------------------------------------------------

	Type::~Type()
	{
	}

	// -------------------------------------------------------------------------------------------------------------------

	bool Type::isEmpty() const
	{
		if (m_id == INVALID_ID)
		{
			return true;
		}

		if (m_name.isEmpty() == true)
		{
			return true;
		}

		return false;
	}

	// -------------------------------------------------------------------------------------------------------------------

	void Type::clear()
	{
		m_id = INVALID_ID;
		m_name.clear();
	}

	// -------------------------------------------------------------------------------------------------------------------

	bool Type::readFromXml(XmlReadHelper& xml, int version)
	{
		bool result = true;

		switch(version)
		{
			case 1:

				result &= xml.readUInt32Attribute("TypeID", &m_id);
				result &= xml.readStringAttribute("Name", &m_name);

				break;

			default:

				qDebug() << tr("ProviderType::readFromXml - Undefined version");
				assert(0);

				break;
		}

		//	if (m_equipmentID.isEmpty() == false)
		//	{
		//		m_hash = calcHash(m_equipmentID);
		//	}

		return result;
	}

	// -------------------------------------------------------------------------------------------------------------------

	void Type::writeToXml(XmlWriteHelper& xml, int version)
	{
		switch(version)
		{
			case 1:

				xml.writeStartElement(xmlTagProviderType);
				{
					xml.writeUInt32Attribute("TypeID", typeID(), false);
					xml.writeStringAttribute("Name", name());
				}

				xml.writeEndElement();

			break;

		default:

			qDebug() << tr("ProviderType::readFromXml - Undefined version");
			assert(0);

			break;
		}
	}

	// -------------------------------------------------------------------------------------------------------------------

	Type& Type::operator=(const Type& from)
	{
		m_id = from.m_id;
		m_name = from.m_name;

		return *this;
	}

	// -------------------------------------------------------------------------------------------------------------------
	//
	// ProviderTypeBase class implementation
	//
	// -------------------------------------------------------------------------------------------------------------------

	TypeBase theProviderTypeBase;

	// -------------------------------------------------------------------------------------------------------------------

	TypeBase::TypeBase(QObject *parent) :
		QObject(parent)
	{
	}


	// -------------------------------------------------------------------------------------------------------------------

	TypeBase::~TypeBase()
	{
	}

	// -------------------------------------------------------------------------------------------------------------------

	void TypeBase::clear()
	{
		QMutexLocker locker(&m_mutex);

		m_typeList.clear();
	}

	// -------------------------------------------------------------------------------------------------------------------

	int TypeBase::count() const
	{
		QMutexLocker locker(&m_mutex);

		return m_typeList.count();
	}

	// -------------------------------------------------------------------------------------------------------------------

	int TypeBase::append(const Type& type)
	{
		QMutexLocker locker(&m_mutex);

		m_typeList.append(type);

		return m_typeList.count() - 1;
	}

	// -------------------------------------------------------------------------------------------------------------------

	void TypeBase::append(const QVector<Type>& list)
	{
		QMutexLocker locker(&m_mutex);

		m_typeList.append(list);
	}

	// -------------------------------------------------------------------------------------------------------------------

	void TypeBase::remove(int index)
	{
		QMutexLocker locker(&m_mutex);

		if (index < 0 || index >= m_typeList.count())
		{
			return;
		}

		m_typeList.remove(index);
	}

	// -------------------------------------------------------------------------------------------------------------------

	Type TypeBase::type(int index) const
	{
		QMutexLocker locker(&m_mutex);

		if (index < 0 || index >= m_typeList.count())
		{
			return Type();
		}

		return m_typeList[index];
	}

	// -------------------------------------------------------------------------------------------------------------------

	Type* TypeBase::typePtr(int index)
	{
		QMutexLocker locker(&m_mutex);

		if (index < 0 || index >= m_typeList.count())
		{
			return nullptr;
		}

		return &m_typeList[index];
	}

	// -------------------------------------------------------------------------------------------------------------------

	void TypeBase::setType(int index, const Type &type)
	{
		QMutexLocker locker(&m_mutex);

		if (index < 0 || index >= m_typeList.count())
		{
			return;
		}

		m_typeList[index] = type;
	}

	// -------------------------------------------------------------------------------------------------------------------

	Type* TypeBase::data() const
	{
		QMutexLocker locker(&m_mutex);

		return (Type*) m_typeList.data();
	}

	// -------------------------------------------------------------------------------------------------------------------

	bool TypeBase::readFromXml(const QByteArray& cfgXmlData, int version)
	{
		bool result = true;

		XmlReadHelper xml(cfgXmlData);

		if (xml.findElement(xmlTagProviderTypes) == false)
		{
			qDebug() << "ProviderTypeBase::readFromXml - ProviderTypes section not found";
			return false;
		}

		Type type;

		int typeCount = 0;
		result &= xml.readIntAttribute("Count", &typeCount);

		for(int r = 0; r < typeCount; r++)
		{
			if (xml.findElement(xmlTagProviderType) == false)
			{
				result = false;
				break;
			}

			result &= type.readFromXml(xml, version);
			if (result == false)
			{
				continue;
			}

			append(type);
		}

		if (count() != typeCount)
		{
			qDebug() << "ProviderTypeBase::readFromXml - ProviderTypes loading error, loaded: " << count() << " from " << typeCount;
			assert(false);
			return false;
		}

		qDebug() << "ProviderTypeBase::readFromXml - ProviderTypes were loaded: " << count();

		emit cfgXmlDataLoaded();

		return result;
	}

	// -------------------------------------------------------------------------------------------------------------------

	void TypeBase::writeToXml(XmlWriteHelper& xml, int version)
	{
		// Writing provider types
		//
		xml.writeStartElement(xmlTagProviderTypes);
		{
			int typeCount = ::theProviderTypeBase.count();

			xml.writeIntAttribute("Count", typeCount);

			for(int i = 0; i < typeCount; i++)
			{
				Type* type = ::theProviderTypeBase.typePtr(i);
				if (type == nullptr)
				{
					continue;
				}

				if (type->isEmpty() == true)
				{
					assert(type->isEmpty() == false);
					continue;
				}

				type->writeToXml(xml, version);
			}
		}
		xml.writeEndElement();	// </Types>
	}

	// -------------------------------------------------------------------------------------------------------------------

	TypeBase& TypeBase::operator=(const TypeBase& from)
	{
		QMutexLocker locker(&m_mutex);

		m_typeList = from.m_typeList;

		return *this;
	}

	// -------------------------------------------------------------------------------------------------------------------
}
