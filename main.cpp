#include "mainwindow.h"
#include <QApplication>

//REVIEW [STRCUT][UNUSED_INCLUDE] : why does main needs to know those 3 inclusions? they're not being use.
//                                  remove unnecessary code.
#include <QGraphicsRectItem>
#include <QGraphicsView>
#include "GameManager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.show();

    return a.exec();
}
