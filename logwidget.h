#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QDockWidget>
#include <QTextEdit>

class Data;

namespace Ui {
    class LogWidget;
}

class LogWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit LogWidget(QWidget *parent, const QString &title, Data *data);
    void push(const QString &s);
    void clear();
    QString text();
    void setText(const QString &s);
    ~LogWidget();
private:
    Data *data;
    Ui::LogWidget *ui;
    QTextEdit *edit;
};

#endif // LOGWIDGET_H
