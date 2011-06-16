#include "popuplistwidget.h"
#include <QtGui>

PopupListWidget::PopupListWidget(QWidget *parent) :
    QListWidget(parent)
{
}

/*
void PopupListWidget::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
    {
        hide();
        return;
    }
    else if (e->key() == Qt::Key_Enter)
    {

    }
}
*/
