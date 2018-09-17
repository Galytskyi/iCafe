#include "MainWindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QThread>

#include "Database.h"

#include "../Lib/Provider.h"

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
}

// -------------------------------------------------------------------------------------------------------------------

bool MainWindow::createInterface()
{
//	setWindowIcon(QIcon(":/icons/Metrology.ico"));
	setWindowTitle(tr("Сервер заказов"));
	move(QApplication::desktop()->availableGeometry().center() - rect().center());

//	createActions();
//	createMenu();
//	createToolBars();
//	createMeasureViews();
//	createPanels();
//	createStatusBar();
//	createContextMenu();

//	loadSettings();

//	setMeasureType(MEASURE_TYPE_LINEARITY);

	return true;
}

// -------------------------------------------------------------------------------------------------------------------

bool MainWindow::loadBase()
{
	if (thePtrDB == nullptr)
	{
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

	theProviderBase.append(providerList);
	theProviderTypeBase.append(typeList);

	qDebug() << "ProviderBase::load() - Loaded providers: " << theProviderBase.count() << "Loaded provider types: " << theProviderTypeBase.count()  << ", Time for load: " << responseTime.elapsed() << " ms";

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
	m_configSocket = new ConfigSocket(QHostAddress::Any, PORT_CONFIG_XML_REQUEST);
	if (m_configSocket == nullptr)
	{
		return false;
	}

	QThread *pThread = new QThread;
	if (pThread == nullptr)
	{
		delete m_configSocket;
		m_configSocket = nullptr;

		return false;
	}

	m_configSocket->moveToThread(pThread);

	connect(pThread, &QThread::started, m_configSocket, &Udp::ServerSocket::slot_onThreadStarted);
	connect(pThread, &QThread::finished, m_configSocket, &Udp::ServerSocket::slot_onThreadFinished);

	pThread->start();

	return true;
}

// -------------------------------------------------------------------------------------------------------------------

bool MainWindow::stopConfigUdpThread()
{
	if (m_configSocket == nullptr)
	{
		return false;
	}

	QThread *pThread = m_configSocket->thread();
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
	m_customerOrderSocket = new CustomerOrderSocket(QHostAddress::Any, PORT_CUSTOMER_ORDER_REQUEST);
	if (m_customerOrderSocket == nullptr)
	{
		return false;
	}

	QThread *pThread = new QThread;
	if (pThread == nullptr)
	{
		delete m_customerOrderSocket;
		m_customerOrderSocket = nullptr;

		return false;
	}

	m_customerOrderSocket->moveToThread(pThread);

	connect(pThread, &QThread::started, m_customerOrderSocket, &CustomerOrderSocket::slot_onThreadStarted);
	connect(pThread, &QThread::finished, m_customerOrderSocket, &CustomerOrderSocket::slot_onThreadFinished);

	pThread->start();

	return true;
}

// -------------------------------------------------------------------------------------------------------------------

bool MainWindow::stopCustomerOrderUdpThread()
{
	if (m_customerOrderSocket == nullptr)
	{
		return false;
	}

	QThread *pThread = m_customerOrderSocket->thread();
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
	m_providerOrderSocket = new ProviderOrderSocket(QHostAddress::Any, PORT_PROVIDER_ORDER_REQUEST);
	if (m_providerOrderSocket == nullptr)
	{
		return false;
	}

	QThread *pThread = new QThread;
	if (pThread == nullptr)
	{
		delete m_providerOrderSocket;
		m_providerOrderSocket = nullptr;

		return false;
	}

	m_providerOrderSocket->moveToThread(pThread);

	connect(pThread, &QThread::started, m_providerOrderSocket, &ProviderOrderSocket::slot_onThreadStarted);
	connect(pThread, &QThread::finished, m_providerOrderSocket, &ProviderOrderSocket::slot_onThreadFinished);

	pThread->start();

	return true;
}

// -------------------------------------------------------------------------------------------------------------------

bool MainWindow::stopProviderOrderUdpThread()
{
	if (m_providerOrderSocket == nullptr)
	{
		return false;
	}

	QThread *pThread = m_providerOrderSocket->thread();
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
	m_removeOrderThread = new RemoveOrderThread;
	if (m_removeOrderThread == nullptr)
	{
		return false;
	}

	QThread *pThread = new QThread;
	if (pThread == nullptr)
	{
		delete m_removeOrderThread;
		m_removeOrderThread = nullptr;

		return false;
	}

	m_removeOrderThread->moveToThread(pThread);

	connect(pThread, &QThread::started, m_removeOrderThread, &RemoveOrderThread::slot_onThreadStarted);
	connect(pThread, &QThread::finished, m_removeOrderThread, &RemoveOrderThread::slot_onThreadFinished);

	pThread->start();

	return true;
}

// -------------------------------------------------------------------------------------------------------------------

bool MainWindow::stopRemoveOrderThread()
{
	if (m_removeOrderThread == nullptr)
	{
		return false;
	}

	QThread *pThread = m_removeOrderThread->thread();
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
