#include <QtGui>
#include "mainwindow.h"

int main(int argc, char * argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("uicrawler");

    MainWindow *browser = new MainWindow();
    browser->show();
    return app.exec();
}
