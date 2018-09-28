#ifndef PROVIDERVIEW_H
#define PROVIDERVIEW_H

#include <QTableView>
#include <QStyledItemDelegate>
#include <QMutex>

#include "../../../lib/Provider.h"
#include "../../../lib/Order.h"


// ==============================================================================================

const char* const		ProviderColumn[] =
{
						QT_TRANSLATE_NOOP("ProviderView.h", "Еstablishment"),
};

const int				PROVIDER_COLUMN_COUNT		= sizeof(ProviderColumn)/sizeof(ProviderColumn[0]);

const int				PROVIDER_COLUMN_NAME		= 0;

// ==============================================================================================

class ProviderDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	ProviderDelegate(QObject *parent);

private:

	int					m_dpi = 0;
	int					m_iconSize = 0;
	int					m_iconSmallSize = 0;

	QFont*				m_providerNameFont = nullptr;
	QFont*				m_providerAddressFont = nullptr;

	QPixmap				m_dotsBlackPixmap;
	QPixmap				m_dotsGrayPixmap;
	QPixmap				n_tablePixmap;
	QPixmap				m_dinnerPixmap;

public:

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

// ==============================================================================================

class ProviderItem
{

public:

	ProviderItem();
	ProviderItem(const Provider::Item& provider);
	virtual ~ProviderItem();

private:

	Provider::Item		m_provider;
	Order::Item			m_order;

public:

	Provider::Item		provider() const { return m_provider; }
	void				setProvider(const Provider::Item& provider) { m_provider = provider; }

	Order::Item			order() const { return m_order; }
	void				setOrder(const Order::Item& order) { m_order = order; }

	ProviderItem&		operator=(const ProviderItem& from);
};

// ----------------------------------------------------------------------------------------------

Q_DECLARE_METATYPE(ProviderItem)	// for type QVariant

// ==============================================================================================

class ProviderTable : public QAbstractTableModel
{
	Q_OBJECT

public:

	explicit ProviderTable(QObject* parent = 0);
	virtual ~ProviderTable();

private:

	QList<ProviderItem>	m_providerList;

	int					columnCount(const QModelIndex &parent) const;
	int					rowCount(const QModelIndex &parent=QModelIndex()) const;

	QVariant			headerData(int section,Qt::Orientation orientation, int role=Qt::DisplayRole) const;
	QVariant			data(const QModelIndex &index, int role) const;

public:

	int					count() const { return m_providerList.count(); }
	ProviderItem		at(int index) const;
	void				set(int index, const ProviderItem& item);
	void				set(const QList<ProviderItem> &list_add);
	void				clear();

	QString				text(int row, int column, const ProviderItem& item) const;
	void				updateRow(int row);
	void				updateColumn(int column);
};

// ==============================================================================================

class ProviderView : public QTableView
{
	Q_OBJECT

private:

	ProviderTable	m_table;

	QString			m_textFilter;

public:

	ProviderView();
	virtual ~ProviderView();

	ProviderTable&		table() { return m_table; }

	QString				filter() const { return m_textFilter; }
	void				setFilter(const QString& text) { m_textFilter = text; }

protected:

	bool				event(QEvent* e);

public slots:

	// slots for updating
	//
	void				updateList();
	void				updateOrderList();
	void				orderStateChanged(const Order::Item& order);

	// slots for filter
	//
	void				setTextFilter(const QString& text);
};

// ==============================================================================================

#endif // PROVIDERVIEW_H
