#ifndef COMPACT_DFA
#define COMPACT_DFA

#include <vector>
#include <list>
#include <unordered_set>
#include <string>
#include "fa_transition.h"

/*
 * A class Representing a DFA.
 * Supports minimization and acceptence testing.
 * Implemented using adjacency lists
 */
class Compact_DFA
{
  private:
    std::vector<std::list<fa_transition>> state_map;
    std::unordered_set<unsigned> accepted_set;
    unsigned start_state_id;

    std::vector<std::unordered_set<unsigned>> hopcroft();
    void split(std::vector<std::unordered_set<unsigned>>& partition,
        size_t element);
    
    int get_loc(const std::vector<std::unordered_set<unsigned>>& partition,
        int search);
  
  public:
    Compact_DFA(const std::vector<std::list<fa_transition>>& s,
        const std::unordered_set<unsigned>& a, unsigned start_state) :
            state_map(s), accepted_set(a), start_state_id(start_state) {}

    int delta(unsigned, char character);
    int delta(const std::vector<std::list<fa_transition>>& s_map,
        unsigned s, char c);
    
    void minimize();
    bool accept(const std::string& to_accept);
    void print();
};

#endif
