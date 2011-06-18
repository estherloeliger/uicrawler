#include <QtGui>
#include <QSet>
#include <QSettings>
#include "data.h"
#include "arrow.h"
#include "browser.h"

class QWebView;
class QLineEdit;
class LogWidget;
class GraphWidget;
class ProfileWidget;
class FilterWidget;
class MappingWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

public slots:
protected slots:
    void adjustLocation();
    void changeLocation();
    void adjustTitle();
    void setProgress(int p);
    void onLoadFinished(bool);
    void viewSource();
    void slotSourceDownloaded();
    void open();
    void save();
    void state();
    void clearLogs(bool b = true);
    void stats();
    void model();
    void goToFiles();
    void setBusyFlag();
    void openBlacklistDialog();
    void stop();
    void apply();

    //bookmark slots
    void bookmarkIbis() { browser->navigate("http://www.ibisreader.com/library"); }
    void bookmarkTwenty() { browser->navigate("http://www.20thingsilearned.com/foreword"); }
    void bookmarkBooks() { browser->navigate("http://books.google.com/ebooks/reader?id=ey8EAAAAQAAJ&printsec=frontcover&output=reader&pg=GBS.PP1.w.1.0.0"); }
    void bookmarkFourmilab() { browser->navigate("http://www.fourmilab.ch/cgi-bin/Solar/"); }
    void bookmarkFree() { browser->navigate("http://www.fsf.org"); }
    void bookmarkAlertbox() { browser->navigate("http://www.useit.com/alertbox/nng-anniversary.html"); }
    void bookmarkTest() { browser->navigate("http://team.sourceforge.net/test.html"); }
    void bookmarkGrep() { browser->navigate("http://www.gnu.org/software/grep/index.html"); }
    void bookmarkVi() { browser->navigate("http://unixhelp.ed.ac.uk/vi/"); }
    void bookmarkUnspace() { browser->navigate("http://unspace.ca/"); }
    void bookmarkBetterInteractive() { browser->navigate("http://www.betterinteractive.com/"); }
    void bookmarkBigCartel() { browser->navigate("http://bigcartel.com/"); }
public slots:
    void visualizeAffordances();
    void visualizeActions();
    void visualizeAbstract();

protected:
    void refreshMapping(
        const QSet<QString> &mapNodes,
        const QSet<QString> &mapEdges,
        LogWidget *widget);
    void refreshPullback();

private:
    Data data; //model

    QString jQuery, sha1, uicrawler; //JS libraries
    QString blacklistString, lastOpenedFile;
    QSet<QString> blacklist;
    void updateBlacklist();
    QLineEdit *locationEdit;
    QSettings *settings;
    QAction *rotateAction;
    Browser *browser;
    LogWidget
        *logWidget,
        *dotWidgetAffordances,
        *dotWidgetActions,
        *dotWidgetAbstract,
        *dotWidgetPullback;
    GraphWidget
        *graphWidgetAffordances,
        *graphWidgetActions,
        *graphWidgetAbstract,
        *graphWidgetPullback;
    FilterWidget *filterWidget;
    ProfileWidget *profileWidget;
    MappingWidget *mappingWidgetAffordance, *mappingWidgetAction;
    int progress;
    QString triggerString(int trigger);

    QString machineState();
    void setMachineState(const QString &s);

    QHash<int, QString> nodeNames, linkNames;

    QString arrowVectorToPullback(QVector<Arrow> &v);
    QString stateVectorToPullback(QVector<State> &v);
    bool stopFlag, busyFlag, skipOutOfScopeUrls;
};
