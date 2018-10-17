#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QStackedWidget>
#include <QMainWindow>
#include <QLineEdit>

#include "Options.h"
#include "ConfigSocket.h"
#include "OrderStateSocket.h"
#include "ProviderStateSocket.h"
#include "ProviderView.h"
#include "OrderDialog.h"
#include "OptionsDialog.h"
#include "AppAboutDialog.h"

#include "../lib/Order.h"

// ==============================================================================================

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	virtual ~MainWindow();

private:

	ConfigSocket*			m_pConfigSocket = nullptr;
	ProviderStateSocket*	m_pProviderStateSocket = nullptr;
	OrderStateSocket*		m_pOrderStateSocket = nullptr;

private:

	// Actions of main menu
	//
							// menu - Order
							//
	QAction*				m_pSearchAction = nullptr;
	QAction*				m_pOrderTableAction = nullptr;
	QAction*				m_pOrderDinnerAction = nullptr;
	QAction*				m_pCancelOrderAction = nullptr;
	QAction*				m_pOptionsAction = nullptr;
	QAction*				m_pInfoAction = nullptr;

private:

	QStackedWidget*			m_pStackedWidget = nullptr;

	// Elements of interface - Order
	//
	QMenu*					m_pOrderMenu = nullptr;

	// Elements of interface - ToolBar
	//
	QToolBar*				m_pOrderControlToolBar = nullptr;
	QLineEdit*				m_searchProvider  = nullptr;
	QLabel*					m_connectLabel = nullptr;

	// Elements of interface - view
	//
	ProviderView*			m_pView = nullptr;
	OrderDialog*			m_pOrderDialog = nullptr;
	OptionsDialog*			m_pOptionsDialog = nullptr;
	AppAboutDialog*			m_pAppAboutDialog = nullptr;

public:

	void					loadOrders();
	bool					createInterface();

	void					createActions();
	bool					createToolBars();
	ProviderView*			createProviderView();

	bool					startConfigUdpThread();
	bool					stopConfigUdpThread();

	bool					startProviderStateUdpThread();
	bool					stopProviderStateUdpThread();

	bool					startOrderStateUdpThread();
	bool					stopOrderStateUdpThread();

	bool					createOrder(int type);

protected:

	void					closeEvent(QCloseEvent* e);

signals:

	void					setTextFilter(const QString& text);

	void					requestProviderState(int index);

	void					requestCreateOrder(const Order::Item& order);
	void					requestRemoveOrder(const Order::Item& order);

private slots:

	// Slots of main menu
	//

	// menu - Order
	//

	void					onSearch();
	void					onSearchTextChanged(const QString& text);
	void					onOrderTable();
	void					onOrderDinner();
	void					onOrderCancel();
	void					onOptions();
	void					onAppAbout();

	// Slots of widgets
	//
	void					sendRequestCreateOrder();
	void					onSetMainWidget();

	// Slots of view
	//
	void					onProviderListClick(const QModelIndex& index);

	// Slots of sockets
	//
	void					cfgXmlReceived(const QByteArray& cfgXmlData, int version);
	void					orderStateChanged(const Order::Item& order);
	void					providerStateChanged(quint32 providerID, quint32 state);
};

// ==============================================================================================

#endif // MAINWINDOW_H
