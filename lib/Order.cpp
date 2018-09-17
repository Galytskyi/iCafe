#include "Order.h"

#include <QFile>
#include <assert.h>

// -------------------------------------------------------------------------------------------------------------------

Order::Base theOrderBase;

// -------------------------------------------------------------------------------------------------------------------

namespace Order
{
	// -------------------------------------------------------------------------------------------------------------------
	//
	// OrderItem class implementation
	//
	// -------------------------------------------------------------------------------------------------------------------

	Item::Item(QObject *parent)
		: QObject(parent)
	{
		clear();
	}

	// -------------------------------------------------------------------------------------------------------------------

	Item::Item(const Item& order, QObject *parent)
		: QObject(parent)
	{
		*this = order;
	}

	// -------------------------------------------------------------------------------------------------------------------

	Item::Item(const wrapOrder& wo, QObject *parent)
		: QObject(parent)
	{
		this->fromProtoWrap(wo);
	}

	// -------------------------------------------------------------------------------------------------------------------

	Item::~Item()
	{
	}

	// -------------------------------------------------------------------------------------------------------------------

	bool Item::isEmpty() const
	{
		if (m_state == STATE_UNDEFINED)
		{
			return true;
		}

		if (m_type == TYPE_UNDEFINED)
		{
			return true;
		}

		if (m_handle.ID == EMPTY_ORDER_HANDLE)
		{
			return true;
		}

		return false;
	}

	// -------------------------------------------------------------------------------------------------------------------

	bool Item::isValid() const
	{
		if (m_state < 0 || m_state >= STATE_COUNT)
		{
			return false;
		}

		if (m_type < 0 || m_type >= TYPE_COUNT)
		{
			return false;
		}

		if (m_handle.ID == EMPTY_ORDER_HANDLE)
		{
			return false;
		}

		return true;
	}

	// -------------------------------------------------------------------------------------------------------------------

	void Item::clear()
	{
		m_state = STATE_UNDEFINED;
		m_type = TYPE_UNDEFINED;

		m_handle.ID = EMPTY_ORDER_HANDLE;
		m_orderTime = 0;
		m_people = 0;

		m_cancelCode = 0;
	}

	// -------------------------------------------------------------------------------------------------------------------

	int Item::state() const
	{
		if (m_state < 0 || m_state >= STATE_COUNT)
		{
			assert(0);
			return STATE_UNDEFINED;
		}

		return m_state;
	}

	bool Item::setState(int state)
	{
		if (state < 0 || state >= STATE_COUNT)
		{
			assert(0);
			m_state = STATE_UNDEFINED;
			return false;
		}

		m_state = state;

		return true;
	}

	int Item::type() const
	{
		if (m_type < 0 || m_type >= TYPE_COUNT)
		{
			assert(0);
			return TYPE_UNDEFINED;
		}

		return m_type;
	}

	bool Item::setType(int type)
	{
		if (type < 0 || type >= TYPE_COUNT)
		{
			assert(0);
			m_type = TYPE_UNDEFINED;
			return false;
		}

		m_type = type;

		return true;
	}

	// -------------------------------------------------------------------------------------------------------------------

	int Item::createCancelCode()
	{
		m_cancelCode = (rand() - 0)*(9999-1000)/(RAND_MAX - 0) + 1000;

		return m_cancelCode;
	}

	// -------------------------------------------------------------------------------------------------------------------

	Order::wrapOrder Item::toProtoWrap() const
	{
		Order::wrapOrder wo;

		wo.set_version(ORDER_PROTO_WRAP_VERSION);

		switch (wo.version())
		{
			case 1:

				wo.set_state(m_state);
				wo.set_type(m_type);
				wo.set_order_id(m_handle.ID);
				wo.set_order_time(m_orderTime.time);
				wo.set_people(m_people);
				wo.set_cancel_code(m_cancelCode);

				break;

			default:
				qDebug() << "Item::toWrap_proto - Undefined version:" << wo.version();
				assert(0);
				break;
		}

		return wo;
	}

	// -------------------------------------------------------------------------------------------------------------------

	void Item::toProtoWrap(Order::wrapOrder* pWo)
	{
		if (pWo == nullptr)
		{
			return;
		}

		pWo->set_version(ORDER_PROTO_WRAP_VERSION);

		switch (pWo->version())
		{
			case 1:

				pWo->set_state(m_state);
				pWo->set_type(m_type);
				pWo->set_order_id(m_handle.ID);
				pWo->set_order_time(m_orderTime.time);
				pWo->set_people(m_people);
				pWo->set_cancel_code(m_cancelCode);

				break;

			default:
				qDebug() << "Item::toWrap_proto - Undefined version:" << pWo->version();
				assert(0);
				break;
		}
	}

	// -------------------------------------------------------------------------------------------------------------------

	void Item::fromProtoWrap(const Order::wrapOrder& wo)
	{
		switch (wo.version())
		{
			case 1:

				m_state = wo.state();
				m_type = wo.type();
				m_handle = wo.order_id();
				m_orderTime.time = wo.order_time();
				m_people = wo.people();
				m_cancelCode = wo.cancel_code();

				break;

			default:
				qDebug() << "Item::fromWrap_proto - Undefined version:" << wo.version();
				assert(0);
				break;
		}
	}

	// -------------------------------------------------------------------------------------------------------------------

	void Item::fromProtoWrap(Order::wrapOrder* pWo)
	{
		if (pWo == nullptr)
		{
			return;
		}

		switch (pWo->version())
		{
			case 1:

				m_state = pWo->state();
				m_type = pWo->type();
				m_handle = pWo->order_id();
				m_orderTime = pWo->order_time();
				m_people = pWo->people();
				m_cancelCode = pWo->cancel_code();

				break;

			default:
				qDebug() << "Item::fromWrap_proto - Undefined version:" << pWo->version();
				assert(0);
				break;
		}
	}

	// -------------------------------------------------------------------------------------------------------------------

	bool Item::readFromXml(XmlReadHelper& xml, int version)
	{
		bool result = true;

		switch(version)
		{
			case 1:

				result &= xml.readIntAttribute("State", &m_state);
				result &= xml.readIntAttribute("Type", &m_type);

				result &= xml.readUInt64Attribute("OrderID", &m_handle.ID);
				result &= xml.readUInt32Attribute("OrderTime", &m_orderTime.time);
				result &= xml.readIntAttribute("People", &m_people);

				result &= xml.readIntAttribute("CancelCode", &m_cancelCode);

				break;

			default:

				qDebug() << "OrderItem::readFromXml - Undefined version:" << version;
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

			xml.writeStartElement(xmlTagOrder);
			{
				xml.writeIntAttribute("State", state());
				xml.writeIntAttribute("Type", type());

				xml.writeUInt64Attribute("OrderID", handle().ID);
				xml.writeUInt32Attribute("OrderTime", orderTime().time, false);
				xml.writeIntAttribute("People", people());

				xml.writeIntAttribute("CancelCode", cancelCode());
			}

			xml.writeEndElement();

			break;

		default:

			qDebug() << "OrderItem::readFromXml - Undefined version:" << version;
			assert(0);

			break;
		}
	}

	// -------------------------------------------------------------------------------------------------------------------

	Item& Item::operator=(const Item& from)
	{
		m_state = from.m_state;
		m_type = from.m_type;

		m_handle.ID = from.m_handle.ID;
		m_orderTime = from.m_orderTime;
		m_removeTime = from.m_removeTime;
		m_people = from.m_people;

		m_cancelCode = from.m_cancelCode;

		return *this;
	}

	// -------------------------------------------------------------------------------------------------------------------
	//
	// OrderBase class implementation
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

		m_orderMap.clear();
		m_orderForProviderList.clear();
	}

	// -------------------------------------------------------------------------------------------------------------------

	int Base::count() const
	{
		QMutexLocker locker(&m_mutex);

		return m_orderMap.count();
	}

	// -------------------------------------------------------------------------------------------------------------------

	bool Base::isExist(quint64 orderID) const
	{
		QMutexLocker locker(&m_mutex);

		return m_orderMap.contains(orderID);
	}

	// -------------------------------------------------------------------------------------------------------------------

	bool Base::append(const Item& order)
	{
		QMutexLocker locker(&m_mutex);

		if (m_orderMap.contains(order.handle().ID) == true)
		{
			return false;
		}

		m_orderMap.insert(order.handle().ID, order);

		if (order.state() == Order::STATE_SERVER_CREATED_ORDER)
		{
			m_orderForProviderList.append(order);
		}

		emit signal_stateChanged(order);

		return true;
	}

	// -------------------------------------------------------------------------------------------------------------------

	void Base::slot_appendOrder(const Item &order)
	{
		append(order);
	}

	// -------------------------------------------------------------------------------------------------------------------

	bool Base::remove(quint64 orderID)
	{
		QMutexLocker locker(&m_mutex);

		if (m_orderMap.contains(orderID) == false)
		{
			return false;
		}

		Item order = m_orderMap[orderID];
		order.setState(STATE_UNDEFINED);

		int result = m_orderMap.remove(orderID);
		if(result == 0)
		{
			return false;
		}

		int count = m_orderForProviderList.count();
		for(int i = 0; i < count; i ++)
		{
			if (m_orderForProviderList[i].handle().ID == orderID)
			{
				m_orderForProviderList.remove(i);

				break;
			}
		}

		emit signal_stateChanged(order);

		return true;
	}

	// -------------------------------------------------------------------------------------------------------------------

	Item Base::order(quint64 orderID) const
	{
		QMutexLocker locker(&m_mutex);

		if (m_orderMap.contains(orderID) == false)
		{
			return Item();
		}

		return m_orderMap[orderID];
	}

	// -------------------------------------------------------------------------------------------------------------------

	Item* Base::orderPtr(quint64 orderID)
	{
		QMutexLocker locker(&m_mutex);

		if (m_orderMap.contains(orderID) == false)
		{
			return nullptr;
		}

		return &m_orderMap[orderID];
	}

	// -------------------------------------------------------------------------------------------------------------------

	QList<Item> Base::orderList() const
	{
		QMutexLocker locker(&m_mutex);

		return m_orderMap.values();
	}

	// -------------------------------------------------------------------------------------------------------------------

	int Base::orderState(quint64 orderID) const
	{
		QMutexLocker locker(&m_mutex);

		if (m_orderMap.contains(orderID) == false)
		{
			return STATE_UNDEFINED;
		}

		return m_orderMap[orderID].state();
	}

	// -------------------------------------------------------------------------------------------------------------------

	bool Base::readFromXml(const QString& fileName)
	{
		if (fileName.isEmpty() == true)
		{
			assert(0);
			return false;
		}

		QByteArray cfgXmlData;

		// read form file
		//
		QFile file(fileName);
		if (file.open(QIODevice::ReadOnly) == false)
		{
			return false;
		}

		cfgXmlData = file.readAll();
		file.close();

		bool result = true;

		XmlReadHelper xml(cfgXmlData);

		if (xml.findElement(xmlTagOrders) == false)
		{
			qDebug() << "OrderBase::readFromXml - Orders section not found";
			return false;
		}

		Item order;

		int orderCount = 0;
		result &= xml.readIntAttribute("Count", &orderCount);

		for(int r = 0; r < orderCount; r++)
		{
			if (xml.findElement(xmlTagOrder) == false)
			{
				result = false;
				break;
			}

			result &= order.readFromXml(xml, xmlTagOrderLastVersion);
			if (result == false)
			{
				continue;
			}

			append(order);
		}

		if (count() != orderCount)
		{
			qDebug() << "OrderBase::readFromXml - Orders loading error, loaded: " << count() << " from " << orderCount;
			assert(false);
			return false;
		}

		qDebug() << "OrderBase::readFromXml - Orders were loaded: " << count();

		return result;
	}

	// -------------------------------------------------------------------------------------------------------------------

	void Base::writeToXml(const QString& fileName)
	{
		if (fileName.isEmpty() == true)
		{
			assert(0);
			return;
		}

		QByteArray cfgXmlData;
		XmlWriteHelper xml(&cfgXmlData);

		xml.setAutoFormatting(true);
		xml.writeStartDocument();
		{
			xml.writeStartElement(xmlTagOrderFile);
			{
				xml.writeIntAttribute("Version", xmlTagOrderFileLastVersion );

				// Writing orders
				//
				xml.writeStartElement(xmlTagOrders);
				{
					QList<Order::Item> list = theOrderBase.orderList();

					int orderCount = list.count();

					xml.writeIntAttribute("Count", orderCount);

					for(int i = 0; i < orderCount; i++)
					{
						Item order = list[i];

						if (order.isEmpty() == true )
						{
							assert(order.isEmpty() == false);
							continue;
						}

						order.writeToXml(xml, xmlTagOrderLastVersion);
					}
				}
				xml.writeEndElement();	// </Orders>

			}
			xml.writeEndElement();	// <ConfigFime>
		}
		xml.writeEndDocument();

		// write to file
		//
		QFile file(fileName);
		if (file.open(QIODevice::WriteOnly) == true)
		{
			file.write(cfgXmlData);
			file.close();
		}
	}

	// -------------------------------------------------------------------------------------------------------------------

	Base& Base::operator=(const Base& from)
	{
		QMutexLocker locker(&m_mutex);

		m_orderMap = from.m_orderMap;
		m_orderForProviderList = from.m_orderForProviderList;

		return *this;
	}

	// -------------------------------------------------------------------------------------------------------------------

	Item Base::hasOrderForProvider(quint32 providerID)
	{
		QMutexLocker locker(&m_mutex);

		Item resultOrder;

		int count = m_orderForProviderList.count();
		for(int i = 0; i < count; i ++)
		{
			Item& order = m_orderForProviderList[i];

			if (order.state() != Order::STATE_SERVER_CREATED_ORDER)
			{
				continue;
			}

			if (order.providerID() != providerID)
			{
				continue;
			}

			m_orderForProviderList.remove(i);

			order.setState(STATE_SERVER_SEND_TO_PROVIDER);

			resultOrder = order;

			break;
		}

		return resultOrder;
	}

	// -------------------------------------------------------------------------------------------------------------------

	void Base::slot_changeState(const Item &order)
	{
		QMutexLocker locker(&m_mutex);

		if (m_orderMap.contains(order.handle().ID) == false)
		{
			return;
		}

		Item& aOrder = m_orderMap[order.handle().ID];


		if (aOrder.state() == order.state())
		{
			return;
		}

		aOrder.setState(order.state());

		emit signal_stateChanged(order);
	}

	// -------------------------------------------------------------------------------------------------------------------

	void Base::slot_removeOrder(const Item& order)
	{
		remove(order.handle().ID);
	}

	// -------------------------------------------------------------------------------------------------------------------
}