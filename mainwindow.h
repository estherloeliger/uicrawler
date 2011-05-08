#include <QtGui>

#define STATE_MAX 32

class QWebView;
class QLineEdit;
class LogWidget;
class GraphWidget;
class ProfileWidget;

#include "data.h"
#include "arrow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QUrl& url);
    ~MainWindow() { };

protected slots:

    void adjustLocation();
    void changeLocation();
    void adjustTitle();
    void setProgress(int p);
    void finishLoading(bool);

    void viewSource();
    void slotSourceDownloaded();

    void state();
    void clearLogs();
    void stats();
    void model();
    void goToFiles();
    void setBusyFlag();
    void openFilterDialog();
    void stop();
    void visualizeAffordances(const QString &filter = "");
    void visualizeActions(const QString &filter = "");

private:
    QString jQuery, uicrawler; //JS libraries
    QWebView *view;
    QLineEdit *locationEdit;
    QAction *rotateAction;
    LogWidget *logWidget, *dotWidgetAffordances, *dotWidgetActions;
    GraphWidget *graphWidgetAffordances, *graphWidgetActions;
    ProfileWidget *profileWidget;
    int progress;
    Data data;
    void wait(int timeout = 3);
    void runJs(const QString &code);
    int intFromJs(const QString &code);
    QString stringFromJs(const QString &code);
    void navigate(const QString &url);
    void recurse(
        Data *data,
        const QString &url,
        QString &affordanceLabel,
        QString &actionLabel,
        int parentStateId,
        int arrowType,
        int profile);
    QString triggerString(int trigger);
    QHash<int, QString> nodeNames, linkNames;
    QString makeState(int i, Data *data);
    int linkType(const QString &current, const QString &href);
    QString linkLabel(const QString &current, const QString &href);
    QString truncateString(const QString &s);

    bool busy, stopFlag;
};

enum triggers {
    TRIGGER_CLICK = 0,
    TRIGGER_MOUSEOVER,
    TRIGGER_SUBMIT,
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
    ARROW_TYPE_EVENT_MOUSEOVER,
    ARROW_TYPE_EVENT_SUBMIT,
    ARROW_TYPE_LINK_EXTERNAL,
    ARROW_TYPE_LINK_SAME_HOST,
    ARROW_TYPE_LINK_FRAGMENT,
    ARROW_TYPE_ACTION,
    ARROW_TYPE_INIT
};
