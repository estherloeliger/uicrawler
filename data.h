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
    Data() : counter(0), idCounter(0), affordanceCounter(0), actionCounter(0), abstractCounter(0), skipOutOfScopeUrls(false) { }

    QHash<QString, int> states;

    QHash<int, QString> affordanceStateTitles;
    QHash<int, QString> actionStateTitles;
    QHash<int, QString> abstractStateTitles;

    QVector<State> affordanceStates;
    QVector<State> actionStates;
    QVector<State> abstractStates;

    QVector<Arrow> affordanceEdges;
    QVector<Arrow> actionEdges;
    QVector<Arrow> abstractEdges;

    //QSet<QString> mapAffordanceToAbstractEdges;
    //QSet<QString> mapActionToAbstractEdges;
    QVector<QPair<Arrow, Arrow> > mapAffordanceToAbstractEdges;
    QVector<QPair<Arrow, Arrow> > mapActionToAbstractEdges;
    QSet<QString> mapAffordanceToAbstractNodes;
    QSet<QString> mapActionToAbstractNodes;

    QString originalUrl, lastLocalUrl;

    int counter, idCounter, affordanceCounter, actionCounter, abstractCounter;
    bool skipOutOfScopeUrls;
    void clear()
    {
        states.clear();
        affordanceStateTitles.clear();
        affordanceEdges.clear();
        actionEdges.clear();
        abstractEdges.clear();
        affordanceStates.clear();
        actionStates.clear();
        counter = 0;
    }
};

#endif // DATA_H
