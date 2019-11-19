#ifndef DFA_H
#define DFA_H

#include <vector>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include <string>

#include "NFA.h"
#include "DFA_Transition.h"
#include "DFA_State.h"

/*
 * A class Representing a DFA.
 * Supports minimization and acceptence testing.
 */
class DFA
{
  private:
    // Adjacency lists representation of the DFA
    std::unordered_map<DFA_State, std::list<DFA_Transition>> state_map;
    
    // Set of accepting states
    std::unordered_set<DFA_State> accepted_set;
    
    // The DFA's start state
    DFA_State start_state;

    /*
     * Perform Hopcroft's algorithm
     * returns the resulting set partition
     */
    std::vector<std::unordered_set<DFA_State>> hopcroft();
    
    /*
     * Checks if all the states in a particular subset have
     * the same behavior in response to all characters in the alphabet.
     * If all the elements in the subset don't have the same behavior,
     * the subset gets split
     */
    void split(std::vector<std::unordered_set<DFA_State>>& partition,
        size_t element);
    
    /*
     * Returns an iterator pointing to the element containing the value search
     */
    std::vector<std::unordered_set<DFA_State>>::iterator
      get_loc(std::vector<std::unordered_set<DFA_State>>& partition,
        DFA_State search);

    /*
     * Overload of the delta function that accepts any state map
     */
    DFA_State delta(std::unordered_map<DFA_State, 
        std::list<DFA_Transition>>&, DFA_State, char);
  
  public:

    /*
     * Constructs a DFA from an NFA
     */
    DFA(const NFA&);
    
    /*
     * DFA's transition function
     * If a transition from state x to y exists 
     * over character c, then delta(x, c) = y
     *
     * returns DFA::ERROR if no transition from x exists over c
     */
    DFA_State delta(DFA_State, char character);
    
    /*
     * Minimizes the DFA
     */ 
    void minimize();
    
    /*
     * Checks if a given string can be accepted by the DFA
     * returns true iff the DFA recognizes the input string
     */
    bool accept(const std::string& to_accept);
    
    /*
     * Print a description of the DFA
     */
    void print();
    
    /*
     * Represents a DFA error state
     */
    static const DFA_State ERROR;
};

#endif
