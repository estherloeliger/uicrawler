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
        //url = "file:///home/esther/uicrawler/test/index.html";
        //url = "file://" + QCoreApplication::applicationDirPath() + "/test/index.html";
        url = ("http://books.google.com/ebooks/reader?id=ey8EAAAAQAAJ&printsec=frontcover&output=reader&pg=GBS.PP1.w.1.0.0");
        //url = ("http://team.sourceforge.net/test.html");
        //qDebug() << url;
        //url = ("http://books.google.com/ebooks/reader?id=2Zc3AAAAYAAJ&printsec=frontcover&hl=en&output=reader&pg=GBS.PP1");
    }
    MainWindow *browser = new MainWindow(url);
    browser->show();
    return app.exec();
}
