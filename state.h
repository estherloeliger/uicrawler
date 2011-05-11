#ifndef STATE_H
#define STATE_H

#include <QString>

class State {
public:
    State(int idP = -1, QString titleP = "", QString bodyP = "") : id(idP), title(titleP), body(bodyP) { }
    int id;
    QString title, body;
    bool operator== ( const State& s ) const
    {
            return ( ( s.id == id ) &&
                     ( s.title == title ) &&
                     ( s.body == body ));
    }
};

#endif // STATE_H
