#ifndef PROFILEWIDGET_H
#define PROFILEWIDGET_H

#include <QDockWidget>
class QListWidget;

class ProfileWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit ProfileWidget(QWidget *parent = 0, const QString &title = "Profile");
    int row();

signals:

public slots:

private:
    QListWidget *list;
};

#endif // PROFILEWIDGET_H
