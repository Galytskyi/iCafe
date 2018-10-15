#include "MainWindow.h"

#include <QApplication>
#include <QScreen>
#include <QDesktopWidget>
#include <QThread>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>

#include "../lib/wassert.h"

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
	startProviderStateUdpThread();
	startOrderStateUdpThread();
}

// -------------------------------------------------------------------------------------------------------------------

MainWindow::~MainWindow()
{
	stopOrderStateUdpThread();
	stopProviderStateUdpThread();
	stopConfigUdpThread();
}

// -------------------------------------------------------------------------------------------------------------------

bool MainWindow::createInterface()
{
	setWindowIcon(QIcon(":/icons/Logo.png"));
	setWindowTitle(tr("Клиент заказов"));
	setMinimumSize(480, 640);
	move(QApplication::desktop()->availableGeometry().center() - rect().center());

	createActions();
	createToolBars();

	m_pStackedWidget = new QStackedWidget(this);
	if (m_pStackedWidget == nullptr)
	{
		return false;
	}

	// ProviderView
	//
	m_pView = createProviderView();
	if (m_pView == nullptr)
	{
		wassert(0);
	}

	m_pStackedWidget->addWidget(m_pView);

	// OrderDialog
	//
	m_pOrderDialog = new OrderDialog(this);
	if (m_pOrderDialog == nullptr)
	{
		wassert(0);
	}
	else
	{
		connect(m_pOrderDialog, &OrderDialog::accepted, this, &MainWindow::sendRequestCreateOrder, Qt::QueuedConnection);
		connect(m_pOrderDialog, &OrderDialog::rejected, this, &MainWindow::onSetMainWidget, Qt::QueuedConnection);
	}

	m_pStackedWidget->addWidget(m_pOrderDialog);

	// OptionsDialog
	//
	m_pOptionsDialog = new OptionsDialog(this);
	if (m_pOptionsDialog == nullptr)
	{
		wassert(0);
	}
	else
	{
		connect(m_pOptionsDialog, &OptionsDialog::accepted, this, &MainWindow::onSetMainWidget, Qt::QueuedConnection);
		connect(m_pOptionsDialog, &OptionsDialog::rejected, this, &MainWindow::onSetMainWidget, Qt::QueuedConnection);
	}

	m_pStackedWidget->addWidget(m_pOptionsDialog);

	// AppAboutDialog
	//
	m_pAppAboutDialog = new AppAboutDialog(this);
	if (m_pAppAboutDialog == nullptr)
	{
		wassert(0);
	}
	else
	{
		connect(m_pAppAboutDialog, &AppAboutDialog::accepted, this, &MainWindow::onSetMainWidget, Qt::QueuedConnection);
		connect(m_pAppAboutDialog, &AppAboutDialog::rejected, this, &MainWindow::onSetMainWidget, Qt::QueuedConnection);
	}

	m_pStackedWidget->addWidget(m_pAppAboutDialog);

	//
	//
	setCentralWidget(m_pStackedWidget);

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

	connect(m_pConfigSocket, &ConfigSocket::cfgXmlReceived, this, &MainWindow::cfgXmlReceived, Qt::QueuedConnection);

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

	disconnect(m_pConfigSocket, &ConfigSocket::cfgXmlReceived, this, &MainWindow::cfgXmlReceived);

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

bool MainWindow::startProviderStateUdpThread()
{
	m_pProviderStateSocket = new ProviderStateSocket(QHostAddress(theOptions.customerData().serverIP()), theOptions.customerData().serveProviderPort());
	if (m_pProviderStateSocket == nullptr)
	{
		return false;
	}

	QThread *pThread = new QThread;
	if (pThread == nullptr)
	{
		delete m_pProviderStateSocket;
		m_pProviderStateSocket = nullptr;

		return false;
	}

	connect(m_pProviderStateSocket, &ProviderStateSocket::providerInitStateReceived, m_pView, &ProviderView::updateProviderList, Qt::QueuedConnection);
	connect(m_pProviderStateSocket, &ProviderStateSocket::providerStateChanged, this, &MainWindow::providerStateChanged, Qt::QueuedConnection);
	connect(this, &MainWindow::requestProviderState, m_pProviderStateSocket, &ProviderStateSocket::requestGetProviderInitState, Qt::QueuedConnection);

	m_pProviderStateSocket->moveToThread(pThread);

	//
	//
	connect(pThread, &QThread::started, m_pProviderStateSocket, &Udp::ClientSocket::slot_onThreadStarted);
	connect(pThread, &QThread::finished, m_pProviderStateSocket, &Udp::ClientSocket::slot_onThreadFinished);

	//
	//
	pThread->start();

	return true;
}

// -------------------------------------------------------------------------------------------------------------------

bool MainWindow::stopProviderStateUdpThread()
{
	if (m_pProviderStateSocket == nullptr)
	{
		return false;
	}

	disconnect(this, &MainWindow::requestProviderState, m_pProviderStateSocket, &ProviderStateSocket::requestGetProviderInitState);

	//
	//
	QThread *pThread = m_pProviderStateSocket->thread();
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
	QFont* menuFont =  new QFont("Arial", 18, 2);

	// Order
	//
	m_pSearchAction = new QAction(tr("Поиск"), this);
	m_pSearchAction->setIcon(QIcon(":/icons/Search.png"));
	m_pSearchAction->setFont(*menuFont);
	//m_pSearchAction->setDisabled(true);
	//m_pSearchAction->setCheckable(true);
	connect(m_pSearchAction, &QAction::triggered, this, &MainWindow::onSearch);

	m_pOrderTableAction = new QAction(tr("Бронировать столик"), this);
	m_pOrderTableAction->setIcon(QIcon(":/icons/Table.png"));
	m_pOrderTableAction->setFont(*menuFont);
	connect(m_pOrderTableAction, &QAction::triggered, this, &MainWindow::onOrderTable);

	m_pOrderDinnerAction = new QAction(tr("Заказать обед"), this);
	m_pOrderDinnerAction->setIcon(QIcon(":/icons/Dinner.png"));
	m_pOrderDinnerAction->setFont(*menuFont);
	connect(m_pOrderDinnerAction, &QAction::triggered, this, &MainWindow::onOrderDinner);

	m_pCancelOrderAction = new QAction(tr("Отменить заказ"), this);
	m_pCancelOrderAction->setFont(*menuFont);
	connect(m_pCancelOrderAction, &QAction::triggered, this, &MainWindow::onOrderCancel);

	m_pOptionsAction = new QAction(tr("Настройки"), this);
	m_pOptionsAction->setIcon(QIcon(":/icons/Options.png"));
	m_pOptionsAction->setFont(*menuFont);
	connect(m_pOptionsAction, &QAction::triggered, this, &MainWindow::onOptions);

	m_pInfoAction = new QAction(tr("Информация"), this);
	m_pInfoAction->setIcon(QIcon(":/icons/Info.png"));
	m_pInfoAction->setFont(*menuFont);
	connect(m_pInfoAction, &QAction::triggered, this, &MainWindow::onAppAbout);
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
		m_pOrderControlToolBar->setWindowTitle(tr("Панель заказов"));
		m_pOrderControlToolBar->setObjectName(m_pOrderControlToolBar->windowTitle());
		m_pOrderControlToolBar->setMovable(false);
		addToolBarBreak(Qt::TopToolBarArea);
		addToolBar(m_pOrderControlToolBar);

		//m_pOrderControlToolBar->addAction(m_pSearchAction);

		//QFont* font = new QFont("Arial", 14, 2);

		//m_searchProvider = new QLineEdit(m_pOrderControlToolBar);
		//connect(m_searchProvider, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged, Qt::QueuedConnection);
		//m_searchProvider->setFont(*font);
		//m_pOrderControlToolBar->addWidget(m_searchProvider);

		m_pOrderControlToolBar->addAction(m_pInfoAction);

		if (theOptions.platformType() == PLATFORM_TYPE_WINDOWS)
		{
			m_pOrderControlToolBar->addAction(m_pOptionsAction);
		}

		m_connectLabel = new QLabel(m_pOrderControlToolBar);
		m_connectLabel->setStyleSheet("color: rgb(255, 0, 0);");
		m_connectLabel->setText(NO_CONNECTION_STR);
		//m_connectLabel->setAlignment(Qt::AlignCenter);

		m_pOrderControlToolBar->addWidget(m_connectLabel);

		m_pOrderControlToolBar->setLayoutDirection(Qt::RightToLeft);

		if (theOptions.platformType() == PLATFORM_TYPE_ANDROID)
		{
			int size = QApplication::screens().at(0)->logicalDotsPerInch() / 3;
			m_pOrderControlToolBar->setIconSize(QSize(size, size));
		}
	}

	return true;
}

// -------------------------------------------------------------------------------------------------------------------

ProviderView* MainWindow::createProviderView()
{
	ProviderView* pView = new ProviderView;
	if (pView == nullptr)
	{
		return nullptr;
	}

	connect(this, &MainWindow::setTextFilter, pView, &ProviderView::setTextFilter, Qt::QueuedConnection);

	connect(&theProviderBase, &Provider::Base::cfgXmlDataLoaded, pView, &ProviderView::updateList, Qt::QueuedConnection);
	connect(&theProviderBase, &Provider::Base::cfgXmlDataLoaded, pView, &ProviderView::updateOrderList, Qt::QueuedConnection);
	connect(&theOrderBase, &Order::Base::signal_stateChanged, pView, &ProviderView::orderStateChanged, Qt::QueuedConnection);

	connect(pView, &QTableView::clicked, this, &MainWindow::onProviderListClick);

	return pView;
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::onSetMainWidget()
{
	if (m_pStackedWidget == nullptr)
	{
		return;
	}

	if (m_pView == nullptr)
	{
		return;
	}

	int widgetIndex = m_pStackedWidget->indexOf(m_pView);
	if (widgetIndex < 0 || widgetIndex >= m_pStackedWidget->count())
	{
		return;
	}

	m_pStackedWidget->setCurrentIndex(widgetIndex);
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::onSearch()
{
	if (m_searchProvider == nullptr)
	{
		return;
	}

	//QMessageBox::information(this, tr("Поиск"), m_searchProvider->text());

	emit setTextFilter(m_searchProvider->text().toUpper());

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
	emit setTextFilter(text.toUpper());
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

bool MainWindow::createOrder(int type)
{
	if (type < 0 || type >= Order::TYPE_COUNT)
	{
		return false;
	}

	if (m_pView == nullptr)
	{
		return false;
	}

	int index = m_pView->currentIndex().row();
	if (index < 0 || index >= m_pView->table().count())
	{
		return false;
	}

	if (theOrderBase.count() >= MAX_CUSTOMER_ORDER_COUNT)
	{
		QMessageBox::information(this, tr("Создание заказа"), tr("Вы уже сделали %1 заказов. Чтобы сделать новый заказ, отмените любой предыдущий заказ.").arg(theOrderBase.count()));
		return false;
	}

	ProviderItem item = m_pView->table().at(index);

	Provider::Item provider = item.provider();
	if (provider.isEmpty() == true)
	{
		return false;
	}

	Order::Item order = item.order();
	if (order.state() != Order::STATE_UNDEFINED)
	{
		QMessageBox::information(this, tr("Создание заказа"), tr("Пожалуйста, отмените предыдущий заказ."));
		return false;
	}

	if (m_pStackedWidget == nullptr)
	{
		return false;
	}

	if (m_pOrderDialog == nullptr)
	{
		return false;
	}

	theOptions.customerData().setOrderType(type);
	theOptions.customerData().setProviderID(provider.providerID());

	m_pOrderDialog->setProviderName(provider.name());
	m_pOrderDialog->setCustomerData(theOptions.customerData());

	m_pOrderDialog->initDialog();

	int widgetIndex = m_pStackedWidget->indexOf(m_pOrderDialog);
	if (widgetIndex < 0 || widgetIndex >= m_pStackedWidget->count())
	{
		return false;
	}

	m_pStackedWidget->setCurrentIndex(widgetIndex);

	return true;
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::sendRequestCreateOrder()
{
	Order::Item order;

	order.setState(Order::STATE_CUSTOMER_CREATING_ORDER);
	order.setType(theOptions.customerData().orderType());
	order.setProviderID(theOptions.customerData().providerID());
	order.setPhone(theOptions.customerData().phone());
	order.setOrderTime(theOptions.customerData().orderTime());
	order.setPeople(theOptions.customerData().people());

	emit requestCreateOrder(order);

	emit onSetMainWidget();
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
		QMessageBox::information(this, tr("Отмена заказа"), tr("Чтобы отменить заказ, пожалуйста, позвоните в заведение <b>\"%1\"</b> по телефону: <b>%2</b> и сообщите код отмены: <b style=\"color: #FF0000\">%3</b>.<br>Или подождите %4 минут и Ваш заказ будет отменен автоматически.").arg(provider.name()).arg(provider.phone()).arg(order.cancelCode()).arg( MAX_SECONDS_ORDER_LIVE / 60 ) );
		return;
	}

	QMessageBox::StandardButton reply;

	reply = QMessageBox::question(this, tr("Отмена заказа"), tr("Вы хотите отменить заказ?"), QMessageBox::Yes|QMessageBox::No);

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
	if (m_pStackedWidget == nullptr)
	{
		return;
	}

	if (m_pOptionsDialog == nullptr)
	{
		return;
	}

	int widgetIndex = m_pStackedWidget->indexOf(m_pOptionsDialog);
	if (widgetIndex < 0 || widgetIndex >= m_pStackedWidget->count())
	{
		return;
	}

	m_pOptionsDialog->setCustomerData(theOptions.customerData());
	m_pOptionsDialog->initDialog();

	m_pStackedWidget->setCurrentIndex(widgetIndex);
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::onAppAbout()
{
	if (m_pStackedWidget == nullptr)
	{
		return;
	}

	if (m_pAppAboutDialog == nullptr)
	{
		return;
	}

	int widgetIndex = m_pStackedWidget->indexOf(m_pAppAboutDialog);
	if (widgetIndex < 0 || widgetIndex >= m_pStackedWidget->count())
	{
		return;
	}

	m_pStackedWidget->setCurrentIndex(widgetIndex);
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::onProviderListClick(const QModelIndex& index)
{
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

	Provider::Item provider = item.provider();

	if (provider.enableAcceptOrder() == false)
	{
		return;
	}

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

		if (provider.enableDinner() == true)
		{
			pContextMenu->addAction(m_pOrderDinnerAction);
		}
	}
	else
	{
		pContextMenu->addAction(m_pCancelOrderAction);
	}

	QPoint pt = mapToGlobal(QPoint(width() - QApplication::screens().at(0)->logicalDotsPerInch() * 2.5 , 0));
	pt.setY(QCursor::pos().y());

	pContextMenu->exec(pt);
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::cfgXmlReceived(const QByteArray& cfgXmlData, int version)
{
	theProviderBase.readFromXml(cfgXmlData, version);
	theProviderTypeBase.readFromXml(cfgXmlData, version);

	if(theProviderBase.count() != 0)
	{
		emit requestProviderState(0);
	}

	m_connectLabel->setText(QString());
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::providerStateChanged(quint32 providerID, quint32 state)
{
	if (m_pView == nullptr)
	{
		return;
	}

	m_pView->table().updateProviderState(providerID, state);
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::closeEvent(QCloseEvent* e)
{
	theOrderBase.writeToXml(QString("customer%1").arg(xmlOrderFileName));

	QMainWindow::closeEvent(e);
}

// -------------------------------------------------------------------------------------------------------------------
