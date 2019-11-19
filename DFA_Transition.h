#ifndef DFA_TRANSITION_H
#define DFA_TRANSITION_H

class DFA_State;

#include "DFA_State.h"

/*
 * A class representing a transition in a DFA
 */
class DFA_Transition
{
  public:
    DFA_Transition(char c, DFA_State id) :
      character(c), dst_node_id(id) {}
    char character;
    DFA_State dst_node_id;
};
#endif
