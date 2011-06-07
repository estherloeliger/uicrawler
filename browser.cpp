#include "browser.h"
#include "webpage.h"
#include "mainwindow.h"
#include <QWebFrame>
#include <QWebSettings>
#include <QNetworkDiskCache>
#include <QNetworkConfigurationManager>
#include <QDir>

Browser::Browser(QMainWindow *parent, bool *busyP) : QWebView(parent), busy(busyP)
{
    WebPage *page = new WebPage(this);
    this->setPage((QWebPage *)page);

    QWebSettings *webSettings = this->page()->settings();
    webSettings->setAttribute(QWebSettings::AutoLoadImages, false);

    QNetworkDiskCache *diskCache = new QNetworkDiskCache(this);
    diskCache->setCacheDirectory(QDir::tempPath());
    this->page()->networkAccessManager()->setCache(diskCache);

    QNetworkConfigurationManager configManager;
    QNetworkConfiguration config = configManager.defaultConfiguration();
    this->page()->networkAccessManager()->setConfiguration(config);
}

bool Browser::navigate(const QString &url)
{
    //don't open media files
    if (
        //tbd: check modeller to ensure blacklist entries aren't passed to navigate
        //blacklist.contains(url) ||
        url.endsWith(".mp3") ||
        url.endsWith(".mp4") ||
        url.endsWith(".flv") ||
        url.endsWith(".pdf") ||
        url.contains("mailto:") ||
        url.contains("https:"))
    {
        return false;
    }

    QString code = "window.location = ";
    code += "'";
    code += url;
    code += "'";
    //this->page()->mainFrame()->evaluateJavaScript(code);
    runJS(code);
    wait(false);

    return true;
}

void Browser::wait(bool force, int timeout)
{
    QTime dieTime = QTime::currentTime().addSecs(timeout);
    while (QTime::currentTime() < dieTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        if (!(*busy) && !force)
            break;
    }

    //fall-back: timeout if busy == true still
    if (*busy)
    {
        qDebug() << "Extended wait...";
        dieTime = QTime::currentTime().addSecs(10);
        while ( QTime::currentTime() < dieTime )
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
            if (!(*busy))
                break;
        }
    }

    *busy = false; //override in case onFinished has not signalled success
}

int Browser::JStoInt(const QString &cmd)
{
    QVariant result = this->page()->mainFrame()->evaluateJavaScript(cmd);
    wait();
    return result.toInt();
}

QString Browser::JStoString(const QString &cmd)
{
    QVariant result = this->page()->mainFrame()->evaluateJavaScript(cmd);
    wait();
    return result.toString();
}

void Browser::runJS(const QString &cmd)
{
    this->page()->mainFrame()->evaluateJavaScript(cmd);
}
