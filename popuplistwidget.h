#ifndef POPUPLISTWIDGET_H
#define POPUPLISTWIDGET_H

#include <QListWidget>

class QKeyEvent;

class PopupListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit PopupListWidget(QWidget *parent = 0);
    //void keyPressEvent(QKeyEvent *e);

signals:

public slots:

};

#endif // POPUPLISTWIDGET_H
