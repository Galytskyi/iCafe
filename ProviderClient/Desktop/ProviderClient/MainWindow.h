#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>

#include "Options.h"
#include "OrderReceiveSocket.h"
#include "OrderStateSocket.h"
#include "OrderView.h"

#include "../../../lib/Order.h"

// ==============================================================================================

#include <QMainWindow>

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
	QAction*				m_pOptionsAction = nullptr;

private:

	// Elements of interface - Order
	//
	QMenu*					m_pOrderMenu = nullptr;
	QMenu*					m_pContextMenu = nullptr;

	// Elements of interface - ToolBar
	//
	QToolBar*				m_pOrderControlToolBar = nullptr;

	QLabel*					m_connectLabel = nullptr;

	// Elements of interface - view
	//
	OrderView*				m_pView= nullptr;

public:

	bool					createInterface();

	void					createActions();
	void					createMenu();
	bool					createToolBars();
	void					createOrderView();
	void					createStatusBar();
	void					createContextMenu();

	bool					startOrderReceiveUdpThread();
	bool					stopOrderReceiveUdpThread();

	bool					startOrderStateUdpThread();
	bool					stopOrderStateUdpThread();

protected:

	void					closeEvent(QCloseEvent* e);

signals:

private slots:

	// Slots of main menu
	//

	// menu - Order
	//
	void					onOptions();

	// Slots of contex menu
	//
	void					onContextMenu(QPoint);

	// Slots of sockets
	//
	void					onSocketConnection(bool connect);
};

// ==============================================================================================

#endif // MAINWINDOW_H
