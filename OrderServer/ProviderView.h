#ifndef PROVIDERVIEW_H
#define PROVIDERVIEW_H

#include <QTableView>
#include <QStyledItemDelegate>
#include <QMutex>

#include "../lib/Provider.h"
#include "../lib/Order.h"


// ==============================================================================================

const char* const		ProviderColumn[] =
{
						QT_TRANSLATE_NOOP("ProviderView.h", "ID"),
						QT_TRANSLATE_NOOP("ProviderView.h", "Active"),
						QT_TRANSLATE_NOOP("ProviderView.h", "Accept orders"),
						QT_TRANSLATE_NOOP("ProviderView.h", "Enable dinner"),
						QT_TRANSLATE_NOOP("ProviderView.h", "Name"),
						QT_TRANSLATE_NOOP("ProviderView.h", "Address"),
						QT_TRANSLATE_NOOP("ProviderView.h", "Phone"),
};

const int				PROVIDER_COLUMN_COUNT		= sizeof(ProviderColumn)/sizeof(ProviderColumn[0]);

const int				PROVIDER_COLUMN_ID			= 0,
						PROVIDER_COLUMN_ACTIVE		= 1,
						PROVIDER_COLUMN_ACCEPT_ORDERS	= 2,
						PROVIDER_COLUMN_ENABLE_DINNER	= 3,
						PROVIDER_COLUMN_NAME		= 4,
						PROVIDER_COLUMN_ADDRESS		= 5,
						PROVIDER_COLUMN_PHONE		= 6;

const int				ProviderColumnWidth[PROVIDER_COLUMN_COUNT] =
{
						 50, //PROVIDER_COLUMN_ID
						120, //PROVIDER_COLUMN_ACTIVE
						 80, //PROVIDER_COLUMN_TAKE_ORDERS
						 80, //PROVIDER_COLUMN_TAKE_DINNER
						150, //PROVIDER_COLUMN_NAME
						200, //PROVIDER_COLUMN_ADDRESS
						100, //PROVIDER_COLUMN_PHONE
};

// ==============================================================================================

class ProviderTable : public QAbstractTableModel
{
	Q_OBJECT

public:

	explicit ProviderTable(QObject* parent = 0);
	virtual ~ProviderTable();

private:

	QList<Provider::Item>	m_providerList;

	int					columnCount(const QModelIndex &parent) const;
	int					rowCount(const QModelIndex &parent=QModelIndex()) const;

	QVariant			headerData(int section,Qt::Orientation orientation, int role=Qt::DisplayRole) const;
	QVariant			data(const QModelIndex &index, int role) const;

public:

	int					count() const { return m_providerList.count(); }
	void				clear();

	void				append(const Provider::Item& item);

	Provider::Item		at(int index) const;

	void				set(int index, const Provider::Item& item);
	void				set(const QList<Provider::Item> &list_add);

	void				remove(int index);

	QString				text(int row, int column, const Provider::Item& provider) const;
	void				updateRow(int row);
	void				updateColumn(int column);
};

// ==============================================================================================

class ProviderView : public QTableView
{
	Q_OBJECT

private:

	ProviderTable	m_table;

public:

	ProviderView();
	virtual ~ProviderView();

	ProviderTable&		table() { return m_table; }

public slots:

	// slots for updating
	//
	void				updateList();
};

// ==============================================================================================

#endif // PROVIDERVIEW_H
