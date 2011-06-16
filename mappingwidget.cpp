#include "arrow.h"
#include "mappingwidget.h"
#include "popuplistwidget.h"
#include <QTreeWidget>
#include <QtGui>

MappingWidget::MappingWidget(QWidget *parent, const QString &title) :
    QDockWidget(title, parent), listWidget(0)
{
    treeWidget = new QTreeWidget(this);
    treeWidget->setColumnCount(2);
    treeWidget->setAlternatingRowColors(true);

    QStringList headerLabels;
    headerLabels.append("From");
    headerLabels.append("To");
    treeWidget->setHeaderLabels(headerLabels);

    setWidget(treeWidget);

    connect(treeWidget, SIGNAL(itemDoubleClicked (QTreeWidgetItem*,int)), this, SLOT(clickHandler())); //double click
    connect(treeWidget, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(clickHandler())); //e.g. enter pressed


    listWidget = new PopupListWidget(this);
    listWidget->setWindowFlags(Qt::Popup | Qt::Window | Qt::WindowCloseButtonHint);
    connect(listWidget, SIGNAL( itemActivated(QListWidgetItem *) ), this, SLOT( popupSelectionHandler(QListWidgetItem *) ) );

    noTargetString = "---";
    cancelString = "Cancel";
}

void MappingWidget::clickHandler()
{
    QTreeWidgetItem *item = treeWidget->currentItem();

    QRect itemRect, boxRect;
    itemRect = treeWidget->visualItemRect(item);

    int col1Width = treeWidget->columnWidth(0);
    QPoint bottomLeft = QPoint(itemRect.left() + col1Width, itemRect.bottom());

    boxRect = QRect(treeWidget->mapToGlobal(bottomLeft), QSize(itemRect.width() - col1Width, 200));

    QString source, destination;
    source = item->text(0);
    destination = item->text(1);

    int index = allTargetItems.indexOf(destination);
    if (index == -1)
        index = 0;

    listWidget->clear();

    int itemsCount = allTargetItems.count();
    for (int i = 0; i < itemsCount; i++)
    {
        listWidget->addItem(allTargetItems.at(i));
    }

    listWidget->setGeometry(boxRect);

    listWidget->setCurrentRow(index);
    listWidget->show();
    listWidget->setFocus();
}

QString MappingWidget::arrowsToMapString(const Arrow &a, const Arrow &b)
{
    QString s;
    s += "(\"";
    s += QString::number(a.source);
    s += "->";
    s += QString::number(a.target);
    s += "[label=\\\"";
    s += a.label;
    s += "\\\"]\",\"";
    s += QString::number(b.source);
    s += "->";
    s += QString::number(b.target);
    s += "[label=\\\"";
    s += b.label;
    s += "\\\"]\")";

    return s;
}

QString MappingWidget::toString()
{
    QVector<QPair<Arrow, Arrow> > v;
    toVector(v);

    QString s;

    s = "[";

    int count = v.count();
    for (int i = 0; i < count; i++)
    {
        if (i)
        {
            s += ",";
        }
        s += arrowsToMapString(v.at(i).first, v.at(i).second);
    }

    s += "]";

    return s;
}

void MappingWidget::toVector(QVector<QPair<Arrow, Arrow> > &v)
{
    //tbd: fill v using contents of widget
    int count = treeWidget->topLevelItemCount();
    QString s, t;
    for (int i = 0; i < count; i++)
    {
        QTreeWidgetItem *item = treeWidget->topLevelItem(i);

        s = item->text(0);
        t = item->text(1);

        if (arrowMap.contains(s) && arrowMap.contains(t))
        {
            v.append(qMakePair(arrowMap[s], arrowMap[t]));
        }
    }
}

void MappingWidget::refresh(QVector<QPair<Arrow, Arrow> > &v)
{
    allTargetItems.clear();
    allTargetItems.append(noTargetString);
    treeWidget->clear();
    QVectorIterator<QPair<Arrow, Arrow> > i(v);
    while (i.hasNext())
    {
        QPair<Arrow, Arrow> p = i.next();

        Arrow a, b;
        a = p.first;
        b = p.second;

        QString s, t;
        s = a.toString();
        t = b.toString();
        s = s.replace("\\n", "");
        t = t.replace("\\n", "");
        s = s.replace("\\r", "");
        t = t.replace("\\r", "");

        QStringList list;
        list.append(s);
        list.append(t);

        allTargetItems.append(t);

        arrowMap.insert(s, a);
        arrowMap.insert(t, b);

        QTreeWidgetItem *item = new QTreeWidgetItem(list);

        treeWidget->insertTopLevelItem(0, item);
    }

    allTargetItems.append(cancelString);
}

void MappingWidget::popupSelectionHandler(QListWidgetItem *item)
{
    QString selection = item->text();
    if (selection != "Cancel")
    {
        QTreeWidgetItem *treeItem = treeWidget->currentItem();
        treeItem->setText(1, item->text());
    }
    listWidget->hide();
}
