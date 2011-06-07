#ifndef DATA_H
#define DATA_H

#include <set>
#include <QHash>
#include <QString>
#include <QVector>
#include <QSet>
#include "arrow.h"
#include "state.h"

class Data {
public:
    Data() : counter(0), idCounter(0), affordanceCounter(0), actionCounter(0), abstractCounter(0) { }

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

    //QHash<int, QPair<int, QString> > mappingAffordanceToAbstract;
    //QHash<int, QPair<int, QString> > mappingActionToAbstract;
    QSet<QString> mapAffordanceToAbstractEdges;
    QSet<QString> mapActionToAbstractEdges;
    QSet<QString> mapAffordanceToAbstractNodes;
    QSet<QString> mapActionToAbstractNodes;

    QString originalUrl, lastLocalUrl;

    int counter, idCounter, affordanceCounter, actionCounter, abstractCounter;
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
