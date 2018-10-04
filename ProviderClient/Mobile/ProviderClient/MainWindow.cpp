#include "MainWindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QHeaderView>
#include <QThread>
#include <QMenuBar>
#include <QToolBar>
#include <QMessageBox>
#include <assert.h>

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
	setWindowIcon(QIcon(":/icons/Logo.png"));
	setWindowTitle(tr("Поставщик заказов"));
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
	m_pView = createOrderView();
	if (m_pView == nullptr)
	{
		assert(0);
	}

	m_pStackedWidget->addWidget(m_pView);


	// CancelCodeDialog
	//
	m_pCancelOrderDialog = new CanceOrderDialog(this);
	if (m_pCancelOrderDialog == nullptr)
	{
		assert(0);
	}
	else
	{
		connect(m_pCancelOrderDialog, &CanceOrderDialog::accepted, this, &MainWindow::onSendRequestCancelOrder, Qt::QueuedConnection);
		connect(m_pCancelOrderDialog, &CanceOrderDialog::rejected, this, &MainWindow::onSetMainWidget, Qt::QueuedConnection);
	}

	m_pStackedWidget->addWidget(m_pCancelOrderDialog);


	// OptionsDialog
	//
	m_pOptionsDialog = new OptionsDialog(this);
	if (m_pOptionsDialog == nullptr)
	{
		assert(0);
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
		assert(0);
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

	//
	connect(&theOrderBase, &Order::Base::signal_stateChanged, this, &MainWindow::orderStateChanged, Qt::QueuedConnection);

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
	connect(m_pOrderReceiveSocket, &OrderReceiveSocket::appendOrderToBase, this, &MainWindow::appendOrder, Qt::QueuedConnection);

	connect(this, &MainWindow::sendRequestChangeState, m_pOrderReceiveSocket, &OrderReceiveSocket::requestSetOrderState, Qt::QueuedConnection);
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
	disconnect(this, &MainWindow::sendRequestChangeState, m_pOrderReceiveSocket, &OrderReceiveSocket::requestSetOrderState);

	disconnect(m_pOrderReceiveSocket, &OrderReceiveSocket::appendOrderToBase, this, &MainWindow::appendOrder);
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
	connect(m_pOrderStateSocket, &OrderStateSocket::removeOrderFromBase, this, &MainWindow::removeOrder, Qt::QueuedConnection);
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
	disconnect(m_pOrderStateSocket, &OrderStateSocket::removeOrderFromBase, this, &MainWindow::removeOrder);
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
	m_pOptionsAction = new QAction(tr("Настройки"), this);
	m_pOptionsAction->setShortcut(Qt::CTRL + Qt::Key_O);
	m_pOptionsAction->setIcon(QIcon(":/icons/Options.png"));
	m_pOptionsAction->setToolTip(tr("Настройки"));
	connect(m_pOptionsAction, &QAction::triggered, this, &MainWindow::onOptions);

	m_pInfoAction = new QAction(tr("Информация"), this);
	m_pInfoAction->setIcon(QIcon(":/icons/Info.png"));
	m_pInfoAction->setToolTip(tr("Информация"));
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

		m_pOrderControlToolBar->addAction(m_pInfoAction);
		m_pOrderControlToolBar->addAction(m_pOptionsAction);

		m_connectLabel = new QLabel(m_pOrderControlToolBar);
		QSize cellSize = QFontMetrics(m_connectLabel->font()).size(Qt::TextSingleLine, NO_CONNECTION_STR);
		m_connectLabel->setStyleSheet("color: rgb(255, 0, 0);");
		m_connectLabel->setText(tr(NO_CONNECTION_STR));
		m_connectLabel->setFixedWidth(cellSize.width());
		m_connectLabel->setFixedHeight(cellSize.height());
		m_connectLabel->setEnabled(false);


		m_pOrderControlToolBar->setLayoutDirection(Qt::RightToLeft);
	}

	return true;
}

// -------------------------------------------------------------------------------------------------------------------

OrderView* MainWindow::createOrderView()
{
	OrderView* pView = new OrderView;
	if (pView == nullptr)
	{
		return nullptr;
	}

	connect(pView, &QTableView::clicked, this, &MainWindow::onOrderListClick);

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

void MainWindow::onSendRequestCancelOrder()
{
	if (m_pView == nullptr)
	{
		return;
	}

	int i = m_pView->selectionModel()->currentIndex().row();
	if (i < 0 || i >= m_pView->table().count())
	{
		return;
	}

	Order::Item order = m_pView->table().order(i);

	order.setState(Order::STATE_ORDER_CANCEL);

	emit sendRequestChangeState(order);

	emit onSetMainWidget();
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

	m_pOptionsDialog->setProviderData(theOptions.providerData());
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

void MainWindow::onOrderListClick(const QModelIndex& index)
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

	Order::Item order = m_pView->table().order(i);

	switch (order.state())
	{
		case Order::STATE_ORDER_OK:
			{

				QMessageBox::StandardButton reply;

				reply = QMessageBox::question(this, tr("Отмена заказа"), tr("Вы хотите отменить заказ?<br><br>Для того чтобы отменить заказ, пожалуйста, позвоните клиенту по телефону: <b>+380%1</b> и спросите код отмены.").arg(order.phone()).arg(order.phone()), QMessageBox::Yes|QMessageBox::No);
				if (reply == QMessageBox::No)
				{
					break;
				}

				if (m_pStackedWidget == nullptr)
				{
					break;
				}

				if (m_pCancelOrderDialog == nullptr)
				{
					return;
				}

				int widgetIndex = m_pStackedWidget->indexOf(m_pCancelOrderDialog);
				if (widgetIndex < 0 || widgetIndex >= m_pStackedWidget->count())
				{
					break;
				}

				m_pCancelOrderDialog->setCancelCode(order.cancelCode());
				m_pCancelOrderDialog->initDialog();

				m_pStackedWidget->setCurrentIndex(widgetIndex);
			}

			break;

		case Order::STATE_ORDER_PROCESSING:
			{
				QMessageBox::StandardButton reply;

				reply = QMessageBox::question(this, tr("Принять заказ"), tr("Вы хотите принять заказ?"), QMessageBox::Yes|QMessageBox::No);

				if (reply == QMessageBox::Yes)
				{
					order.setState(Order::STATE_ORDER_OK);
				}
				else
				{
					order.setState(Order::STATE_ORDER_CANCEL);
				}

				emit sendRequestChangeState(order);
			}

			break;

		default:
			break;
	}
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

void MainWindow::appendOrder(const Order::Item& order)
{
	bool result =  theOrderBase.append(order);
	if (result == false)
	{
		return;
	}

	if (m_pView == nullptr)
	{
		return;
	}

	m_pView->appentToList(order);
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::orderStateChanged(const Order::Item& order)
{
	qDebug() << "MainWindow::orderStateChanged - ID : " << order.handle().ID << ", state: " << order.state();

	if (m_pView == nullptr)
	{
		return;
	}

	m_pView->changeState(order);
}


// -------------------------------------------------------------------------------------------------------------------

void MainWindow::removeOrder(const Order::Item& order)
{
	bool result = theOrderBase.remove(order.handle().ID);
	if (result == false)
	{
		return;
	}

	if (m_pView == nullptr)
	{
		return;
	}

	m_pView->removeFromList(order);
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::closeEvent(QCloseEvent* e)
{
	theOrderBase.writeToXml(QString("provider%1").arg(xmlOrderFileName));

	QMainWindow::closeEvent(e);
}

// -------------------------------------------------------------------------------------------------------------------
