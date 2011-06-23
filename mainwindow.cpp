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
    whitelistString = settings->value("whitelistString", "").toString();
    updateBlackWhitelists();

    skipOutOfScopeUrls = settings->value("skipOutOfScopeUrls", true).toBool();

    QString pathDefault;
#ifdef Q_WS_WIN
    pathDefault = QDir::homePath();
#else
    pathDefault = QDir::homePath() + "/Desktop";
#endif

    lastOpenedFile = settings->value("lastOpenedFile", pathDefault).toString();

    progress = 0;

    setWindowIcon(QIcon(":/uicrawler.ico"));

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

    QKeySequence ctrlo("Ctrl+O");
    QKeySequence ctrls("Ctrl+S");
    QKeySequence ctrlf("Ctrl+F");
    QKeySequence ctrlb("Ctrl+B");
    QKeySequence ctrlw("Ctrl+W");
    QKeySequence altf4("Alt+F4");
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction("&Open...", this, SLOT(open()), ctrlo);
    fileMenu->addAction("&Save As...", this, SLOT(save()), ctrls);
    fileMenu->addAction("Go to &Files", this, SLOT(goToFiles()), ctrlf);
    fileMenu->addSeparator();
    fileMenu->addAction("Edit &Blacklist...", this, SLOT(openBlacklistDialog()), ctrlb);
    fileMenu->addAction("Edit &Whitelist...", this, SLOT(openWhitelistDialog()), ctrlw);
    fileMenu->addSeparator();
    fileMenu->addAction("&Quit", this, SLOT(close()), altf4);

    //tbd

    QKeySequence alt1("Alt+1");
    QKeySequence alt2("Alt+2");
    QKeySequence alt3("Alt+3");
    QKeySequence alt4("Alt+4");
    QKeySequence alt5("Alt+5");
    QKeySequence alt6("Alt+6");
    QKeySequence alt7("Alt+7");
    QKeySequence alt8("Alt+8");
    QKeySequence alt9("Alt+9");
    QKeySequence alt0("Alt+0");
    QKeySequence altminus("Alt+-");
    QKeySequence ctrlt("Ctrl+T");

    QMenu *bookmarksMenu = menuBar()->addMenu(tr("&Bookmarks"));
    bookmarksMenu->addAction("20 Things", this, SLOT(bookmarkTwenty()), alt1);
    bookmarksMenu->addAction("Alertbox", this, SLOT(bookmarkAlertbox()), alt2);
    bookmarksMenu->addAction("Better Interactive\tAlt+3", this, SLOT(bookmarkBetterInteractive()), alt3);
    bookmarksMenu->addAction("Big Cartel\tAlt+4", this, SLOT(bookmarkBigCartel()), alt4);
    bookmarksMenu->addAction("Fourmilab", this, SLOT(bookmarkFourmilab()), alt5);
    bookmarksMenu->addAction("Free Software Foundation", this, SLOT(bookmarkFree()), alt6);
    bookmarksMenu->addAction("grep", this, SLOT(bookmarkGrep()), alt7);
    bookmarksMenu->addAction("Google Books", this, SLOT(bookmarkBooks()), alt8);
    bookmarksMenu->addAction("Ibis Reader", this, SLOT(bookmarkIbis()), alt9);
    bookmarksMenu->addAction("Unspace", this, SLOT(bookmarkUnspace()), alt0);
    bookmarksMenu->addAction("vi", this, SLOT(bookmarkVi()), altminus);

    bookmarksMenu->addSeparator();

    bookmarksMenu->addAction("Test", this, SLOT(bookmarkTest()), ctrlt);

    QKeySequence f1("F1");
    QKeySequence f2("F2");
    QKeySequence f3("F3");
    QKeySequence f4("F4");
    QKeySequence f5("F5");
    QKeySequence f6("F6");
    QMenu *toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction("&Model", this, SLOT(model()), f1);
    toolsMenu->addAction("&Apply Changes", this, SLOT(apply()), f2);
    toolsMenu->addSeparator();
    toolsMenu->addAction("&Stats", this, SLOT(stats()), f3);
    toolsMenu->addAction("S&tate", this, SLOT(state()), f4);
    toolsMenu->addSeparator();
    toolsMenu->addAction("St&op", this, SLOT(stop()), f5);
    toolsMenu->addAction("&Clear", this, SLOT(clearLogs()), f6);

    filterWidget = new FilterWidget(this, "Filter", &data);
    filterWidget->setAllowedAreas(Qt::RightDockWidgetArea);

    profileWidget = new ProfileWidget(this);
    profileWidget->setAllowedAreas(Qt::RightDockWidgetArea);

    logWidget = new LogWidget(this, "Log", &data);
    logWidget->setAllowedAreas(Qt::RightDockWidgetArea);

    dotWidgetAffordances = new LogWidget(this, "Aff", &data);
    dotWidgetAffordances->setAllowedAreas(Qt::RightDockWidgetArea);

    dotWidgetActions = new LogWidget(this, "Act", &data);
    dotWidgetActions->setAllowedAreas(Qt::RightDockWidgetArea);

    dotWidgetAbstract = new LogWidget(this, "Abs", &data);
    dotWidgetAbstract->setAllowedAreas(Qt::RightDockWidgetArea);

    dotWidgetPullback = new LogWidget(this, "Pullback", &data);
    dotWidgetPullback->setAllowedAreas(Qt::RightDockWidgetArea);

    mappingWidgetAffordance = new MappingWidget(this, "Aff -> Abs", &data, MAPPING_TYPE_AFFORDANCE);
    mappingWidgetAffordance->setAllowedAreas(Qt::RightDockWidgetArea);

    mappingWidgetAction = new MappingWidget(this, "Act -> Abs", &data, MAPPING_TYPE_ACTION);
    mappingWidgetAction->setAllowedAreas(Qt::RightDockWidgetArea);

    graphWidgetAffordances = new GraphWidget(this, "Aff graph", &data, GRAPH_TYPE_AFFORDANCE);
    graphWidgetAffordances->setAllowedAreas(Qt::BottomDockWidgetArea);

    graphWidgetActions = new GraphWidget(this, "Act graph", &data, GRAPH_TYPE_ACTION);
    graphWidgetActions->setAllowedAreas(Qt::BottomDockWidgetArea);

    graphWidgetAbstract = new GraphWidget(this, "Abs graph", &data, GRAPH_TYPE_ABSTRACT);
    graphWidgetAbstract->setAllowedAreas(Qt::BottomDockWidgetArea);

    graphWidgetPullback = new GraphWidget(this, "Pullback graph", &data, GRAPH_TYPE_PULLBACK);
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

    tabifyDockWidget(logWidget, dotWidgetAffordances);
    tabifyDockWidget(logWidget, dotWidgetActions);
    tabifyDockWidget(logWidget, dotWidgetAbstract);
    tabifyDockWidget(logWidget, dotWidgetPullback);
    tabifyDockWidget(logWidget, mappingWidgetAffordance);
    tabifyDockWidget(logWidget, mappingWidgetAction);
    tabifyDockWidget(logWidget, filterWidget);
    tabifyDockWidget(logWidget, profileWidget);
    logWidget->raise();

    tabifyDockWidget(graphWidgetAffordances, graphWidgetActions);
    tabifyDockWidget(graphWidgetAffordances, graphWidgetAbstract);
    tabifyDockWidget(graphWidgetAffordances, graphWidgetPullback);
    graphWidgetAffordances->raise();

    setCentralWidget(browser);
    setUnifiedTitleAndToolBarOnMac(true);

    setMinimumWidth(1024);
    setMinimumHeight(768);
    locationEdit->setFocus();
}

MainWindow::~MainWindow()
{
    QVariant blacklistStringVariant(blacklistString);
    QVariant whitelistStringVariant(whitelistString);
    QVariant lastOpenedFileVariant(lastOpenedFile);
    settings->setValue("blacklistString", blacklistStringVariant);
    settings->setValue("whitelistString", whitelistStringVariant);
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
    QString url = locationEdit->text();

    clearLogs(true);
    data.clear();
    stopFlag = false;
    data.originalUrl = data.lastLocalUrl = url;
    data.skipOutOfScopeUrls = skipOutOfScopeUrls;

    logWidget->raise();
    logWidget->push("=== model ===\n");

    Modeler modeler(browser, logWidget, &blacklist, &whitelist, &stopFlag);
    modeler.run(&data, url);

    dotWidgetAffordances->setText(MyString::dataToDotString(&data, DOT_TYPE_AFFORDANCE));
    dotWidgetActions->setText(MyString::dataToDotString(&data, DOT_TYPE_ACTION));
    dotWidgetAbstract->setText(MyString::dataToDotString(&data, DOT_TYPE_ABSTRACT));
    dotWidgetPullback->setText(MyString::dataToPullbackDotString(&data));

    filterWidget->refreshAffordances(data.affordanceEdges);
    logWidget->push("=== visualizing affordances ===\n");
    visualizeAffordances();
    filterWidget->refreshActions(data.actionEdges);
    logWidget->push("=== visualizing actions ===\n");
    visualizeActions();
    logWidget->push("=== visualizing abstract ===\n");
    visualizeAbstract();

    logWidget->push("=== refreshing mappings affordances -> abstract ===\n");
    mappingWidgetAffordance->refresh();

    logWidget->push("=== refreshing mappings actions -> abstract ===\n");
    mappingWidgetAction->refresh();

    logWidget->push("=== refreshing pullback ===\n");
    graphWidgetPullback->refresh();

    graphWidgetAffordances->setFocus();
}

void MainWindow::apply()
{
    //tbd: apply changes for dotWidgets?

    mappingWidgetAffordance->applyChanges();
    mappingWidgetAction->applyChanges();

    graphWidgetAffordances->refresh();
    graphWidgetActions->refresh();
    graphWidgetAbstract->refresh();
    graphWidgetPullback->refresh();
    graphWidgetAffordances->setFocus();
}

void MainWindow::visualizeAffordances()
{
    graphWidgetAffordances->refresh();
}

void MainWindow::visualizeActions()
{
    graphWidgetActions->refresh();
}

void MainWindow::visualizeAbstract()
{
    graphWidgetAbstract->refresh();
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
    updateBlackWhitelists();
}

void MainWindow::openWhitelistDialog()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Whitelist"),
        tr("URLs (e.g. http://www.google.com;http://www.yahoo.com)                                                                                     "),
        QLineEdit::Normal,
        whitelistString,
        &ok);
    if (!ok || text.isEmpty())
        return;

    whitelistString = text;
    updateBlackWhitelists();
}

void MainWindow::updateBlackWhitelists()
{
    blacklist.clear();
    whitelist.clear();

    //blacklist
    QStringList list = blacklistString.split(";", QString::SkipEmptyParts);
    int count = list.count();
    QString field;
    for (int i = 0; i < count; i++)
    {
        field = list.at(i);
        blacklist.insert(field);
    }

    QVariant blacklistVariant(blacklistString);
    settings->setValue("blacklistString", blacklistVariant);

    //whitelist
    list = whitelistString.split(";", QString::SkipEmptyParts);
    count = list.count();
    for (int i = 0; i < count; i++)
    {
        field = list.at(i);
        whitelist.insert(field);
    }

    QVariant whitelistVariant(whitelistString);
    settings->setValue("whitelistString", whitelistVariant);

    //sync all
    settings->sync();
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
    dotWidgetPullback->setText(pullback);

    //update graphical views
    apply();

    //update filter widget
    filterWidget->refreshAffordances(affordances);
    filterWidget->refreshActions(actions);

    //update data structure
    QVector<Arrow> affordanceArrows, actionArrows;
    filterWidget->dotToArrows(affordances, affordanceArrows);
    filterWidget->dotToArrows(actions, actionArrows);

    data.affordanceEdges = affordanceArrows;
    data.actionEdges = actionArrows;
}
