#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>

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

	// Elements of interface - StatusBar
	//
	QLabel*					m_statusEmpty = nullptr;
	QLabel*					m_statusProviderCount = nullptr;
	QLabel*					m_statusOrderCount = nullptr;

private:

	ConfigSocket*			m_pConfigSocket = nullptr;
	CustomerOrderSocket*	m_pCustomerOrderSocket = nullptr;
	ProviderOrderSocket*	m_pProviderOrderSocket = nullptr;

	RemoveOrderThread*		m_pRemoveOrderThread = nullptr;

public:

	bool					createInterface();
	void					createStatusBar();

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

public slots:

	void					msgBox(const QString &title, const QString& text);
	void					orderStateChanged(const Order::Item& order);
};

// ==============================================================================================

#endif // MAINWINDOW_H
