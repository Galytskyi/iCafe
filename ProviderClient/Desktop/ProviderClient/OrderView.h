#ifndef ORDERVIEW_H
#define ORDERVIEW_H

#include <QTableView>
#include <QStyledItemDelegate>
#include <QMutex>

#include "../../../lib/Order.h"

// ==============================================================================================

const char* const		OrderColumn[] =
{
						QT_TRANSLATE_NOOP("OrderView.h", "Order"),
};

const int				ORDER_COLUMN_COUNT	= sizeof(OrderColumn)/sizeof(OrderColumn[0]);

const int				ORDER_COLUMN_NAME	= 0;

// ==============================================================================================

const int				ORDER_ICON_SIZE			= 16;

// ----------------------------------------------------------------------------------------------

class OrderDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	OrderDelegate(QObject *parent);

private:

	QFont* m_orderNameFont;
	QFont* m_OrderPhoneFont;

public:

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

// ==============================================================================================

class OrderTable : public QAbstractTableModel
{
	Q_OBJECT

public:

	explicit OrderTable(QObject* parent = 0);
	virtual ~OrderTable();

private:

	QList<Order::Item>	m_orderList;

	int					columnCount(const QModelIndex &parent) const;
	int					rowCount(const QModelIndex &parent=QModelIndex()) const;

	QVariant			headerData(int section,Qt::Orientation orientation, int role=Qt::DisplayRole) const;
	QVariant			data(const QModelIndex &index, int role) const;

public:

	int					count() const { return m_orderList.count(); }

	void				append(const Order::Item &order);
	void				remove(int index);

	Order::Item			order(int index) const;
	void				set(int index, const Order::Item& order);
	void				set(const QList<Order::Item>& list_add);

	void				clear();

	QString				text(int row, int column, const Order::Item& order) const;
	void				updateColumn(int column);
};

// ==============================================================================================

class OrderView : public QTableView
{
	Q_OBJECT

private:

	OrderTable	m_table;

public:

	OrderView();
	virtual ~OrderView();

	OrderTable&		table() { return m_table; }

	QTimer			m_timer;

protected:

	bool			event(QEvent* e);

signals:

	void			orderStateChanged(const Order::Item& order);

public slots:

	// slots for updating
	//
	void			appentToList(const Order::Item& order);
	void			removeFromList(const Order::Item& order);

	void			updateLiveTime();

	// slots for click
	//
	void			onClick(const QModelIndex& index);
};

// ==============================================================================================

#endif // ORDERVIEW_H
