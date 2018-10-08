#include "ProviderView.h"

#include <QApplication>
#include <QScreen>
#include <QScrollBar>
#include <QHeaderView>
#include <QEvent>
#include <QResizeEvent>
#include <QPainter>
#include <QMessageBox>
#include <assert.h>

// -------------------------------------------------------------------------------------------------------------------
//
// ProviderDelegate
//
// -------------------------------------------------------------------------------------------------------------------

ProviderDelegate::ProviderDelegate(QObject *parent) :
	QStyledItemDelegate(parent)
{
	m_dpi = QApplication::screens().at(0)->logicalDotsPerInch() / 1.5;
	m_iconSize = m_dpi / 1.5;
	m_iconSmallSize = m_iconSize / 2;

	m_providerNameFont = new QFont("Arial", 14, 10);
	m_providerAddressFont = new QFont("Arial", 10, 10);

	m_dotsBlackPixmap = QPixmap(":/icons/MenuBlack.png");
	m_dotsGrayPixmap = QPixmap(":/icons/MenuGray.png");
	n_tablePixmap = QPixmap(":/icons/Table.png");
	m_dinnerPixmap = QPixmap(":/icons/Dinner.png");
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	ProviderItem item = qvariant_cast<ProviderItem>(index.data(Qt::UserRole));

	Provider::Item		provider = item.provider();
	Order::Item			order  = item.order();

	if ((option.state & QStyle::State_Selected) != 0)
	{
		if ((option.state & QStyle::State_HasFocus) != 0)
		{
			painter->fillRect(option.rect, Qt::white);
		}
		else
		{
			painter->fillRect(option.rect, QColor(0xE0, 0xFF, 0xFF));
		}
	}


	QString orderState;

	switch(item.order().state())
	{
		case Order::STATE_ORDER_OK:
			orderState = tr("Добро пожаловать!");
			painter->fillRect(option.rect, QColor(0xA0, 0xF0, 0xA0));	break;
			break;

		case Order::STATE_ORDER_PROCESSING:
		case Order::STATE_SERVER_CREATED_ORDER:
			orderState = tr("Ждите ответа");
			painter->fillRect(option.rect, QColor(0xff, 0xfa, 0xd1));	break;
			break;
	}


	int midDots_x = option.rect.right() - m_iconSmallSize - 20;
	int midDots_y = option.rect.top() + 10;

	if ((option.state & QStyle::State_Selected) == 0)
	{
		painter->drawPixmap(midDots_x, midDots_y, m_iconSmallSize, m_iconSmallSize, m_dotsGrayPixmap);
	}
	else
	{
		painter->drawPixmap(midDots_x, midDots_y, m_iconSmallSize, m_iconSmallSize, m_dotsBlackPixmap);
	}

	// provider data
	//
	QRect providerDataRect = option.rect;
	providerDataRect.adjust(m_dpi, 5, -m_dpi, 0);

	painter->setFont(*m_providerNameFont);
	painter->setPen(QColor(0x0, 0x0, 0x0));
	painter->drawText(providerDataRect, Qt::AlignLeft, provider.name());

	QSize cellSize = QFontMetrics(*m_providerNameFont).size(Qt::TextSingleLine,"A");
	providerDataRect.adjust(0, cellSize.height(), 0, 0);

	painter->setFont(*m_providerAddressFont);
	painter->setPen(QColor(0x70, 0x70, 0x70));
	painter->drawText(providerDataRect, Qt::AlignLeft, provider.address());

	providerDataRect.adjust(0, cellSize.height() - 4, 0, 0);

	//painter->setFont(*m_providerAddressFont);
	painter->setPen(QColor(0x70, 0x70, 0x70));
	painter->drawText(providerDataRect, Qt::AlignLeft, provider.phone() );

	// order state
	//

	QRect stateOrderRect = providerDataRect;

	stateOrderRect.setLeft(option.rect.right() - 500);
	stateOrderRect.setRight(option.rect.right() - 30);

	painter->setPen(QColor(0x40, 0x40, 0x40));
	painter->drawText(stateOrderRect, Qt::AlignRight, orderState);

	// cancel code
	//

	if(item.order().state() == Order::STATE_ORDER_OK)
	{
		QRect codeOrder = option.rect;

		codeOrder.adjust(00, 10, 0, 0);
		codeOrder.setRight( midDots_x - 20 );

		painter->setPen(QColor(0xFF, 0x80, 0x80));
		painter->drawText(codeOrder, Qt::AlignRight, QString("%1").arg(order.cancelCode()));
	}

	// Order time
	//

	if (orderState.isEmpty() == false)
	{
		int icon_x = m_dpi / 2 - m_iconSize / 2;

		switch (order.type())
		{
			case Order::TYPE_TABLE:		painter->drawPixmap(icon_x, option.rect.y() + 3, m_iconSize, m_iconSize, n_tablePixmap);	break;
			case Order::TYPE_DINNER:	painter->drawPixmap(icon_x, option.rect.y() + 3, m_iconSize, m_iconSize, m_dinnerPixmap);	break;
		}

		QRect timeOrderRect = providerDataRect;

		timeOrderRect.setLeft(0);
		timeOrderRect.setRight(m_dpi);
		timeOrderRect.adjust(0,-4,0,0);

		Order::Time32 orderTime = order.orderTime();
		QString orderTimeStr = QString().sprintf("%02d:%02d", orderTime.hour, orderTime.minute);

		painter->setPen(QColor(0x40, 0x40, 0x40));
		painter->drawText(timeOrderRect, Qt::AlignCenter, orderTimeStr);
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

	if (role == Qt::UserRole)
	{
		QVariant var;
		var.setValue(item);
		return var;
	}

	return QVariant();
}

// -------------------------------------------------------------------------------------------------------------------

QString ProviderTable::text(int row, int column, const ProviderItem& item) const
{
	Q_UNUSED(item);

	if (row < 0 || row >= m_providerList.count())
	{
		return QString();
	}

	if (column < 0 || column > PROVIDER_COLUMN_COUNT)
	{
		return QString();
	}

	QString result;

//	switch (column)
//	{
//		case PROVIDER_COLUMN_NAME:

//			result = item.provider().name() + QString("\n");

//			switch(item.order().state())
//			{
//				case Order::STATE_ORDER_OK:							result += tr("Your order has been accepted, Wellcome!");	break;
//				case Order::STATE_ORDER_CANCEL:						result += tr("Your order has been cancel, try later");	break;
//				case Order::STATE_ORDER_PROCESSING:					result += tr("Your order is processing by administrator");	break;
//				case Order::STATE_PROVIDER_IS_NOT_CONNECTED:		result += tr("Not connection with establishment");	break;
//				case Order::STATE_SERVER_CREATED_ORDER:				result += tr("Wait answer from establishment");	break;
//			}

//			break;

//		default:
//			assert(0);
//			break;
//	}

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
	verticalHeader()->hide();

	//verticalScrollBar()->setStyleSheet("background-color: rgb(255, 160, 160);");
	//verticalScrollBar()->setStyleSheet("width: 5;");

	setSelectionMode(QAbstractItemView::SingleSelection);

	ProviderDelegate* textDelegate = new ProviderDelegate(this);
	setItemDelegateForColumn(PROVIDER_COLUMN_NAME, textDelegate);


	QFont* listFont =  new QFont("Arial", 14, 2);
	setFont(*listFont);

	QSize cellSize = QFontMetrics(font()).size(Qt::TextSingleLine,"A");
	verticalHeader()->setDefaultSectionSize(cellSize.height() * 3);
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
		if (provider.isEmpty() == true)
		{
			continue;
		}

		if (m_textFilter.isEmpty() == false)
		{
			QString name = provider.name().toUpper();
			if (name.indexOf(m_textFilter) == -1)
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
				QMessageBox::information(this, tr("Отмена заказа"), tr("\"%1\" отменили ваш заказ").arg(provider.name()));
			}

			break;
		}
	}
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderView::setTextFilter(const QString& text)
{
	setFilter(text);
	updateList();
}

// -------------------------------------------------------------------------------------------------------------------

bool ProviderView::event(QEvent* e)
{
	if (e->type() == QEvent::Resize)
	{
		QResizeEvent* resizeEvent = static_cast<QResizeEvent*>(e);

		setColumnWidth(PROVIDER_COLUMN_NAME, resizeEvent->size().width() - QApplication::screens().at(0)->logicalDotsPerInch() / 2.4);
	}

	return QTableView::event(e);
}

// -------------------------------------------------------------------------------------------------------------------
