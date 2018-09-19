#include "ProviderView.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QEvent>
#include <QResizeEvent>
#include <QPainter>
#include <assert.h>

// -------------------------------------------------------------------------------------------------------------------
//
// ProviderItem
//
// -------------------------------------------------------------------------------------------------------------------

ProviderDelegate::ProviderDelegate(QObject *parent) :
	QStyledItemDelegate(parent)
{

	m_providerNameFont = new QFont("Arial", 14, 10);
	m_providerAddressFont = new QFont("Arial", 10, 10);

	//m_providerNameFont->setBold(true);
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QRect textRect = option.rect;
	QRect textOrder = option.rect;
	QRect timeOrder = option.rect;
	textRect.adjust(60, 5, 0, 0);


	ProviderItem item = qvariant_cast<ProviderItem>(index.data(Qt::UserRole));

	Provider::Item		provider = item.provider();
	Order::Item			order  = item.order();

	QPixmap infoPixmapG(":/icons/MenuGreen.png");
	QPixmap infoPixmap(":/icons/MenuBlack.png");
	QPixmap clockPixmap(":/icons/Clock.png");

	QPixmap tablePixmap(":/icons/Table.png");
	QPixmap dinnerPixmap(":/icons/Dinner.png");

	int midInfo_x = option.rect.right() - PROVIDER_ICON_SIZE - 20;
	int midInfo_y = option.rect.top() + 10;


	QSize cellSize = QFontMetrics(*m_providerNameFont).size(Qt::TextSingleLine,"A");


//	if ((option.state & QStyle::State_Selected) != 0)
//	{
////			if ((option.state & QStyle::State_HasFocus) != 0)
////			{
//				//painter->fillRect(option.rect, QColor(0xF0, 0xF7, 0xFC));
//				painter->drawPixmap(midInfo_x, midInfo_y, PROVIDER_ICON_SIZE, PROVIDER_ICON_SIZE, infoPixmap);
////			}
////			else
////			{
////				painter->fillRect(option.rect, option.palette.window());
////				painter->drawPixmap( option.rect.right() - PROVIDER_ICON_SIZE, option.rect.bottom() - PROVIDER_ICON_SIZE, 16, 16, pixmap);
////			}
//	}


	switch(item.order().state())
		{
			case Order::STATE_ORDER_OK:							painter->fillRect(option.rect, QColor(0xA0, 0xF0, 0xA0));	break;
			case Order::STATE_ORDER_PROCESSING:					painter->fillRect(option.rect, QColor(0xff, 0xf4, 0xc4));	break;
			case Order::STATE_SERVER_CREATED_ORDER:				painter->fillRect(option.rect, QColor(0xff, 0xf4, 0xc4));	break;
		}

	painter->drawPixmap(midInfo_x, midInfo_y, PROVIDER_ICON_SIZE, PROVIDER_ICON_SIZE, infoPixmapG);

	painter->setFont(*m_providerNameFont);
	painter->setPen(QColor(0x0, 0x0, 0x0));
	painter->drawText(textRect, Qt::AlignLeft, provider.name());

	textRect.adjust(0, cellSize.height() , 0, 0);

	painter->setFont(*m_providerAddressFont);
	painter->setPen(QColor(0x70, 0x70, 0x70));
	painter->drawText(textRect, Qt::AlignLeft, provider.address());

	textRect.adjust(0, cellSize.height() - 4, 0, 0);

	painter->setFont(*m_providerAddressFont);
	painter->setPen(QColor(0x70, 0x70, 0x70));
	painter->drawText(textRect, Qt::AlignLeft, "+38 " + provider.phone() );


	QString orderState;

	switch(item.order().state())
	{
		case Order::STATE_ORDER_OK:							orderState = tr("Wellcome!");	break;
		case Order::STATE_ORDER_PROCESSING:					orderState = tr("Wait answer");	break;
		case Order::STATE_SERVER_CREATED_ORDER:				orderState = tr("Wait answer");	break;
	}

//	painter->setPen(Qt::gray);


	//painter->setFont(*m_providerNameFont);
	textOrder.adjust(00, 45, 0, 0);
	textOrder.setRight( textOrder.right() - 30 );

	painter->setPen(QColor(0x40, 0x40, 0x40));
	painter->drawText(textOrder, Qt::AlignRight, orderState);

	if(item.order().state() == Order::STATE_ORDER_OK)
	{
		QRect codeOrder = option.rect;

		codeOrder.adjust(00, 10, 0, 0);
		codeOrder.setRight( codeOrder.right() - 40 );

		painter->setPen(QColor(0xA0, 0xA0, 0xA0));
		painter->drawText(codeOrder, Qt::AlignRight, tr("%1").arg(order.cancelCode()));
	}


	switch(item.order().state())
	{
		case Order::STATE_ORDER_OK:
		case Order::STATE_ORDER_PROCESSING:
		case Order::STATE_SERVER_CREATED_ORDER:

			Order::Time32 orderTime = order.orderTime();

			QString orderTime1 = QString().sprintf("%02d:%02d", orderTime.hour, orderTime.minute);

			switch (order.type())
			{
				case Order::TYPE_TABLE:		painter->drawPixmap(option.rect.x() + 10, option.rect.y() + 10 , 32, 32, tablePixmap);	break;
				case Order::TYPE_DINNER:	painter->drawPixmap(option.rect.x() + 10, option.rect.y() + 10 , 32, 32, dinnerPixmap);	break;
				default:					painter->drawPixmap(option.rect.x() + 10, option.rect.y() + 10 , 32, 32, clockPixmap);	break;
			}

			timeOrder.adjust(10, 45, 0, 0);
			timeOrder.setRight( timeOrder.right() - 25 );

			painter->setPen(QColor(0x40, 0x40, 0x40));
			painter->drawText(timeOrder, Qt::AlignLeft, orderTime1);



			break;
	}


	if ((option.state & QStyle::State_Selected) == 0)
	{
		painter->drawPixmap(midInfo_x, midInfo_y, PROVIDER_ICON_SIZE, PROVIDER_ICON_SIZE, infoPixmap);
	}
}

// -------------------------------------------------------------------------------------------------------------------
//
// ProviderItem
//
// -------------------------------------------------------------------------------------------------------------------

ProviderItem::ProviderItem()
{
	m_provider.clear();
	m_order.clear();
}

// -------------------------------------------------------------------------------------------------------------------

ProviderItem::ProviderItem(const Provider::Item& provider)
	: m_provider(provider)
{
	m_order.clear();
}

// -------------------------------------------------------------------------------------------------------------------

ProviderItem::~ProviderItem()
{

}

// -------------------------------------------------------------------------------------------------------------------

ProviderItem& ProviderItem::operator=(const ProviderItem& from)
{
	m_provider = from.m_provider;
	m_order = from.m_order;

	return *this;
}

// -------------------------------------------------------------------------------------------------------------------
//
// ProviderTable
//
// -------------------------------------------------------------------------------------------------------------------

ProviderTable::ProviderTable(QObject*)
{
}

// -------------------------------------------------------------------------------------------------------------------

ProviderTable::~ProviderTable()
{
	m_providerList.clear();
}

// -------------------------------------------------------------------------------------------------------------------

int ProviderTable::columnCount(const QModelIndex&) const
{
	return PROVIDER_COLUMN_COUNT;
}

// -------------------------------------------------------------------------------------------------------------------

int ProviderTable::rowCount(const QModelIndex&) const
{
	return m_providerList.count();
}

// -------------------------------------------------------------------------------------------------------------------

QVariant ProviderTable::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
	{
		return QVariant();
	}

	QVariant result = QVariant();

	if (orientation == Qt::Horizontal)
	{
		if (section >= 0 && section < PROVIDER_COLUMN_COUNT)
		{
			result = ProviderColumn[section];
		}
	}

	if (orientation == Qt::Vertical)
	{
		result = QString("%1").arg(section + 1);
	}

	return result;
}

// -------------------------------------------------------------------------------------------------------------------

QVariant ProviderTable::data(const QModelIndex &index, int role) const
{
	if (index.isValid() == false)
	{
		return QVariant();
	}

	int row = index.row();
	if (row < 0 || row >= m_providerList.count())
	{
		return QVariant();
	}

	int column = index.column();
	if (column < 0 || column > PROVIDER_COLUMN_COUNT)
	{
		return QVariant();
	}

	ProviderItem item = m_providerList.at(row);
	if (item.provider().isEmpty() == true)
	{
		return QVariant();
	}

	if (role == Qt::TextAlignmentRole)
	{
		return Qt::AlignLeft;
	}

	if (role == Qt::TextColorRole)
	{
		if (item.provider().active() == false)
		{
			return QColor(0xA0, 0xA0, 0xA0);
		}

		return QVariant();
	}

	if (role == Qt::BackgroundColorRole)
	{
		QVariant result = QVariant();

		switch(item.order().state())
		{
			case Order::STATE_ORDER_OK:							result = QColor(0xA0, 0xFF, 0xA0);	break;
			case Order::STATE_ORDER_CANCEL:						result = QColor(0xFF, 0xA0, 0xA0);	break;
			case Order::STATE_ORDER_PROCESSING:					result = QColor(0xFF, 0xFF, 0xA0);	break;
			case Order::STATE_SERVER_CREATED_ORDER:				result = QColor(0xA0, 0xA0, 0xA0);	break;
			default:											result = QVariant();		break;
		}

		return result;
	}

	if (role == Qt::UserRole)
	{
		QVariant var;
		var.setValue(item);
		return var;
	}

	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		return text(row, column, item);
	}

	return QVariant();
}

// -------------------------------------------------------------------------------------------------------------------

QString ProviderTable::text(int row, int column, const ProviderItem& item) const
{
	if (row < 0 || row >= m_providerList.count())
	{
		return QString();
	}

	if (column < 0 || column > PROVIDER_COLUMN_COUNT)
	{
		return QString();
	}

	QString result;

	switch (column)
	{
		case PROVIDER_COLUMN_NAME:

			result = item.provider().name() + QString("\n");

			switch(item.order().state())
			{
				case Order::STATE_ORDER_OK:							result += tr("Your order has been accepted, Wellcome!");	break;
				case Order::STATE_ORDER_CANCEL:						result += tr("Your order has been cancel, try later");	break;
				case Order::STATE_ORDER_PROCESSING:					result += tr("Your order is processing by administrator");	break;
				case Order::STATE_PROVIDER_IS_NOT_CONNECTED:		result += tr("Not connection with establishment");	break;
				case Order::STATE_SERVER_CREATED_ORDER:				result += tr("Wait answer from establishment");	break;
			}

			break;

		default:
			assert(0);
			break;
	}

	return result;
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderTable::updateRow(int row)
{
	if (row < 0 || row >= m_providerList.count())
	{
		return;
	}

	for (int column = 0; column < PROVIDER_COLUMN_COUNT; column ++)
	{
		QModelIndex cellIndex = index(row, column);

		emit dataChanged(cellIndex, cellIndex, QVector<int>() << Qt::DisplayRole);
	}
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderTable::updateColumn(int column)
{
	if (column < 0 || column >= PROVIDER_COLUMN_COUNT)
	{
		return;
	}

	int count = m_providerList.count();
	for (int row = 0; row < count; row ++)
	{
		QModelIndex cellIndex = index(row, column);

		emit dataChanged(cellIndex, cellIndex, QVector<int>() << Qt::DisplayRole);
	}
}

// -------------------------------------------------------------------------------------------------------------------

ProviderItem ProviderTable::at(int index) const
{
	if (index < 0 || index >= count())
	{
		return ProviderItem();
	}

	return m_providerList.at(index);
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderTable::set(int index, const ProviderItem& item)
{
	if (index < 0 || index >= count())
	{
		return;
	}

	m_providerList[index] = item;
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderTable::set(const QList<ProviderItem>& list_add)
{
	int count = list_add.count();
	if (count == 0)
	{
		return;
	}

	beginInsertRows(QModelIndex(), 0, count - 1);

		m_providerList = list_add;

	endInsertRows();
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderTable::clear()
{
	int count = m_providerList.count();
	if (count == 0)
	{
		return;
	}

	beginRemoveRows(QModelIndex(), 0, count - 1);

		m_providerList.clear();

	endRemoveRows();
}

// -------------------------------------------------------------------------------------------------------------------
//
// ProviderView
//
// -------------------------------------------------------------------------------------------------------------------

ProviderView::ProviderView()
{
	setModel(&m_table);

	horizontalHeader()->hide();
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::SingleSelection);

	ProviderDelegate* textDelegate = new ProviderDelegate(this);
	setItemDelegateForColumn(PROVIDER_COLUMN_NAME, textDelegate);


	QFont* listFont =  new QFont("Arial", 14, 2);
	setFont(*listFont);

	QSize cellSize = QFontMetrics(font()).size(Qt::TextSingleLine,"A");
	verticalHeader()->setDefaultSectionSize(cellSize.height()*3);

	//connect(this, &QTableView::clicked, this, &ProviderView::onClick);
}

// -------------------------------------------------------------------------------------------------------------------

ProviderView::~ProviderView()
{
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderView::updateList()
{
	m_table.clear();

	QList<ProviderItem>	list;

	int count = theProviderBase.count();
	for(int i = 0; i < count; i++)
	{
		Provider::Item provider = theProviderBase.provider(i);
		if (provider.isEmpty() == true || provider.active()== false)
		{
			continue;
		}

		if (m_textFilter.isEmpty() == false)
		{
			if (provider.name().indexOf(m_textFilter) == -1)
			{
				continue;
			}
		}

		list.append(provider);
	}

	m_table.set(list);
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderView::updateOrderList()
{
	QList<Order::Item> list = theOrderBase.orderList();

	int orderCount = list.count();
	for(int o = 0 ; o < orderCount; o++)
	{
		Order::Item order = list[o];

		int itemCount = m_table.count();
		for(int p = 0; p < itemCount; p++)
		{
			ProviderItem item = m_table.at(p);

			if (order.providerID() == item.provider().providerID())
			{
				item.setOrder(order);

				m_table.set(p, item);

				break;
			}
		}
	}
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderView::orderStateChanged(const Order::Item& order)
{
	int count = m_table.count();
	for(int i = 0; i < count; i++)
	{
		ProviderItem item = m_table.at(i);

		Provider::Item provider = item.provider();

		if (item.provider().providerID() == order.providerID())
		{
			item.setOrder(order);

			m_table.set(i, item);

			m_table.updateRow(i);

			if (order.state() == Order::STATE_ORDER_CANCEL || order.state() == Order::STATE_UNDEFINED)
			{
				QMessageBox::information(this, tr("Order"), tr("\"%1\" canceled your order").arg(provider.name()));
			}

			break;
		}
	}
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderView::setTextFilter(const QString& text)
{
	// setFilter(text.toUpper());
	setFilter(text);
	updateList();
}

// -------------------------------------------------------------------------------------------------------------------

bool ProviderView::event(QEvent* e)
{
	if (e->type() == QEvent::Resize)
	{
		QResizeEvent* resizeEvent = static_cast<QResizeEvent*>(e);

		setColumnWidth(PROVIDER_COLUMN_NAME, resizeEvent->size().width() - 20);
	}

	return QTableView::event(e);
}

// -------------------------------------------------------------------------------------------------------------------
