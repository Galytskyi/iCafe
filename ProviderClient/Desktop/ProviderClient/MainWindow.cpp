#include "MainWindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QThread>
#include <QMenuBar>
#include <QToolBar>

#include "OptionsDialog.h"

// -------------------------------------------------------------------------------------------------------------------
//
// MainWindow class implementation
//
// -------------------------------------------------------------------------------------------------------------------

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	theOrderBase.readFromXml(QString("provider%1").arg(xmlOrderFileName));

	createInterface();

	startOrderReceiveUdpThread();
	startOrderStateUdpThread();
}

// -------------------------------------------------------------------------------------------------------------------

MainWindow::~MainWindow()
{
	stopOrderStateUdpThread();
	stopOrderReceiveUdpThread();
}

// -------------------------------------------------------------------------------------------------------------------

bool MainWindow::createInterface()
{
	setWindowIcon(QIcon(":/icons/Table.ico"));
	setWindowTitle(tr("Поставщик заказов"));
	resize(500, 500);
	move(QApplication::desktop()->availableGeometry().center() - rect().center());

	createActions();
	createMenu();
	createToolBars();
	createOrderView();
	createStatusBar();
	createContextMenu();

//	loadSettings();

	return true;
}

// -------------------------------------------------------------------------------------------------------------------

bool MainWindow::startOrderReceiveUdpThread()
{
	m_pOrderReceiveSocket = new OrderReceiveSocket(QHostAddress(theOptions.providerData().serverIP()), theOptions.providerData().serveProviderPort());
	if (m_pOrderReceiveSocket == nullptr)
	{
		return false;
	}

	QThread *pThread = new QThread;
	if (pThread == nullptr)
	{
		delete m_pOrderReceiveSocket;
		m_pOrderReceiveSocket = nullptr;

		return false;
	}

	m_pOrderReceiveSocket->moveToThread(pThread);

	//
	//
	connect(pThread, &QThread::started, m_pOrderReceiveSocket, &OrderReceiveSocket::slot_onThreadStarted);
	connect(pThread, &QThread::finished, m_pOrderReceiveSocket, &OrderReceiveSocket::slot_onThreadFinished);

	//
	//
	connect(m_pOrderReceiveSocket, &OrderReceiveSocket::socketConnection, this, &MainWindow::onSocketConnection, Qt::QueuedConnection);

	connect(m_pOrderReceiveSocket, &OrderReceiveSocket::appendOrderToBase, &theOrderBase, &Order::Base::slot_appendOrder, Qt::QueuedConnection);
	connect(m_pOrderReceiveSocket, &OrderReceiveSocket::appendOrderToBase, m_pView, &OrderView::appentToList, Qt::QueuedConnection);

	connect(m_pView, &OrderView::orderStateChanged, m_pOrderReceiveSocket, &OrderReceiveSocket::requestSetOrderState, Qt::QueuedConnection);
	//
	//
	pThread->start();

	return true;
}

// -------------------------------------------------------------------------------------------------------------------

bool MainWindow::stopOrderReceiveUdpThread()
{
	if (m_pOrderReceiveSocket == nullptr)
	{
		return false;
	}

	//
	//
	disconnect(m_pView, &OrderView::orderStateChanged, m_pOrderReceiveSocket, &OrderReceiveSocket::requestSetOrderState);

	disconnect(m_pOrderReceiveSocket, &OrderReceiveSocket::appendOrderToBase, m_pView, &OrderView::appentToList);
	disconnect(m_pOrderReceiveSocket, &OrderReceiveSocket::appendOrderToBase, &theOrderBase, &Order::Base::slot_appendOrder);
	//
	//
	QThread *pThread = m_pOrderReceiveSocket->thread();
	if (pThread == nullptr)
	{
		return false;
	}

	pThread->quit();
	pThread->wait();
	pThread->deleteLater();

	return true;
}


// -------------------------------------------------------------------------------------------------------------------

bool MainWindow::startOrderStateUdpThread()
{
	m_pOrderStateSocket = new OrderStateSocket(QHostAddress(theOptions.providerData().serverIP()), theOptions.providerData().serverCustomerPort());
	if (m_pOrderStateSocket == nullptr)
	{
		return false;
	}

	QThread *pThread = new QThread;
	if (pThread == nullptr)
	{
		delete m_pOrderStateSocket;
		m_pOrderStateSocket = nullptr;

		return false;
	}

	m_pOrderStateSocket->moveToThread(pThread);

	//
	//
	connect(pThread, &QThread::started, m_pOrderStateSocket, &OrderStateSocket::slot_onThreadStarted);
	connect(pThread, &QThread::finished, m_pOrderStateSocket, &OrderStateSocket::slot_onThreadFinished);

	//
	//
	connect(m_pOrderStateSocket, &OrderStateSocket::orderRemoved, m_pView, &OrderView::removeFromList, Qt::QueuedConnection);
	//
	//
	pThread->start();

	return true;
}

// -------------------------------------------------------------------------------------------------------------------

bool MainWindow::stopOrderStateUdpThread()
{
	if (m_pOrderStateSocket == nullptr)
	{
		return false;
	}

	disconnect(m_pOrderStateSocket, &OrderStateSocket::orderRemoved, m_pView, &OrderView::removeFromList);
	//
	//
	QThread *pThread = m_pOrderStateSocket->thread();
	if (pThread == nullptr)
	{
		return false;
	}

	pThread->quit();
	pThread->wait();
	pThread->deleteLater();

	return true;
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::createActions()
{
	// Order
	//
	m_pOptionsAction = new QAction(tr("Options"), this);
	m_pOptionsAction->setShortcut(Qt::CTRL + Qt::Key_O);
	m_pOptionsAction->setIcon(QIcon(":/icons/Options.png"));
	m_pOptionsAction->setToolTip(tr("Options"));
	connect(m_pOptionsAction, &QAction::triggered, this, &MainWindow::onOptions);
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::createMenu()
{
	/*
	QMenuBar* pMenuBar = menuBar();
	if (pMenuBar == nullptr)
	{
		return;
	}

	m_pOrderMenu = pMenuBar->addMenu(tr("&Order"));

	m_pOrderMenu->addAction(m_pOptionsAction);

	/*m_pMeasureMenu->addSeparator();
	m_pMeasureMenu->addAction(m_pExportMeasureAction);


	m_pEditMenu = pMenuBar->addMenu(tr("&Edit"));

	m_pEditMenu->addAction(m_pCopyMeasureAction);
	m_pEditMenu->addAction(m_pRemoveMeasureAction);
	m_pEditMenu->addSeparator();
	m_pEditMenu->addAction(m_pSelectAllMeasureAction);
	m_pEditMenu->addSeparator();


	m_pViewMenu = pMenuBar->addMenu(tr("&View"));

	m_pViewPanelMenu = new QMenu("&Panels", m_pViewMenu);
	m_pViewMenu->addMenu(m_pViewPanelMenu);
	m_pViewMenu->addSeparator();
	m_pViewMenu->addAction(m_pShowCalculatorAction);


	m_pSettingMenu = pMenuBar->addMenu(tr("&Tools"));

	m_pSettingMenu->addAction(m_pCalibratorsAction);
	m_pSettingMenu->addSeparator();
	m_pSettingMenu->addAction(m_pShowRackListAction);
	m_pSettingMenu->addAction(m_pShowSignalListAction);
	m_pSettingMenu->addSeparator();
	m_pSettingMenu->addAction(m_pShowComparatorsListAction);
	m_pSettingMenu->addAction(m_pShowOutputSignalListAction);
	m_pSettingMenu->addAction(m_pShowTuningSignalListAction);
	m_pSettingMenu->addSeparator();
	m_pSettingMenu->addAction(m_pOptionsAction);


	m_pInfoMenu = pMenuBar->addMenu(tr("&?"));

	m_pInfoMenu->addAction(m_pShowStatisticAction);
	m_pInfoMenu->addSeparator();
	m_pInfoMenu->addAction(m_pAboutConnectionAction);
	m_pInfoMenu->addAction(m_pAboutAppAction);*/
}

// -------------------------------------------------------------------------------------------------------------------

bool MainWindow::createToolBars()
{
	// Control panel measure process
	//
	m_pOrderControlToolBar = new QToolBar(this);
	if (m_pOrderControlToolBar != nullptr)
	{
		m_pOrderControlToolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
		m_pOrderControlToolBar->setWindowTitle(tr("Control panel measure process"));
		m_pOrderControlToolBar->setObjectName(m_pOrderControlToolBar->windowTitle());
		m_pOrderControlToolBar->setMovable(false);
		addToolBarBreak(Qt::TopToolBarArea);
		addToolBar(m_pOrderControlToolBar);


		m_pOrderControlToolBar->addAction(m_pOptionsAction);

		QFont* font = new QFont("Arial", 14, 2);

		m_connectLabel = new QLabel(m_pOrderControlToolBar);
		m_connectLabel->setFont(*font);
		m_connectLabel->setStyleSheet("color: rgb(117, 187, 253);");
		m_connectLabel->setText(tr(NO_CONNECTION_STR));
		m_connectLabel->setFixedWidth(250);
		m_connectLabel->setEnabled(false);


		m_pOrderControlToolBar->setLayoutDirection(Qt::RightToLeft);
	}

	return true;
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::createOrderView()
{
	m_pView = new OrderView;
	if (m_pView == nullptr)
	{
		return;
	}

	setCentralWidget(m_pView);

	connect(m_pView, &OrderView::orderStateChanged, &theOrderBase, &Order::Base::slot_changeState, Qt::QueuedConnection);
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::createStatusBar()
{
	/*QStatusBar* pStatusBar = statusBar();
	if (pStatusBar == nullptr)
	{
		return;
	}

	m_statusEmpty = new QLabel(pStatusBar);
	m_statusMeasureThreadInfo = new QLabel(pStatusBar);
	m_statusMeasureTimeout = new QProgressBar(pStatusBar);
	m_statusMeasureThreadState = new QLabel(pStatusBar);
	m_statusCalibratorCount = new QLabel(pStatusBar);
	m_statusConnectToConfigServer = new QLabel(pStatusBar);
	m_statusConnectToAppDataServer = new QLabel(pStatusBar);
	m_statusConnectToTuningServer = new QLabel(pStatusBar);

	m_statusMeasureTimeout->setTextVisible(false);
	m_statusMeasureTimeout->setRange(0, 100);
	m_statusMeasureTimeout->setFixedWidth(100);
	m_statusMeasureTimeout->setFixedHeight(10);
	m_statusMeasureTimeout->setLayoutDirection(Qt::LeftToRight);

	pStatusBar->addWidget(m_statusConnectToTuningServer);
	pStatusBar->addWidget(m_statusConnectToAppDataServer);
	pStatusBar->addWidget(m_statusConnectToConfigServer);
	pStatusBar->addWidget(m_statusCalibratorCount);
	pStatusBar->addWidget(m_statusMeasureThreadState);
	pStatusBar->addWidget(m_statusMeasureTimeout);
	pStatusBar->addWidget(m_statusMeasureThreadInfo);
	pStatusBar->addWidget(m_statusEmpty);

	pStatusBar->setLayoutDirection(Qt::RightToLeft);

	m_statusEmpty->setText(QString());

	m_statusConnectToConfigServer->setText(tr(" ConfigurationService: off "));
	m_statusConnectToConfigServer->setStyleSheet("background-color: rgb(255, 160, 160);");
	m_statusConnectToConfigServer->setToolTip(tr("Please, connect to server\nclick menu \"Tool\" - \"Options...\" - \"Connect to server\""));

	m_statusConnectToAppDataServer->setText(tr(" AppDataService: off "));
	m_statusConnectToAppDataServer->setStyleSheet("background-color: rgb(255, 160, 160);");
	m_statusConnectToAppDataServer->setToolTip(tr("Please, connect to server\nclick menu \"Tool\" - \"Options...\" - \"Connect to server\""));

	m_statusConnectToTuningServer->setText(tr(" TuningService: off "));
	m_statusConnectToTuningServer->setStyleSheet("background-color: rgb(255, 160, 160);");
	m_statusConnectToTuningServer->setToolTip(tr("Please, connect to server\nclick menu \"Tool\" - \"Options...\" - \"Connect to server\""));*/
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::createContextMenu()
{
	// create context menu
	//
	m_pContextMenu = new QMenu(this);

	m_pContextMenu->addAction(m_pOptionsAction);

	// init context menu
	//
	if (m_pView == nullptr)
	{
		return;
	}

	m_pView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_pView, &QTableView::customContextMenuRequested, this, &MainWindow::onContextMenu);
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::onOptions()
{
	ProviderDataOption pd = theOptions.providerData();

	OptionsDialog dialog(pd);
	if (dialog.exec() != QDialog::Accepted)
	{
		return;
	}

	theOptions.setProviderData(dialog.providerData());
	theOptions.save();
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::onContextMenu(QPoint)
{
	if (m_pContextMenu == nullptr)
	{
		return;
	}

	m_pContextMenu->exec(QCursor::pos());
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::onSocketConnection(bool connect)
{
	if (connect == true)
	{
		m_connectLabel->setText("");
	}
	else
	{
		m_connectLabel->setText(NO_CONNECTION_STR);
	}
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::closeEvent(QCloseEvent* e)
{
	theOrderBase.writeToXml(QString("provider%1").arg(xmlOrderFileName));

	QMainWindow::closeEvent(e);
}

// -------------------------------------------------------------------------------------------------------------------
