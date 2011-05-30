#include <QtGui>
#include <QtWebKit>
#include <QRegExp>
#include <map>
#include "mainwindow.h"
#include "logwidget.h"
#include "graphwidget.h"
#include "profilewidget.h"
#include "filterwidget.h"
#include "state.h"
#define MAX_STATE_COUNT 64;

MainWindow::MainWindow(const QUrl& url) : stopFlag(false)
{
    QCoreApplication::setOrganizationName("laika57");
    QCoreApplication::setOrganizationDomain("https://github.com/laika57/uicrawler");
    QCoreApplication::setApplicationName("uicrawler");

    settings = new QSettings();

    blacklistString = settings->value("blacklistString", "default").toString();
    updateBlacklist();

    progress = 0;

    this->setWindowIcon(QIcon(":/uicrawler.ico"));

    QFile file;
    file.setFileName(":/jquery-latest.js");
    file.open(QIODevice::ReadOnly);
    jQuery = file.readAll();
    file.close();

    QFile fileSha1;
    fileSha1.setFileName(":/sha1.js");
    fileSha1.open(QIODevice::ReadOnly);
    sha1 = fileSha1.readAll();
    fileSha1.close();

    QFile fileUic;
    //tbd: use resource when code is more or less ready
    fileUic.setFileName(":/uicrawler.js");
/*
#ifdef Q_WS_WIN
    fileUic.setFileName("C:/Qt/2010.05/tools/uicrawler/uicrawler.js");
#endif
#ifdef Q_WS_X11
    fileUic.setFileName("/home/esther/uicrawler/uicrawler.js");
#endif
*/
    fileUic.open(QIODevice::ReadOnly);
    uicrawler = fileUic.readAll();
    fileUic.close();

    QNetworkProxyFactory::setUseSystemConfiguration(true);

    view = new QWebView(this);

    QWebSettings *webSettings = view->page()->settings();
    webSettings->setAttribute(QWebSettings::AutoLoadImages, false);

    QNetworkDiskCache *diskCache = new QNetworkDiskCache(this);
    diskCache->setCacheDirectory(QDir::tempPath());
    view->page()->networkAccessManager()->setCache(diskCache);

    QNetworkConfigurationManager configManager;
    QNetworkConfiguration config = configManager.defaultConfiguration();
    view->page()->networkAccessManager()->setConfiguration(config);

    view->load(url);
    connect(view, SIGNAL(loadFinished(bool)), SLOT(onLoadFinished(bool)));
    connect(view, SIGNAL(titleChanged(QString)), SLOT(adjustTitle()));
    connect(view, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
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
    toolBar->addAction("Draw", this, SLOT(draw()));
    toolBar->addSeparator();
    toolBar->addAction("Stats", this, SLOT(stats()));
    toolBar->addAction("State", this, SLOT(state()));
    toolBar->addSeparator();
    toolBar->addAction("Stop", this, SLOT(stop()));
    toolBar->addAction("Clear", this, SLOT(clearLogs()));

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction("Open...", this, SLOT(open()));
    fileMenu->addAction("Save As...", this, SLOT(save()));
    fileMenu->addAction("Go to Files", this, SLOT(goToFiles()));
    fileMenu->addAction("Edit Blacklist...", this, SLOT(openBlacklistDialog()));
    fileMenu->addSeparator();
    fileMenu->addAction("Quit", this, SLOT(close()));

    //tbd

    QMenu *bookmarksMenu = menuBar()->addMenu(tr("&Bookmarks"));

    bookmarksMenu->addAction("20 Things", this, SLOT(bookmarkTwenty()));
    bookmarksMenu->addAction("Google Books", this, SLOT(bookmarkBooks()));
    bookmarksMenu->addAction("Ibis Reader", this, SLOT(bookmarkIbis()));
    bookmarksMenu->addAction("Free Software Foundation", this, SLOT(bookmarkFree()));
    bookmarksMenu->addAction("Test", this, SLOT(bookmarkTest()));

    QMenu *toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction("Model", this, SLOT(model()));
    toolsMenu->addAction("Draw", this, SLOT(draw()));
    toolsMenu->addSeparator();
    toolsMenu->addAction("Stats", this, SLOT(stats()));
    toolsMenu->addAction("State", this, SLOT(state()));
    toolsMenu->addSeparator();
    toolsMenu->addAction("Stop", this, SLOT(stop()));
    toolsMenu->addAction("Clear", this, SLOT(clearLogs()));

    filterWidget = new FilterWidget(this, "Filter");
    filterWidget->setAllowedAreas(Qt::RightDockWidgetArea);

    profileWidget = new ProfileWidget(this);
    profileWidget->setAllowedAreas(Qt::RightDockWidgetArea);

    logWidget = new LogWidget(this);
    logWidget->setAllowedAreas(Qt::RightDockWidgetArea);

    dotWidgetAffordances = new LogWidget(this, "Aff");
    dotWidgetAffordances->setAllowedAreas(Qt::RightDockWidgetArea);

    dotWidgetActions = new LogWidget(this, "Act");
    dotWidgetActions->setAllowedAreas(Qt::RightDockWidgetArea);

    dotWidgetAbstract = new LogWidget(this, "Abs");
    dotWidgetAbstract->setAllowedAreas(Qt::RightDockWidgetArea);

    dotWidgetMappingAffordance = new LogWidget(this, "Aff -> Abs");
    dotWidgetMappingAffordance->setAllowedAreas(Qt::RightDockWidgetArea);

    dotWidgetMappingAction = new LogWidget(this, "Act -> Abs");
    dotWidgetMappingAction->setAllowedAreas(Qt::RightDockWidgetArea);

    dotWidgetPullback = new LogWidget(this, "Pullback");
    dotWidgetPullback->setAllowedAreas(Qt::RightDockWidgetArea);

    graphWidgetAffordances = new GraphWidget(this, "Aff graph");
    graphWidgetAffordances->setAllowedAreas(Qt::BottomDockWidgetArea);

    graphWidgetActions = new GraphWidget(this, "Act graph");
    graphWidgetActions->setAllowedAreas(Qt::BottomDockWidgetArea);

    graphWidgetAbstract = new GraphWidget(this, "Abs graph");
    graphWidgetAbstract->setAllowedAreas(Qt::BottomDockWidgetArea);

    graphWidgetPullback = new GraphWidget(this, "Pullback graph");
    graphWidgetPullback->setAllowedAreas(Qt::BottomDockWidgetArea);

    addDockWidget(Qt::RightDockWidgetArea, filterWidget);

    addDockWidget(Qt::RightDockWidgetArea, logWidget);
    addDockWidget(Qt::RightDockWidgetArea, dotWidgetAffordances);
    addDockWidget(Qt::RightDockWidgetArea, dotWidgetActions);    
    addDockWidget(Qt::RightDockWidgetArea, dotWidgetAbstract);
    addDockWidget(Qt::RightDockWidgetArea, dotWidgetMappingAffordance);
    addDockWidget(Qt::RightDockWidgetArea, dotWidgetMappingAction);
    addDockWidget(Qt::RightDockWidgetArea, dotWidgetPullback);
    addDockWidget(Qt::RightDockWidgetArea, profileWidget);

    addDockWidget(Qt::BottomDockWidgetArea, graphWidgetAffordances);
    addDockWidget(Qt::BottomDockWidgetArea, graphWidgetActions);
    addDockWidget(Qt::BottomDockWidgetArea, graphWidgetPullback);

    this->tabifyDockWidget(logWidget, dotWidgetAffordances);
    this->tabifyDockWidget(logWidget, dotWidgetActions);
    this->tabifyDockWidget(logWidget, dotWidgetAbstract);
    this->tabifyDockWidget(logWidget, dotWidgetMappingAffordance);
    this->tabifyDockWidget(logWidget, dotWidgetMappingAction);
    this->tabifyDockWidget(logWidget, dotWidgetPullback);
    this->tabifyDockWidget(logWidget, filterWidget);
    this->tabifyDockWidget(logWidget, profileWidget);
    logWidget->raise();

    this->tabifyDockWidget(graphWidgetAffordances, graphWidgetActions);
    this->tabifyDockWidget(graphWidgetAffordances, graphWidgetAbstract);
    this->tabifyDockWidget(graphWidgetAffordances, graphWidgetPullback);
    graphWidgetAffordances->raise();

    setCentralWidget(view);
    setUnifiedTitleAndToolBarOnMac(true);

    this->setMinimumWidth(1024);
    this->setMinimumHeight(768);
    locationEdit->setFocus();
}

MainWindow::~MainWindow()
{
    QVariant blacklistStringVariant(blacklistString);
    settings->setValue("blacklistString", blacklistStringVariant);
    delete settings;
}


void MainWindow::onLoadFinished(bool b)
{
    if (!b)
    {
        qDebug() << "MainWindow::onLoadFinished(false)";

        return; //needed to prevent crash on failed load
    }
    busy = false;
    progress = 100;
    view->page()->mainFrame()->evaluateJavaScript(jQuery);
    //view->page()->mainFrame()->evaluateJavaScript(sha1);
    view->page()->mainFrame()->evaluateJavaScript(uicrawler);
    adjustLocation();
    adjustTitle();
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
    {
        if (view->title().isEmpty())
        {
            setWindowTitle("uicrawler");
        }
        else
        {
            setWindowTitle("uicrawler - " + view->title());
        }
    }
    else
    {
        setWindowTitle(QString("%1 (%2%)").arg(view->title()).arg(progress));
    }
}

void MainWindow::setProgress(int p)
{
    progress = p;
    adjustTitle();
}

void MainWindow::state()
{
    int profile = profileWidget->row();

    QString code;
    code = "state(";
    code += QString::number(profile);
    code += ")";

    logWidget->push(stringFromJs(code));
    logWidget->raise();
}

void MainWindow::stats()
{
    int profile = profileWidget->row();

    QString code;
    code = "stats(";
    code += QString::number(profile);
    code += ")";

    logWidget->push(stringFromJs(code));
    logWidget->raise();
}

void MainWindow::clearLogs()
{
    logWidget->clear();
    dotWidgetAffordances->clear();
    dotWidgetActions->clear();
    graphWidgetAffordances->clear();
    graphWidgetActions->clear();
}

void MainWindow::wait(bool force, int timeout)
{
    QTime dieTime = QTime::currentTime().addSecs(timeout);
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        if (!busy && !force)
            break;
    }

    //fall-back: timeout if busy == true still
    if (busy)
    {
        qDebug() << "Extended waiting period...";
        dieTime = QTime::currentTime().addSecs(10);
        while ( QTime::currentTime() < dieTime )
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
            if (!busy)
                break;
        }
    }

    busy = false; //override in case onFinished has not signalled success
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

    logWidget->raise();
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

    filterWidget->refreshAffordances(data.affordanceEdges);

    refreshMapping(data.mapAffordanceToAbstractNodes, data.mapAffordanceToAbstractEdges, dotWidgetMappingAffordance);
    refreshMapping(data.mapActionToAbstractNodes, data.mapActionToAbstractEdges, dotWidgetMappingAction);

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
    filterWidget->refreshActions(data.actionEdges);
    visualizeActions(""); //no filter

    refreshPullback();

    graphWidgetAffordances->setFocus();
}

QString MainWindow::arrowsToMapString(const Arrow &a, const Arrow &b)
{
    QString s;
    s += "(\"";
    s += QString::number(a.source);
    s += "->";
    s += QString::number(a.target);
    s += "[label=\\\"";
    s += a.label;
    s += "\\\"]\",\"";
    s += QString::number(b.source);
    s += "->";
    s += QString::number(b.target);
    s += "[label=\\\"";
    s += b.label;
    s += "\\\"]\")";

    return s;
}

QString MainWindow::stateToMapString(int id1, int id2, const QString &label1, const QString &label2)
{
    QString s;
    s += "(\"";
    s += QString::number(id1);
    s += "[label=\\\"";
    s += label1;
    s += "\\\"]\",\"";
    s += QString::number(id2);
    s += "[label=\\\"";
    s += label2;
    s += "\\\"]\")";

    return s;
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
    QString stateTitle = truncateString(stringFromJs(code));//truncateString(flattenString(stringFromJs(code)));

    data->affordanceStates.append(State(data->affordanceCounter++, stateTitle, state));
    data->actionStates.append(State(data->actionCounter++, stateTitle, state));
    data->abstractStates.append(State(data->abstractCounter++, stateTitle, state));

    //keeping track of lastLocalUrl
    if (linkType(data->originalUrl, url) != LINK_EXTERNAL &&
        locationInScope(locationEdit->text(), data->originalUrl))
    {
        data->lastLocalUrl = url;
    }

    int stateId = 0;

    //arrows to known states
    if (data->states.contains(state))
    {
        (data->affordanceCounter)++;
        Arrow affordanceArrow(data->affordanceCounter, parentStateId, data->states[state], arrowType, true, affordanceLabel);
        data->affordanceEdges.append(affordanceArrow);
        Arrow actionArrow((data->actionCounter) + 1, parentStateId, data->states[state], ARROW_TYPE_ACTION, true, actionLabel);

        if(affordanceLabel != "init")
        {
            data->mapAffordanceToAbstractEdges.insert(arrowsToMapString(affordanceArrow, actionArrow));//mapping
        }

        if (!data->actionEdges.contains(actionArrow))
        {
            (data->actionCounter)++; //delayed increment
            data->actionEdges.append(actionArrow);
            data->abstractEdges.append(actionArrow); //interim: abstract == action graph
            if(actionLabel != "init")
            {
                data->mapActionToAbstractEdges.insert(arrowsToMapString(actionArrow, actionArrow));//mapping
            }
        }


        if (
            linkType(data->originalUrl, url) == LINK_EXTERNAL ||
            !locationInScope(locationEdit->text(), data->originalUrl))
        {
            QString log = "===transported to out-of-scope address ";
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
        (data->affordanceCounter)++;
        stateId = data->counter;
        data->states.insert(state, stateId);

        data->affordanceStateTitles.insert(stateId, stateTitle);
        data->actionStateTitles.insert(stateId, stateTitle);
        data->abstractStateTitles.insert(stateId, stateTitle);

        Arrow affordanceArrow(data->affordanceCounter, parentStateId, stateId, arrowType, true, affordanceLabel);
        data->affordanceEdges.append(affordanceArrow);
        Arrow actionArrow((data->actionCounter) + 1, parentStateId, stateId, ARROW_TYPE_ACTION, true, actionLabel);

        //mapping
        if(affordanceLabel != "init")
        {
            data->mapAffordanceToAbstractEdges.insert(arrowsToMapString(affordanceArrow, actionArrow));
        }
        QString affordanceStateTitle, actionStateTitle;
        affordanceStateTitle = (data->affordanceStateTitles.count(stateId)) ?
                        makeState(stateId, data->affordanceStateTitles[stateId]) :
                            "Affordance state " + QString::number(stateId);
        actionStateTitle = (data->actionStateTitles.count(stateId)) ?
                        makeState(stateId, data->actionStateTitles[stateId]) :
                        "Action state " + QString::number(stateId);
        data->mapAffordanceToAbstractNodes.insert(stateToMapString(stateId, stateId, affordanceStateTitle, actionStateTitle));
        data->mapActionToAbstractNodes.insert(stateToMapString(stateId, stateId, actionStateTitle, actionStateTitle));
        //end mapping

        if (!data->actionEdges.contains(actionArrow))
        {
            (data->actionCounter)++; //delayed increment
            data->actionEdges.append(actionArrow);
            data->abstractEdges.append(actionArrow);
            data->actionStateTitles.insert(data->actionCounter, stateTitle);
            if(actionLabel != "init")
            {
                data->mapActionToAbstractEdges.insert(arrowsToMapString(actionArrow, actionArrow));//mapping
            }
        }

        if (this->linkType(data->originalUrl, url) == LINK_EXTERNAL ||
            !locationInScope(locationEdit->text(), data->originalUrl))
        {
            QString log = "===transported to out-of-scope address ";
            log += url;
            log += "; returning to ";
            log += data->lastLocalUrl;
            log += "===\n";
            this->logWidget->push(log);
            navigate(data->lastLocalUrl);
            return;
        }
    }

    wait();

    //actions
    code = "nodeCount(";
    code += QString::number(profile);
    code += ")";
    int count = intFromJs(code);

    QString origin;
    origin = data->lastLocalUrl;//url; //exp

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

        if (
            (nodeName.isEmpty() &&
            nodeText.isEmpty()) ||
            (nodeName == "\"\"" &&
            nodeText == "\"\""))
        {
            qDebug() << "Bailing out - node with empty name and text";
            wait(true, 2);
        }

        qDebug() << "Processing #" << i << " - node " << nodeName << " - label " << nodeText << "\n";

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

            wait(true);
            //wait(); //check if faster but correct?

            affordanceLabel = triggerString(j);
            if (!nodeText.isEmpty())
            {
                affordanceLabel += " '";
                affordanceLabel += truncateString(flattenString(nodeText));
                affordanceLabel += "'";
            }
            else if (!nodeName.isEmpty())
            {
               affordanceLabel += " '";
               affordanceLabel += nodeName;
               affordanceLabel += "'";
            }

            actionLabel = (nodeText.isEmpty()) ? "" : truncateString(flattenString(nodeText));

            arrowType = ARROW_TYPE_INIT;
            switch (j)
            {
            case TRIGGER_CLICK:
                arrowType = ARROW_TYPE_EVENT_CLICK;
                break;
            case TRIGGER_MOUSEDOWN:
                arrowType = ARROW_TYPE_EVENT_MOUSEDOWN;
                break;
            case TRIGGER_MOUSEUP:
                arrowType = ARROW_TYPE_EVENT_MOUSEUP;
                break;
            case TRIGGER_MOUSEOVER:
                arrowType = ARROW_TYPE_EVENT_MOUSEOVER;
                break;
            case TRIGGER_SUBMIT:
                arrowType = ARROW_TYPE_EVENT_SUBMIT;
                break;
            case TRIGGER_ARIA:
                arrowType = ARROW_TYPE_EVENT_ARIA;
                break;
            }

            recurse(
                data,
                this->locationEdit->text(),
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

    QString linkText, linkHref, linkHrefAbsolute;
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

        code = "linkHrefAbsolute(";
        code += QString::number(i);
        code += ", ";
        code += QString::number(profile);
        code += ")";
        linkHrefAbsolute = stringFromJs(code);

        qDebug() << "Link #" << i << " of " << count << " - absolute: " << linkHrefAbsolute << " - relative: " << linkHref << " - text: " << linkText;

        //skip conditions
        if (
                linkHref.isEmpty() ||
                linkHref == "\"\"" ||
                blacklist.contains(linkHrefAbsolute) ||
                linkHref.contains("mailto:") ||
                linkHrefAbsolute.contains("https:")
           )
        {
            continue;
        }

        //use linkText if available
        if (!linkText.isEmpty())
        {
            affordanceLabel += " '";
            affordanceLabel += truncateString(flattenString(linkText));
            affordanceLabel += "'";
        }
        else //otherwise, truncate appropriate portion of URL
        {
            affordanceLabel = truncateString(flattenString(linkLabel(url,linkHref)));
        }

        actionLabel = (linkText.isEmpty()) ? "action" : linkText;

        view->load(linkHrefAbsolute);
        logWidget->push("===set location to " + linkHrefAbsolute + "===\n");
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
            this->locationEdit->text(),
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

    int size = data.affordanceEdges.count();
    for (int i = 1; i < size; i++) //omit initial state 0
    {
        int sourceId, targetId;
        sourceId = data.affordanceEdges.at(i).source;
        targetId = data.affordanceEdges.at(i).target;
        QString source, target;
        source = makeState(sourceId, data.affordanceStateTitles[sourceId]);
        target = makeState(targetId, data.affordanceStateTitles[targetId]);
        states.insert(std::make_pair(sourceId, source));
        states.insert(std::make_pair(targetId, target));
    }

    std::map<int, QString>::iterator it;
    for (it = states.begin(); it != states.end(); it++)
    {
        bool active = true;
        if (!filter.isEmpty() && !filteredSet.contains(it->first))
        {
            active = false;
        }

        QString state;

        if (!active)
        {
            state += "//";
        }
        state += QString::number(it->first);
        state += " [label=\"";
        state += it->second;
        state += "\"]\n";
        dotWidgetAffordances->push(state);
    }

    //arrows
    size = data.affordanceEdges.count();
    QString arrowLabel;
    int source, target;

    for (int i = 1; i < size; i++) //omit initial state 0
    {
        source = data.affordanceEdges.at(i).source;
        target = data.affordanceEdges.at(i).target;

        bool active = true;
        if (!filter.isEmpty() &&
            ((!filteredSet.contains(source) || !filteredSet.contains(target))))
        {
            active = false;
        }

        QString arrow;

        if (!active)
        {
            arrow += "//";
        }
        arrow = QString::number(data.affordanceEdges.at(i).source);
        arrow += " -> ";
        arrow += QString::number(data.affordanceEdges.at(i).target);
        arrow += " [label=\"";

        arrowLabel = data.affordanceEdges.at(i).label;

        arrowLabel = this->flattenString(arrowLabel);
        arrow += arrowLabel;

        QString arrowColor, arrowStyle;
        int arrowType = data.affordanceEdges.at(i).type;
        switch(arrowType)
        {
        case ARROW_TYPE_EVENT_CLICK:
            arrowColor = "firebrick2";
            arrowStyle = "solid";
            break;
        case ARROW_TYPE_EVENT_MOUSEDOWN:
            arrowColor = "firebrick2";
            arrowStyle = "solid";
            break;
        case ARROW_TYPE_EVENT_MOUSEUP:
            arrowColor = "firebrick2";
            arrowStyle = "solid";
            break;
        case ARROW_TYPE_EVENT_MOUSEOVER:
            arrowColor = "gold";
            arrowStyle = "solid";
            break;
        case ARROW_TYPE_EVENT_SUBMIT:
            arrowColor = "chocolate";
            arrowStyle = "solid";
            break;
        case ARROW_TYPE_EVENT_ARIA:
            arrowColor = "firebrick2";
            arrowStyle = "solid";
            break;
        case ARROW_TYPE_LINK_EXTERNAL:
            arrowColor = "dodgerblue3";
            arrowStyle = "dashed";
            break;
        case ARROW_TYPE_LINK_SAME_HOST:
            arrowColor = "dodgerblue4";
            arrowStyle = "solid";
            break;
        case ARROW_TYPE_LINK_FRAGMENT:
            arrowColor = "dodgerblue4";
            arrowStyle = "solid";
            break;
        default:
            arrowColor = "black";
            arrowStyle = "dotted";
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

    QVectorIterator<Arrow> i(data.actionEdges);
    Arrow a;
    if (i.hasNext())
    {
        a = i.next(); //ignore first
    }

    while (i.hasNext())
    {
        //populate states
        a = i.next();

        bool active = true;
        if (!filter.isEmpty() && (!filteredSet.contains(a.source) || !filteredSet.contains(a.target)))
        {
            active = false;
        }

        QString source, target;
        source = makeState(a.source, data.actionStateTitles[a.source]);
        target = makeState(a.target, data.actionStateTitles[a.target]);
        states.insert(std::make_pair(a.source, source));
        states.insert(std::make_pair(a.target, target));

        //populate edges
        QString edge, edgeLabel;

        if (!active)
        {
            edge += "//";
        }
        edge += QString::number(a.source);
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
        bool active = true;
        if (!filter.isEmpty() && !filteredSet.contains(it->first))
        {
            active = false;
        }

        QString state;

        if (!active)
        {
            state += "//";
        }
        state += QString::number(it->first);
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

    dotWidgetAbstract->push(dotWidgetActions->text());

    graphWidgetActions->refresh(dotWidgetActions->text());
    graphWidgetAbstract->refresh(dotWidgetAbstract->text());

    //placeholder: abstract same as actions same as
}

void MainWindow::refreshMapping(
    const QSet<QString> &mapNodes,
    const QSet<QString> &mapEdges,
    LogWidget *widget)
{
    QSetIterator<QString> nodeIt(mapNodes);
    QSetIterator<QString> edgeIt(mapEdges);

    //nodes
    QString buffer = "[";
    bool first = true;
    while (nodeIt.hasNext())
    {
        if (first)
        {
            first = false;
        }
        else
        {
            buffer += ",";
        }
        buffer += nodeIt.next();
    }
    buffer += "]";
    widget->push(buffer);

    //edges
    buffer = "[";
    first = true;
    while (edgeIt.hasNext())
    {
        if (first)
        {
            first = false;
        }
        else
        {
            buffer += ",";
        }
        buffer += edgeIt.next();
    }
    buffer += "]";

    widget->push(buffer);
    //done - no graph needed
}

QString MainWindow::makeState(int i, const QString &title)
{
    QString s = "state ";
    s += QString::number(i);
    s += "\\n";
    s += title;
    return s;
}

void MainWindow::runJs(const QString &code)
{
    view->page()->mainFrame()->evaluateJavaScript(code);
}

QString MainWindow::stringFromJs(const QString &code)
{
    QVariant result = view->page()->mainFrame()->evaluateJavaScript(code);
    wait();
    return result.toString();
}

int MainWindow::intFromJs(const QString &code)
{
    QVariant result = view->page()->mainFrame()->evaluateJavaScript(code);
    wait();
    return result.toInt();
}

void MainWindow::navigate(const QString &url)
{
    //don't open media files
    if (
        blacklist.contains(url) ||
        url.endsWith(".mp3") ||
        url.endsWith(".mp4") ||
        url.endsWith(".flv") ||
        url.endsWith(".pdf") ||
        url.contains("mailto:") ||
        url.contains("https:"))
    {
        return;
    }

    QString code = "window.location = ";
    code += "'";
    code += url;
    code += "'";
    view->page()->mainFrame()->evaluateJavaScript(code);
    wait(false);
}

QString MainWindow::triggerString(int trigger)
{
    switch (trigger)
    {
    case TRIGGER_CLICK:
        return "click";
        break;
    case TRIGGER_MOUSEDOWN:
        return "mousedown";
        break;
    case TRIGGER_MOUSEUP:
        return "mouseup";
        break;
    case TRIGGER_MOUSEOVER:
        return "mouseover";
        break;
    case TRIGGER_SUBMIT:
        return "submit";
        break;
    case TRIGGER_ARIA:
        return "aria";
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

int MainWindow::linkType(const QString &original, const QString &href)
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

    QUrl currentUrl(original);
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

bool MainWindow::locationInScope(const QString &currentLocation, const QString &original)
{
    int lastSlash = original.lastIndexOf("/");
    QString originalPathSegment = original.left(lastSlash);

    if (blacklist.contains(currentLocation))
    {
        qDebug() << currentLocation << " on blacklist";
        return false;
    }

    return currentLocation.startsWith(originalPathSegment);
}

QString MainWindow::truncateString(const QString &s)
{
    if (s.length() < 24)
        return s;
    return s.left(20) + "...";
}

QString MainWindow::flattenString(const QString &s)
{
    QString t = s;
    t = t.replace("\n", " ");
    t = t.replace("\r", " ");
    t = t.replace("\t", " ");
    t = t.replace(" {2,}", " ");
    return t;
}

void MainWindow::openBlacklistDialog()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Blacklist"),
        tr("URLs (e.g. http://www.google.com;http://www.yahoo.com)                                                                                     "),
        QLineEdit::Normal,
        blacklistString,
        &ok);
    if (!ok || text.isEmpty())
        return;

    blacklistString = text;
    updateBlacklist();
}

void MainWindow::updateBlacklist()
{
    blacklist.clear();

    QStringList list = blacklistString.split(";", QString::SkipEmptyParts);
    int count = list.count();
    QString url;
    for (int i = 0; i < count; i++)
    {
        url = list.at(i);
        blacklist.insert(url);
    }

    QVariant variant(blacklistString);
    settings->setValue("blacklistString", variant);
    settings->sync();
}

void MainWindow::draw()
{
    graphWidgetAffordances->refresh(dotWidgetAffordances->text());
    graphWidgetActions->refresh(dotWidgetActions->text());
    graphWidgetAbstract->refresh(dotWidgetAbstract->text());
    refreshPullback();
}

void MainWindow::open()
{
    QString f = QFileDialog::getOpenFileName(
        this, tr("Open File..."),
        QString(),
        tr("All Files (*.*)"));

    if (f.isEmpty())
        return;

    QFile file;
    file.setFileName(f);
    file.open(QIODevice::ReadOnly);

    QByteArray array = file.readAll();

    QString buffer = QString::fromUtf8(array.constData(), array.count());

    file.close();

    setMachineState(buffer);
}

void MainWindow::save()
{
    QString f = QFileDialog::getSaveFileName(this, tr("Save as..."),
        QString(), tr("All Files (*.*)"));
    if (f.isEmpty())
        return;

    QFile file(f);

    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, "Can't write to file", file.errorString());
        return;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << machineState();
    out.flush();
    file.close();
}

QString MainWindow::machineState()
{
    QString s, url, affordances, actions, abstract, mappingAffordance, mappingAction, pullback;

    url = locationEdit->text();
    affordances = dotWidgetAffordances->text();
    actions = dotWidgetActions->text();
    abstract = dotWidgetAbstract->text();
    mappingAffordance = dotWidgetMappingAffordance->text();
    mappingAction = dotWidgetMappingAction->text();
    pullback = dotWidgetPullback->text();
    affordances.replace("\n", "__RETURN__");
    actions.replace("\n", "__RETURN__");
    abstract.replace("\n", "__RETURN__");
    mappingAffordance.replace("\n", "__RETURN__");
    mappingAction.replace("\n", "__RETURN__");
    pullback.replace("\n", "__RETURN__");

    s += url;
    s += "\n";
    s += affordances;
    s += "\n";
    s += actions;
    s += "\n";
    s += abstract;
    s += "\n";
    s += mappingAffordance;
    s += "\n";
    s += mappingAction;
    s += "\n";
    s += pullback;
    s += "\n";

    return s;
}

void MainWindow::setMachineState(const QString &s)
{
    QStringList list = s.split("\n");
    if (list.count() < 4)
    {
        qDebug() << "Invalid machine state:\n" << s;
        return;
    }

    QString url, affordances, actions, abstract, mappingAffordance, mappingAction, pullback;

    url = list.at(0);
    affordances = list.at(1);
    actions = list.at(2);
    abstract = list.at(3);
    mappingAffordance = list.at(4);
    mappingAction = list.at(5);
    pullback = list.at(6);

    affordances = affordances.replace("__RETURN__", "\n");
    actions = actions.replace("__RETURN__", "\n");
    abstract = abstract.replace("__RETURN__", "\n");
    mappingAffordance = mappingAffordance.replace("__RETURN__", "\n");
    mappingAction = mappingAction.replace("__RETURN__", "\n");
    pullback = pullback.replace("__RETURN__", "\n");

    view->load(url);
    dotWidgetAffordances->setText(affordances);
    dotWidgetActions->setText(actions);
    dotWidgetAbstract->setText(abstract);
    dotWidgetMappingAffordance->setText(mappingAffordance);
    dotWidgetMappingAction->setText(mappingAction);
    dotWidgetPullback->setText(pullback);

    //update graphical views
    draw();

    //update filter widget
    this->filterWidget->refreshAffordances(affordances);
    this->filterWidget->refreshActions(actions);

    //update data structure
    QVector<Arrow> affordanceArrows, actionArrows;
    filterWidget->dotToArrows(affordances, affordanceArrows);
    filterWidget->dotToArrows(actions, actionArrows);

    data.affordanceEdges = affordanceArrows;
    data.actionEdges = actionArrows;
}

QString MainWindow::arrowVectorToPullback(QVector<Arrow> &v)
{
    int count = v.count();

    QStringList edges;
    for (int i = 0; i < count; i++)
    {
        QString edge;
        Arrow a = v.at(i);
        edge += QString::number(a.source);
        edge += "->";
        edge += QString::number(a.target);
        edge += "[label=\\\"";
        edge += a.label;
        edge += "\\\"]";

        edges.append(edge);

        //tbd: populate states
    }

    QString s;
    s = "[";

    int edgeCount = edges.count();
    for (int i = 0; i < edgeCount; i++)
    {
        if (i)
        {
            s += ",";
        }
        s += "\"";
        s += edges.at(i);
        s += "\"";
    }

    s += "]\n";

    return s;
}


QString MainWindow::stateVectorToPullback(QVector<State> &v)
{
    int count = v.count();

    QStringList nodes;
    for (int i = 0; i < count; i++)
    {
        QString node;
        State s = v.at(i);
        node += QString::number(s.id);
        node += "[label=\\\"";
        node += s.title;
        node += "\\\"]";

        nodes.append(node);

        //tbd: populate states
    }

    QString s;
    s = "[";

    int nodeCount = nodes.count();
    for (int i = 0; i < nodeCount; i++)
    {
        if (i)
        {
            s += ",";
        }
        s += "\"";
        s += nodes.at(i);
        s += "\"";
    }

    s += "]\n";

    return s;
}


void MainWindow::refreshPullback()
{
    QVector<State> affordanceStates, actionStates, abstractStates;
    QVector<Arrow> affordanceArrows, actionArrows, abstractArrows;

    filterWidget->dotToStates(this->dotWidgetAffordances->text(), affordanceStates);
    filterWidget->dotToArrows(this->dotWidgetAffordances->text(), affordanceArrows);
    filterWidget->dotToStates(this->dotWidgetActions->text(), actionStates);
    filterWidget->dotToArrows(this->dotWidgetActions->text(), actionArrows);
    filterWidget->dotToStates(this->dotWidgetAbstract->text(), abstractStates);
    filterWidget->dotToArrows(this->dotWidgetAbstract->text(), abstractArrows);

    QString buffer;

    buffer += stateVectorToPullback(affordanceStates);
    buffer += arrowVectorToPullback(affordanceArrows);
    buffer += stateVectorToPullback(actionStates);
    buffer += arrowVectorToPullback(actionArrows);
    buffer += stateVectorToPullback(abstractStates);
    buffer += arrowVectorToPullback(abstractArrows);
    buffer += this->dotWidgetMappingAffordance->text();
    buffer += "\n";
    buffer += this->dotWidgetMappingAction->text();


    //test
    qDebug() << buffer;
    //end test

    //fetch dot file from Haskell routine
    QString tempPath = QDir::tempPath();

    QString serializePath, dotPath;

    for (int i = 0; ; i++)
    {
        serializePath = tempPath;
        serializePath += "/uic";
        serializePath += QString::number(i);
        serializePath += + ".txt";
        if (!QFile::exists(serializePath))
            break;
    }

    for (int j = 0; ; j++)
    {
        dotPath = tempPath;
        dotPath += "/uic";
        dotPath += QString::number(j);
        dotPath += ".dot";
        if (!QFile::exists(dotPath))
            break;
    }

    QFile serializeFile(serializePath);
    serializeFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&serializeFile);
    out << buffer;
    serializeFile.close();

#ifdef Q_OS_LINUX
    QProcess::execute("cd /tmp");
#endif

    QString haskellCmd;
/*
#ifdef Q_OS_LINUX
            haskellCmd += "/home/esther/uicrawler-build-desktop";
#else
            haskellCmd += QCoreApplication::applicationDirPath();
#endif
*/
    haskellCmd += QCoreApplication::applicationDirPath();
    haskellCmd += "/bin/pullbacks_first ";
    haskellCmd += serializePath;
    haskellCmd += " ";
    haskellCmd += dotPath;

    qDebug() << haskellCmd;
    int ret = QProcess::execute(haskellCmd);

    if (ret)
    {
        qDebug() << "Haskell command returned error value";
        return;
    }

    //read dot file
    QString dot;

    QFile file;
    file.setFileName(dotPath);
    file.open(QIODevice::ReadOnly);
    dot = file.readAll();
    file.close();

    //refresh controls
    dotWidgetPullback->setText(dot);
    graphWidgetPullback->refresh(dot);
}
