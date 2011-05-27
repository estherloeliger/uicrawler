#include "logwidget.h"

LogWidget::LogWidget(QWidget *parent, const QString &title) :
    QDockWidget(title, parent)
{
    edit = new QTextEdit(this);
    edit->document()->setMaximumBlockCount(4096);
    this->setWidget(edit);
    this->setMinimumWidth(512);
}

LogWidget::~LogWidget()
{
    //tbd: check if edit deleted by parent
}

void LogWidget::push(const QString &s)
{
    edit->append(s);
    edit->ensureCursorVisible();
}

void LogWidget::clear()
{
    edit->clear();
}

QString LogWidget::text()
{
    return edit->toPlainText();
}

void LogWidget::setText(const QString &s)
{
    edit->clear();
    edit->setText(s);
}
