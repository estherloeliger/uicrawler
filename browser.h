#ifndef BROWSER_H
#define BROWSER_H

#include <QWebView>
#include <QtGui>

class MainWindow;

class Browser : public QWebView
{
public:
    Browser(QMainWindow *parent, bool *busy);
    bool navigate(const QString &url);
    void wait(bool force = false, int timeout = 1);
    int JStoInt(const QString &cmd);
    QString JStoString(const QString &cmd);
    void runJS(const QString &cmd);
private:
    bool *busy;
};

#endif // BROWSER_H
