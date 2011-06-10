#ifndef MAPPINGWIDGET_H
#define MAPPINGWIDGET_H

#include <QDockWidget>
#include <QSet>
#include <QPair>
#include <QVector>
#include "arrow.h"

class QTreeWidget;

class MappingWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit MappingWidget(QWidget *parent = 0, const QString &title = "Mapping");
    void refresh(QVector<QPair<Arrow, Arrow> > &v);
    QString toString();
private:
    QTreeWidget *treeWidget;

signals:

public slots:

private slots:
    void clickHandler();
};

#endif // MAPPINGWIDGET_H
