#ifndef STATE_H
#define STATE_H

#include <QString>

class State {
public:
    State(int idP = 0, QString titleP = "", QString bodyP = "") : id(idP), title(titleP), body(bodyP) { }
    int id;
    QString title, body;
    bool operator== ( const State& s ) const
    {
            return ( //ignore unique ID
                     //( s.id == id ) &&
                     ( s.title == title ) &&
                     ( s.body == body ));
    }
    QString toString();
};

#endif // STATE_H
