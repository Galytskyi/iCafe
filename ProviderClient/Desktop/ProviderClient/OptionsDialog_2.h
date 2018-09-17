#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QList>
#include <QMap>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

#include "../qtpropertybrowser/src/qtpropertymanager.h"
#include "../qtpropertybrowser/src/qtvariantproperty.h"
#include "../qtpropertybrowser/src/qttreepropertybrowser.h"

#include "Options.h"

// ==============================================================================================

const char* const				OptionGroupTitle[] =
{
								QT_TRANSLATE_NOOP("OptionsDialog.h", "Connect to server"),
								QT_TRANSLATE_NOOP("OptionsDialog.h", "Provider data"),
								QT_TRANSLATE_NOOP("OptionsDialog.h", "Order view"),
								QT_TRANSLATE_NOOP("OptionsDialog.h", "Database"),
};

const int						OPTION_GROUP_COUNT			= sizeof(OptionGroupTitle)/sizeof(OptionGroupTitle[0]);

const int						OPTION_GROUP_UNKNOWN		= -1,
								OPTION_GROUP_SERVER			= 0,
								OPTION_GROUP_PROVIDER_DATA	= 1,
								OPTION_GROUP_ORDER_VIEW		= 2,
								OPTION_GROUP_DATABASE		= 3;

// ==============================================================================================

const char* const				OptionPageTitle[] =
{
								QT_TRANSLATE_NOOP("OptionsDialog.h", "Connection to Order Server - TCP/IP"),
								QT_TRANSLATE_NOOP("OptionsDialog.h", "Provider data"),
								QT_TRANSLATE_NOOP("OptionsDialog.h", "Displaying data in the list of orders"),
								QT_TRANSLATE_NOOP("OptionsDialog.h", "Database settings"),
};

const int						OPTION_PAGE_COUNT				= sizeof(OptionPageTitle)/sizeof(OptionPageTitle[0]);

const int						OPTION_PAGE_UNKNOWN				= -1,
								OPTION_PAGE_ORDER_SOCKET		= 0,
								OPTION_PAGE_PROVIDER_DATA		= 1,
								OPTION_PAGE_ORDER_VIEW			= 2,
								OPTION_PAGE_DATABASE			= 3;

// ----------------------------------------------------------------------------------------------

const char* const				OptionPageShortTitle[OPTION_PAGE_COUNT] =
{
								QT_TRANSLATE_NOOP("OptionsDialog.h", "OrderServer"),
								QT_TRANSLATE_NOOP("OptionsDialog.h", "Provider"),
								QT_TRANSLATE_NOOP("OptionsDialog.h", "Orders"),
								QT_TRANSLATE_NOOP("OptionsDialog.h", "Settings"),
};

// ----------------------------------------------------------------------------------------------

const int						OptionGroupPage[OPTION_PAGE_COUNT] =
{
								OPTION_GROUP_SERVER,		// Group: Connect to server --		Page : ConfigService"),
								OPTION_GROUP_PROVIDER_DATA,	// Group: Linearity --				Page : Measurements"),
								OPTION_GROUP_LINEARITY,		// Group: Linearity --				Page : Points"),
								OPTION_GROUP_SETTING,		// Group: Comparators --			Page : Measurements"),
								OPTION_GROUP_MEASURE_VIEW,	// Group: List of measurements --	Page : Display"),
								OPTION_GROUP_MEASURE_VIEW,	// Group: List of measurements --	Page : Columns"),
								OPTION_GROUP_SIGNAL_INFO,	// Group: Information of signal --	Page : Displaying"),
								OPTION_GROUP_DATABASE,		// Group: Database --				Page : Settings"),
								OPTION_GROUP_BACKUP,		// Group: Backup measurements --	Page : Settings"),
};

// ==============================================================================================

const int						PROPERTY_PAGE_TYPE_UNKNOWN	= -1,
								PROPERTY_PAGE_TYPE_LIST		= 0,
								PROPERTY_PAGE_TYPE_DIALOG	= 1;

const int						PROPERTY_PAGE_TYPE_COUNT	= 2;

// ----------------------------------------------------------------------------------------------

class PropertyPage : public QObject
{
	Q_OBJECT

public:

	PropertyPage(QtVariantPropertyManager* manager, QtVariantEditorFactory* factory, QtTreePropertyBrowser* editor);
	explicit PropertyPage(QDialog* dialog);
	virtual ~PropertyPage();

private:

	int							m_type = PROPERTY_PAGE_TYPE_UNKNOWN;

	QWidget*					m_pWidget = nullptr;

	// PROPERTY_PAGE_TYPE_LIST
	//
	QtVariantPropertyManager*	m_pManager = nullptr;
	QtVariantEditorFactory*		m_pFactory = nullptr;
	QtTreePropertyBrowser*		m_pEditor = nullptr;

	// PROPERTY_PAGE_TYPE_DIALOG
	//
	QDialog*					m_pDialog = nullptr;

public:

	QWidget*					getWidget() { return m_pWidget; }
	int							type() const { return m_type; }

	int							m_page = OPTION_PAGE_UNKNOWN;
	QTreeWidgetItem*			m_pTreeWidgetItem = nullptr;

	QtTreePropertyBrowser*		treeEditor() { return m_pEditor; }
};

// ==============================================================================================

class OptionsDialog : public QDialog
{
	Q_OBJECT

public:

	explicit OptionsDialog(QWidget *parent = 0);
	virtual ~OptionsDialog();

private:

	Options						m_options;

	static int					m_activePage;
	bool						setActivePage(int page);

	void						createInterface();

	QTreeWidget*				m_pPageTree = nullptr;
	QHBoxLayout*				m_pagesLayout = nullptr;
	QHBoxLayout*				m_buttonsLayout = nullptr;

	QHBoxLayout*				createPages();
	void						removePages();

	QHBoxLayout*				createButtons();

	QList<PropertyPage*>		m_pageList;

	PropertyPage*				createPage(int page);
	PropertyPage*				createPropertyList(int page);
	PropertyPage*				createPropertyDialog(int page);


	QMap<QtProperty*,int>		m_propertyItemList;
	QMap<QtProperty*,QVariant>	m_propertyValueList;

	void						appendProperty(QtProperty* property, int page, int param);
	void						expandProperty(QtTreePropertyBrowser* pEditor, int page, int param, bool expanded);
	void						clearProperty();

	QtProperty*					m_currentPropertyItem = nullptr;
	QVariant					m_currentPropertyValue = 0;

	void						restoreProperty();
	void						applyProperty();

	void						loadSettings();
	void						saveSettings();

protected:

	bool						event(QEvent * e);

private slots:

	void						onPageChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
	void						onPropertyValueChanged(QtProperty *property, const QVariant &value);

	void						onBrowserItem(QtBrowserItem*pItem);

	void						updateServerPage();
	void						updateLinearityPage(bool isDialog);
	void						updateMeasureViewPage(bool isDialog);

	void						onOk();
	void						onApply();
};

// ==============================================================================================

#endif // OPTIONSDIALOG_H
