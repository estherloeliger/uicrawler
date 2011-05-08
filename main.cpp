#include <QtGui>
#include "mainwindow.h"

int main(int argc, char * argv[])
{
    QApplication app(argc, argv);
    QUrl url;
    if (argc > 1)
        url = QUrl(argv[1]);
    else
    {
        //url = "file:///home/esther/uicrawler/test/index.html";
        //url = "file://" + QCoreApplication::applicationDirPath() + "/test/index.html";
        url = ("http://team.sourceforge.net/test.html");
        //qDebug() << url;
    }
    MainWindow *browser = new MainWindow(url);
    browser->show();
    return app.exec();
}
