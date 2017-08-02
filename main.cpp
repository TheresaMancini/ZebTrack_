#include "zebtrack.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    zebtrack w;
    w.show();

    return a.exec();
}
