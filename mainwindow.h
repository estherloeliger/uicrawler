#include <QtGui>

#define STATE_MAX 32

class QWebView;
class QLineEdit;
class LogWidget;
class GraphWidget;
class ProfileWidget;
class FilterWidget;

#include <QSet>
#include <QSettings>
#include "data.h"
#include "arrow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QUrl& url);
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
    void clearLogs();
    void stats();
    void model();
    void draw();
    void goToFiles();
    void setBusyFlag();
    void openBlacklistDialog();
    void stop();

    //bookmark slots
    void bookmarkIbis() { navigate("http://www.ibisreader.com/library/view/Alice's+Adventures+in+Wonderland/43239/cover.xml?first_item=1"); }
    void bookmarkTwenty() { navigate("http://www.20thingsilearned.com/foreword"); }
    void bookmarkBooks() { navigate("http://books.google.com/ebooks/reader?id=ey8EAAAAQAAJ&printsec=frontcover&output=reader&pg=GBS.PP1.w.1.0.0"); }
    void bookmarkFree() { navigate("http://www.fsf.org"); }
    void bookmarkTest() { navigate("http://team.sourceforge.net/test.html"); }

public slots:
    void visualizeAffordances(const QString &filter = "");
    void visualizeActions(const QString &filter = "");

protected:
    void refreshMapping(
        const QSet<QString> &mapNodes,
        const QSet<QString> &mapEdges,
        LogWidget *widget);
    void refreshPullback();
    QString arrowsToMapString(const Arrow &a, const Arrow &b);
    QString stateToMapString(int id1, int id2, const QString &label1, const QString &label2);

private:
    QString jQuery, sha1, uicrawler; //JS libraries
    QString blacklistString;
    QSet<QString> blacklist;
    void updateBlacklist();
    QWebView *view;
    QLineEdit *locationEdit;
    QSettings *settings;
    QAction *rotateAction;
    LogWidget
        *logWidget,
        *dotWidgetAffordances,
        *dotWidgetActions,
        *dotWidgetAbstract,
        *dotWidgetMappingAffordance,
        *dotWidgetMappingAction,
        *dotWidgetPullback;
    GraphWidget
        *graphWidgetAffordances,
        *graphWidgetActions,
        *graphWidgetAbstract,
        *graphWidgetPullback;
    FilterWidget *filterWidget;
    ProfileWidget *profileWidget;
    int progress;
    Data data;
    void wait(bool force = false, int timeout = 1);
    void runJs(const QString &code);
    int intFromJs(const QString &code);
    QString stringFromJs(const QString &code);
    void recurse(
        Data *data,
        const QString &url,
        QString &affordanceLabel,
        QString &actionLabel,
        int parentStateId,
        int arrowType,
        int profile);
    void navigate(const QString &url);
    QString triggerString(int trigger);

    QString machineState();
    void setMachineState(const QString &s);

    QHash<int, QString> nodeNames, linkNames;
    QString makeState(int i, const QString &title);
    int linkType(const QString &original, const QString &href);
    bool locationInScope(const QString &currentLocation, const QString &original);
    QString linkLabel(const QString &current, const QString &href);
    QString truncateString(const QString &s);
    QString flattenString(const QString &s);

    QString arrowVectorToPullback(QVector<Arrow> &v);
    QString stateVectorToPullback(QVector<State> &v);
    bool busy, stopFlag;
};

enum triggers {
    TRIGGER_CLICK = 0,
    TRIGGER_MOUSEDOWN,
    TRIGGER_MOUSEUP,
    TRIGGER_MOUSEOVER,
    TRIGGER_SUBMIT,
    TRIGGER_ARIA,
    TRIGGER_LINK,
    TRIGGER_INIT
};

enum linkTypes {
    LINK_INTERNAL = 0,
    LINK_RELATIVE,
    LINK_SAME_HOST,
    LINK_EXTERNAL
};

enum arrowTypes {
    ARROW_TYPE_EVENT_CLICK = 0,
    ARROW_TYPE_EVENT_MOUSEDOWN,
    ARROW_TYPE_EVENT_MOUSEUP,
    ARROW_TYPE_EVENT_MOUSEOVER,
    ARROW_TYPE_EVENT_SUBMIT,
    ARROW_TYPE_EVENT_ARIA,
    ARROW_TYPE_LINK_EXTERNAL,
    ARROW_TYPE_LINK_SAME_HOST,
    ARROW_TYPE_LINK_FRAGMENT,
    ARROW_TYPE_ACTION,
    ARROW_TYPE_INIT
};
