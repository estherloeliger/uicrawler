#ifndef MODELER_H
#define MODELER_H

#include <QString>
#include <QSet>
#include <QWebView>
#include "logwidget.h"
#include "data.h"
#include "browser.h"

class MainWindow;

class Modeler
{
public:
    Modeler(
            Browser *browser,
            LogWidget *logWidget,
            QSet<QString> *blacklist,
            bool *stopFlag);
    bool run(Data *data, const QString &url, int profile);
private:
    QString arrowsToMapString(const Arrow &a, const Arrow &b);
    bool handleAction(Data *data, int index, int stateId, int arrowType, int profile, const QString &origin);
    bool handleHyperlink(Data *data, int index, const QString &url, int stateId, int arrowType, int profile, const QString &origin);
    QString linkLabel(const QString &current, const QString &href);
    int linkType(const QString &original, const QString &href);
    bool locationInScope(const QString &currentLocation, const QString &original);
    void recurse(
        Data *data,
        const QString &url,
        QString &affordanceLabel,
        QString &actionLabel,
        int parentStateId,
        int arrowType,
        int arrowFlags,
        int profile);
    Browser *browser;
    LogWidget *logWidget;
    QSet<QString> *blacklist;
    bool *stopFlag;
    MainWindow *mainWindow;
};

#endif // MODELER_H
