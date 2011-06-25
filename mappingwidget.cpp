#include "mappingwidget.h"
#include "data.h"
#include "popuplistwidget.h"
#include <QtGui>

MappingWidget::MappingWidget(QWidget *parent, const QString &title, Data *dataP, int typeP) :
    QDockWidget(title, parent), data(dataP), type(typeP)
{
    stateTree = new QTreeWidget(this);
    stateTree->setColumnCount(2);
    stateTree->setAlternatingRowColors(true);

    arrowTree = new QTreeWidget(this);
    arrowTree->setColumnCount(2);
    arrowTree->setAlternatingRowColors(true);

    QStringList stateHeaderLabels;
    stateHeaderLabels.append("Source node");
    stateHeaderLabels.append("Target node");
    stateTree->setHeaderLabels(stateHeaderLabels);
    QStringList arrowHeaderLabels;
    arrowHeaderLabels.append("Source edge");
    arrowHeaderLabels.append("Target edge");
    arrowTree->setHeaderLabels(arrowHeaderLabels);

    QGridLayout *layout = new QGridLayout();

    layout->addWidget(stateTree, 0, 0);
    layout->addWidget(arrowTree, 1, 0);

    QWidget *wi = new QWidget;
    wi->setLayout(layout);

    setWidget(wi);
    connect(arrowTree, SIGNAL(itemDoubleClicked (QTreeWidgetItem*,int)), this, SLOT(clickHandler(QTreeWidgetItem*))); //double click
    connect(arrowTree, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(clickHandler(QTreeWidgetItem*))); //e.g. enter pressed
    connect(stateTree, SIGNAL(itemDoubleClicked (QTreeWidgetItem*,int)), this, SLOT(clickHandler(QTreeWidgetItem*))); //double click
    connect(stateTree, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(clickHandler(QTreeWidgetItem*))); //e.g. enter pressed

    listWidget = new PopupListWidget(this);
    listWidget->setWindowFlags(Qt::Popup);
    connect(listWidget, SIGNAL( itemActivated(QListWidgetItem *) ), this, SLOT( popupSelectionHandler(QListWidgetItem *) ) );

    noTargetString = "---";
}

void MappingWidget::clickHandler(QTreeWidgetItem* item)
{
    QTreeWidget *tree = item->treeWidget();
    QStringList allTargetStrings;
    bool isArrowItem = item->text(0).contains("->");
    allTargetStrings = isArrowItem ? allArrowTargetStrings : allStateTargetStrings;

    QRect itemRect, boxRect;
    itemRect = tree->visualItemRect(item);

    int col1Width = tree->columnWidth(0);
    QPoint bottomLeft = QPoint(itemRect.left() + col1Width, itemRect.bottom());

    boxRect = QRect(tree->mapToGlobal(bottomLeft), QSize(itemRect.width() - col1Width, 200));

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
        if(isArrowItem)
        {
            listWidget->addItem(allTargetStrings.at(i));
        }
        else
        {
            listWidget->addItem(allTargetStrings.at(i));
        }
    }

    listWidget->setGeometry(boxRect);

    listWidget->setCurrentRow(index);
    listWidget->show();
    listWidget->setFocus();
}

void MappingWidget::applyChanges()
{
    QVector<QPair<Arrow, Arrow> > *vpa;
    QVector<QPair<State, State> > *vps;

    switch (type)
    {
    case MAPPING_TYPE_AFFORDANCE:
        vpa = &data->mapAffordanceToAbstractEdges;
        vps = &data->mapAffordanceToAbstractNodes;
        break;
    case MAPPING_TYPE_ACTION:
        vpa = &data->mapActionToAbstractEdges;
        vps = &data->mapActionToAbstractNodes;
        break;
    default:
        return;
    }
    vpa->clear();
    vps->clear();

    int count = stateTree->topLevelItemCount();
    QString s, t;
    for (int i = 0; i < count; i++)
    {
        QTreeWidgetItem *item = stateTree->topLevelItem(i);

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

        State a, b;
        if (stateMap.contains(idA) && stateMap.contains(idB))
        {
            a = stateMap[idA];
            b = stateMap[idB];
        }
        else
        {
            continue;
        }

        vps->push_back(qMakePair(a, b));
    }


    count = arrowTree->topLevelItemCount();
    for (int i = 0; i < count; i++)
    {
        QTreeWidgetItem *item = arrowTree->topLevelItem(i);

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

        vpa->push_back(qMakePair(a, b));
    }
}

void MappingWidget::refresh()
{
    stateMap.clear();
    arrowMap.clear();
    allArrowTargetStrings.clear();
    allStateTargetStrings.clear();
    arrowTree->clear();
    stateTree->clear();

    QVector<QPair<Arrow, Arrow> > va;
    QVector<QPair<State, State> > vs;
    switch (type)
    {
    case MAPPING_TYPE_AFFORDANCE:
        va = data->mapAffordanceToAbstractEdges;
        vs = data->mapAffordanceToAbstractNodes;
        break;
    case MAPPING_TYPE_ACTION:
        va = data->mapActionToAbstractEdges;
        vs = data->mapActionToAbstractNodes;
        break;
    default:
        break;
    }

    allStateTargetStrings.append(noTargetString);

    QVectorIterator<QPair<State, State> > is(vs);

    while (is.hasNext())
    {
        QPair<State, State> p = is.next();

        State a, b;
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

        allStateTargetStrings.append(t);

        stateMap.insert(a.id, a);
        stateMap.insert(b.id, b);

        QTreeWidgetItem *item = new QTreeWidgetItem(list);

        stateTree->insertTopLevelItem(0, item);
    }


    allArrowTargetStrings.append(noTargetString);

    QVectorIterator<QPair<Arrow, Arrow> > ia(va);

    while (ia.hasNext())
    {
        QPair<Arrow, Arrow> p = ia.next();

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

        allArrowTargetStrings.append(t);

        arrowMap.insert(a.id, a);
        arrowMap.insert(b.id, b);

        QTreeWidgetItem *item = new QTreeWidgetItem(list);

        arrowTree->insertTopLevelItem(0, item);
    }
}

void MappingWidget::popupSelectionHandler(QListWidgetItem *item)
{
    QString selection = item->text();
    QTreeWidgetItem *treeItem = (selection.contains("->")) ? arrowTree->currentItem() : stateTree->currentItem();
    if (!selection.isEmpty())
    {
        treeItem->setText(1, item->text());
    }
    listWidget->hide();
}
