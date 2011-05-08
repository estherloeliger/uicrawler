#ifndef ARROW_H
#define ARROW_H

#include <QString>

class Arrow {
public:
    Arrow(int sourceP = -1, int targetP = -1, int typeP = -1, QString labelP = "invalid") : source(sourceP), target(targetP), type(typeP), label(labelP) { }
    int source;
    int target;
    int type;
    QString label;
    bool operator== ( const Arrow& a ) const
    {
            return ( ( a.source == source ) &&
                     ( a.target == target ) &&
                     ( a.type == type ) &&
                     ( a.label == label ));
    }
};

#endif // ARROW_H
