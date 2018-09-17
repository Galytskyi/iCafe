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

void RemoveOrderThread::autoRemoveTimeout()
{
//	QTime t;
//	t.start();

	QList<Order::Item> list = theOrderBase.orderList();

	int count = list.count();
	for(int i = 0; i < count; i++)
	{
		if (list[i].removeTime() > QDateTime::currentDateTime())
		{
			continue;
		}

		emit removeOrder(list[i]);
	}

//	qDebug("RemoveOrderThread::Time elapsed: %d ms, count: %d", t.elapsed(), count);
}

// -------------------------------------------------------------------------------------------------------------------

