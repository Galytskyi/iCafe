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

	theOptions.unload();

	int result = a.exec();

	google::protobuf::ShutdownProtobufLibrary();

	return result;
}

// -------------------------------------------------------------------------------------------------------------------
