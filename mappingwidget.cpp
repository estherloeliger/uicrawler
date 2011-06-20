#include "mappingwidget.h"
#include "data.h"
#include "popuplistwidget.h"
#include <QTreeWidget>
#include <QtGui>

MappingWidget::MappingWidget(QWidget *parent, const QString &title, Data *dataP, int typeP) :
    QDockWidget(title, parent), data(dataP), type(typeP)
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
    listWidget->setWindowFlags(Qt::Popup);
    connect(listWidget, SIGNAL( itemActivated(QListWidgetItem *) ), this, SLOT( popupSelectionHandler(QListWidgetItem *) ) );

    noTargetString = "---";
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

    int index = allTargetStrings.indexOf(destination);
    if (index == -1)
        index = 0;

    listWidget->clear();

    int itemsCount = allTargetStrings.count();
    for (int i = 0; i < itemsCount; i++)
    {
        listWidget->addItem(allTargetStrings.at(i));
    }

    listWidget->setGeometry(boxRect);

    listWidget->setCurrentRow(index);
    listWidget->show();
    listWidget->setFocus();
}

void MappingWidget::applyChanges()
{
    QVector<QPair<Arrow, Arrow> > *vp;

    switch (type)
    {
    case MAPPING_TYPE_AFFORDANCE:
        vp = &data->mapAffordanceToAbstractEdges;
        break;
    case MAPPING_TYPE_ACTION:
        vp = &data->mapActionToAbstractEdges;
        break;
    default:
        return;
    }

    vp->clear();

    int count = treeWidget->topLevelItemCount();
    QString s, t;
    for (int i = 0; i < count; i++)
    {
        QTreeWidgetItem *item = treeWidget->topLevelItem(i);

        s = item->text(0);
        t = item->text(1);

        if (t == this->noTargetString)
            continue;

        int idA, idB;

        QRegExp rx("(\\d+)$");

        int pos = -1;
        if ((pos = rx.indexIn(s)) != -1)
        {
            QString no = rx.cap(1);
            idA = no.toInt();
        }
        else
            continue;

        if ((pos = rx.indexIn(t)) != -1)
        {
            QString no = rx.cap(1);
            idB = no.toInt();
        }
        else
            continue;

        Arrow a, b;
        if (arrowMap.contains(idA) && arrowMap.contains(idB))
        {
            a = arrowMap[idA];
            b = arrowMap[idB];
        }
        else
        {
            continue;
        }

        vp->push_back(qMakePair(a, b));
    }
}

void MappingWidget::refresh()
{
    arrowMap.clear();
    allTargetStrings.clear();
    treeWidget->clear();

    QVector<QPair<Arrow, Arrow> > v;
    switch (type)
    {
    case MAPPING_TYPE_AFFORDANCE:
        v = data->mapAffordanceToAbstractEdges;
        break;
    case MAPPING_TYPE_ACTION:
        v = data->mapActionToAbstractEdges;
        break;
    default:
        break;
    }

    allTargetStrings.append(noTargetString);

    QVectorIterator<QPair<Arrow, Arrow> > i(v);

    while (i.hasNext())
    {
        QPair<Arrow, Arrow> p = i.next();

        Arrow a, b;
        a = p.first;
        b = p.second;

        QString s, t;
        s = a.toString() + " #" + QString::number(a.id);
        t = b.toString() + " #" + QString::number(b.id);
        s = s.replace("\n", "");
        t = t.replace("\n", "");
        s = s.replace("\r", "");
        t = t.replace("\r", "");

        QStringList list;
        list.append(s);
        list.append(t);

        allTargetStrings.append(t);

        arrowMap.insert(a.id, a);
        arrowMap.insert(b.id, b);

        QTreeWidgetItem *item = new QTreeWidgetItem(list);

        treeWidget->insertTopLevelItem(0, item);
    }
}

void MappingWidget::popupSelectionHandler(QListWidgetItem *item)
{
    QString selection = item->text();
    if (!selection.isEmpty())
    {
        QTreeWidgetItem *treeItem = treeWidget->currentItem();
        treeItem->setText(1, item->text());
    }
    listWidget->hide();
}
