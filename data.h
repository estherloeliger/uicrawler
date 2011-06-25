#ifndef DATA_H
#define DATA_H

#include <set>
#include <QHash>
#include <QString>
#include <QVector>
#include <QSet>
#include <QPair>
#include "arrow.h"
#include "state.h"

class Data {
public:
    Data();
    void clear();
    QString toString();
    void applyString(const QString &s);

    //public for direct access to model
    QHash<QString, int> states;

    QHash<int, QString> affordanceStateTitles;
    QHash<int, QString> actionStateTitles;
    QHash<int, QString> abstractStateTitles;

    QVector<Arrow> affordanceEdges;
    QVector<Arrow> actionEdges;
    QVector<Arrow> abstractEdges;

    QVector<State> affordanceStates;
    QVector<State> actionStates;
    QVector<State> abstractStates;

    QVector<QPair<Arrow, Arrow> > mapAffordanceToAbstractEdges;
    QVector<QPair<Arrow, Arrow> > mapActionToAbstractEdges;
    QVector<QPair<State, State> > mapAffordanceToAbstractNodes;
    QVector<QPair<State, State> > mapActionToAbstractNodes;

    QString originalUrl, lastLocalUrl, affordanceFilter, actionFilter, originalTitle;

    int counter, idCounter, affordanceCounter, actionCounter, abstractCounter;
    bool skipOutOfScopeUrls;

private:
    void applyLine(const QString &key, const QString &value);
    QString escapeString(const QString &s);
    QString unescapeString(const QString &s);
};

#endif // DATA_H
