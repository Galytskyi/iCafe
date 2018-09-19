#ifndef REMOVEORDERTHREAD_H
#define REMOVEORDERTHREAD_H

#include <QObject>
#include <QMutex>

#include "../lib/Order.h"

// ==============================================================================================

const int			AUTO_REMOVE_ORDER_TIMEOUT = 30 * 60 * 1000; // 30 min

//const int			AUTO_REMOVE_ORDER_TIMEOUT = 3000;

// ==============================================================================================

class RemoveOrderThread : public QObject
{
	Q_OBJECT

public:

	explicit RemoveOrderThread(QObject *parent = nullptr);
	virtual ~RemoveOrderThread();

private:

	QTimer			m_removeTimer;

	virtual void	onThreadStarted();
	virtual void	onThreadFinished();

signals:

	void			removeOrder(const Order::Item& order);

public slots:

	void			slot_onThreadStarted();
	void			slot_onThreadFinished();

private slots:

	void			autoRemoveTimeout();
};

// ==============================================================================================

#endif // REMOVEORDERTHREAD_H
