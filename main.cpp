#include <QtGui>
#include "mainwindow.h"

int main(int argc, char * argv[])
{
    QApplication app(argc, argv);
    //test
    app.setApplicationName("uicrawler");
    //end test

    QUrl url;
    if (argc > 1)
        url = QUrl(argv[1]);
    else
    {
        url = "";
    }
    MainWindow *browser = new MainWindow(url);
    browser->show();
    return app.exec();
}
