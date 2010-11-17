#include <QApplication>
#include "GUI/gui.h"
#include "CORE/config.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GUI g;
    g.setGeometry(100, 100, 800, 600);
    g.setWindowIcon(QIcon(QPixmap(APP_ICON)));
    g.show();
    return a.exec();
}
