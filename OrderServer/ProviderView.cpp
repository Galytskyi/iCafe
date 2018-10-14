#include "ProviderView.h"

#include <QApplication>
#include <QScreen>
#include <QScrollBar>
#include <QHeaderView>
#include <QEvent>
#include <QResizeEvent>
#include <QPainter>
#include <QMessageBox>

#include "../lib/wassert.h"

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

	Provider::Item provider = m_providerList.at(row);
	if (provider.isEmpty() == true)
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

		switch (column)
		{
			case PROVIDER_COLUMN_ACTIVE:

				if (provider.isActive() == false)
				{
					result = QColor(0xF0, 0xA0, 0xA0);
				}
				else
				{
					result = QColor(0xA0, 0xF0, 0xA0);
				}

				break;

			case PROVIDER_COLUMN_ACCEPT_ORDERS:

				if (provider.enableAcceptOrder() == true)
				{
					return QColor(0xA0, 0xF0, 0xA0);
				}

				break;

			case PROVIDER_COLUMN_ENABLE_DINNER:

				if (provider.enableDinner() == true)
				{
					return QColor(0xA0, 0xF0, 0xA0);
				}

				break;


			default:
				result = QVariant();
				break;
		}

		return result;
	}

	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		return text(row, column, provider);
	}

	return QVariant();
}

// -------------------------------------------------------------------------------------------------------------------

QString ProviderTable::text(int row, int column, const Provider::Item& provider) const
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
		case PROVIDER_COLUMN_ID:			result = QString::number(provider.providerID());					break;
		case PROVIDER_COLUMN_ACTIVE:		result = provider.isActive() ? provider.activeTime() : tr("No");	break;
		case PROVIDER_COLUMN_ACCEPT_ORDERS:	result = provider.enableAcceptOrder() ? "Yes" : tr("No");			break;
		case PROVIDER_COLUMN_ENABLE_DINNER:	result = provider.enableDinner() ? "Yes" : tr("No");				break;
		case PROVIDER_COLUMN_NAME:			result = provider.name();											break;
		case PROVIDER_COLUMN_ADDRESS:		result = provider.address();										break;
		case PROVIDER_COLUMN_PHONE:			result = provider.phone();											break;
		default:							wassert(0);															break;
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

void ProviderTable::append(const Provider::Item& item)
{
	if (item.isEmpty() == true)
	{
		return;
	}

	int index = m_providerList.count();

	beginInsertRows(QModelIndex(), index, index);

		m_providerList.append(item);

	endInsertRows();
}

// -------------------------------------------------------------------------------------------------------------------

Provider::Item ProviderTable::at(int index) const
{
	if (index < 0 || index >= count())
	{
		return Provider::Item();
	}

	return m_providerList.at(index);
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderTable::set(int index, const Provider::Item& item)
{
	if (index < 0 || index >= count())
	{
		return;
	}

	m_providerList[index] = item;
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderTable::set(const QList<Provider::Item>& list_add)
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

void ProviderTable::remove(int index)
{
	if (index < 0 || index >= m_providerList.count())
	{
		return;
	}

	beginRemoveRows(QModelIndex(), index, index);

		m_providerList.removeAt(index);

	endRemoveRows();
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

	QSize cellSize = QFontMetrics(font()).size(Qt::TextSingleLine,"A");
	verticalHeader()->setDefaultSectionSize(cellSize.height()+5);

	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::SingleSelection);

	verticalHeader()->hide();

	for(int column = 0; column < PROVIDER_COLUMN_COUNT; column++)
	{
		setColumnWidth(column, ProviderColumnWidth[column]);
	}
}

// -------------------------------------------------------------------------------------------------------------------

ProviderView::~ProviderView()
{
}

// -------------------------------------------------------------------------------------------------------------------

void ProviderView::updateList()
{
	m_table.clear();

	QList<Provider::Item>	list;

	int count = theProviderBase.count();
	for(int i = 0; i < count; i++)
	{
		Provider::Item provider = theProviderBase.provider(i);
		if (provider.isEmpty() == true)
		{
			continue;
		}

		list.append(provider);
	}

	m_table.set(list);
}

// -------------------------------------------------------------------------------------------------------------------
