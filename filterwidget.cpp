#include "filterwidget.h"
#include <QGridLayout>
#include <QPushButton>
#include <QStringList>
#include <QTreeWidget>
#include <QDebug>
#include <QInputDialog>
#include <QSet>
#include <QRegExp>
#include "mainwindow.h"

FilterWidget::FilterWidget(QWidget *parent, const QString &title) :
    QDockWidget(title, parent)
{
    parentWindow = (MainWindow *)parent;

    affordancesTree = new QTreeWidget(this);
    affordancesTree->setColumnCount(5);
    actionsTree = new QTreeWidget(this);
    actionsTree->setColumnCount(5);

    affordancesTree->setColumnWidth(0, 50);
    affordancesTree->setColumnWidth(1, 150);
    affordancesTree->setColumnWidth(2, 50);
    affordancesTree->setColumnWidth(3, 50);
    affordancesTree->setColumnWidth(4, 50);

    actionsTree->setColumnWidth(0, 50);
    actionsTree->setColumnWidth(1, 150);
    actionsTree->setColumnWidth(2, 50);
    actionsTree->setColumnWidth(3, 50);
    actionsTree->setColumnWidth(4, 50);

    QStringList affordanceHeaderLabels;
    affordanceHeaderLabels << "Show" << "Affordance" << "Source" << "Target" << "#";

    QStringList actionHeaderLabels;
    actionHeaderLabels << "Show" << "Action" << "Source" << "Target" << "#";

    affordancesTree->setHeaderLabels(affordanceHeaderLabels);
    actionsTree->setHeaderLabels(actionHeaderLabels);

    QPushButton *matchAffordances = new QPushButton("Match...", this);
    QPushButton *matchActions = new QPushButton("Match...", this);
    QPushButton *apply = new QPushButton("Apply", this);

    QGridLayout *layout = new QGridLayout();

    layout->addWidget(affordancesTree, 0, 0);
    layout->addWidget(matchAffordances, 0, 1);

    layout->addWidget(actionsTree, 1, 0);
    layout->addWidget(matchActions, 1, 1);

    layout->addWidget(apply, 2, 0);

    QWidget *wi = new QWidget;
    wi->setLayout(layout);

    setWidget(wi);

    connect(matchAffordances, SIGNAL(clicked()), this, SLOT(matchAffordances()));
    connect(matchActions, SIGNAL(clicked()), this, SLOT(matchActions()));
    connect(apply, SIGNAL(clicked()), this, SLOT(apply()));
}

void FilterWidget::matchAffordances()
{
    matchTree(affordancesTree);
}

void FilterWidget::matchActions()
{
    matchTree(actionsTree);
}

void FilterWidget::matchTree(QTreeWidget *tree)
{
    bool ok;
    QString text = QInputDialog::getText(
        this,
        tr("Match affordances"),
        tr("Regular expression (e.g. 'Go to .*')                                                                                     "),
        QLineEdit::Normal,
        "",
        &ok);
    if (!ok || text.isEmpty())
        return;

    QRegExp regex(text);
    if (!regex.isValid())
    {
        qDebug() << text << " is not a valid regular expression";
        return;
    }

    int count = tree->topLevelItemCount();
    for (int i = 0; i < count; i++)
    {
        QTreeWidgetItem *item = tree->topLevelItem(i);
        QString label = item->text(1);
        bool check = regex.exactMatch(label);
        item->setCheckState(0, (check) ? Qt::Checked : Qt::Unchecked);
    }
}

void FilterWidget::apply()
{
    QString affordanceFilter, actionFilter;

    affordanceFilter = filterString(affordancesTree);
    actionFilter = filterString(actionsTree);

    parentWindow->visualizeAffordances(affordanceFilter);
    parentWindow->visualizeActions(actionFilter);
}

QString FilterWidget::filterString(QTreeWidget *tree)
{
    QString filter;

    int count = tree->topLevelItemCount();
    for (int i = 0; i < count; i++)
    {
        QTreeWidgetItem *item = tree->topLevelItem(i);
        if (item->checkState(0) == Qt::Checked)
        {
            filter += item->text(2);
            filter += ",";
            filter += item->text(3);
            filter += ",";
        }
    }

    return filter;
}

FilterWidget::~FilterWidget()
{
}

void FilterWidget::dotToArrows(const QString &s, QVector<Arrow> &v)
{
    v.clear();

    QStringList list = s.split("\n");

    int count = list.count();

    QString line;
    QRegExp match("(\\d+) -> (\\d+) \\[label=\"([A-Za-z0-9 ]+)");

    if (!match.isValid())
    {
        qDebug() << "invalid regex: " << match.errorString();
        return;
    }

    int id = 0;
    bool active = true;
    for (int i = 0; i < count; i++)
    {
        line = list.at(i);
        active = !line.startsWith("//");

        QString source, target, label;

        if (match.indexIn(line) != -1)
        {
            source = match.cap(1);
            target = match.cap(2);
            label = match.cap(3);
            v.push_back(
                Arrow(
                    id++,
                    source.toInt(),
                    target.toInt(),
                    ARROW_TYPE_LINK_SAME_HOST,
                    active,
                    label));
        }
    }
}

void FilterWidget::refreshAffordances(const QString &s)
{
    QVector<Arrow> v;
    dotToArrows(s, v);
    refreshAffordances(v);
}

void FilterWidget::refreshActions(const QString &s)
{
    QVector<Arrow> v;
    dotToArrows(s, v);
    refreshActions(v);
}

void FilterWidget::refreshAffordances(QVector<Arrow> &v)
{
    allAffordances = v;
    refresh(this->affordancesTree, v);
}

void FilterWidget::refreshActions(QVector<Arrow> &v)
{
    allActions = v;
    refresh(this->actionsTree, v);
}

void FilterWidget::refresh(QTreeWidget *tree, QVector<Arrow> &v)
{
    tree->clear();
    int count = v.count();
    for (int i = 1; i < count; i++)
    {
        QStringList list;
        Arrow a = v.at(i);
        list.append("");
        list.append(a.label);
        list.append(QString::number(a.source));
        list.append(QString::number(a.target));
        list.append(QString::number(i));
        QTreeWidgetItem *item = new QTreeWidgetItem(list);
        item->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        item->setCheckState(0, (a.active) ? Qt::Checked : Qt::Unchecked);
        tree->addTopLevelItem(item);
    }
}

void FilterWidget::clear()
{
    affordancesTree->clear();
    actionsTree->clear();
}

