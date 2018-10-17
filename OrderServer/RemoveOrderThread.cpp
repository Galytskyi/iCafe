#include "RemoveOrderThread.h"

#include <QThread>
#include <QDateTime>

#include "../lib/Provider.h"
#include "../lib/Order.h"
#include "../lib/wassert.h"

// -------------------------------------------------------------------------------------------------------------------
//
// RemoveOrderThread class implementation
//
// -------------------------------------------------------------------------------------------------------------------


RemoveOrderThread::RemoveOrderThread(QObject *parent)
	: QObject(parent)
	, m_removeTimer(this)

{
}

// -------------------------------------------------------------------------------------------------------------------

RemoveOrderThread::~RemoveOrderThread()
{
}

// -------------------------------------------------------------------------------------------------------------------

void RemoveOrderThread::slot_onThreadStarted()
{
	onThreadStarted();
}

// -------------------------------------------------------------------------------------------------------------------

void RemoveOrderThread::slot_onThreadFinished()
{
	onThreadFinished();
	deleteLater();
}

// -------------------------------------------------------------------------------------------------------------------

void RemoveOrderThread::onThreadStarted()
{
	emit appendMessageToArch(ARCH_MSG_TYPE_EVENT, __FUNCTION__, "started");

	connect(&m_removeTimer, &QTimer::timeout, this, &RemoveOrderThread::autoRemoveTimeout, Qt::QueuedConnection);

	m_removeTimer.start(MAX_SECONDS_AUTO_REMOVE_ORDER);
}

// -------------------------------------------------------------------------------------------------------------------

void RemoveOrderThread::onThreadFinished()
{
	m_removeTimer.stop();

	emit appendMessageToArch(ARCH_MSG_TYPE_EVENT, __FUNCTION__, "finished");
}

// -------------------------------------------------------------------------------------------------------------------

void RemoveOrderThread::removeFrendlyOrdersByProviderID(quint32 providerID)
{
	Provider::Item* pProvider = theProviderBase.providerPtr( providerID );
	if (pProvider == nullptr)
	{
		return;
	}

	QList<Order::Item> list = pProvider->orderBase().orderList();

	int orderCount = list.count();
	for(int i = 0; i < orderCount; i++)
	{
		Order::Item order = list[i];

		if (order.state() == Order::STATE_ORDER_OK)
		{
			continue;
		}

		pProvider->orderBase().remove(order.handle().ID);
	}
}

// -------------------------------------------------------------------------------------------------------------------

void RemoveOrderThread::removeFrendlyOrdersByPhone(quint32 phone)
{
	int providerCount = theProviderBase.count();

	for(int p = 0; p < providerCount; p++)
	{
		Provider::Item* pProvider = theProviderBase.providerPtr( p );
		if (pProvider == nullptr)
		{
			continue;
		}

		QList<Order::Item> list = pProvider->orderBase().orderList();

		int orderCount = list.count();
		for(int i = 0; i < orderCount; i++)
		{
			Order::Item order = list[i];

			if (order.phone() == phone)
			{
				if (order.state() == Order::STATE_ORDER_PROCESSING || order.state() == Order::STATE_SERVER_CREATED_ORDER)
				{
					pProvider->orderBase().remove(order.handle().ID);
				}
			}
		}
	}
}

// -------------------------------------------------------------------------------------------------------------------

void RemoveOrderThread::autoRemoveTimeout()
{
//	QTime t;
//	t.start();

	int removedCount = 0;

	int providerCount = theProviderBase.count();

	for(int p = 0; p < providerCount; p++)
	{
		Provider::Item* pProvider = theProviderBase.providerPtr( p );
		if (pProvider == nullptr)
		{
			continue;
		}

		QList<Order::Item> list = pProvider->orderBase().orderList();

		int orderCount = list.count();
		for(int o = 0; o < orderCount; o++)
		{
			Order::Item order = list[o];

			if (order.removeTime() > QDateTime::currentDateTime())
			{
				continue;
			}

			if (pProvider->orderBase().remove(order.handle().ID) == true)
			{
				removedCount++;
			}
		}
	}

//	qDebug("RemoveOrderThread::Time elapsed: %d ms, count: %d", t.elapsed(), count);

	if (removedCount != 0)
	{
		emit appendMessageToArch(ARCH_MSG_TYPE_EVENT, __FUNCTION__, QString("removed: %1").arg(removedCount) );
	}
}

// -------------------------------------------------------------------------------------------------------------------
