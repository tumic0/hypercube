#include <QApplication>
#include "GUI/gui.h"
#include "CORE/config.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QString locale = QLocale::system().name();
	QTranslator translator;
	translator.load(QString("hypercube_") + locale, "/home/tumic/workspace/hypercube/lang/");
	app.installTranslator(&translator);

	GUI gui;
	gui.setWindowIcon(QIcon(QPixmap(APP_ICON)));
	gui.show();
	return app.exec();
}
