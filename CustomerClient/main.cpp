#include <QApplication>
#include <QTranslator>

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

	a.setApplicationName("CustomerClient");
	a.setOrganizationName("BookingRest");
	a.setOrganizationDomain("BookingRest.com.ua");

	a.setApplicationVersion(QString("1.0.1"));

	bool hasLanguageFiles = false;
	if (hasLanguageFiles == true)
	{
		QTranslator translator;
		if (translator.load("CustomerClient_ru.qm", QApplication::applicationDirPath() + "/translations") == false)
		{
			qDebug() << "Options::loadLanguage() - didn't load language file";
		}
		qApp->installTranslator(&translator);
	}

	theOptions.load();

	MainWindow w;
	w.show();

	int result = a.exec();

	theOptions.unload();

	return result;
}

// -------------------------------------------------------------------------------------------------------------------
