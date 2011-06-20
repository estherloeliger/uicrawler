#include "arrow.h"
#include "constants.h"

QString Arrow::toString()
{
    //determine color & style
    QString arrowColor, arrowStyle;
    switch(type)
    {
    case ARROW_TYPE_ACTION:
        arrowColor = "dodgerblue4";
        arrowStyle = "solid";
        break;
    case ARROW_TYPE_EVENT_CLICK:
        arrowColor = "firebrick2";
        arrowStyle = "solid";
        break;
    case ARROW_TYPE_EVENT_MOUSEDOWN:
        arrowColor = "firebrick2";
        arrowStyle = "solid";
        break;
    case ARROW_TYPE_EVENT_MOUSEUP:
        arrowColor = "firebrick2";
        arrowStyle = "solid";
        break;
    case ARROW_TYPE_EVENT_MOUSEOVER:
        arrowColor = "gold";
        arrowStyle = "solid";
        break;
    case ARROW_TYPE_EVENT_SUBMIT:
        arrowColor = "chocolate";
        arrowStyle = "solid";
        break;
    case ARROW_TYPE_EVENT_ARIA:
        arrowColor = "firebrick2";
        arrowStyle = "solid";
        break;
    case ARROW_TYPE_LINK_EXTERNAL:
        arrowColor = "dodgerblue3";
        arrowStyle = "dashed";
        break;
    case ARROW_TYPE_LINK_SAME_HOST:
        arrowColor = "dodgerblue4";
        arrowStyle = "solid";
        break;
    case ARROW_TYPE_LINK_FRAGMENT:
        arrowColor = "dodgerblue4";
        arrowStyle = "solid";
        break;
    default:
        arrowColor = "black";
        arrowStyle = "dotted";
        break;
    }


    QString s = QString::number(source);
    s += " -> ";
    s += QString::number(target);
    s += " [label=\"";
    s += label;
    s += "\" color=\"";
    s += arrowColor;
    s += "\" style=\"";
    s += arrowStyle;

    //add flags as custom comma-separated style fields
    if (flags & ARROW_FLAG_VISIBLE)
    {
        s += ",visible";
    }
    if (flags & ARROW_FLAG_EVENT)
    {
        s += ",event";
    }
    if (flags & ARROW_FLAG_LINK)
    {
        s += ",link";
    }
    if (flags & ARROW_FLAG_ARIA)
    {
        s += ",aria";
    }

    s += "\"]\n";
    return s;
}
void Arrow::setFlag(int flagP)
{
    flags |= flagP;
}
void Arrow::setFlags(int flagsP)
{
    flags = flagsP;
}

