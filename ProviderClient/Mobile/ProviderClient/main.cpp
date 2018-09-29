#include <QApplication>

#include "MainWindow.h"
#include "Options.h"

// -------------------------------------------------------------------------------------------------------------------
//
// main
//
// -------------------------------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	a.setApplicationName("ProviderClient");
	a.setOrganizationName("iCafe");
	a.setOrganizationDomain("iCafe.com");

	theOptions.load();

	MainWindow w;
	w.show();

	int result = a.exec();

	theOptions.unload();

	return result;
}

// -------------------------------------------------------------------------------------------------------------------