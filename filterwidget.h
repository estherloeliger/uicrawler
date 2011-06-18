#ifndef FILTERWIDGET_H
#define FILTERWIDGET_H

#include <QDockWidget>
#include <QString>
#include <QVector>
#include "arrow.h"
#include "state.h"
class QTreeWidget;
class MainWindow;
class Data;

class FilterWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit FilterWidget(QWidget *parent, const QString &title, Data *data);
    ~FilterWidget();
    void refreshAffordances(const QString &s);
    void refreshAffordances(QVector<Arrow> &v);
    void refreshActions(const QString &s);
    void refreshActions(QVector<Arrow> &v);
    void refresh(QTreeWidget *tree, QVector<Arrow> &v);
    void clear();
    void dotToArrows(const QString &s, QVector<Arrow> &v); //public to be accessible from MainWindow
    void dotToStates(const QString &s, QVector<State> &v); //public to be accessible from MainWindow
protected slots:
    void matchAffordances();
    void matchActions();
    void matchTree(QTreeWidget *tree);
    void apply();
    QString filterString(QTreeWidget *tree);
private:
    Data *data;
    QTreeWidget *affordancesTree, *actionsTree;
    QVector<Arrow> allAffordances, allActions;
    MainWindow *parentWindow;
};

#endif // FILTERWIDGET_H
