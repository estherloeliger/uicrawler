#ifndef FILTERWIDGET_H
#define FILTERWIDGET_H

#include <QDockWidget>
#include <QString>
#include <QVector>
#include "arrow.h"
class QTreeWidget;

class FilterWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit FilterWidget(QWidget *parent = 0, const QString &title = "Filter");
    ~FilterWidget();
    void refresh(QVector<Arrow> &v);
    void clear();
private:
    QTreeWidget *affordancesTree, *actionsTree;
};

#endif // FILTERWIDGET_H
