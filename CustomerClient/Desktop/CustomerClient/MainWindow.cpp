#include "MainWindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QThread>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>

#include "OrderDialog.h"
#include "OptionsDialog.h"

// -------------------------------------------------------------------------------------------------------------------
//
// MainWindow class implementation
//
// -------------------------------------------------------------------------------------------------------------------

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	theOrderBase.readFromXml(QString("customer%1").arg(xmlOrderFileName));

	createInterface();

	startConfigUdpThread();
	startOrderStateUdpThread();
}

// -------------------------------------------------------------------------------------------------------------------

MainWindow::~MainWindow()
{
	stopConfigUdpThread();
	stopOrderStateUdpThread();
}

// -------------------------------------------------------------------------------------------------------------------

bool MainWindow::createInterface()
{
	setWindowIcon(QIcon(":/icons/Table.ico"));
	setWindowTitle(tr("Клиент заказов"));
	resize(500, 500);
	move(QApplication::desktop()->availableGeometry().center() - rect().center());

	createActions();
	createMenu();
	createToolBars();
	createProviderView();

	return true;
}

// -------------------------------------------------------------------------------------------------------------------

bool MainWindow::startConfigUdpThread()
{
	m_pConfigSocket = new ConfigSocket(QHostAddress(theOptions.customerData().serverIP()), theOptions.customerData().serverConfigPort());
	if (m_pConfigSocket == nullptr)
	{
		return false;
	}

	QThread *pThread = new QThread;
	if (pThread == nullptr)
	{
		delete m_pConfigSocket;
		m_pConfigSocket = nullptr;

		return false;
	}

	m_pConfigSocket->moveToThread(pThread);

	connect(pThread, &QThread::started, m_pConfigSocket, &ConfigSocket::slot_onThreadStarted);
	connect(pThread, &QThread::finished, m_pConfigSocket, &ConfigSocket::slot_onThreadFinished);

	pThread->start();

	return true;
}

// -------------------------------------------------------------------------------------------------------------------

bool MainWindow::stopConfigUdpThread()
{
	if (m_pConfigSocket == nullptr)
	{
		return false;
	}

	QThread *pThread = m_pConfigSocket->thread();
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
	m_pOrderStateSocket = new OrderStateSocket(QHostAddress(theOptions.customerData().serverIP()), theOptions.customerData().serverCustomerPort());
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
	connect(pThread, &QThread::started, m_pOrderStateSocket, &Udp::ClientSocket::slot_onThreadStarted);
	connect(pThread, &QThread::finished, m_pOrderStateSocket, &Udp::ClientSocket::slot_onThreadFinished);
	//
	//
	connect(this, &MainWindow::requestCreateOrder, m_pOrderStateSocket, &OrderStateSocket::requestCreateOrder, Qt::QueuedConnection);
	connect(this, &MainWindow::requestRemoveOrder, m_pOrderStateSocket, &OrderStateSocket::requestRemoveOrder, Qt::QueuedConnection);

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

	//
	//
	disconnect(this, &MainWindow::requestCreateOrder, m_pOrderStateSocket, &OrderStateSocket::requestCreateOrder);
	disconnect(this, &MainWindow::requestRemoveOrder, m_pOrderStateSocket, &OrderStateSocket::requestRemoveOrder);

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
	QFont* listFont =  new QFont("Arial", 14, 2);

	// Order
	//
	m_pSearchAction = new QAction(tr("Search"), this);
	m_pSearchAction->setIcon(QIcon(":/icons/Search.png"));
	m_pSearchAction->setDisabled(true);
	//m_pSearchAction->setCheckable(true);
	//connect(m_pSearchAction, &QAction::triggered, this, &MainWindow::onSearch);

	m_pOrderTableAction = new QAction(tr("Order table"), this);
	m_pOrderTableAction->setIcon(QIcon(":/icons/Table.png"));
	connect(m_pOrderTableAction, &QAction::triggered, this, &MainWindow::onOrderTable);
	m_pOrderTableAction->setFont(*listFont);

	m_pOrderDinnerAction = new QAction(tr("Order lunch"), this);
	m_pOrderDinnerAction->setIcon(QIcon(":/icons/Dinner.png"));
	connect(m_pOrderDinnerAction, &QAction::triggered, this, &MainWindow::onOrderDinner);
	m_pOrderDinnerAction->setFont(*listFont);

	m_pCancelOrderAction = new QAction(tr("Cancel order"), this);
	m_pCancelOrderAction->setIcon(QIcon(":/icons/Cancel.png"));
	connect(m_pCancelOrderAction, &QAction::triggered, this, &MainWindow::onOrderCancel);
	m_pCancelOrderAction->setFont(*listFont);

	m_pOptionsAction = new QAction(tr("Options"), this);
	m_pOptionsAction->setIcon(QIcon(":/icons/Options.png"));
	connect(m_pOptionsAction, &QAction::triggered, this, &MainWindow::onOptions);
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::createMenu()
{
//	QMenuBar* pMenuBar = menuBar();
//	if (pMenuBar == nullptr)
//	{
//		return;
//	}

//	m_pOrderMenu = pMenuBar->addMenu(tr("&Order"));

//	m_pOrderMenu->addAction(m_pOrderTableAction);
//	m_pOrderMenu->addAction(m_pOrderDinnerAction);
//	m_pOrderMenu->addAction(m_pOrderCancelAction);
//	m_pOrderMenu->addSeparator();
//	m_pOrderMenu->addAction(m_pInfoAction);
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

		m_pOrderControlToolBar->addAction(m_pSearchAction);

		QFont* font = new QFont("Arial", 14, 2);

		m_searchProvider = new QLineEdit(m_pOrderControlToolBar);
		m_searchProvider->setFont(*font);
		m_pOrderControlToolBar->addWidget(m_searchProvider);
		connect(m_searchProvider, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);

		m_pOrderControlToolBar->addAction(m_pOptionsAction);
	}

	return true;
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::createProviderView()
{
	m_pView = new ProviderView;
	if (m_pView == nullptr)
	{
		return;
	}

	setCentralWidget(m_pView);

	connect(this, &MainWindow::setTextFilter, m_pView, &ProviderView::setTextFilter, Qt::QueuedConnection);

	connect(&theProviderBase, &Provider::Base::cfgXmlDataLoaded, m_pView, &ProviderView::updateList, Qt::QueuedConnection);
	connect(&theProviderBase, &Provider::Base::cfgXmlDataLoaded, m_pView, &ProviderView::updateOrderList, Qt::QueuedConnection);
	connect(&theOrderBase, &Order::Base::signal_stateChanged, m_pView, &ProviderView::orderStateChanged, Qt::QueuedConnection);

	connect(m_pView, &QTableView::clicked, this, &MainWindow::onProviderListClick);
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::onSearch()
{
	if (m_searchProvider == nullptr)
	{
		return;
	}

	return;

	if (m_pSearchAction->isChecked() == false)
	{
		m_searchProvider->show();
		//m_pSearchAction->setChecked(tr);
	}
	else
	{
		m_searchProvider->hide();
		//m_pSearchAction->setChecked(true);
	}
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::onSearchTextChanged(const QString& text)
{
	emit setTextFilter(text);
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::onOrderTable()
{
	createOrder(Order::TYPE_TABLE);
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::onOrderDinner()
{
	createOrder(Order::TYPE_DINNER);
}

// -------------------------------------------------------------------------------------------------------------------

int MainWindow::createOrder(int type)
{
	if (type < 0 || type >= Order::TYPE_COUNT)
	{
		return -1;
	}

	if (m_pView == nullptr)
	{
		return -1;
	}

	int index = m_pView->currentIndex().row();
	if (index < 0 || index >= m_pView->table().count())
	{
		return -1;
	}

	if (theOrderBase.count() >= MAX_CUSTOMER_ORDER_COUNT)
	{
		QMessageBox::information(this, tr("Create order"), tr("You have already made %1 orders. To make a new order, cancel any of the previous orders.").arg(theOrderBase.count()));
		return -1;
	}

	ProviderItem item = m_pView->table().at(index);

	Provider::Item provider = item.provider();
	if (provider.isEmpty() == true)
	{
		return -1;
	}

	Order::Item order = item.order();
	if (order.state() != Order::STATE_UNDEFINED)
	{
		QMessageBox::information(this, tr("Create order"), tr("Please, cancel the previous order"));
		return -1;
	}

	CustomerDataOption ud = theOptions.customerData();

	OrderDialog dialog(provider.name(), ud);
	if (dialog.exec() != QDialog::Accepted)
	{
		return -1;
	}

	order.setState(Order::STATE_CUSTOMER_CREATING_ORDER);
	order.setType(type);
	order.setProviderID(provider.providerID());
	order.setPhone(dialog.customerData().phone());
	order.setOrderTime(dialog.customerData().orderTime());
	order.setPeople(dialog.customerData().people());

	emit requestCreateOrder(order);

	theOptions.customerData() = dialog.customerData();
	theOptions.save();

	return 0;
}
// -------------------------------------------------------------------------------------------------------------------

void MainWindow::onOrderCancel()
{
	if (m_pView == nullptr)
	{
		return;
	}

	int index = m_pView->currentIndex().row();
	if (index < 0 || index >= m_pView->table().count())
	{
		return;
	}

	ProviderItem item = m_pView->table().at(index);

	Provider::Item provider = item.provider();
	if (provider.isEmpty() == true)
	{
		return;
	}

	Order::Item order = item.order();
	if (order.isEmpty() == true || order.isValid() == false)
	{
		return;
	}

	if (order.state() == Order::STATE_ORDER_OK)
	{
		QMessageBox::information(this, tr("Cancel order"), tr("To cancel the order, please, call the establishment <b>\"%1\"</b> by phone: <b>%2</b>, and say cancel code: <b>%3</b>").arg(provider.name()).arg(provider.phone()).arg(order.cancelCode()) );
		return;
	}

	QMessageBox::StandardButton reply;

	reply = QMessageBox::question(this, "Cancel order", "Do you want to cancel the order?", QMessageBox::Yes|QMessageBox::No);

	if (reply == QMessageBox::No)
	{
		return;
	}

	order.setState(Order::STATE_CUSTOMER_REMOVING_ORDER);

	emit requestRemoveOrder(order);
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::onOptions()
{
	CustomerDataOption cd = theOptions.customerData();

	OptionsDialog dialog(cd);
	if (dialog.exec() != QDialog::Accepted)
	{
		return;
	}

	theOptions.setCustomerData(dialog.customerData());
	theOptions.save();
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::onProviderListClick(const QModelIndex& index)
{
	if (width() - mapFromGlobal(QCursor::pos()).x() > 50)
	{
		return;
	}

	if (m_pView == nullptr)
	{
		return;
	}

	int i = index.row();
	if (i < 0 || i >= m_pView->table().count())
	{
		return;
	}

	ProviderItem item = m_pView->table().at(i);

	Order::Item order = item.order();

	// create context menu
	//
	QMenu* pContextMenu = new QMenu(this);
	if (pContextMenu == nullptr)
	{
		return;
	}

	if (order.state() == Order::STATE_UNDEFINED)
	{
		pContextMenu->addAction(m_pOrderTableAction);
		pContextMenu->addSeparator();
		pContextMenu->addAction(m_pOrderDinnerAction);
	}
	else
	{
		pContextMenu->addAction(m_pCancelOrderAction);
	}

	QPoint pt = QCursor::pos();
	QPoint pt1 = mapToGlobal(QPoint(width(), QCursor::pos().y()));

	pt.setX(pt1.x() - 220);

	pContextMenu->exec(pt);
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::closeEvent(QCloseEvent* e)
{
	theOrderBase.writeToXml(QString("customer%1").arg(xmlOrderFileName));

	QMainWindow::closeEvent(e);
}

// -------------------------------------------------------------------------------------------------------------------
