#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QDockWidget>
#include <QString>
class QTextEdit;

class GraphWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit GraphWidget(QWidget *parent, const QString &title = "Graph");
    bool refresh(const QString &graph);
    void clear();
private:
    QTextEdit *doc;

signals:

public slots:

};

#endif // GRAPHWIDGET_H
