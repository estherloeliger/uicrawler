#include "data.h"

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
    return s;
}

//stub for machine state
void Data::applyString(const QString &s)
{
}
