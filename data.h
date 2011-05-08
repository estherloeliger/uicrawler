#ifndef DATA_H
#define DATA_H

#include <set>
#include <QHash>
#include <QString>
#include <QVector>
#include "arrow.h"

class Data {
public:
    Data() : counter(0) { }
    QHash<QString, int> states;
    QHash<int, QString> stateTitles;
    QVector<Arrow> arrows;
    QVector<Arrow> actions;
    QString originalUrl, lastLocalUrl;

    int counter;
    void clear() { states.clear(); stateTitles.clear(); arrows.clear(); actions.clear(); counter = 0; }
};

#endif // DATA_H
