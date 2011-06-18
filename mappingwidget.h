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
class Data;

class MappingWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit MappingWidget(QWidget *parent, const QString &title, Data *data, int type);
    void refresh(); //new, uses data and type
    void applyChanges(); //new

    //QString toString(); //deprecated
    //void toVector(QVector<QPair<Arrow, Arrow> > &v); //deprecated

private:
    Data *data;
    int type; //could be MAPPING_TYPE_AFFORDANCE or MAPPING_TYPE_ACTION
    QString arrowsToMapString(const Arrow &a, const Arrow &b);
    QTreeWidget *treeWidget;
    QMap<int, Arrow> arrowMap; //for quick access to arrow from arrow ID
    PopupListWidget *listWidget;
    QStringList allTargetStrings;
    QString noTargetString;
signals:

public slots:

private slots:
    void clickHandler();
    void popupSelectionHandler(QListWidgetItem *);
};

#endif // MAPPINGWIDGET_H
