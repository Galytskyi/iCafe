#ifndef REMOVEORDERTHREAD_H
#define REMOVEORDERTHREAD_H

#include <QObject>
#include <QMutex>

#include "../lib/Order.h"

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

	void			msgBox(const QString &title, const QString& text);
	void			appendMessageToArch(int type, const QString& func, const QString& text);

public slots:

	void			slot_onThreadStarted();
	void			slot_onThreadFinished();

	void			removeFrendlyOrder(quint32 phone);

private slots:

	void			autoRemoveTimeout();
};

// ==============================================================================================

#endif // REMOVEORDERTHREAD_H
