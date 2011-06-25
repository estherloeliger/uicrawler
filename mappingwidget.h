#ifndef MAPPINGWIDGET_H
#define MAPPINGWIDGET_H

#include <QDockWidget>
#include <QSet>
#include <QPair>
#include <QStringList>
#include <QVector>
#include <QMap>
#include <QTreeWidget>
#include "arrow.h"
#include "state.h"

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
    void refresh();
    void applyChanges();

private:
    Data *data;
    int type; //could be MAPPING_TYPE_AFFORDANCE or MAPPING_TYPE_ACTION
    QString arrowsToMapString(const Arrow &a, const Arrow &b);
    QTreeWidget *arrowTree, *stateTree;
    QMap<int, Arrow> arrowMap; //for quick access to arrow from arrow ID
    QMap<int, State> stateMap; //for quick access to state from state ID
    PopupListWidget *listWidget;
    QStringList allArrowTargetStrings, allStateTargetStrings;
    QString noTargetString;
signals:

public slots:

private slots:
    void clickHandler(QTreeWidgetItem *);
    void popupSelectionHandler(QListWidgetItem *);
};

#endif // MAPPINGWIDGET_H
