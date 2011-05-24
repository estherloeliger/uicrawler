#ifndef ARROW_H
#define ARROW_H

#include <QString>

//tbd: write helper method to compare two Arrow objects ignoring IDs
class Arrow {
public:
    Arrow(int idP = -1, int sourceP = -1, int targetP = -1, int typeP = -1, bool activeP = true, QString labelP = "invalid") : id(idP), source(sourceP), target(targetP), type(typeP), active(activeP), label(labelP) { }
    int id;
    int source;
    int target;
    int type;
    bool active;
    QString label;
    bool operator== ( const Arrow& a ) const
    {
            return (
                     //( a.id == id ) &&
                     ( a.source == source ) &&
                     ( a.target == target ) &&
                     ( a.type == type ) &&
                     ( a.active == active ) &&
                     ( a.label == label ));
    }
};

#endif // ARROW_H
