#ifndef NFA_H
#define NFA_H

#include <memory>
#include <vector>
#include <list>
#include <unordered_set>

#include "NFA_Transition.h"

/*
 * A class representing an NFA.
 */
class NFA
{
  private:

    // NFA adjacency lists representation
    std::vector<std::list<NFA_Transition>> state_map;

    // The NFA's final state
    unsigned final_state_id;

    // The NFA's start state
    unsigned start_state_id;

  public:
    static const char EPSILON;
    static const unsigned ERROR;
    
    /*
     * Constructs a trivial NFA accepting one character
     */
    NFA(char c);
    
    /*
     * Construct an NFA corresponding to the disjunction of both operands'
     * regular expressions
     */
    void disjunction(const std::unique_ptr<NFA>& other);
    
    /*
     * Constructs an NFA corresponding to the concatenation of both operands'
     * regular expressions
     */
    void concatenate(const std::unique_ptr<NFA>& other);
    
    /*
     * Construct the NFA corresponding to the closure of the current NFA's
     * regular language
     */
    void closure();
   
    /*
     * The NFA's transition function
     * If a transition exists from state x to y over character c,
     * delta(x, c) = y
     *
     * returns NFA::ERROR if no transition from x exists over c
     */
    unsigned delta(unsigned state, char character) const;
    
    /*
     * Returns the set of states reachable by 0 or more epsilon transitions
     */
    std::unordered_set<unsigned> epsilon_closure(unsigned initial_state) const;
   
    unsigned get_final_state_id() const { return final_state_id; }
    unsigned get_start_state_id() const { return start_state_id; }
};

#endif
