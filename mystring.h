#ifndef MYSTRING_H
#define MYSTRING_H

#include <QString>

class MyString
{
public:
    MyString();
    static QString process(const QString &s);
    static QString flatten(const QString &s);
    static QString truncate(const QString &s);
    static QString makeState(int i, const QString &title);
    static QString stateToMapString(int id1, int id2, const QString &label1, const QString &label2);
    static QString triggerString(int trigger);
private:
};

#endif // MYSTRING_H
