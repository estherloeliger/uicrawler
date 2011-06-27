#include "data.h"
#include <QtGui>

Data::Data() : counter(0), idCounter(0), affordanceCounter(0), actionCounter(0), abstractCounter(0), skipOutOfScopeUrls(false)
{
}

void Data::clear()
{
    states.clear();

    affordanceStateTitles.clear();
    actionStateTitles.clear();
    abstractStateTitles.clear();

    affordanceStates.clear();
    actionStates.clear();
    abstractStates.clear();

    affordanceEdges.clear();
    actionEdges.clear();
    abstractEdges.clear();

    mapAffordanceToAbstractEdges.clear();
    mapActionToAbstractEdges.clear();
    mapAffordanceToAbstractNodes.clear();
    mapActionToAbstractNodes.clear();

    counter = idCounter = affordanceCounter = actionCounter = abstractCounter = 0;
    originalUrl = originalTitle = lastLocalUrl = affordanceFilter = actionFilter = "";
}

//stub for machine state
QString Data::toString()
{
    QString s;

    //states hash
    s += "states=";
    QHashIterator<QString, int> statesIt(this->states);
    bool first = true;
    while (statesIt.hasNext())
    {
        if (!first)
        {
            s += ",";
        }
        else
        {
            first = false;
        }
        statesIt.next();
        s += escapeString(statesIt.key());
        s += "->";
        s += QString::number(statesIt.value());
    }
    s += "\n";

    //affordanceStateTitles
    s += "affordanceStateTitles=";
    s += stateTitlesString(affordanceStateTitles);
    s += "\n";

    //actionStateTitles
    s += "actionStateTitles=";
    s += stateTitlesString(actionStateTitles);
    s += "\n";

    //abstractStateTitles
    s += "abstractStateTitles=";
    s += stateTitlesString(abstractStateTitles);
    s += "\n";

    //affordanceEdges
    s += "affordanceEdges=";
    s += arrowVectorString(affordanceEdges);
    s += "\n";

    //actionEdges
    s += "actionEdges=";
    s += arrowVectorString(actionEdges);
    s += "\n";

    //abstractEdges
    s += "abstractEdges=";
    s += arrowVectorString(abstractEdges);
    s += "\n";

    //affordanceStates
    s += "affordanceStates=";
    s += stateVectorString(affordanceStates);
    s += "\n";

    //actionStates
    s += "actionStates=";
    s += stateVectorString(actionStates);
    s += "\n";

    //abstractStates
    s += "abstractStates=";
    s += stateVectorString(abstractStates);
    s += "\n";

    //mapAffordanceToAbstractEdges
    s += "mapAffordanceToAbstractEdges=";
    s += arrowMappingVectorString(mapAffordanceToAbstractEdges);
    s += "\n";

    //mapActionToAbstractEdges
    s += "mapActionToAbstractEdges=";
    s += arrowMappingVectorString(mapActionToAbstractEdges);
    s += "\n";

    //mapAffordanceToAbstractNodes
    s += "mapAffordanceToAbstractNodes=";
    s += stateMappingVectorString(mapAffordanceToAbstractNodes);
    s += "\n";

    //mapActionToAbstractNodes
    s += "mapActionToAbstractNodes=";
    s += stateMappingVectorString(mapActionToAbstractNodes);
    s += "\n";

    //QString
    s += "originalUrl=";
    s += escapeString(originalUrl);
    s += "\n";

    s += "lastLocalUrl=";
    s += escapeString(lastLocalUrl);
    s += "\n";

    s += "affordanceFilter=";
    s += escapeString(affordanceFilter);
    s += "\n";

    s += "originalTitle=";
    s += escapeString(originalTitle);
    s += "\n";

    s += "affordanceFilter=";
    s += escapeString(affordanceFilter);
    s += "\n";

    //int
    s += "counter=";
    s += QString::number(counter);
    s += "\n";

    s += "idCounter=";
    s += QString::number(idCounter);
    s += "\n";
    s += "affordanceCounter=";
    s += QString::number(affordanceCounter);
    s += "\n";
    s += "actionCounter=";
    s += QString::number(actionCounter);
    s += "\n";
    s += "abstractCounter=";
    s += QString::number(abstractCounter);
    s += "\n";

    //bool
    s += "skipOutOfScopeUrls=";
    s += (skipOutOfScopeUrls) ? "true" : "false";
    s += "\n";

    return s;
}

QString Data::arrowMappingVectorString(const QVector<QPair<Arrow, Arrow> > &v)
{
    QString s;

    int count = v.count();
    for (int i = 0; i < count; i++)
    {
        if (i)
        {
            s += ",";
        }
        QPair<Arrow, Arrow> p = v.at(i);
        s += ";";
        s += arrowString(p.first);
        s += "|";
        s += arrowString(p.second);
        s += ";";
    }

    return s;
}

QString Data::stateMappingVectorString(const QVector<QPair<State, State> > &v)
{
    QString s;

    int count = v.count();
    for (int i = 0; i < count; i++)
    {
        if (i)
        {
            s += ",";
        }
        QPair<State, State> p = v.at(i);
        s += ";";
        s += stateString(p.first);
        s += "|";
        s += stateString(p.second);
        s += ";";
    }

    return s;
}

QString Data::stateMappingString(const QPair<State, State> &p)
{
    QString s;
    s += "%";
    s += stateString(p.first);
    s += "$";
    s += stateString(p.second);
    s += "%";

    return s;
}

QString Data::arrowMappingString(const QPair<Arrow, Arrow> &p)
{
    QString s;
    s += ";";
    s += arrowString(p.first);
    s += "|";
    s += arrowString(p.second);
    s += ";";

    return s;
}

QString Data::stateVectorString(const QVector<State> &v)
{
    QString s;

    int count = v.count();
    for (int i = 0; i < count; i++)
    {
        State state = v.at(i);
        if (i)
        {
            s += ",";
        }
        s += stateString(state);
    }

    return s;
}

QString Data::arrowVectorString(const QVector<Arrow> &v)
{
    QString s;

    int count = v.count();
    for (int i = 0; i < count; i++)
    {
        Arrow a = v.at(i);
        if (i)
        {
            s += ",";
        }
        s += arrowString(a);
    }

    return s;
}

QString Data::arrowString(const Arrow &a)
{
    QString s;

    s += "%";
    s += QString::number(a.id);
    s += "$";
    s += QString::number(a.source);
    s += "$";
    s += QString::number(a.target);
    s += "$";
    s += QString::number(a.type);
    s += "$";
    s += (a.active) ? "true" : "false";
    s += "$";
    s += escapeString(a.label);

    return s;
}

QString Data::stateString(const State &state)
{
    QString s;

    s += "%";
    s += QString::number(state.id);
    s += "$";
    s += escapeString(state.title);
    s += "$";
    s += escapeString(state.body);

    return s;
}

QString Data::stateTitlesString(const QHash<int, QString> &h)
{
    QString s;
    QHashIterator<int, QString> it(h);
    bool first = true;
    while (it.hasNext())
    {
        if (!first)
        {
            s += ",";
        }
        else
        {
            first = false;
        }
        it.next();
        s += QString::number(it.key());
        s += "->";
        s += escapeString(it.value());
    }
    return s;
}

void Data::applyString(const QString &s)
{
    clear();

    QStringList lines = s.split("\n");
    int linesCount = lines.count();
    for (int i = 0; i < linesCount; i++)
    {
        QStringList pair = lines.at(i).split("=");
        if (pair.count() < 2)
            continue;
        applyLine(pair.at(0), pair.at(1));
    }
}

//private - called by applyString
void Data::applyLine(const QString &key, const QString &value)
{
    if (key == "states")
    {
        applyStates(value);
    }
    else if (key == "affordanceStateTitles")
    {
        applyStateTitles(value, affordanceStateTitles);
    }
    else if (key == "actionStateTitles")
    {
        applyStateTitles(value, actionStateTitles);
    }
    else if (key == "abstractStateTitles")
    {
        applyStateTitles(value, abstractStateTitles);
    }
    else if (key == "affordanceEdges")
    {
        applyArrowVector(value, affordanceEdges);
    }
    else if (key == "actionEdges")
    {
        applyArrowVector(value, actionEdges);
    }
    else if (key == "abstractEdges")
    {
        applyArrowVector(value, abstractEdges);
    }
    else if (key == "affordanceStates")
    {
        applyStateVector(value, affordanceStates);
    }
    else if (key == "actionStates")
    {
        applyStateVector(value, actionStates);
    }
    else if (key == "abstractStates")
    {
        applyStateVector(value, abstractStates);
    }
    else if (key == "mapAffordanceToAbstractEdges")
    {
        applyArrowMapping(value, mapAffordanceToAbstractEdges);
    }
    else if (key == "mapActionToAbstractEdges")
    {
        applyArrowMapping(value, mapActionToAbstractEdges);
    }
    else if (key == "mapAffordanceToAbstractNodes")
    {
        applyStateMapping(value, mapAffordanceToAbstractNodes);
    }
    else if (key == "mapActionToAbstractNodes")
    {
        applyStateMapping(value, mapActionToAbstractNodes);
    }
    else if (key == "originalUrl")
    {
        originalUrl = value;
    }
    else if (key == "lastLocalUrl")
    {
        lastLocalUrl = value;
    }
    else if (key == "affordanceFilter")
    {
        affordanceFilter = value;
    }
    else if (key == "actionFilter")
    {
        actionFilter = value;
    }
    else if (key == "originalTitle")
    {
        originalTitle = value;
    }
    else if (key == "counter")
    {
        counter = value.toInt();
    }
    else if (key == "idCounter")
    {
        idCounter = value.toInt();
    }
    else if (key == "affordanceCounter")
    {
        affordanceCounter = value.toInt();
    }
    else if (key == "actionCounter")
    {
        actionCounter = value.toInt();
    }
    else if (key == "abstractCounter")
    {
        abstractCounter = value.toInt();
    }
    else if (key == "skipOutOfScopeUrls")
    {
        skipOutOfScopeUrls = (value == "true") ? true : false;
    }
}

void Data::applyArrowMapping(const QString &s, QVector<QPair<Arrow, Arrow> > &v)
{
    v.clear();
    QStringList fields = s.split(",");
    int count = fields.count();
    for (int i = 0; i < count; i++)
    {
        QString field = fields.at(i);
        QStringList pairs = field.split(";", QString::SkipEmptyParts);
        int pairCount = pairs.count(0);

        for (int j = 0; j < pairCount; j++)
        {
            QString pair = pairs.at(j);
            QStringList arrows = pair.split("|", QString::SkipEmptyParts);
            int arrowCount = arrows.count();
            if (arrowCount < 2)
                continue;

            Arrow a, b;
            applyArrow(arrows.at(0), a);
            applyArrow(arrows.at(1), b);
            v.push_back(qMakePair(a, b));
        }
    }
}

void Data::applyStateMapping(const QString &s, QVector<QPair<State, State> > &v)
{
    v.clear();
    QStringList fields = s.split(",");
    int count = fields.count();
    for (int i = 0; i < count; i++)
    {
        QString field = fields.at(i);
        QStringList pairs = field.split(";", QString::SkipEmptyParts);
        int pairCount = pairs.count(0);

        for (int j = 0; j < pairCount; j++)
        {
            QString pair = pairs.at(j);
            QStringList states = pair.split("|", QString::SkipEmptyParts);
            int stateCount = states.count();
            if (stateCount < 2)
                continue;

            State s, t;
            applyState(states.at(0), s);
            applyState(states.at(1), t);
            v.push_back(qMakePair(s, t));
        }
    }
}

void Data::applyStateVector(const QString &s, QVector<State> &v)
{
    v.clear();
    QStringList fields = s.split(",");
    int count = fields.count();
    for (int i = 0; i < count; i++)
    {
        QString field = fields.at(i);
        QStringList states = field.split(";", QString::SkipEmptyParts);
        int stateCount = states.count();
        for (int j = 0; j < stateCount; j++)
        {
            State state;
            applyState(states.at(j), state);
            v.push_back(state);
        }
    }
}

void Data::applyArrowVector(const QString &s, QVector<Arrow> &v)
{
    v.clear();
    QStringList fields = s.split(",");
    int count = fields.count();
    for (int i = 0; i < count; i++)
    {
        QString field = fields.at(i);
        QStringList arrows = field.split(";", QString::SkipEmptyParts);
        int arrowCount = arrows.count();
        for (int j = 0; j < arrowCount; j++)
        {
            Arrow a;
            applyArrow(arrows.at(j), a);
            v.push_back(a);
        }
    }
}

void Data::applyState(const QString &s, State &state)
{
    QStringList fields = s.split("$", QString::SkipEmptyParts);
    int count = fields.count();
    if (count < 3)
        return;

    QString id, title, body;

    id = fields.at(0);
    title = fields.at(1);
    body = fields.at(2);

    state.id = id.toInt();
    state.title = title;
    state.body = body;
}

void Data::applyArrow(const QString &s, Arrow &a)
{
    QStringList fields = s.split("$", QString::SkipEmptyParts);
    int count = fields.count();
    if (count < 6)
        return;

    QString id, source, target, type, active, label;

    id = fields.at(0);
    source = fields.at(1);
    target = fields.at(2);
    type = fields.at(3);
    active = fields.at(4);
    label = fields.at(5);

    a.id = id.toInt();
    a.source = source.toInt();
    a.target = target.toInt();
    a.type = type.toInt();
    a.active = (active == "true") ? true : false;
    a.label = label;
}

void Data::applyStateTitles(const QString &s, QHash<int, QString> &h)
{
    h.clear();
    QStringList fields = s.split(",");
    int count = fields.count();
    for (int i = 0; i < count; i++)
    {
        QString field = fields.at(i);
        QStringList elements = field.split("->");
        if (elements.count() < 2)
            continue;
        QString a, b;
        a = elements.at(0);
        b = elements.at(1);

        h.insert(a.toInt(), b);
    }
}

void Data::applyStates(const QString &s)
{
    states.clear();
    QStringList fields = s.split(",");
    int count = fields.count();
    for (int i = 0; i < count; i++)
    {
        QStringList pair = fields.at(i).split("->");
        if (pair.count() < 2)
            continue;
        states.insert(unescapeString(pair.at(0)), pair.at(1).toInt());
    }
}

QString Data::escapeString(const QString &s)
{
    QString t = s;
    t.replace("=", "__EQ__");
    t.replace(">", "__GT__");
    t.replace(",", "__COMMA__");
    t.replace(";", "__SEMI__");
    t.replace("|", "__PIPE__");
    t.replace("%", "__PERC__");
    t.replace("$", "__DOLLAR__");
    t.replace("\n", "__N__");
    t.replace("\r", ""); //ensure Unix-LF
    return t;
}
QString Data::unescapeString(const QString &s)
{
    QString t = s;
    t.replace("__EQ__", "=");
    t.replace("__GT__", ">");
    t.replace("__COMMA__", ",");
    t.replace("__SEMI__", ";");
    t.replace("__PIPE__", "|");
    t.replace("__PERC__", "%");
    t.replace("__DOLLAR__", "$");
    t.replace("__N__", "\n");
    return t;
}
