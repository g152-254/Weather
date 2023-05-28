#include "mainwindow.h"
#include <QApplication>

extern QString sPath;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    sPath = a.applicationDirPath();
    MainWindow w;
    w.show();
    return a.exec();
}
