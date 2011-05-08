#include "graphwidget.h"
#include <QLabel>
#include <QDir>
#include <QTextStream>
#include <QProcess>
#include <QDebug>
#include <QTextEdit>

GraphWidget::GraphWidget(QWidget *parent, const QString &title) :
        QDockWidget(title, parent)
{
    doc = new QTextEdit(this);
    doc->setReadOnly(true);
    setWidget(doc);
    this->setMinimumHeight(300);
}

bool GraphWidget::refresh(const QString &graph)
{
    QString tempPath = QDir::tempPath();

    QString dotPath, pngPath;

    for (int i = 0; ; i++)
    {
        dotPath = tempPath;
        dotPath += "/uic";
        dotPath += QString::number(i);
        dotPath += + ".dot";
        if (!QFile::exists(dotPath))
            break;
    }

    for (int j = 0; ; j++)
    {
        pngPath = tempPath;
        pngPath += "/uic";
        pngPath += QString::number(j);
        pngPath += ".png";
        if (!QFile::exists(pngPath))
            break;
    }

    QFile dotFile(dotPath);
    dotFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&dotFile);
    out << graph;
    dotFile.close();

#ifdef Q_OS_LINUX
    QProcess::execute("cd /tmp");
#endif

    QString cmd = "dot -Tpng ";
    cmd += dotPath;
    cmd += " -o ";
    cmd += pngPath;

    int ret = QProcess::execute(cmd);

    if (ret)
    {
        return false;
    }


    QString html = "<img src=\"";
    html += pngPath;
    html += "\"></img>";
    doc->setHtml(html);

    return true;
}

void GraphWidget::clear()
{
    doc->clear();
}
