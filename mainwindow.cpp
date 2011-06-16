#include <QtGui>
#include <QtWebKit>
#include <QRegExp>
#include <map>
#include "mainwindow.h"
#include "logwidget.h"
#include "graphwidget.h"
#include "profilewidget.h"
#include "filterwidget.h"
#include "mappingwidget.h"
#include "state.h"
#include "webpage.h"
#include "modeler.h"
#include "constants.h"
#include "mystring.h"

MainWindow::MainWindow() : stopFlag(false), busyFlag(false)
{
    QCoreApplication::setOrganizationName("laika57");
    QCoreApplication::setOrganizationDomain("https://github.com/laika57/uicrawler");
    QCoreApplication::setApplicationName("uicrawler");

    settings = new QSettings();

    blacklistString = settings->value("blacklistString", "").toString();
    updateBlacklist();

    skipOutOfScopeUrls = settings->value("skipOutOfScopeUrls", true).toBool();

    QString pathDefault;
#ifdef Q_WS_WIN
    pathDefault = QDir::homePath();
#else
    pathDefault = QDir::homePath() + "/Desktop";
#endif

    lastOpenedFile = settings->value("lastOpenedFile", pathDefault).toString();

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

    browser = new Browser(this, &busyFlag);

    //view->load(url);
    connect(browser, SIGNAL(loadFinished(bool)), SLOT(onLoadFinished(bool)));
    connect(browser, SIGNAL(titleChanged(QString)), SLOT(adjustTitle()));
    connect(browser, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
    connect(browser, SIGNAL(loadStarted()), SLOT(setBusyFlag()));

    locationEdit = new QLineEdit(this);
    locationEdit->setSizePolicy(QSizePolicy::Expanding, locationEdit->sizePolicy().verticalPolicy());
    connect(locationEdit, SIGNAL(returnPressed()), SLOT(changeLocation()));

    QToolBar *toolBar = addToolBar(tr("Navigation"));
    toolBar->addAction(browser->pageAction(QWebPage::Back));
    toolBar->addAction(browser->pageAction(QWebPage::Forward));
    toolBar->addAction(browser->pageAction(QWebPage::Reload));
    toolBar->addAction(browser->pageAction(QWebPage::Stop));
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
    bookmarksMenu->addAction("Alertbox", this, SLOT(bookmarkAlertbox()));
    bookmarksMenu->addAction("Better Interactive", this, SLOT(bookmarkBetterInteractive()));
    bookmarksMenu->addAction("Big Cartel", this, SLOT(bookmarkBigCartel()));
    bookmarksMenu->addAction("Fourmilab", this, SLOT(bookmarkFourmilab()));
    bookmarksMenu->addAction("Free Software Foundation", this, SLOT(bookmarkFree()));
    bookmarksMenu->addAction("grep", this, SLOT(bookmarkGrep()));
    bookmarksMenu->addAction("Google Books", this, SLOT(bookmarkBooks()));
    bookmarksMenu->addAction("Ibis Reader", this, SLOT(bookmarkIbis()));
    bookmarksMenu->addAction("Unspace", this, SLOT(bookmarkUnspace()));
    bookmarksMenu->addAction("vi", this, SLOT(bookmarkVi()));

    bookmarksMenu->addSeparator();

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

    dotWidgetPullback = new LogWidget(this, "Pullback");
    dotWidgetPullback->setAllowedAreas(Qt::RightDockWidgetArea);

    mappingWidgetAffordance = new MappingWidget(this, "Aff -> Abs");
    mappingWidgetAffordance->setAllowedAreas(Qt::RightDockWidgetArea);

    mappingWidgetAction = new MappingWidget(this, "Act -> Abs");
    mappingWidgetAction->setAllowedAreas(Qt::RightDockWidgetArea);

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
    addDockWidget(Qt::RightDockWidgetArea, dotWidgetPullback);
    addDockWidget(Qt::RightDockWidgetArea, mappingWidgetAffordance);
    addDockWidget(Qt::RightDockWidgetArea, mappingWidgetAction);
    addDockWidget(Qt::RightDockWidgetArea, profileWidget);

    addDockWidget(Qt::BottomDockWidgetArea, graphWidgetAffordances);
    addDockWidget(Qt::BottomDockWidgetArea, graphWidgetActions);
    addDockWidget(Qt::BottomDockWidgetArea, graphWidgetPullback);

    this->tabifyDockWidget(logWidget, dotWidgetAffordances);
    this->tabifyDockWidget(logWidget, dotWidgetActions);
    this->tabifyDockWidget(logWidget, dotWidgetAbstract);
    this->tabifyDockWidget(logWidget, dotWidgetPullback);
    this->tabifyDockWidget(logWidget, mappingWidgetAffordance);
    this->tabifyDockWidget(logWidget, mappingWidgetAction);
    this->tabifyDockWidget(logWidget, filterWidget);
    this->tabifyDockWidget(logWidget, profileWidget);
    logWidget->raise();

    this->tabifyDockWidget(graphWidgetAffordances, graphWidgetActions);
    this->tabifyDockWidget(graphWidgetAffordances, graphWidgetAbstract);
    this->tabifyDockWidget(graphWidgetAffordances, graphWidgetPullback);
    graphWidgetAffordances->raise();

    setCentralWidget(browser);
    setUnifiedTitleAndToolBarOnMac(true);

    this->setMinimumWidth(1024);
    this->setMinimumHeight(768);
    locationEdit->setFocus();
}

MainWindow::~MainWindow()
{
    QVariant blacklistStringVariant(blacklistString);
    QVariant lastOpenedFileVariant(lastOpenedFile);
    settings->setValue("blacklistString", blacklistStringVariant);
    settings->setValue("lastOpenedFile", lastOpenedFileVariant);
    settings->setValue("skipOutOfScopeUrls", skipOutOfScopeUrls);
    delete settings;
}


void MainWindow::onLoadFinished(bool b)
{
    if (!b)
    {
        qDebug() << "MainWindow::onLoadFinished(false)";

        return; //needed to prevent crash on failed load
    }
    busyFlag = false;
    progress = 100;
    browser->page()->mainFrame()->evaluateJavaScript(jQuery);
    browser->page()->mainFrame()->evaluateJavaScript(uicrawler);
    adjustLocation();
    adjustTitle();
}

void MainWindow::viewSource()
{
    QNetworkAccessManager* accessManager = browser->page()->networkAccessManager();
    QNetworkRequest request(browser->url());
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
    locationEdit->setText(browser->url().toString());
}

void MainWindow::changeLocation()
{
    QUrl url = QUrl(QUrl::fromUserInput(locationEdit->text()));

    browser->load(url);
    browser->setFocus();
}

void MainWindow::adjustTitle()
{
    if (progress <= 0 || progress >= 100)
    {
        if (browser->title().isEmpty())
        {
            setWindowTitle("uicrawler");
        }
        else
        {
            setWindowTitle("uicrawler - " + browser->title());
        }
    }
    else
    {
        setWindowTitle(QString("%1 (%2%)").arg(browser->title()).arg(progress));
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

    logWidget->push(browser->JStoString(code));
    logWidget->raise();
}

void MainWindow::stats()
{
    int profile = profileWidget->row();

    QString code;
    code = "stats(";
    code += QString::number(profile);
    code += ")";

    logWidget->push(browser->JStoString(code));
    logWidget->raise();
}

void MainWindow::clearLogs(bool clearInputDotWidgets)
{
    logWidget->clear();

    if(clearInputDotWidgets)
    {
        dotWidgetAffordances->clear();
        dotWidgetActions->clear();
        dotWidgetAbstract->clear();
        //dotWidgetMappingAction->clear();
        //dotWidgetMappingAffordance->clear();
    }
    filterWidget->clear();
    dotWidgetPullback->clear();
    graphWidgetAffordances->clear();
    graphWidgetActions->clear();
    graphWidgetAbstract->clear();
    graphWidgetPullback->clear();
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
    bool isWebsiteModel = !url.isEmpty();

    clearLogs(isWebsiteModel);
    data.clear();
    stopFlag = false;
    data.originalUrl = data.lastLocalUrl = url;
    data.skipOutOfScopeUrls = skipOutOfScopeUrls;

    logWidget->raise();
    logWidget->push("=== model ===\n");

    QString label = "init";

    if (isWebsiteModel)
    {
        Modeler modeler(browser, logWidget, &blacklist, &stopFlag);
        modeler.run(&data, url);

        filterWidget->refreshAffordances(data.affordanceEdges);

        //refreshMapping(data.mapAffordanceToAbstractNodes, data.mapAffordanceToAbstractEdges, dotWidgetMappingAffordance);
        //refreshMapping(data.mapActionToAbstractNodes, data.mapActionToAbstractEdges, dotWidgetMappingAction);

        visualizeAffordances(""); //no filter

        filterWidget->refreshActions(data.actionEdges);
        visualizeActions(""); //no filter

        mappingWidgetAffordance->refresh(data.mapAffordanceToAbstractEdges);
        mappingWidgetAction->refresh(data.mapActionToAbstractEdges);
    }
    else
    {
        graphWidgetAffordances->refresh(dotWidgetAffordances->text());
        graphWidgetActions->refresh(dotWidgetActions->text());
        graphWidgetAbstract->refresh(dotWidgetAbstract->text());
    }

    refreshPullback();
    graphWidgetAffordances->setFocus();
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
        source = MyString::makeState(sourceId, data.affordanceStateTitles[sourceId]);
        target = MyString::makeState(targetId, data.affordanceStateTitles[targetId]);
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
        Arrow a = data.affordanceEdges.at(i);

        source = a.source;
        target = a.target;

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

        arrow += a.toString();

        dotWidgetAffordances->push(arrow);
    }

    //end dot structure
    dotWidgetAffordances->push("}\n");

    graphWidgetAffordances->refresh(dotWidgetAffordances->text());
}

//tbd: base on affordance graph
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
        source = MyString::makeState(a.source, data.actionStateTitles[a.source]);
        target = MyString::makeState(a.target, data.actionStateTitles[a.target]);
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

void MainWindow::setBusyFlag()
{
    busyFlag = true;
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
    QDir dir(lastOpenedFile);
    qDebug() << "lastopenedfile: " << lastOpenedFile;
    qDebug() << dir.absolutePath();

    QString f = QFileDialog::getOpenFileName(
        this, tr("Open File..."),
        dir.absolutePath(),
        tr("All Files (*.*)"));

    if (f.isEmpty())
        return;

    lastOpenedFile = f;

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
    //mappingAffordance = dotWidgetMappingAffordance->text();
    //mappingAction = dotWidgetMappingAction->text();
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

    locationEdit->setText(url);

    WebPage *webPage = new WebPage(this);
    browser->setPage((QWebPage *) webPage);
    browser->load(url);
    dotWidgetAffordances->setText(affordances);
    dotWidgetActions->setText(actions);
    dotWidgetAbstract->setText(abstract);
    //dotWidgetMappingAffordance->setText(mappingAffordance);
    //dotWidgetMappingAction->setText(mappingAction);
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
    logWidget->push("=== refreshPullback ===\n");
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
    buffer += mappingWidgetAffordance->toString();
    buffer += "\n";
    buffer += mappingWidgetAction->toString();
    buffer += "\n";

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
        qDebug() << buffer;
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
