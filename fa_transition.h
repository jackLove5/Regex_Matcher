#ifndef FA_TRANSITION
#define FA_TRANSITION

/*
 * A class representing a transition in a
 * DFA or NFA
 */
class fa_transition
{
  public:
    fa_transition(char c, unsigned id) :
      character(c), dst_node_id(id) {}
    char character;
    unsigned dst_node_id;
};
#endif
