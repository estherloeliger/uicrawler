#ifndef ARROW_H
#define ARROW_H

#include <QString>

class Arrow {
public:
    Arrow(int idP = -1, int sourceP = -1, int targetP = -1, int typeP = -1, QString labelP = "invalid") : id(idP), source(sourceP), target(targetP), type(typeP), label(labelP) { }
    int id;
    int source;
    int target;
    int type;
    QString label;
    bool operator== ( const Arrow& a ) const
    {
            return ( ( a.id == id ) &&
                     ( a.source == source ) &&
                     ( a.target == target ) &&
                     ( a.type == type ) &&
                     ( a.label == label ));
    }
};

#endif // ARROW_H
