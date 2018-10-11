#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QMenuBar>
#include <QToolBar>
#include <QLabel>

#include "ArchThread.h"
#include "ConfigSocket.h"
#include "CustomerOrderSocket.h"
#include "ProviderOrderSocket.h"
#include "RemoveOrderThread.h"
#include "ProviderView.h"

// ==============================================================================================

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	virtual ~MainWindow();

private:

	// Actions of main menu
	//
							// menu - Providers
							//
	QAction*				m_pAppendProviderAction = nullptr;
	QAction*				m_pEditProviderAction = nullptr;
	QAction*				m_pRemoveProviderAction = nullptr;

	// Elements of interface - Menu
	//
	QMenu*					m_pProviderMenu = nullptr;
	QMenu*					m_pContextMenu = nullptr;

	// Elements of interface - ToolBar
	//
	QToolBar*				m_pControlToolBar = nullptr;

	// Elements of interface - view
	//
	ProviderView*			m_pView = nullptr;

	// Elements of interface - StatusBar
	//
	QLabel*					m_statusEmpty = nullptr;
	QLabel*					m_statusProviderCount = nullptr;
	QLabel*					m_statusOrderCount = nullptr;

private:

	ArchThread*				m_pArchThread = nullptr;

	ConfigSocket*			m_pConfigSocket = nullptr;
	CustomerOrderSocket*	m_pCustomerOrderSocket = nullptr;
	ProviderOrderSocket*	m_pProviderOrderSocket = nullptr;

	RemoveOrderThread*		m_pRemoveOrderThread = nullptr;

public:

	bool					createInterface();

	void					createActions();
	void					createMenu();
	bool					createToolBars();
	void					createProviderView();
	void					createStatusBar();
	void					createContextMenu();

	bool					startArchThread();
	bool					stopArchThread();

	bool					loadBase();
	bool					saveProviderBase();

	bool					startConfigUdpThread();
	bool					stopConfigUdpThread();

	bool					startCustomerOrderUdpThread();
	bool					stopCustomerOrderUdpThread();

	bool					startProviderOrderUdpThread();
	bool					stopProviderOrderUdpThread();

	bool					startRemoveOrderThread();
	bool					stopRemoveOrderThread();

protected:

	void					closeEvent(QCloseEvent* e);

signals:

	void					appendMessageToArch(int type, const QString& func, const QString& text);

public slots:

	//
	//
	void					msgBox(const QString &title, const QString& text);

	//
	//
	void					orderStateChanged(const Order::Item& order);

	// Slots of Main menu - Providers
	//
	void					onAppendProvider();
	void					onEditProvider();
	void					onRemoveProvider();

	// Slots of contex menu
	//
	void					onContextMenu(QPoint);

	// Slots of view
	//
	void					onProviderListClick(const QModelIndex& index);
};

// ==============================================================================================

#endif // MAINWINDOW_H
