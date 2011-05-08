#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QDockWidget>
#include <QTextEdit>

namespace Ui {
    class LogWidget;
}

class LogWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit LogWidget(QWidget *parent = 0, const QString &title = "Log");
    void push(const QString &s);
    void clear();
    QString text();
    ~LogWidget();
private:
    Ui::LogWidget *ui;
    QTextEdit *edit;
};

#endif // LOGWIDGET_H
