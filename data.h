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

    QVector<QPair<Arrow, Arrow> > mapAffordanceToAbstractEdges;
    QVector<QPair<Arrow, Arrow> > mapActionToAbstractEdges;
    QSet<QString> mapAffordanceToAbstractNodes;
    QSet<QString> mapActionToAbstractNodes;

    QString originalUrl, lastLocalUrl, affordanceFilter, actionFilter;

    int counter, idCounter, affordanceCounter, actionCounter, abstractCounter;
    bool skipOutOfScopeUrls;
};

#endif // DATA_H
