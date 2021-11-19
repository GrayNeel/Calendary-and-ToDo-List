#include "gui/mainwindow.h"

#include "logic/client.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    Client client;
    w.show();
    return a.exec();
}
