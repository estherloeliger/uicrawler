#include "mystring.h"
#include "constants.h"

MyString::MyString()
{

}

QString MyString::process(const QString &s)
{
    return MyString::truncate(MyString::flatten(s));
}

QString MyString::flatten(const QString &s)
{
    QString t = s;
    t = t.replace("\n", " ");
    t = t.replace("\r", " ");
    t = t.replace("\t", " ");
    t = t.replace(" {2,}", " ");
    return t;
}

QString MyString::truncate(const QString &s)
{
    if (s.length() < 24)
        return s;
    return s.left(20) + "...";
}

QString MyString::makeState(int i, const QString &title)
{
    QString s = "state ";
    s += QString::number(i);
    s += "\\n";
    s += title;
    return s;
}

QString MyString::stateToMapString(int id1, int id2, const QString &label1, const QString &label2)
{
    QString s;
    s += "(\"";
    s += QString::number(id1);
    s += "[label=\\\"";
    s += label1;
    s += "\\\"]\",\"";
    s += QString::number(id2);
    s += "[label=\\\"";
    s += label2;
    s += "\\\"]\")";

    return s;
}

QString MyString::triggerString(int trigger)
{
    switch (trigger)
    {
    case TRIGGER_CLICK:
        return "click";
        break;
    case TRIGGER_MOUSEDOWN:
        return "mousedown";
        break;
    case TRIGGER_MOUSEUP:
        return "mouseup";
        break;
    case TRIGGER_MOUSEOVER:
        return "mouseover";
        break;
    case TRIGGER_SUBMIT:
        return "submit";
        break;
    case TRIGGER_ARIA:
        return "aria";
        break;
    case TRIGGER_LINK:
        return "link";
        break;
    case TRIGGER_INIT:
        return "init";
        break;
    }
    return "invalid";
}
