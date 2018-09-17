#include "OrderView.h"

#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QEvent>
#include <QResizeEvent>
#include <QPainter>
#include <assert.h>

// -------------------------------------------------------------------------------------------------------------------
//
// OrderItem
//
// -------------------------------------------------------------------------------------------------------------------

OrderDelegate::OrderDelegate(QObject *parent) :
	QStyledItemDelegate(parent)
{

	m_orderNameFont = new QFont("Arial", 14, 10);
	m_OrderPhoneFont = new QFont("Arial", 10, 10);

	//m_providerNameFont->setBold(true);
}

// -------------------------------------------------------------------------------------------------------------------

void OrderDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QRect textRect = option.rect;
	QRect textOrder = option.rect;
	QRect timeOrder = option.rect;
	textRect.adjust(60, 5, 0, 0);


	Order::Item order = qvariant_cast<Order::Item>(index.data(Qt::UserRole));

	QPixmap infoPixmap(":/icons/MenuBlack.png");
	QPixmap clockPixmap(":/icons/Clock.png");

	int midInfo_x = option.rect.right() - ORDER_ICON_SIZE - 20;
	int midInfo_y = option.rect.top() + 10;


	QSize cellSize = QFontMetrics(*m_orderNameFont).size(Qt::TextSingleLine,"A");


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


	switch(order.state())
		{
			case Order::STATE_ORDER_OK:							painter->fillRect(option.rect, QColor(0xA0, 0xFF, 0xA0));	break;
			case Order::STATE_ORDER_CANCEL:						painter->fillRect(option.rect, QColor(0xFF, 0xA0, 0xA0));	break;
			case Order::STATE_ORDER_PROCESSING:					painter->fillRect(option.rect, QColor(0xff, 0xf4, 0xc4));	break;
		}

	painter->drawPixmap(midInfo_x, midInfo_y, ORDER_ICON_SIZE, ORDER_ICON_SIZE, infoPixmap);

	Order::Time32 orderTime = order.orderTime();

	QString orderstr = tr("%1 people").arg(order.people()) + tr(", at the: %1").arg(QString().sprintf("%02d:%02d", orderTime.hour, orderTime.minute));

	switch(order.type())
	{
		case Order::TYPE_TABLE:		orderstr.insert(0, tr("Table for "));	break;
		case Order::TYPE_DINNER:	orderstr.insert(0, tr("Dinner for "));	break;
		default:					assert(0);								break;
	}

	painter->setFont(*m_orderNameFont);
	painter->setPen(QColor(0x0, 0x0, 0x0));
	painter->drawText(textRect, Qt::AlignLeft, orderstr);

	QString orderphone;

	if (order.state() == Order::STATE_ORDER_OK || order.state() == Order::STATE_ORDER_CANCEL)
	{
		orderphone = tr("Phone number: %1").arg(QString::number(order.phone()));
	}
	else
	{
		orderphone= tr("Phone number: +38 (***) **-**-***");
	}


	textRect.adjust(0, cellSize.height() + 5, 0, 0);

	painter->setFont(*m_OrderPhoneFont);
	painter->setPen(QColor(0x70, 0x70, 0x70));
	painter->drawText(textRect, Qt::AlignLeft, orderphone );


	QString orderState;

	switch(order.state())
	{
		case Order::STATE_ORDER_OK:							orderState = tr("Order accepted");	break;
		case Order::STATE_ORDER_CANCEL:						orderState = tr("Order canceled");	break;
		case Order::STATE_ORDER_PROCESSING:					orderState = tr("Wait answer");	break;
	}

//	painter->setPen(Qt::gray);


	//painter->setFont(*m_providerNameFont);


	textOrder.adjust(00, 45, 0, 0);
	textOrder.setRight( textOrder.right() - 30 );

	painter->setPen(QColor(0x40, 0x40, 0x40));
	painter->drawText(textOrder, Qt::AlignRight, orderState);


	switch(order.state())
	{
		case Order::STATE_ORDER_OK:
		case Order::STATE_ORDER_PROCESSING:

			Order::Time32 orderTime = order.orderTime();

			QString orderTime1 = QString().sprintf("%02d:%02d", orderTime.hour, orderTime.minute);

			painter->drawPixmap(option.rect.x() + 10, option.rect.y() + 10 , 32, 32, clockPixmap);

			timeOrder.adjust(10, 45, 0, 0);
			timeOrder.setRight( timeOrder.right() - 25 );

			painter->setPen(QColor(0x40, 0x40, 0x40));
			painter->drawText(timeOrder, Qt::AlignLeft, orderTime1);



			break;
	}


	painter->drawPixmap(midInfo_x, midInfo_y, ORDER_ICON_SIZE, ORDER_ICON_SIZE, infoPixmap);
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

	if (role == Qt::TextAlignmentRole)
	{
		return Qt::AlignLeft;
	}

	if (role == Qt::TextColorRole)
	{
		return QVariant();
	}

	if (role == Qt::BackgroundColorRole)
	{
		QVariant result = QVariant();

		switch(order.state())
		{
			case Order::STATE_ORDER_OK:			result = QColor(0xA0, 0xFF, 0xA0);	break;
			case Order::STATE_ORDER_CANCEL:		result = QColor(Qt::gray);			break;
			case Order::STATE_ORDER_PROCESSING:	result = QColor(0xFF, 0xA0, 0xA0);	break;
			default:							result = QVariant();				break;
		}

		return result;
	}

	if (role == Qt::UserRole)
	{
		QVariant var;
		var.setValue(order);
		return var;
	}

	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		return text(row, column, order);
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

				result = tr("  %1 people").arg(order.people()) + tr(", at the: %1").arg(QString().sprintf("%02d:%02d", orderTime.hour, orderTime.minute)) + QString("\n");

				if (order.state() == Order::STATE_ORDER_OK)
				{
					result += tr("  Phone number: %1").arg(QString::number(order.phone()));
				}
				else
				{
					result += tr("  Phone number: +38 (***) **-**-***");
				}
			}
			break;

		default:
			assert(0);
			break;
	}

	return result;
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
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::SingleSelection);

	OrderDelegate* textDelegate = new OrderDelegate(this);
	setItemDelegateForColumn(ORDER_COLUMN_NAME, textDelegate);

	QFont* listFont = new QFont("Arial", 14, 2);
	setFont(*listFont);

	QSize cellSize = QFontMetrics(font()).size(Qt::TextSingleLine,"A");
	verticalHeader()->setDefaultSectionSize(cellSize.height()*3);

	connect(this, &QTableView::clicked, this, &OrderView::onClick);

	connect(&m_timer, &QTimer::timeout, this, &OrderView::updateLiveTime);
	m_timer.start(500);		//	500 ms

	m_table.set(theOrderBase.orderList());
}

// -------------------------------------------------------------------------------------------------------------------

OrderView::~OrderView()
{
	m_timer.stop();
}

// -------------------------------------------------------------------------------------------------------------------

void OrderView::appentToList(const Order::Item& order)
{
	m_table.append(order);
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

void OrderView::updateLiveTime()
{
	//m_table.updateColumn(ORDER_COLUMN_LIVE_TIME);
}

// -------------------------------------------------------------------------------------------------------------------

void OrderView::onClick(const QModelIndex& index)
{
	int i = index.row();

	if (i < 0 || i >= m_table.count() )
	{
		return;
	}

	Order::Item order = m_table.order(i);

	switch (order.state())
	{
		case Order::STATE_ORDER_OK:
			{

				QMessageBox::StandardButton reply;

				reply = QMessageBox::question(this, tr("Cancel order"), tr("Do you want to cancel the order?\n\nTo cancel the order, please, call the client by phone: %1, and ask cancel code").arg(order.phone()).arg(order.phone()), QMessageBox::Yes|QMessageBox::No);
				if (reply == QMessageBox::No)
				{
					break;
				}


				// create dialog check cancel code
				//
				QDialog* dialog = new QDialog(this);
				dialog->setWindowFlags(Qt::Drawer);
				dialog->setFixedSize(200, 80);
				dialog->setWindowTitle(tr("Cancel code"));
				//dialog->move(this->geometry().center() - dialog->rect().center());


					// cancel code
					//
					QHBoxLayout *codeLayout = new QHBoxLayout ;

					QLabel* codeLabel = new QLabel;
					codeLabel->setText(tr("Cancel code:"));

					QLineEdit* codeEdit = new QLineEdit(this);

					QFont* listFont = new QFont("Arial", 14, 2);
					codeEdit->setFont(*listFont);

					QValidator *validator = new QIntValidator(1, 9999, this);
					codeEdit->setValidator(validator);

					codeLayout->addWidget(codeLabel);
					codeLayout->addWidget(codeEdit);

					// buttons
					//
					QHBoxLayout *buttonLayout = new QHBoxLayout ;

					QPushButton* okButton = new QPushButton(tr("OK"));
					QPushButton* cancelButton = new QPushButton(tr("Cancel"));

					connect(okButton, &QPushButton::clicked, dialog, &QDialog::accept);
					connect(cancelButton, &QPushButton::clicked, dialog, &QDialog::reject);

					buttonLayout->addWidget(okButton);
					buttonLayout->addWidget(cancelButton);

					// add layoyt
					//
					QVBoxLayout *mainLayout = new QVBoxLayout ;

					mainLayout->addLayout(codeLayout);
					mainLayout->addStretch();
					mainLayout->addLayout(buttonLayout);

				dialog->setLayout(mainLayout);

				if (dialog->exec() != QDialog::Accepted)
				{
					break;
				}

				int code = codeEdit->text().toInt();

				if (code  != order.cancelCode())
				{
					QMessageBox::information(this, tr("Cancel code"), tr("You had been input incorrect cancel code!"));
					break;
				}

				order.setState(Order::STATE_ORDER_CANCEL);
				m_table.set(i, order);

				emit orderStateChanged(order);
			}

			break;

		case Order::STATE_ORDER_PROCESSING:
			{
				QMessageBox::StandardButton reply;

				reply = QMessageBox::question(this, "Take order", "Do you want to take order?", QMessageBox::Yes|QMessageBox::No);

				if (reply == QMessageBox::Yes)
				{
					order.setState(Order::STATE_ORDER_OK);
				}
				else
				{
					order.setState(Order::STATE_ORDER_CANCEL);
				}

				m_table.set(i, order);

				emit orderStateChanged(order);
			}

			break;

		default:
			break;
	}
}

// -------------------------------------------------------------------------------------------------------------------

bool OrderView::event(QEvent* e)
{
	if (e->type() == QEvent::Resize)
	{
		QResizeEvent* resizeEvent = static_cast<QResizeEvent*>(e);

		setColumnWidth(ORDER_COLUMN_NAME, resizeEvent->size().width() - 20);
	}

	return QTableView::event(e);
}

// -------------------------------------------------------------------------------------------------------------------
