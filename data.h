#ifndef DATA_H
#define DATA_H

#include <set>
#include <QHash>
#include <QString>
#include <QVector>
#include "arrow.h"
#include "state.h"

class Data {
public:
    Data() : counter(0), idCounter(0) { }
    QHash<QString, int> states;
    QHash<int, QString> stateTitles;
    QVector<State> affordanceStates;
    QVector<State> actionStates;
    QVector<Arrow> affordances;
    QVector<Arrow> actions;
    QHash<int, int> affordancesToActions;
    QString originalUrl, lastLocalUrl;

    int counter, idCounter;
    void clear() { states.clear(); stateTitles.clear(); affordances.clear(); actions.clear(); affordanceStates.clear(); actionStates.clear(); counter = 0; }
};

#endif // DATA_H
