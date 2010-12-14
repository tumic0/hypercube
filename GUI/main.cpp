#include <QApplication>
#include "GUI/gui.h"
#include "CORE/config.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GUI g;
    g.setWindowIcon(QIcon(QPixmap(APP_ICON)));
    g.show();
    return a.exec();
}
