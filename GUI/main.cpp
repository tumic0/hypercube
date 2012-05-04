#include <QApplication>
#include "GUI/gui.h"
#include "GUI/icons.h"


int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QString locale = QLocale::system().name();
	QTranslator translator;
	translator.load(QString("hypercube_") + locale);
	app.installTranslator(&translator);

	GUI gui;
	gui.setWindowIcon(QIcon(QPixmap(APP_ICON)));
	gui.show();

	for (int i = 1; i < argc; i++)
		gui.openFile(argv[i]);

	return app.exec();
}
