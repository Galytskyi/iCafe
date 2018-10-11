#include "ArchThread.h"

#include <QApplication>
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
	qDebug() << "ArchThread::onThreadStarted() - started";

	m_archFile.setFileName(qApp->applicationDirPath() + "/" + ArchFileName);
	if (m_archFile.open(QIODevice::Append) == false)
	{
		qDebug() << "ArchThread::onThreadStarted() - arch file was not opened";
		assert(0);
		return;
	}

	appendMessage(ARCH_MSG_TYPE_EVENT, __FUNCTION__, QString("arch file opened"));
}

// -------------------------------------------------------------------------------------------------------------------

void ArchThread::onThreadFinished()
{
	appendMessage(ARCH_MSG_TYPE_EVENT, __FUNCTION__, "finished");

	if (m_archFile.isOpen() == true)
	{
		m_archFile.close();
	}
}

// -------------------------------------------------------------------------------------------------------------------

void ArchThread::appendMessage(int type, const QString& func, const QString& text)
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

	msgStr.append(ArchFileTerminator);

	m_archFile.write(msgStr.toLocal8Bit(), msgStr.toLocal8Bit().count());
	m_archFile.flush();

	qDebug() << timeStr << ": " << func << " - " << text;
}

// -------------------------------------------------------------------------------------------------------------------

