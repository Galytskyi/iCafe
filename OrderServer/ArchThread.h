#ifndef ARCHTHREAD_H
#define ARCHTHREAD_H

#include <QObject>
#include <QFile>
#include <QMutex>

#include "../lib/Order.h"

// ==============================================================================================

class ArchThread : public QObject
{
	Q_OBJECT

public:

	explicit ArchThread(QObject *parent = nullptr);
	virtual ~ArchThread();

private:

	QMutex			m_mutex;
	QFile			m_archFile;

	virtual void	onThreadStarted();
	virtual void	onThreadFinished();

signals:

	void			msgBox(const QString &title, const QString& text);

public slots:

	void			slot_onThreadStarted();
	void			slot_onThreadFinished();

public slots:

	void			appendMessage(int type, const QString& func, const QString& text, const Order::Item& order);
};

// ==============================================================================================

#endif // ARCHTHREAD_H
