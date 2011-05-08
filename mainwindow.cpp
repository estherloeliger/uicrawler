#include <QtGui>
#include <QtWebKit>
#include <map>
#include "mainwindow.h"
#include "logwidget.h"
#include "graphwidget.h"
#include "profilewidget.h"
#define MAX_STATE_COUNT 64;

MainWindow::MainWindow(const QUrl& url) : stopFlag(false)
{
    progress = 0;

    this->setWindowIcon(QIcon(":/uicrawler.ico"));

    QFile file;
    file.setFileName(":/jquery-latest.js");
    file.open(QIODevice::ReadOnly);
    jQuery = file.readAll();
    file.close();

    QFile fileUic;

    //tbd: use resource when code is more or less ready
    //fileUic.setFileName(":/uicrawler.js");
#ifdef Q_WS_WIN
    fileUic.setFileName("C:/Qt/2010.05/tools/uicrawler/uicrawler.js");
#endif
#ifdef Q_WS_X11
    fileUic.setFileName("/home/esther/uicrawler/uicrawler.js");
#endif
    fileUic.open(QIODevice::ReadOnly);
    uicrawler = fileUic.readAll();
    fileUic.close();

    QNetworkProxyFactory::setUseSystemConfiguration(true);

    view = new QWebView(this);
    view->load(url);
    connect(view, SIGNAL(loadFinished(bool)), SLOT(adjustLocation()));
    connect(view, SIGNAL(titleChanged(QString)), SLOT(adjustTitle()));
    connect(view, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
    connect(view, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));
    connect(view, SIGNAL(loadStarted()), SLOT(setBusyFlag()));

    locationEdit = new QLineEdit(this);
    locationEdit->setSizePolicy(QSizePolicy::Expanding, locationEdit->sizePolicy().verticalPolicy());
    connect(locationEdit, SIGNAL(returnPressed()), SLOT(changeLocation()));

    QToolBar *toolBar = addToolBar(tr("Navigation"));
    toolBar->addAction(view->pageAction(QWebPage::Back));
    toolBar->addAction(view->pageAction(QWebPage::Forward));
    toolBar->addAction(view->pageAction(QWebPage::Reload));
    toolBar->addAction(view->pageAction(QWebPage::Stop));
    toolBar->addWidget(locationEdit);

    toolBar->addAction("Model", this, SLOT(model()));
    toolBar->addSeparator();
    toolBar->addAction("Stats", this, SLOT(stats()));
    toolBar->addAction("State", this, SLOT(state()));
    toolBar->addSeparator();
    toolBar->addAction("Filter...", this, SLOT(openFilterDialog()));
    toolBar->addSeparator();
    toolBar->addAction("Stop", this, SLOT(stop()));
    toolBar->addAction("Clear", this, SLOT(clearLogs()));

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction("Go to files", this, SLOT(goToFiles()));

    //tbd

    QMenu *toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction("Model", this, SLOT(model()));
    toolsMenu->addSeparator();
    toolsMenu->addAction("Stats", this, SLOT(stats()));
    toolsMenu->addAction("State", this, SLOT(state()));
    toolsMenu->addSeparator();
    toolsMenu->addAction("Filter...", this, SLOT(openFilterDialog()));
    toolsMenu->addSeparator();
    toolsMenu->addAction("Stop", this, SLOT(stop()));
    toolsMenu->addAction("Clear", this, SLOT(clearLogs()));

    profileWidget = new ProfileWidget(this);
    profileWidget->setAllowedAreas(Qt::RightDockWidgetArea);

    logWidget = new LogWidget(this);
    logWidget->setAllowedAreas(Qt::RightDockWidgetArea);

    dotWidgetAffordances = new LogWidget(this, "Affordances");
    dotWidgetAffordances->setAllowedAreas(Qt::RightDockWidgetArea);

    dotWidgetActions = new LogWidget(this, "Actions");
    dotWidgetActions->setAllowedAreas(Qt::RightDockWidgetArea);

    graphWidgetAffordances = new GraphWidget(this, "Affordance graph");
    graphWidgetAffordances->setAllowedAreas(Qt::RightDockWidgetArea);

    graphWidgetActions = new GraphWidget(this, "Action graph");
    graphWidgetActions->setAllowedAreas(Qt::RightDockWidgetArea);

    addDockWidget(Qt::RightDockWidgetArea, logWidget);
    addDockWidget(Qt::RightDockWidgetArea, dotWidgetAffordances);
    addDockWidget(Qt::RightDockWidgetArea, dotWidgetActions);
    addDockWidget(Qt::RightDockWidgetArea, profileWidget);

    addDockWidget(Qt::BottomDockWidgetArea, graphWidgetAffordances);
    addDockWidget(Qt::BottomDockWidgetArea, graphWidgetActions);

    this->tabifyDockWidget(logWidget, dotWidgetAffordances);
    this->tabifyDockWidget(logWidget, dotWidgetActions);
    this->tabifyDockWidget(logWidget, profileWidget);
    logWidget->raise();

    this->tabifyDockWidget(graphWidgetAffordances, graphWidgetActions);
    graphWidgetAffordances->raise();

    setCentralWidget(view);
    setUnifiedTitleAndToolBarOnMac(true);

    this->setMinimumWidth(1024);
    this->setMinimumHeight(768);
    locationEdit->setFocus();
}

void MainWindow::viewSource()
{
    QNetworkAccessManager* accessManager = view->page()->networkAccessManager();
    QNetworkRequest request(view->url());
    QNetworkReply* reply = accessManager->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(slotSourceDownloaded()));
}

void MainWindow::slotSourceDownloaded()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(const_cast<QObject*>(sender()));
    QTextEdit* textEdit = new QTextEdit(NULL);
    textEdit->setAttribute(Qt::WA_DeleteOnClose);
    textEdit->show();
    textEdit->setPlainText(reply->readAll());
    reply->deleteLater();
}

void MainWindow::adjustLocation()
{
    locationEdit->setText(view->url().toString());
}

void MainWindow::changeLocation()
{
    QUrl url = QUrl(QUrl::fromUserInput(locationEdit->text()));

    view->load(url);
    view->setFocus();
}

void MainWindow::adjustTitle()
{
    if (progress <= 0 || progress >= 100)
        setWindowTitle("uicrawler - " + view->title());
    else
        setWindowTitle(QString("%1 (%2%)").arg(view->title()).arg(progress));
}

void MainWindow::setProgress(int p)
{
    busy = true;
    progress = p;
    adjustTitle();
}

void MainWindow::finishLoading(bool)
{
    busy = false;
    progress = 100;
    adjustTitle();
    view->page()->mainFrame()->evaluateJavaScript(jQuery);
    view->page()->mainFrame()->evaluateJavaScript(uicrawler);
}

void MainWindow::state()
{
    int profile = profileWidget->row();

    QString code;
    code = "state(";
    code += QString::number(profile);
    code += ")";

    logWidget->push(stringFromJs(code));
}

void MainWindow::stats()
{
    int profile = profileWidget->row();

    QString code;
    code = "stats(";
    code += QString::number(profile);
    code += ")";

    logWidget->push(stringFromJs(code));
}

void MainWindow::clearLogs()
{
    logWidget->clear();
    dotWidgetAffordances->clear();
    dotWidgetActions->clear();
    graphWidgetAffordances->clear();
    graphWidgetActions->clear();
}

void MainWindow::wait(int timeout)
{
    QTime dieTime = QTime::currentTime().addSecs(timeout);
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        if (!busy)
            break;
    }
}

void MainWindow::goToFiles()
{
    QString tempPath = QDir::tempPath();
    QDesktopServices::openUrl(QUrl("file:///" + tempPath));
}

void MainWindow::stop()
{
    stopFlag = true;
}

void MainWindow::model()
{
    QString url = this->locationEdit->text();

    clearLogs();
    data.clear();
    stopFlag = false;
    data.originalUrl = data.lastLocalUrl = url;

    logWidget->push("=== model ===\n");

    int profile = profileWidget->row();

    QString label = "init";
    recurse(
        &data,
        url,
        label, //affordance label
        label, //action label
        TRIGGER_INIT, //trigger type
        ARROW_TYPE_INIT, //arrow type
        profile //profile
    );

    if (stopFlag)
    {
        logWidget->push("===stopped===");
        stopFlag = false;
    }

    visualizeAffordances(""); //no filter

    if (profile != 0)
    {
        logWidget->push("===default profile model===");
        data.clear();
        stopFlag = false;
        data.originalUrl = data.lastLocalUrl = url;
        recurse(
            &data,
            url,
            label, //affordance label
            label, //action label
            TRIGGER_INIT, //trigger type
            ARROW_TYPE_INIT, //arrow type
            0 //override profile
        );
        if (stopFlag)
        {
            logWidget->push("===stopped===");
            stopFlag = false;
        }
    }
    visualizeActions(""); //no filter

    graphWidgetAffordances->setFocus();
}

void MainWindow::recurse(Data *data, const QString &url, QString &affordanceLabel, QString &actionLabel, int parentStateId, int arrowType, int profile)
{
    //exit condition 1: limit total number of states
    if (data->counter >= STATE_MAX)
    {
        logWidget->push("===state max limit===\n");
        stop();
        return;
    }

    //exit condition 2: bail out if stopFlag set
    if (stopFlag)
    {
        return;
    }

    QString code;
    code = "state()";
    QString state = stringFromJs(code);

    code = "document.title";
    QString stateTitle = "'";
    stateTitle += truncateString(stringFromJs(code));
    stateTitle += "'";

    //keeping track of return url
    //if (!this->linkType(data->originalUrl, url) == LINK_EXTERNAL)
    if (linkType(data->originalUrl, url) != LINK_EXTERNAL)
    {
        data->lastLocalUrl = url;
    }

    int stateId = 0;

    //arrows to known states
    if (data->states.contains(state))
    {
        data->arrows.append(Arrow(parentStateId, data->states[state], arrowType, affordanceLabel));

        Arrow action(parentStateId, data->states[state], ARROW_TYPE_ACTION, actionLabel);
        if (!data->actions.contains(action))
        {
            data->actions.append(action);
        }

        if (this->linkType(data->originalUrl, url) == LINK_EXTERNAL)
        {
            QString log = "===transported to external website ";
            log += url;
            log += "; returning to ";
            log += data->lastLocalUrl;
            log += "===\n";
            this->logWidget->push(log);
            navigate(data->lastLocalUrl);
        }
        return;
    }
    else //all other arrows
    {
        (data->counter)++;
        stateId = data->counter;
        data->states.insert(state, stateId);
        data->stateTitles.insert(stateId, stateTitle);
        data->arrows.append(Arrow(parentStateId, stateId, arrowType, affordanceLabel));

        Arrow action(parentStateId, stateId, ARROW_TYPE_ACTION, actionLabel);
        if (!data->actions.contains(action))
        {
            data->actions.append(action);
        }

        if (this->linkType(data->originalUrl, url) == LINK_EXTERNAL)
        {
            QString log = "===transported to external website ";
            log += url;
            log += "; returning to ";
            log += data->lastLocalUrl;
            log += "===\n";
            this->logWidget->push(log);
            navigate(data->lastLocalUrl);
            return;
        }
    }

    //actions
    code = "nodeCount(";
    code += QString::number(profile);
    code += ")";
    int count = intFromJs(code);

    QString origin;
    origin = url;

    for (int i = 0; i < count; i++)
    {
        if (stopFlag)
        {
            return;
        }

        affordanceLabel.clear();
        actionLabel.clear();

        //determine nodename
        code = "nodeName(";
        code += QString::number(i);
        code += ", ";
        code += QString::number(profile);
        code += ")";
        QString nodeName = stringFromJs(code);

        //determine node text
        code = "nodeText(";
        code += QString::number(i);
        code += ", ";
        code += QString::number(profile);
        code += ")";
        QString nodeText = stringFromJs(code);

        for (int j = TRIGGER_CLICK; j < TRIGGER_INIT; j++)
        {
            code = "triggerAction(";
            code += QString::number(j); //event types
            code += ", ";
            code += QString::number(i); //index
            code += ", ";
            code += QString::number(profile);
            code += ")";

            QString result = stringFromJs(code);
            if (result.isEmpty())
                continue;

            logWidget->push(result);

            if (result.contains("===out of range"))
            {
                stop();
                break;
            }

            wait();

            affordanceLabel = triggerString(j);
            if (!nodeText.isEmpty())
            {
                affordanceLabel += " '";
                affordanceLabel += truncateString(nodeText);
                affordanceLabel += "'";
            }
            else if (!nodeName.isEmpty())
            {
               affordanceLabel += " '";
               affordanceLabel += nodeName;
               affordanceLabel += "'";
            }

            actionLabel = (nodeText.isEmpty()) ? "" : nodeText;

            arrowType = ARROW_TYPE_INIT;
            switch (j)
            {
            case TRIGGER_CLICK:
                arrowType = ARROW_TYPE_EVENT_CLICK;
                break;
            case TRIGGER_MOUSEOVER:
                arrowType = ARROW_TYPE_EVENT_MOUSEOVER;
                break;
            case TRIGGER_SUBMIT:
                arrowType = ARROW_TYPE_EVENT_SUBMIT;
                break;
            }

            recurse(
                data,
                view->url().toString(),
                affordanceLabel,
                actionLabel,
                stateId,
                arrowType,
                profile
            );

            navigate(origin);
        }
    }

    //hyperlinks
    code = "hyperlinkCount(";
    code += QString::number(profile);
    code += ")";
    count = intFromJs(code);

    QString linkText, linkHref;
    for (int i = 0; i < count; i++)
    {
        if (stopFlag)
        {
            return;
        }

        affordanceLabel.clear();
        actionLabel.clear();

        code = "linkText(";
        code += QString::number(i);
        code += ", ";
        code += QString::number(profile);
        code += ")";
        linkText = stringFromJs(code);

        code = "linkHref(";
        code += QString::number(i);
        code += ", ";
        code += QString::number(profile);
        code += ")";
        linkHref = stringFromJs(code);

        if (linkHref.isEmpty())
        {
            continue;
        }

        //use linkText if available
        if (!linkText.isEmpty())
        {
            affordanceLabel += " '";
            affordanceLabel += truncateString(linkText);
            affordanceLabel += "'";
        }
        else //otherwise, truncate appropriate portion of URL
        {
            affordanceLabel = truncateString(linkLabel(url,linkHref));
        }

        actionLabel = (linkText.isEmpty()) ? "action" : linkText;

        code = "followLink(";
        code += QString::number(i);
        code += ", ";
        code += QString::number(profile);
        code += ")";
        logWidget->push(stringFromJs(code));

        wait();

        arrowType = ARROW_TYPE_INIT;
        switch (linkType(url, linkHref))
        {
        case LINK_INTERNAL:
            arrowType = ARROW_TYPE_LINK_FRAGMENT;
            break;
        case LINK_EXTERNAL:
            arrowType = ARROW_TYPE_LINK_EXTERNAL;
            break;
        case LINK_RELATIVE:
        case LINK_SAME_HOST:
            arrowType = ARROW_TYPE_LINK_SAME_HOST;
            break;
        }

        recurse(
            data,
            view->url().toString(),
            affordanceLabel,
            actionLabel,
            stateId,
            arrowType,
            profile
        );

        navigate(origin);
    }
}

void MainWindow::visualizeAffordances(const QString &filter)
{
    //parse filter string
    QStringList stateNos = filter.split(",");
    QSet<int> filteredSet;
    int filteredSetSize = stateNos.count();

    //build set of filtered states
    for (int index = 0; index < filteredSetSize; index++)
    {
        int value = stateNos.at(index).toInt();
        if (!value)
            continue;
        filteredSet.insert(value);
    }

    //create graphviz dot file
    dotWidgetAffordances->clear();

    dotWidgetAffordances->push("digraph d {\n");
    dotWidgetAffordances->push("graph [ bgcolor=\"white\", resolution=\"128\", fontname=\"Helvetica\", fontcolor=\"black\", fontsize=\"10\" ];");
    dotWidgetAffordances->push("node [ fontname=\"Helvetica\", penwidth=\"0.25\", fontcolor=\"gray32\", fontsize=\"8\"];");
    dotWidgetAffordances->push("edge [ color=\"gray32\", arrowsize=\"0.75\", penwidth=\"0.25\", fontname=\"Helvetica\", fontcolor=\"dodgerblue4\", fontsize=\"8\", arrowhead=\"vee\" ];");

    //states
    std::map<int, QString> states;

    int size = data.arrows.count();
    for (int i = 1; i < size; i++) //omit initial state 0
    {
        QString source, target;
        source = makeState(data.arrows.at(i).source, &data);
        target = makeState(data.arrows.at(i).target, &data);
        states.insert(std::make_pair(data.arrows.at(i).source, source));
        states.insert(std::make_pair(data.arrows.at(i).target, target));
    }

    std::map<int, QString>::iterator it;
    for (it = states.begin(); it != states.end(); it++)
    {
        if (!filter.isEmpty() && !filteredSet.contains(it->first))
            continue;

        QString state = QString::number(it->first);
        state += " [label=\"";
        state += it->second;
        state += "\"]\n";
        dotWidgetAffordances->push(state);
    }

    //arrows
    size = data.arrows.count();
    QString arrowLabel;
    int source, target;
    for (int i = 1; i < size; i++) //omit initial state 0
    {
        source = data.arrows.at(i).source;
        target = data.arrows.at(i).target;

        if (!filter.isEmpty() && (!filteredSet.contains(source) || !filteredSet.contains(target)))
            continue;

        QString arrow;
        arrow = QString::number(data.arrows.at(i).source);
        arrow += " -> ";
        arrow += QString::number(data.arrows.at(i).target);
        arrow += " [label=\"";

        arrowLabel = data.arrows.at(i).label;
        arrowLabel = arrowLabel.replace("\n", "");
        arrowLabel = arrowLabel.replace("\r", "");
        arrowLabel = arrowLabel.replace("\t", " ");
        arrow += arrowLabel;

        QString arrowColor, arrowStyle;
        int arrowType = data.arrows.at(i).type;
        switch(arrowType)
        {
        case ARROW_TYPE_EVENT_CLICK:
            arrowColor = "firebrick2";
            arrowStyle = "normal";
            break;
        case ARROW_TYPE_EVENT_MOUSEOVER:
            arrowColor = "gold";
            arrowStyle = "normal";
            break;
        case ARROW_TYPE_EVENT_SUBMIT:
            arrowColor = "chocolate";
            arrowStyle = "normal";
            break;
        case ARROW_TYPE_LINK_EXTERNAL:
            arrowColor = "dodgerblue3";
            arrowStyle = "dotted";
            break;
        case ARROW_TYPE_LINK_SAME_HOST:
            arrowColor = "dodgerblue4";
            arrowStyle = "normal";
            break;
        case ARROW_TYPE_LINK_FRAGMENT:
            arrowColor = "dogerblue4";
            arrowStyle = "normal";
            break;
        default:
            arrowColor = "black";
            arrowStyle = "normal";
            break;
        }
        arrow += "\", color=\"" + arrowColor + "\", style=\"" + arrowStyle;
        arrow += "\"]\n";
        dotWidgetAffordances->push(arrow);
    }

    //end dot structure
    dotWidgetAffordances->push("}\n");

    graphWidgetAffordances->refresh(dotWidgetAffordances->text());
}

void MainWindow::visualizeActions(const QString &filter)
{
    //parse filter string
    QStringList stateNos = filter.split(",");
    QSet<int> filteredSet;
    int filteredSetSize = stateNos.count();

    //build set of filtered states
    for (int index = 0; index < filteredSetSize; index++)
    {
        int value = stateNos.at(index).toInt();
        if (!value)
            continue;
        filteredSet.insert(value);
    }

    //create graphviz dot file
    dotWidgetActions->clear();

    dotWidgetActions->push("digraph d {\n");
    dotWidgetActions->push("graph [ bgcolor=\"white\", resolution=\"128\", fontname=\"Helvetica\", fontcolor=\"black\", fontsize=\"10\" ];");
    dotWidgetActions->push("node [ fontname=\"Helvetica\", penwidth=\"0.25\", fontcolor=\"gray32\", fontsize=\"8\"];");
    dotWidgetActions->push("edge [ color=\"gray32\", arrowsize=\"0.75\", penwidth=\"0.25\", fontname=\"Helvetica\", fontcolor=\"dodgerblue4\", fontsize=\"8\", arrowhead=\"vee\" ];");

    //states
    std::map<int, QString> states;
    std::vector<QString> edges;

    QVectorIterator<Arrow> i(data.actions);
    Arrow a;
    if (i.hasNext())
    {
        a = i.next(); //ignore first
    }

    while (i.hasNext())
    {
        //populate states
        a = i.next();
        QString source, target;
        source = makeState(a.source, &data);
        target = makeState(a.target, &data);
        states.insert(std::make_pair(a.source, source));
        states.insert(std::make_pair(a.target, target));

        //populate edges
        QString edge, edgeLabel;
        edge = QString::number(a.source);
        edge += " -> ";
        edge += QString::number(a.target);
        edge += " [label=\"";

        edgeLabel = a.label;
        edgeLabel = edgeLabel.replace("\n", "");
        edgeLabel = edgeLabel.replace("\r", "");
        edgeLabel = edgeLabel.replace("\t", " ");
        edge += edgeLabel;
        edge += "\"]\n";
        edges.push_back(edge);
    }

    //write out states
    std::map<int, QString>::iterator it;
    for (it = states.begin(); it != states.end(); it++)
    {
        if (!filter.isEmpty() && !filteredSet.contains(it->first))
            continue;

        QString state = QString::number(it->first);
        state += " [label=\"";
        state += it->second;
        state += "\"]\n";
        dotWidgetActions->push(state);
    }

    //write out edges
    std::vector<QString>::iterator itEdge;
    for (itEdge = edges.begin(); itEdge != edges.end(); itEdge++)
    {
        dotWidgetActions->push(*itEdge);
    }

    //end dot structure
    dotWidgetActions->push("}\n");
    graphWidgetActions->refresh(dotWidgetActions->text());
}

QString MainWindow::makeState(int i, Data *data)
{
    QString s = "state ";
    s += QString::number(i);
    s += "\\n";
    s += data->stateTitles[i];
    return s;
}

void MainWindow::runJs(const QString &code)
{
    view->page()->mainFrame()->evaluateJavaScript(code);
}

QString MainWindow::stringFromJs(const QString &code)
{
    QVariant result = view->page()->mainFrame()->evaluateJavaScript(code);
    return result.toString();
}

int MainWindow::intFromJs(const QString &code)
{
    QVariant result = view->page()->mainFrame()->evaluateJavaScript(code);
    return result.toInt();
}

void MainWindow::navigate(const QString &url)
{
    QString code = "window.location = ";
    code += "'";
    code += url;
    code += "'";
    view->page()->mainFrame()->evaluateJavaScript(code);
    wait();
}

QString MainWindow::triggerString(int trigger)
{
    switch (trigger)
    {
    case TRIGGER_CLICK:
        return "click";
        break;
    case TRIGGER_MOUSEOVER:
        return "mouseover";
        break;
    case TRIGGER_SUBMIT:
        return "submit";
        break;
    case TRIGGER_LINK:
        return "link";
        break;
    case TRIGGER_INIT:
        return "init";
        break;
    }
    return "invalid";
}

void MainWindow::openFilterDialog()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Filter"),
        tr("States (e.g. 1, 2, 3, 5)                                                                                     "),
        QLineEdit::Normal,
        "",
        &ok);
    if (!ok || text.isEmpty())
        return;

    graphWidgetAffordances->clear();
    visualizeAffordances(
        text //filter
    );
    graphWidgetActions->clear();
    visualizeActions(
        text //filter
    );
}

void MainWindow::setBusyFlag()
{
    busy = true;
}

QString MainWindow::linkLabel(const QString &current, const QString &href)
{
    int type = linkType(current, href);

    QUrl hrefUrl(href);
    QString hrefPath = hrefUrl.path();
    QString hrefHost = hrefUrl.host();

    switch (type)
    {
    case LINK_INTERNAL:
        return href;
        break;
    case LINK_RELATIVE:
        return href;
        break;
    case LINK_SAME_HOST:
        return hrefPath;
        break;
    case LINK_EXTERNAL:
        return hrefHost;
        break;
    default:
        return "link";
    }
}

int MainWindow::linkType(const QString &current, const QString &href)
{
    //internal link
    if (href.startsWith('#'))
    {
        return LINK_INTERNAL;
    }

    //relative link
    QUrl hrefUrl(href);
    if (hrefUrl.isRelative())
    {
        return LINK_RELATIVE;
    }

    QUrl currentUrl(current);
    QString currentHost, hrefHost, hrefPath;
    currentHost = currentUrl.host();
    hrefHost = hrefUrl.host();
    hrefPath = hrefUrl.path();

    //same-host link
    if (currentHost == hrefHost || hrefHost.isEmpty())
    {
        return LINK_SAME_HOST;
    }

    //external link
    return LINK_EXTERNAL;
}

QString MainWindow::truncateString(const QString &s)
{
    if (s.length() < 24)
        return s;
    return s.left(20) + "...";
}
