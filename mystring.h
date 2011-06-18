#ifndef MYSTRING_H
#define MYSTRING_H

#include <QtGui>
#include <QString>
#include "arrow.h"
#include "state.h"
#include "data.h"

class MyString
{
public:
    MyString();
    static QString process(const QString &s);
    static QString flatten(const QString &s);
    static QString truncate(const QString &s);
    static QString makeState(int i, const QString &title);
    static QString triggerString(int trigger);
    static QString statesToMapString(const State &a, const State &b);
    static QString arrowsToMapString(const Arrow &a, const Arrow &b);
    static QString stateMappingsToString(const QVector<QPair<State, State> > &v);
    static QString edgeMappingsToString(const QVector<QPair<Arrow, Arrow> > &v);
    static QString dataToDotString(Data *data, int type);

private:
};

#endif // MYSTRING_H
