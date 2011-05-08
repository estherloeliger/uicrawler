#include "profilewidget.h"
#include <QListWidget>

ProfileWidget::ProfileWidget(QWidget *parent, const QString &title) :
    QDockWidget(title, parent)
{
    list = new QListWidget(this);
    list->addItem("Default");
    list->addItem("Limited fine motor skills");
    list->setCurrentRow(0);
    this->setWidget(list);
}

int ProfileWidget::row()
{
    return list->currentRow();
}
