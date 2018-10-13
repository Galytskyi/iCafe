#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QStackedWidget>
#include <QMainWindow>
#include <QLabel>

#include "Options.h"
#include "OrderReceiveSocket.h"
#include "OrderStateSocket.h"
#include "OrderView.h"
#include "CancelOrderDialog.h"
#include "OptionsDialog.h"
#include "AppAboutDialog.h"

#include "../lib/Provider.h"
#include "../lib/Order.h"

// ==============================================================================================

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

private:

	OrderReceiveSocket*		m_pOrderReceiveSocket = nullptr;
	OrderStateSocket*		m_pOrderStateSocket = nullptr;

	QTimer					m_timer;

private:

	// Actions of main menu
	//
							// menu - Order
							//
	QAction*				m_pEnableOrderAction = nullptr;
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

	QLabel*					m_connectLabel = nullptr;

	// Elements of interface - view
	//
	OrderView*				m_pView= nullptr;
	CanceOrderDialog*		m_pCancelOrderDialog = nullptr;
	OptionsDialog*			m_pOptionsDialog = nullptr;
	AppAboutDialog*			m_pAppAboutDialog = nullptr;

public:

	bool					createInterface();

	void					createActions();
	bool					createToolBars();
	OrderView*				createOrderView();

	bool					startOrderReceiveUdpThread();
	bool					stopOrderReceiveUdpThread();

	bool					startOrderStateUdpThread();
	bool					stopOrderStateUdpThread();

protected:

	void					closeEvent(QCloseEvent* e);

signals:

	void					sendRequestChangeOrderState(const Order::Item& order);
	void					sendRequestGetProviderState();
	void					sendRequestChangeProviderState(quint32 state);

private slots:

	// Slots of main menu
	//


	// Slots of widgets
	//
	void					onSetMainWidget();
	void					onSendRequestCancelOrder();

	// menu
	//
	void					onEnableTakeOrder();
	void					onOptions();
	void					onAppAbout();

	// Slots of view
	//
	void					onOrderListClick(const QModelIndex& index);

	// Slots of sockets
	//
	void					onSocketConnection(bool connect);
	void					appendOrder(const Order::Item& order);
	void					removeOrder(const Order::Item& order);
	void					orderStateChanged(const Order::Item& order);
	void					providerStateChanged(quint32 state);
};

// ==============================================================================================

#endif // MAINWINDOW_H
