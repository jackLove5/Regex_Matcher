#include <vector>
#include <list>
#include <deque>
#include <unordered_set>
#include <unordered_map>

#include "DFA_Table.h"

/*
 * DFA_Table implementation file
 */

static const char ALPHABET_START {' '};
static const char ALPHABET_END {'~'};

using namespace std;

/**
 * Construct a DFA table from a given NFA.
 * Implements the "subset construction"
 * algorithm
*/
DFA_Table::DFA_Table(const NFA& nfa)
{
  auto dfa_start_state {nfa.epsilon_closure(nfa.get_start_state_id())};

  deque<DFA_Table_State> work_list;
  work_list.push_back(dfa_start_state);

  while (!work_list.empty())
  {
    auto curr_dfa_state {work_list.front()};
    work_list.pop_front();

    // Add all outgoing transitions for the current dfa state
    for (char c {ALPHABET_START}; c <= ALPHABET_END; c++)
    {
      DFA_Table_State dfa_dst_state;
      for (auto nfa_state : curr_dfa_state)
      {
        // If state x has an outgoing transition over character c
        if (nfa.delta(nfa_state, c) != -1) 
        {
          unsigned nfa_dst_state 
            {static_cast<unsigned>(nfa.delta(nfa_state, c))};
          
          // dfa_dst_state = dfa_dst_state U epsilon_closure(nfa_dst_state)
          for (auto x : nfa.epsilon_closure(nfa_dst_state))
          {
            dfa_dst_state.emplace(x);
          }
        }
      }
      
      // Record the transition
      table[curr_dfa_state][c] = dfa_dst_state;
      
      // Enqueue the dst dfa state if it hasn't been processed
      if (!dfa_dst_state.empty() &&
            table.find(dfa_dst_state) == table.end())
      {
        work_list.push_back(dfa_dst_state);
      }
    }
  }
}

/**
 * Convert the dfa table into a more compact representation
 */ 
Compact_DFA DFA_Table::get_compact_dfa(const NFA& nfa)
{
  /*
   * The state map of the new dfa
   * Maps state ids to a list of transitions
   */
  auto state_map 
    {vector<list<fa_transition>>(table.size(), list<fa_transition>())};

  unsigned new_start_state_id;
  unordered_set<unsigned> accepting_state_set;

  // Map the old dfa states to their new state ids
  unordered_map<DFA_Table_State, unsigned, Set_Hash> new_dfa_state_ids;

  unsigned i {0};
  for (auto& x : table)
  {
    new_dfa_state_ids[x.first] = i;
    i++;
  }

  /*
   * Convert the representation of the dfa states from sets
   * to unsigneds
   */
  for (auto& dfa_pair : table)
  {
    auto& old_dfa_state {dfa_pair.first};

    // Assign the new start state id
    if (old_dfa_state == nfa.epsilon_closure(nfa.get_start_state_id()))
    {
      new_start_state_id = new_dfa_state_ids[old_dfa_state];
    }

    // If current state is an accepting state, add it to the list
    if (old_dfa_state.find(nfa.get_final_state_id()) != old_dfa_state.end())
    {
      accepting_state_set.emplace(new_dfa_state_ids[old_dfa_state]);
    }

    /*
     * Add all outgoing transitions to the proper state
     */
    
    // Contains all the transitions for the current dfa state
    auto& dfa_transitions {dfa_pair.second};

    for (auto& t : dfa_transitions)
    {
      auto& dst_dfa_state {t.second};
      if (!dst_dfa_state.empty())
      {
        unsigned new_src_state_id {new_dfa_state_ids[old_dfa_state]};
        unsigned new_dst_state_id {new_dfa_state_ids[dst_dfa_state]};
        state_map.at(new_src_state_id).push_front({t.first, new_dst_state_id});
      }
    }
  }

  return {state_map, accepting_state_set, new_start_state_id};
}
