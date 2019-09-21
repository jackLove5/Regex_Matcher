#ifndef NFA_H
#define NFA_H

#include <vector>
#include <list>
#include <unordered_set>

#include "fa_transition.h"

/*
 * A class representing an NFA.
 * Implemented using adjacency lists
 */
class NFA
{
  private:
    // Maps an NFA state id to a list containing the state's transitions
    std::vector<std::list<fa_transition>> state_map;

    unsigned final_state_id;
    unsigned start_state_id;

  public:
    NFA(char c);
    void disjunction(const NFA* other);
    void concatenate(const NFA* other);
    void closure();
    int delta(unsigned state, char character) const;
    std::unordered_set<unsigned> epsilon_closure(unsigned initial_state) const;
    unsigned get_final_state_id() const { return final_state_id; }
    unsigned get_start_state_id() const { return start_state_id; }
};

#endif
