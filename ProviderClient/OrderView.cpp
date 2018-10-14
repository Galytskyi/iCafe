#include "OrderView.h"

#include <QApplication>
#include <QScreen>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QEvent>
#include <QResizeEvent>
#include <QPainter>

#include "Options.h"

#include "../lib/wassert.h"

// -------------------------------------------------------------------------------------------------------------------
//
// OrderItem
//
// -------------------------------------------------------------------------------------------------------------------

OrderDelegate::OrderDelegate(QObject *parent) :
	QStyledItemDelegate(parent)
{
	m_dpi = QApplication::screens().at(0)->logicalDotsPerInch() / 1.5;
	m_iconSize = m_dpi / 1.5;
	m_iconSmallSize = m_iconSize / 2;

	m_providerNameFont = new QFont("Arial", 14, 10);
	m_providerAddressFont = new QFont("Arial", 10, 10);

	m_dotsBlackPixmap = QPixmap(":/icons/MenuBlack.png");
	m_dotsGrayPixmap = QPixmap(":/icons/MenuGray.png");
	m_tablePixmap = QPixmap(":/icons/Table.png");
	m_dinnerPixmap = QPixmap(":/icons/Dinner.png");
}

// -------------------------------------------------------------------------------------------------------------------

void OrderDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Order::Item order = qvariant_cast<Order::Item>(index.data(Qt::UserRole));

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

	if (theOptions.isWinApp() == false)
	{

	}
	else
	{
		QString orderState;
		QString orderPhone;

		switch(order.state())
		{
			case Order::STATE_ORDER_OK:

				orderState = tr("Заказ принят");
				orderPhone = tr("Номер телефона: %1").arg("+380"+QString::number(order.phone()));
				painter->fillRect(option.rect, QColor(0xA0, 0xFF, 0xA0));
				break;

			case Order::STATE_ORDER_CANCEL:
				orderState = tr("Заказ отменен");
				orderPhone = tr("Номер телефона: %1").arg("+380"+QString::number(order.phone()));
				painter->fillRect(option.rect, QColor(0xFF, 0xA0, 0xA0));
				break;

			case Order::STATE_ORDER_PROCESSING:
				orderState = tr("Ожидает ответа");
				orderPhone= tr("Номер телефона: +38 (***) **-**-***");
				painter->fillRect(option.rect, QColor(0xff, 0xfa, 0xd1));
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

		// order data
		//
		QRect orderDataRect = option.rect;
		orderDataRect.adjust(m_dpi, 5, -m_dpi, 0);

		Order::Time32 orderTime = order.orderTime();

		QString orderDataStr = tr("%1 человек(a)").arg(order.people()) + tr(", на %1").arg(QString().sprintf("%02d:%02d", orderTime.hour, orderTime.minute));

		switch(order.type())
		{
			case Order::TYPE_TABLE:		orderDataStr.insert(0, tr("Столик для "));	break;
			case Order::TYPE_DINNER:	orderDataStr.insert(0, tr("Обед для "));	break;
			default:					wassert(0);									break;
		}

		painter->setFont(*m_providerNameFont);
		painter->setPen(QColor(0x0, 0x0, 0x0));
		painter->drawText(orderDataRect, Qt::AlignLeft, orderDataStr);

		QSize cellSize = QFontMetrics(*m_providerNameFont).size(Qt::TextSingleLine,"A");
		orderDataRect.adjust(0, cellSize.height() + 10, 0, 0);

		painter->setFont(*m_providerAddressFont);
		painter->setPen(QColor(0x70, 0x70, 0x70));
		painter->drawText(orderDataRect, Qt::AlignLeft, orderPhone );

		// order state
		//
		QRect stateOrderRect = orderDataRect;

		stateOrderRect.adjust(0, 10,0,0);
		stateOrderRect.setLeft(option.rect.right() - 500);
		stateOrderRect.setRight(option.rect.right() - 30);

		painter->setPen(QColor(0x40, 0x40, 0x40));
		painter->drawText(stateOrderRect, Qt::AlignRight, orderState);

		// Order time
		//
		if (orderState.isEmpty() == false)
		{
			int icon_x = m_dpi / 2 - m_iconSize / 2;

			switch (order.type())
			{
				case Order::TYPE_TABLE:		painter->drawPixmap(icon_x, option.rect.y() + 3, m_iconSize, m_iconSize, m_tablePixmap);	break;
				case Order::TYPE_DINNER:	painter->drawPixmap(icon_x, option.rect.y() + 3 , m_iconSize, m_iconSize, m_dinnerPixmap);	break;
			}

			QRect timeOrderRect = stateOrderRect;

			timeOrderRect.setLeft(0);
			timeOrderRect.setRight(m_dpi);

			Order::Time32 orderTime = order.orderTime();
			QString orderTimeStr = QString().sprintf("%02d:%02d", orderTime.hour, orderTime.minute);

			painter->setPen(QColor(0x40, 0x40, 0x40));
			painter->drawText(timeOrderRect, Qt::AlignCenter, orderTimeStr);
		}

	}

}

// -------------------------------------------------------------------------------------------------------------------
//
// OrderTable
//
// -------------------------------------------------------------------------------------------------------------------

OrderTable::OrderTable(QObject*)
{
}

// -------------------------------------------------------------------------------------------------------------------

OrderTable::~OrderTable()
{
	m_orderList.clear();
}

// -------------------------------------------------------------------------------------------------------------------

int OrderTable::columnCount(const QModelIndex&) const
{
	return ORDER_COLUMN_COUNT;
}

// -------------------------------------------------------------------------------------------------------------------

int OrderTable::rowCount(const QModelIndex&) const
{
	return m_orderList.count();
}

// -------------------------------------------------------------------------------------------------------------------

QVariant OrderTable::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
	{
		return QVariant();
	}

	QVariant result = QVariant();

	if (orientation == Qt::Horizontal)
	{
		if (section >= 0 && section < ORDER_COLUMN_COUNT)
		{
			result = OrderColumn[section];
		}
	}

	if (orientation == Qt::Vertical)
	{
		result = QString("%1").arg(section + 1);
	}

	return result;
}

// -------------------------------------------------------------------------------------------------------------------

QVariant OrderTable::data(const QModelIndex &index, int role) const
{
	if (index.isValid() == false)
	{
		return QVariant();
	}

	int row = index.row();
	if (row < 0 || row >= m_orderList.count())
	{
		return QVariant();
	}

	int column = index.column();
	if (column < 0 || column > ORDER_COLUMN_COUNT)
	{
		return QVariant();
	}

	Order::Item order = m_orderList.at(row);
	if (order.isEmpty() == true || order.isValid() == false)
	{
		return QVariant();
	}

	if (role == Qt::UserRole)
	{
		QVariant var;
		var.setValue(order);
		return var;
	}

	return QVariant();
}

// -------------------------------------------------------------------------------------------------------------------

QString OrderTable::text(int row, int column, const Order::Item& order) const
{
	if (row < 0 || row >= m_orderList.count())
	{
		return QString();
	}

	if (column < 0 || column > ORDER_COLUMN_COUNT)
	{
		return QString();
	}

	QString result;

	switch (column)
	{
		case ORDER_COLUMN_NAME:
			{
				Order::Time32 orderTime = order.orderTime();

				result = tr("  %1 человек").arg(order.people()) + tr(", на: %1").arg(QString().sprintf("%02d:%02d", orderTime.hour, orderTime.minute)) + QString("\n");

				if (order.state() == Order::STATE_ORDER_OK)
				{
					result += tr("  Номер телеофна: %1").arg(QString::number(order.phone()));
				}
				else
				{
					result += tr("  Номер телефона: +38 (***) **-**-***");
				}
			}
			break;

		default:
			wassert(0);
			break;
	}

	return result;
}

// -------------------------------------------------------------------------------------------------------------------

void OrderTable::updateRow(int row)
{
	if (row < 0 || row >= m_orderList.count())
	{
		return;
	}

	for (int column = 0; column < ORDER_COLUMN_COUNT; column ++)
	{
		QModelIndex cellIndex = index(row, column);

		emit dataChanged(cellIndex, cellIndex, QVector<int>() << Qt::DisplayRole);
	}
}

// -------------------------------------------------------------------------------------------------------------------

void OrderTable::updateColumn(int column)
{
	if (column < 0 || column >= ORDER_COLUMN_COUNT)
	{
		return;
	}

	int count = m_orderList.count();
	for (int row = 0; row < count; row ++)
	{
		QModelIndex cellIndex = index(row, column);

		emit dataChanged(cellIndex, cellIndex, QVector<int>() << Qt::DisplayRole);
	}
}

// -------------------------------------------------------------------------------------------------------------------

Order::Item OrderTable::order(int index) const
{
	if (index < 0 || index >= count())
	{
		return Order::Item();
	}

	return m_orderList[index];
}

// -------------------------------------------------------------------------------------------------------------------

void OrderTable::set(int index, const Order::Item& order)
{
	if (index < 0 || index >= count())
	{
		return;
	}

	m_orderList[index] = order;
}

// -------------------------------------------------------------------------------------------------------------------

void OrderTable::set(const QList<Order::Item>& list_add)
{
	int count = list_add.count();
	if (count == 0)
	{
		return;
	}

	beginInsertRows(QModelIndex(), 0, count - 1);

		m_orderList = list_add;

	endInsertRows();
}

// -------------------------------------------------------------------------------------------------------------------

void OrderTable::append(const Order::Item &order)
{
	if (order.isEmpty() == true || order.isValid() == false)
	{
		return;
	}

	beginInsertRows(QModelIndex(), 0, 0);

		m_orderList.append(order);

	endInsertRows();
}

// -------------------------------------------------------------------------------------------------------------------

void OrderTable::remove(int index)
{
	if (index < 0 || index >= count())
	{
		return;
	}

	beginRemoveRows(QModelIndex(), index, index);

		m_orderList.removeAt(index);

	endRemoveRows();
}

// -------------------------------------------------------------------------------------------------------------------

void OrderTable::clear()
{
	int count = m_orderList.count();
	if (count == 0)
	{
		return;
	}

	beginRemoveRows(QModelIndex(), 0, count - 1);

		m_orderList.clear();

	endRemoveRows();
}

// -------------------------------------------------------------------------------------------------------------------
//
// OrderView
//
// -------------------------------------------------------------------------------------------------------------------

OrderView::OrderView()
{
	setModel(&m_table);

	horizontalHeader()->hide();
	verticalHeader()->hide();

	setSelectionMode(QAbstractItemView::SingleSelection);

	OrderDelegate* textDelegate = new OrderDelegate(this);
	setItemDelegateForColumn(ORDER_COLUMN_NAME, textDelegate);

	QFont* listFont = new QFont("Arial", 14, 2);
	setFont(*listFont);

	QSize cellSize = QFontMetrics(font()).size(Qt::TextSingleLine,"A");
	verticalHeader()->setDefaultSectionSize(cellSize.height()*3);

	m_table.set(theOrderBase.orderList());
}

// -------------------------------------------------------------------------------------------------------------------

OrderView::~OrderView()
{
}

// -------------------------------------------------------------------------------------------------------------------

void OrderView::appentToList(const Order::Item& order)
{
	m_table.append(order);
}

// -------------------------------------------------------------------------------------------------------------------

void OrderView::changeState(const Order::Item& order)
{
	int count = m_table.count();
	for(int i = 0; i < count; i++)
	{
		if (m_table.order(i).handle().ID == order.handle().ID)
		{
			m_table.set(i, order);
			m_table.updateRow(i);

			break;
		}
	}
}

// -------------------------------------------------------------------------------------------------------------------

void OrderView::removeFromList(const Order::Item& order)
{
	int count = m_table.count();
	for(int i = 0; i < count; i++)
	{
		if (m_table.order(i).handle().ID == order.handle().ID)
		{
			m_table.remove(i);

			break;
		}
	}
}

// -------------------------------------------------------------------------------------------------------------------

bool OrderView::event(QEvent* e)
{
	if (e->type() == QEvent::Resize)
	{
		QResizeEvent* resizeEvent = static_cast<QResizeEvent*>(e);

		setColumnWidth(ORDER_COLUMN_NAME, resizeEvent->size().width());
	}

	return QTableView::event(e);
}

// -------------------------------------------------------------------------------------------------------------------
