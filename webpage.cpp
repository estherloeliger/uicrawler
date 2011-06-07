#include "webpage.h"

WebPage::WebPage(QWidget *parent) :
    QWebPage(parent)
{
}

/*
QString WebPage::userAgentForUrl(const QUrl &url ) const
{
    Q_UNUSED(url);
    return QString("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/534.30 (KHTML, like Gecko) Chrome/12.0.742.60 Safari/534.30"); //Chrome 12
    //return QString("Mozilla/5.0 (X11; Linux x86_64; rv:2.0.1) Gecko/20110506 Firefox/4.0.1"); //Firefox 4.0.1
}
*/
