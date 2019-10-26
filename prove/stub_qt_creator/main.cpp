#include "guiwrapper.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GuiWrapper w;
    w.run();

    return a.exec();
}
