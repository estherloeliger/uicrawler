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
    //general helper methods
    void applyLine(const QString &key, const QString &value);
    QString escapeString(const QString &s);
    QString unescapeString(const QString &s);

    //helper methods data->string
    QString stateMappingVectorString(const QVector<QPair<State, State> > &v);
    QString arrowMappingVectorString(const QVector<QPair<Arrow, Arrow> > &v);

    QString stateMappingString(const QPair<State, State> &p);
    QString arrowMappingString(const QPair<Arrow, Arrow> &p);

    QString stateVectorString(const QVector<State> &v);
    QString arrowVectorString(const QVector<Arrow> &v);

    QString stateString(const State &state);
    QString arrowString(const Arrow &a);

    QString stateTitlesString(const QHash<int, QString> &h);

    //helper methods string->data
    void applyStates(const QString &s);
    void applyArrowMapping(const QString &s, QVector<QPair<Arrow, Arrow> > &v);
    void applyStateMapping(const QString &s, QVector<QPair<State, State> > &v);
    void applyStateVector(const QString &s, QVector<State> &v);
    void applyArrowVector(const QString &s, QVector<Arrow> &v);
    void applyState(const QString &s, State &state);
    void applyArrow(const QString &s, Arrow &a);
    void applyStateTitles(const QString &s, QHash<int, QString> &h);




};

#endif // DATA_H
