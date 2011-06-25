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
        s += "=>";
        s += QString::number(statesIt.value());
    }

    //affordanceStateTitles
    s += "affordanceStateTitles=";
    QHashIterator<int, QString> affordanceStateTitlesIt(this->affordanceStateTitles);
    first = true;
    while (affordanceStateTitlesIt.hasNext())
    {
        if (!first)
        {
            s += ",";
        }
        else
        {
            first = false;
        }
        affordanceStateTitlesIt.next();
        s += QString::number(affordanceStateTitlesIt.key());
        s += "=>";
        s += escapeString(affordanceStateTitlesIt.value());
    }

    //actionStateTitles
    s += "actionStateTitles=";
    QHashIterator<int, QString> actionStateTitlesIt(this->actionStateTitles);
    first = true;
    while (actionStateTitlesIt.hasNext())
    {
        if (!first)
        {
            s += ",";
        }
        else
        {
            first = false;
        }
        actionStateTitlesIt.next();
        s += QString::number(actionStateTitlesIt.key());
        s += "=>";
        s += escapeString(actionStateTitlesIt.value());
    }

    //etc

    return s;
}

//stub for machine state
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
    QStringList fields = value.split(",");
    int count = fields.count();

    if (key == "states")
    {
        for (int i = 0; i < count; i++)
        {
            QStringList pair = fields.at(i).split("=>");
            if (pair.count() < 2)
                continue;
            states.insert(unescapeString(pair.at(0)), pair.at(1).toInt());
        }
    }
    //else if etc
}

QString Data::escapeString(const QString &s)
{
    QString t = s;
    t.replace("=", "__EQUALS__");
    t.replace(">", "__GT__");
    t.replace(",", "__COMMA__");
    t.replace("{", "__LEFTCURL__");
    t.replace("}", "__RIGHTCURL__");
    return t;
}
QString Data::unescapeString(const QString &s)
{
    QString t = s;
    t.replace("__EQUALS__", "=");
    t.replace("__GT__", ">");
    t.replace("__COMMA__", ",");
    t.replace("__LEFTCURL__", "{");
    t.replace("__RIGHTCURL__", "}");
    return t;
}
