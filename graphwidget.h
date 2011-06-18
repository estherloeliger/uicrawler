#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QDockWidget>
#include <QString>

class QTextEdit;
class Data;

class GraphWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit GraphWidget(QWidget *parent, const QString &title, Data *data, int type);
    //bool refresh(const QString &graph); //deprecated
    bool refresh();
    void clear();
private:
    Data *data;
    int type;
    QTextEdit *doc;

signals:

public slots:

};

#endif // GRAPHWIDGET_H
