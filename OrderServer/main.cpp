#include <QApplication>

#include "MainWindow.h"
#include "Options.h"

// -------------------------------------------------------------------------------------------------------------------
//
// main
//
// -------------------------------------------------------------------------------------------------------------------

using namespace std;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	a.setApplicationName("OrderServer");
	a.setOrganizationName("BookingRest");
	a.setOrganizationDomain("BookingRest.com.ua");

	a.setApplicationVersion(QString("1.0"));

	theOptions.load();

	MainWindow w;
	w.show();

	int result = a.exec();

	theOptions.unload();

	return result;
}

// -------------------------------------------------------------------------------------------------------------------
