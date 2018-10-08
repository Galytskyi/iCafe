#include "MainWindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QStatusBar>
#include <QThread>
#include <QMessageBox>

#include "Database.h"
#include "ProviderDialog.h"

#include "../lib/Provider.h"

// -------------------------------------------------------------------------------------------------------------------
//
// MainWindow class implementation
//
// -------------------------------------------------------------------------------------------------------------------

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	// init interface
	//
	createInterface();

	startArchThread();

	loadBase();

	startConfigUdpThread();
	startCustomerOrderUdpThread();
	startProviderOrderUdpThread();

	startRemoveOrderThread();
}

// -------------------------------------------------------------------------------------------------------------------

MainWindow::~MainWindow()
{
	stopConfigUdpThread();
	stopCustomerOrderUdpThread();
	stopProviderOrderUdpThread();

	stopRemoveOrderThread();

	stopArchThread();
}

// -------------------------------------------------------------------------------------------------------------------

bool MainWindow::createInterface()
{
//	setWindowIcon(QIcon(":/icons/Metrology.ico"));
	setWindowTitle(tr("Server of orders"));
	setMinimumSize(700, 500);
	move(QApplication::desktop()->availableGeometry().center() - rect().center());

//	createActions();
//	createMenu();
//	createToolBars();
	createProviderView();
//	createPanels();
	createStatusBar();
//	createContextMenu();

//	loadSettings();

//	setMeasureType(MEASURE_TYPE_LINEARITY);

	connect(&theOrderBase, &Order::Base::signal_stateChanged, this, &MainWindow::orderStateChanged, Qt::QueuedConnection);

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

//	connect(this, &MainWindow::setTextFilter, pView, &ProviderView::setTextFilter, Qt::QueuedConnection);

//	connect(&theProviderBase, &Provider::Base::cfgXmlDataLoaded, m_pView, &ProviderView::updateList, Qt::QueuedConnection);
//	connect(&theProviderBase, &Provider::Base::cfgXmlDataLoaded,m_pView, &ProviderView::updateOrderList, Qt::QueuedConnection);
//	connect(&theOrderBase, &Order::Base::signal_stateChanged, m_pView, &ProviderView::orderStateChanged, Qt::QueuedConnection);

	connect(m_pView, &QTableView::doubleClicked, this, &MainWindow::onProviderListClick);

	setCentralWidget(m_pView);
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::createStatusBar()
{
	QStatusBar* pStatusBar = statusBar();
	if (pStatusBar == nullptr)
	{
		return;
	}

	m_statusEmpty = new QLabel(pStatusBar);
	m_statusProviderCount = new QLabel(pStatusBar);
	m_statusOrderCount = new QLabel(pStatusBar);

	m_statusProviderCount->setText(tr("Provider count: 0"));
	m_statusOrderCount->setText(tr("Order count: 0"));

	pStatusBar->addWidget(m_statusOrderCount);
	pStatusBar->addWidget(m_statusProviderCount);
	pStatusBar->addWidget(m_statusEmpty);

	pStatusBar->setLayoutDirection(Qt::RightToLeft);

	m_statusEmpty->setText(QString());
}

// -------------------------------------------------------------------------------------------------------------------

bool MainWindow::startArchThread()
{
	m_pArchThread = new ArchThread;
	if (m_pArchThread == nullptr)
	{
		return false;
	}

	QThread *pThread = new QThread;
	if (pThread == nullptr)
	{
		delete m_pArchThread;
		m_pArchThread = nullptr;

		return false;
	}

	m_pArchThread->moveToThread(pThread);

	connect(pThread, &QThread::started, m_pArchThread, &ArchThread::slot_onThreadStarted);
	connect(pThread, &QThread::finished, m_pArchThread, &ArchThread::slot_onThreadFinished);

	connect(this, &MainWindow::appendMessageToArch, m_pArchThread, &ArchThread::appendMessage, Qt::QueuedConnection);

	pThread->start();

	emit appendMessageToArch(ARCH_MSG_TYPE_EVENT, __FUNCTION__, QString("App started"));

	return true;
}

// -------------------------------------------------------------------------------------------------------------------

bool MainWindow::stopArchThread()
{
	if (m_pArchThread == nullptr)
	{
		return false;
	}

	QThread *pThread = m_pArchThread->thread();
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

bool MainWindow::loadBase()
{
	if (thePtrDB == nullptr)
	{
		emit appendMessageToArch(ARCH_MSG_TYPE_ERROR, __FUNCTION__, "thePtrDB == nullptr");
		return 0;
	}

	QVector<Provider::Item> providerList;
	QVector<Provider::Type> typeList;

	QTime responseTime;
	responseTime.start();

	// load providers
	//
	SqlTable* table = thePtrDB->openTable(SQL_TABLE_PROVIDER);
	if (table != nullptr)
	{
		QMutex providerMutex;

		providerMutex.lock();
			providerList.resize(table->recordCount());
			table->read(providerList.data());
		providerMutex.unlock();

		table->close();
	}

	// load provider types
	//
	table = thePtrDB->openTable(SQL_TABLE_PROVIDER_TYPE);
	if (table != nullptr)
	{
		QMutex typeMutex;

		typeMutex.lock();
			typeList.resize(table->recordCount());
			table->read(typeList.data());
		typeMutex.unlock();

		table->close();
	}

	int rt = responseTime.elapsed();

	theProviderBase.append(providerList);
	theProviderTypeBase.append(typeList);

	QString msg = QString("ProviderBase::load() - Loaded providers: %1, Loaded provider types: %2, Time for load: %3 ms" ).arg(theProviderBase.count()).arg(theProviderTypeBase.count()).arg(rt);

	qDebug() << msg;
	emit appendMessageToArch(ARCH_MSG_TYPE_EVENT, __FUNCTION__, msg);

	if (m_pView != nullptr)
	{
		m_pView->updateList();
	}

	m_statusProviderCount->setText(tr("Provider count: %1").arg(theProviderBase.count()));

	return theProviderBase.count();
}

// -------------------------------------------------------------------------------------------------------------------

bool MainWindow::saveProviderBase()
{
	/*if (thePtrDB == nullptr)
	{
		return false;
	}

	SqlTable* table = thePtrDB->openTable(SQL_TABLE_PROVIDER);
	if (table == nullptr)
	{
		return false;
	}

	if (table->clear() == false)
	{
		table->close();
		return false;
	}

	int writtenRecordCount = 0;

	m_providerMutex.lock();

		writtenRecordCount = table->write(m_providerList.data(), m_providerList.count());

	m_providerMutex.unlock();

	table->close();

	if (writtenRecordCount != count())
	{
		return false;
	}

	qDebug() << "ProviderBase::save() - Written providers: " << writtenRecordCount;*/

	return true;
}

// -------------------------------------------------------------------------------------------------------------------

bool MainWindow::startConfigUdpThread()
{
	m_pConfigSocket = new ConfigSocket(QHostAddress::Any, PORT_CONFIG_XML_REQUEST);
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

	connect(m_pConfigSocket, &ConfigSocket::msgBox, this, &MainWindow::msgBox);

	m_pConfigSocket->moveToThread(pThread);

	connect(pThread, &QThread::started, m_pConfigSocket, &ConfigSocket::slot_onThreadStarted);
	connect(pThread, &QThread::finished, m_pConfigSocket, &ConfigSocket::slot_onThreadFinished);

	connect(m_pConfigSocket, &ConfigSocket::appendMessageToArch, m_pArchThread, &ArchThread::appendMessage, Qt::QueuedConnection);

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

bool MainWindow::startCustomerOrderUdpThread()
{
	m_pCustomerOrderSocket = new CustomerOrderSocket(QHostAddress::Any, PORT_CUSTOMER_ORDER_REQUEST);
	if (m_pCustomerOrderSocket == nullptr)
	{
		return false;
	}

	QThread *pThread = new QThread;
	if (pThread == nullptr)
	{
		delete m_pCustomerOrderSocket;
		m_pCustomerOrderSocket = nullptr;

		return false;
	}

	connect(m_pCustomerOrderSocket, &CustomerOrderSocket::msgBox, this, &MainWindow::msgBox);

	m_pCustomerOrderSocket->moveToThread(pThread);

	connect(pThread, &QThread::started, m_pCustomerOrderSocket, &CustomerOrderSocket::slot_onThreadStarted);
	connect(pThread, &QThread::finished, m_pCustomerOrderSocket, &CustomerOrderSocket::slot_onThreadFinished);

	connect(m_pCustomerOrderSocket, &CustomerOrderSocket::appendMessageToArch, m_pArchThread, &ArchThread::appendMessage, Qt::QueuedConnection);

	pThread->start();

	return true;
}

// -------------------------------------------------------------------------------------------------------------------

bool MainWindow::stopCustomerOrderUdpThread()
{
	if (m_pCustomerOrderSocket == nullptr)
	{
		return false;
	}

	QThread *pThread = m_pCustomerOrderSocket->thread();
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

bool MainWindow::startProviderOrderUdpThread()
{
	m_pProviderOrderSocket = new ProviderOrderSocket(QHostAddress::Any, PORT_PROVIDER_ORDER_REQUEST);
	if (m_pProviderOrderSocket == nullptr)
	{
		return false;
	}

	QThread *pThread = new QThread;
	if (pThread == nullptr)
	{
		delete m_pProviderOrderSocket;
		m_pProviderOrderSocket = nullptr;

		return false;
	}

	m_pProviderOrderSocket->moveToThread(pThread);

	connect(pThread, &QThread::started, m_pProviderOrderSocket, &ProviderOrderSocket::slot_onThreadStarted);
	connect(pThread, &QThread::finished, m_pProviderOrderSocket, &ProviderOrderSocket::slot_onThreadFinished);

	connect(m_pProviderOrderSocket, &ProviderOrderSocket::appendMessageToArch, m_pArchThread, &ArchThread::appendMessage, Qt::QueuedConnection);

	pThread->start();

	return true;
}

// -------------------------------------------------------------------------------------------------------------------

bool MainWindow::stopProviderOrderUdpThread()
{
	if (m_pProviderOrderSocket == nullptr)
	{
		return false;
	}

	QThread *pThread = m_pProviderOrderSocket->thread();
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

bool MainWindow::startRemoveOrderThread()
{
	m_pRemoveOrderThread = new RemoveOrderThread;
	if (m_pRemoveOrderThread == nullptr)
	{
		return false;
	}

	QThread *pThread = new QThread;
	if (pThread == nullptr)
	{
		delete m_pRemoveOrderThread;
		m_pRemoveOrderThread = nullptr;

		return false;
	}

	m_pRemoveOrderThread->moveToThread(pThread);

	connect(pThread, &QThread::started, m_pRemoveOrderThread, &RemoveOrderThread::slot_onThreadStarted);
	connect(pThread, &QThread::finished, m_pRemoveOrderThread, &RemoveOrderThread::slot_onThreadFinished);

	connect(m_pRemoveOrderThread, &RemoveOrderThread::appendMessageToArch, m_pArchThread, &ArchThread::appendMessage, Qt::QueuedConnection);

	if (m_pProviderOrderSocket != nullptr)
	{
		connect(m_pProviderOrderSocket, &ProviderOrderSocket::removeFrendlyOrder, m_pRemoveOrderThread, &RemoveOrderThread::removeFrendlyOrder, Qt::QueuedConnection);
	}

	pThread->start();

	return true;
}

// -------------------------------------------------------------------------------------------------------------------

bool MainWindow::stopRemoveOrderThread()
{
	if (m_pRemoveOrderThread == nullptr)
	{
		return false;
	}

	QThread *pThread = m_pRemoveOrderThread->thread();
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

void MainWindow::onProviderListClick(const QModelIndex& index)
{
	if (thePtrDB == nullptr)
	{
		return;
	}

	if (m_pConfigSocket == nullptr)
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

	Provider::Item provider = m_pView->table().at(i);

	ProviderDialog dialog(provider);
	if (dialog.exec() != QDialog::Accepted)
	{
		return;
	}

	provider = dialog.provider();

	// update data in database
	//
	SqlTable* table = thePtrDB->openTable(SQL_TABLE_PROVIDER);
	if (table == nullptr)
	{
		QMessageBox::information(this, tr("Database"), tr("Error of opening table SQL_TABLE_PROVIDER for write") );
		return;
	}

	if (table->write(&provider, 1, provider.providerID()) != 1)
	{
		QMessageBox::information(this, tr("Database"), tr("Error writing to table") );
		return;
	}

	table->close();

	// update data in theProviderBase
	//
	Provider::Item* pProvider = theProviderBase.providerPtr(provider.providerID());
	if (pProvider == nullptr)
	{
		return;
	}

	//
	//
	pProvider->setActive(provider.isActive());
	pProvider->setActiveTime(provider.activeTime());
	pProvider->setName(provider.name());
	pProvider->setAddress(provider.address());
	pProvider->setPhone(provider.phone());

	// update data in ConfigSocket
	//
	m_pConfigSocket->createCfgXml();

	// update data in View
	//
	m_pView->table().set(i, provider);
	m_pView->table().updateRow(i);
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::msgBox(const QString &title, const QString& text)
{
	QMessageBox::information(this, title, text);
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::orderStateChanged(const Order::Item& order)
{
	Q_UNUSED(order);

	m_statusOrderCount->setText(tr("Orders count: %1").arg(theOrderBase.count()));
}

// -------------------------------------------------------------------------------------------------------------------

void MainWindow::closeEvent(QCloseEvent* e)
{
	emit appendMessageToArch(ARCH_MSG_TYPE_EVENT, __FUNCTION__, "App finished");

	QMainWindow::closeEvent(e);
}

// -------------------------------------------------------------------------------------------------------------------
