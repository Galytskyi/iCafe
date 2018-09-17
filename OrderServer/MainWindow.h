#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ConfigSocket.h"
#include "CustomerOrderSocket.h"
#include "ProviderOrderSocket.h"
#include "RemoveOrderThread.h"

// ==============================================================================================

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	virtual ~MainWindow();

private:

	ConfigSocket*			m_configSocket = nullptr;
	CustomerOrderSocket*	m_customerOrderSocket = nullptr;
	ProviderOrderSocket*	m_providerOrderSocket = nullptr;

	RemoveOrderThread*		m_removeOrderThread = nullptr;

public:

	bool					createInterface();

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
};

// ==============================================================================================

#endif // MAINWINDOW_H
