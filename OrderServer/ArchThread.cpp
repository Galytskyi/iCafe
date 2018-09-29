#include "ArchThread.h"

#include <QThread>
#include <QDateTime>
#include <QDebug>
#include <assert.h>

#include "../lib/SocketIO.h"

// -------------------------------------------------------------------------------------------------------------------
//
// ArchThread class implementation
//
// -------------------------------------------------------------------------------------------------------------------

ArchThread::ArchThread(QObject *parent)
	: QObject(parent)
{
}

// -------------------------------------------------------------------------------------------------------------------

ArchThread::~ArchThread()
{
}

// -------------------------------------------------------------------------------------------------------------------

void ArchThread::slot_onThreadStarted()
{
	onThreadStarted();
}

// -------------------------------------------------------------------------------------------------------------------

void ArchThread::slot_onThreadFinished()
{
	onThreadFinished();
	deleteLater();
}

// -------------------------------------------------------------------------------------------------------------------

void ArchThread::onThreadStarted()
{
	m_archFile.setFileName(ArchFileName);
	if (m_archFile.open(QIODevice::WriteOnly) == false)
	{
		qDebug() << "ArchThread::onThreadStarted() - arch file was not opened";
		assert(0);
		return;
	}

	qDebug() << "ArchThread::onThreadStarted() - arch file opened";
}

// -------------------------------------------------------------------------------------------------------------------

void ArchThread::onThreadFinished()
{
	if (m_archFile.isOpen() == true)
	{
		m_archFile.close();
	}

	qDebug() << "ArchThread::onThreadFinished()";
}

// -------------------------------------------------------------------------------------------------------------------

void ArchThread::appendMessage(int type, const QString& func, const QString& text, const Order::Item& order)
{
	QMutexLocker locker(&m_mutex);

	if (m_archFile.isOpen() == false)
	{
		qDebug() << "ArchThread::onThreadStarted() - arch file was not opened";
		assert(0);
		return;
	}

	QString msgStr;

	QDateTime cdt = QDateTime::currentDateTime();
	QString timeStr = QString().sprintf("%04d-%02d-%02d %02d:%02d:%02d:%03d", cdt.date().year(), cdt.date().month(), cdt.date().day(), cdt.time().hour(), cdt.time().minute(), cdt.time().second(), cdt.time().msec());

	msgStr.append(timeStr);
	msgStr.append(ArchFileDivider);

	if (type < 0 || type >= ARCH_MSG_TYPE_COUNT)
	{
		msgStr.append("[UNDEFINED]");
	}
	else
	{
		msgStr.append(ArchTypeStr[type]);
	}

	msgStr.append(ArchFileDivider);

	msgStr.append(func);
	msgStr.append(ArchFileDivider);

	msgStr.append(text);
	msgStr.append(ArchFileDivider);

	if (order.isEmpty() == true)
	{
		msgStr.append(QString());
		msgStr.append(ArchFileDivider);

		msgStr.append(QString());
		msgStr.append(ArchFileDivider);

		msgStr.append(QString());
		msgStr.append(ArchFileDivider);

		msgStr.append(QString());
		msgStr.append(ArchFileDivider);
	}
	else
	{
		msgStr.append(QString::number(order.providerID()));
		msgStr.append(ArchFileDivider);

		msgStr.append(QString::number(order.phone()));
		msgStr.append(ArchFileDivider);

		msgStr.append(order.address().toString());
		msgStr.append(ArchFileDivider);

		msgStr.append(QString::number(order.port()));
		msgStr.append(ArchFileDivider);
	}

	msgStr.append(ArchFileTerminator);

	m_archFile.write(msgStr.toLocal8Bit(), msgStr.toLocal8Bit().count());
	m_archFile.flush();
}

// -------------------------------------------------------------------------------------------------------------------

