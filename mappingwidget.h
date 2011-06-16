#ifndef MAPPINGWIDGET_H
#define MAPPINGWIDGET_H

#include <QDockWidget>
#include <QSet>
#include <QPair>
#include <QStringList>
#include <QVector>
#include <QMap>
#include "arrow.h"

class QTreeWidget;
class QListWidget;
class QListWidgetItem;
class PopupListWidget;

class MappingWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit MappingWidget(QWidget *parent = 0, const QString &title = "Mapping");
    void refresh(QVector<QPair<Arrow, Arrow> > &v);
    QString toString();
    void toVector(QVector<QPair<Arrow, Arrow> > &v);
private:
    QString arrowsToMapString(const Arrow &a, const Arrow &b);
    QTreeWidget *treeWidget;
    QMap<QString, Arrow> arrowMap;
    PopupListWidget *listWidget;
    QStringList allTargetItems;
    QString noTargetString, cancelString;
signals:

public slots:

private slots:
    void clickHandler();
    void popupSelectionHandler(QListWidgetItem *);
};

#endif // MAPPINGWIDGET_H
