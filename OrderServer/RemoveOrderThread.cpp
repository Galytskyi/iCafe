#include "RemoveOrderThread.h"

#include <QThread>
#include <QDateTime>
#include <assert.h>

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
	qDebug() << "RemoveOrderThread::onThreadStarted()";
	emit appendMessageToArch(ARCH_MSG_TYPE_EVENT, __FUNCTION__, "started", Order::Item());

	connect(this, &RemoveOrderThread::removeOrder, &theOrderBase, &Order::Base::slot_removeOrder, Qt::QueuedConnection);

	connect(&m_removeTimer, &QTimer::timeout, this, &RemoveOrderThread::autoRemoveTimeout, Qt::QueuedConnection);

	m_removeTimer.start(AUTO_REMOVE_ORDER_TIMEOUT);
}

// -------------------------------------------------------------------------------------------------------------------

void RemoveOrderThread::onThreadFinished()
{
	m_removeTimer.stop();

	disconnect(this, &RemoveOrderThread::removeOrder, &theOrderBase, &Order::Base::slot_removeOrder);

	qDebug() << "RemoveOrderThread::onThreadFinished()";
}

// -------------------------------------------------------------------------------------------------------------------

void RemoveOrderThread::removeFrendlyOrder(quint32 phone)
{
	QList<Order::Item> list = theOrderBase.orderList();

	int count = list.count();
	for(int i = 0; i < count; i++)
	{
		Order::Item& order = list[i];

		if (order.phone() == phone)
		{
			if (order.state() == Order::STATE_ORDER_PROCESSING || order.state() == Order::STATE_SERVER_CREATED_ORDER)
			{
				emit removeOrder(order);
			}
		}
	}
}

// -------------------------------------------------------------------------------------------------------------------

void RemoveOrderThread::autoRemoveTimeout()
{
//	QTime t;
//	t.start();

	QList<Order::Item> list = theOrderBase.orderList();

	int count = list.count();
	for(int i = 0; i < count; i++)
	{
		Order::Item& order = list[i];

		if (order.removeTime() > QDateTime::currentDateTime())
		{
			continue;
		}

		emit removeOrder(order);
	}

//	qDebug("RemoveOrderThread::Time elapsed: %d ms, count: %d", t.elapsed(), count);
}

// -------------------------------------------------------------------------------------------------------------------

