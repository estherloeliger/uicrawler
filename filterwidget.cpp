#include "filterwidget.h"
#include "ui_filterwidget.h"
#include <QGridLayout>
#include <QPushButton>
#include <QStringList>

FilterWidget::FilterWidget(QWidget *parent, const QString &title) :
    QDockWidget(title, parent)
{

    /*
    QWidget *wi = new QWidget;
    wi->setLayout(hbox);

    QDockWidget *dw = new QDockWidget(tr("Command"), this);
    dw->setWidget(wi);
    addDockWidget(Qt::BottomDockWidgetArea, dw);
     */

    affordancesTree = new QTreeWidget(this);
    affordancesTree->setColumnCount(2);
    //actionsTree = new QTreeWidget(this);
    //actionsTree->setColumnCount(2);

    QStringList list;
    list << "hola";
    list << "hello";

    QTreeWidgetItem *item = new QTreeWidgetItem(list);
    affordancesTree->addTopLevelItem(item);

    QPushButton *button = new QPushButton("Select all", this);

    QGridLayout *layout = new QGridLayout;

    layout->setRowMinimumHeight(0, 50);
    layout->setColumnMinimumWidth(0, 100);
    layout->setColumnMinimumWidth(1, 100);
    layout->addWidget(affordancesTree, 0, 0);
    //layout->addWidget(actionsTree, 1, 0);
    layout->addWidget(button, 0, 1);

    this->setLayout(layout);

/*

     QGridLayout *mainLayout = new QGridLayout;
     mainLayout->setColumnStretch(0, 1);
     mainLayout->setColumnStretch(3, 1);
     mainLayout->addWidget(renderArea, 0, 0, 1, 4);
     mainLayout->setRowMinimumHeight(1, 6);
     mainLayout->addWidget(shapeLabel, 2, 1, Qt::AlignRight);
     mainLayout->addWidget(shapeComboBox, 2, 2);
     mainLayout->addWidget(penWidthLabel, 3, 1, Qt::AlignRight);
     mainLayout->addWidget(penWidthSpinBox, 3, 2);
     mainLayout->addWidget(penStyleLabel, 4, 1, Qt::AlignRight);
     mainLayout->addWidget(penStyleComboBox, 4, 2);
     mainLayout->addWidget(penCapLabel, 5, 1, Qt::AlignRight);
     mainLayout->addWidget(penCapComboBox, 5, 2);
     mainLayout->addWidget(penJoinLabel, 6, 1, Qt::AlignRight);
     mainLayout->addWidget(penJoinComboBox, 6, 2);
     mainLayout->addWidget(brushStyleLabel, 7, 1, Qt::AlignRight);
     mainLayout->addWidget(brushStyleComboBox, 7, 2);
     mainLayout->setRowMinimumHeight(8, 6);
     mainLayout->addWidget(otherOptionsLabel, 9, 1, Qt::AlignRight);
     mainLayout->addWidget(antialiasingCheckBox, 9, 2);
     mainLayout->addWidget(transformationsCheckBox, 10, 2);
     setLayout(mainLayout);
 */
}

FilterWidget::~FilterWidget()
{
}

void FilterWidget::refresh(QVector<Arrow> &v)
{
    int count = v.count();
    for (int i = 0; i < count; i++)
    {
        ;
    }
}

void FilterWidget::clear()
{
    affordancesTree->clear();
    actionsTree->clear();
}
