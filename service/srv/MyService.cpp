#include "MyService.h"

#include <QDebug>

MyService::MyService(int argc, char **argv) : QtService<QCoreApplication>(argc, argv, "AService")
{
	setServiceDescription("AService setServiceDescription");
	setServiceFlags(QtServiceBase::CanBeSuspended);

	qDebug() << "MyService::MyService";
}

MyService::~MyService()
{
	qDebug() << "MyService::~MyService";
}

void MyService::start()
{
	QCoreApplication *app = application();

	qDebug() << "MyService::start()";
	qDebug() << app->applicationDirPath();
}

void MyService::pause()
{
	qDebug() << "MyService::pause()";
}

void MyService::resume()
{
	qDebug() << "MyService::resume()";
}
