#include "mappingwidget.h"
#include "arrow.h"
#include <QTreeWidget>
#include <QtGui>

MappingWidget::MappingWidget(QWidget *parent, const QString &title) :
    QDockWidget(title, parent)
{
    treeWidget = new QTreeWidget(this);
    treeWidget->setColumnCount(2);

    QStringList headerLabels;
    headerLabels.append("From");
    headerLabels.append("To");
    treeWidget->setHeaderLabels(headerLabels);

    setWidget(treeWidget);

    connect(treeWidget, SIGNAL(itemClicked (QTreeWidgetItem*,int)), this, SLOT(clickHandler())); //single/double click
    connect(treeWidget, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(clickHandler())); //e.g. enter pressed
}

void MappingWidget::clickHandler()
{
    QString noneString = "----";
    QStringList items;

    items.append(noneString);

    int count = treeWidget->topLevelItemCount();
    for (int i = 0; i < count; i++)
    {
        QTreeWidgetItem *current = treeWidget->topLevelItem(i);
        items.append(current->text(1));
    }

    bool ok;
    QTreeWidgetItem *item = treeWidget->currentItem();
    QString source, destination;
    source = item->text(0);
    destination = item->text(1);

    int index = items.indexOf(destination);
    if (index == -1)
        index = 0;

    QString label = "Map '" + source + "' to:";
    QString selection;

    /*
    QString selection = QInputDialog::getItem(this, "Select mapping", label, items, index, false, &ok);

    if (!ok || selection.isEmpty())
        return;

    item->setText(1, selection);
    */

    //http://doc.qt.nokia.com/latest/network-googlesuggest.html
    QListWidget *list = new QListWidget(0);
    list->setWindowFlags(Qt::Popup);
    list->setMouseTracking(true);

    int itemsCount = items.count();
    for (int i = 0; i < itemsCount; i++)
    {
        list->addItem(items.at(i));;
    }

}

QString MappingWidget::toString()
{
    //tbd
    return "";
}

void MappingWidget::refresh(QVector<QPair<Arrow, Arrow> > &v)
{
    treeWidget->clear();
    QVectorIterator<QPair<Arrow, Arrow> > i(v);
    while (i.hasNext())
    {
        QPair<Arrow, Arrow> p = i.next();

        Arrow a, b;
        a = p.first;
        b = p.second;

        QStringList list;
        list.append(a.toString());
        list.append(b.toString());

        qDebug() << a.toString();
        qDebug() << b.toString();

        QTreeWidgetItem *item = new QTreeWidgetItem(list);

        treeWidget->insertTopLevelItem(0, item);
    }
}
