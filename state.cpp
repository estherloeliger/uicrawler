#include "state.h"

QString State::toString()
{
    QString s;
    s = QString::number(id);
    s += " [label=\"";
    s += title;
    s += "\"]\n";
    return s;
}
