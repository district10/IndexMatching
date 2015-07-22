#include "selector.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Selector w;
    w.show();

    return a.exec();
}
