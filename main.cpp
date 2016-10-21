#include "projecteurlaser.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ProjecteurLaser w;
    w.show();

    return a.exec();
}
