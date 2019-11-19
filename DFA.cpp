/*
 * DFA implementation file
 */

#include <iostream>
#include <deque>

#include "DFA.h"
#include "NFA.h"
#include "DFA_State.h"


using namespace std;

static const char ALPHABET_START {' '};
static const char ALPHABET_END {'~'};

const DFA_State DFA::ERROR {{}};

DFA::DFA(const NFA& nfa)
{
  // "subset construction" algorithm

  // unordered_set representing the dfa's start state
  auto start_set {nfa.epsilon_closure(nfa.get_start_state_id())};

  start_state = DFA_State(start_set);

  // add the start state to accepted state set if necessary
  if (start_set.find(nfa.get_final_state_id()) != start_set.end())
  {
    accepted_set.emplace(start_set);
  }

  deque<unordered_set<unsigned>> work_list {start_set};

  // Do the subset construction algorithm
  while (!work_list.empty())
  {
    auto curr_dfa_state {work_list.front()};
    work_list.pop_front();

    // Add all outgoing transitions for the current dfa state
    for (char c {ALPHABET_START}; c <= ALPHABET_END; c++)
    {
      // Construct the destination state over character c
      // This is the union of epsilon closures of delta(si, c)
      unordered_set<unsigned> dst_set;
      for (auto nfa_state : curr_dfa_state)
      {
        if (nfa.delta(nfa_state, c) != NFA::ERROR) 
        { 
          for (auto x : nfa.epsilon_closure(nfa.delta(nfa_state, c)))
          {
            dst_set.emplace(x);
          }
        }
      }

      DFA_State dst_state(dst_set);
      if (dst_state != DFA::ERROR)
      {
        // Record the transition
        state_map[curr_dfa_state].push_front({c, dst_state});

        // if we haven't seen it before
        // Create an entry for and add it to the worklist 
        if (state_map.find(dst_state) == state_map.end())
        {
          work_list.push_back(dst_set);
          state_map[dst_state] = {};
          
          // Add to accepted state set if necessary
          if (dst_set.find(nfa.get_final_state_id()) != dst_set.end())
          {
            accepted_set.emplace(dst_state);
          }
        }
      }
    }
  }
}

bool DFA::accept(const string& to_accept)
{
  DFA_State curr_state {start_state};

  // Traverse the DFA
  for (auto c : to_accept)
  {
    curr_state = delta(curr_state, c);
    if (curr_state == DFA::ERROR)
    {
      // reached error state
      return false;
    }
  }

  // Did not finish at an accepting state
  return accepted_set.find(curr_state) != accepted_set.end();
}

void DFA::minimize()
{
  // Do Hopcroft's algorithm and get the resulting set partition
  auto set_partition {hopcroft()};

  // Rebuild the DFA
  unordered_map<DFA_State, list<DFA_Transition>> new_state_map;
 
  /*
   * Each element of the set partition is a state in the new DFA.
   * New state ids are set to their location in the set partition to guarantee
   * uniqueness
   */
  DFA_State new_start({0});
  unordered_set<DFA_State> new_accepted_set;
 
  // Fill transitions
  for (size_t i{0}; i < set_partition.size(); i++)
  {
    DFA_State new_state({static_cast<unsigned>(i)});
    new_state_map[new_state] = {};

    // for all old DFA_States in the set
    for (auto element : set_partition.at(i))
    {
      // Add to the new DFA's accepted set if needed
      if (accepted_set.find(element) != accepted_set.end())
      {
        new_accepted_set.emplace(new_state);
      }

      // Update the new DFA's starting state id if needed
      if (element == start_state)
      {
        new_start = new_state;
      }

      // Add transitions to the new DFA
      for (auto t : state_map[element])
      {
        auto dst_index {get_loc(set_partition, t.dst_node_id) - 
          set_partition.begin()};
        
        DFA_State dst_state = DFA_State({static_cast<unsigned>(dst_index)});

        if (delta(new_state_map, new_state, t.character) ==
            DFA::ERROR)
        {
          new_state_map[new_state].push_front({t.character, dst_state});
        }
      }
    }
  }

  // Replace all fields
  start_state = new_start;
  state_map = new_state_map;
  accepted_set = new_accepted_set;
}

vector<unordered_set<DFA_State>> DFA::hopcroft()
{
  /*
   * Initialize the set partition with set of accepting states
   * and set of not accepted states.
   */
  unordered_set<DFA_State> accepting_states {accepted_set};
  unordered_set<DFA_State> not_accepting_states;

  for (auto p : state_map)
  {
    if (accepting_states.find(p.first) == accepting_states.end())
    {
      not_accepting_states.emplace(p.first);
    }
  }

  vector<unordered_set<DFA_State>> set_partition;

  // Make sure no subsets are empty (precondition for Hopcroft)
  if (!accepting_states.empty())
  {
    set_partition.push_back(accepting_states);
  }

  if (!not_accepting_states.empty())
  {
    set_partition.push_back(not_accepting_states);
  }

  // Perform splits until the set partition stops changing
  vector<unordered_set<DFA_State>> copy;
  while (set_partition != copy)
  {
    copy = set_partition;
    for (size_t i {0}; i < set_partition.size(); i++)
    {
      split(set_partition, i);
    }
  }

  return set_partition;
}

void DFA::split(vector<unordered_set<DFA_State>>& set_partition,
    size_t element)
{
  auto& subset {set_partition.at(element)};
  
  /*
   * Examine the behavior of set's elements over all
   * characters in the alphabet
   */
  for (char c {ALPHABET_START}; c <= ALPHABET_END; c++)
  {
    auto it {subset.begin()};
    auto behavior {get_loc(set_partition, delta(*it, c))};
    it++;
    
    for (; it != subset.end(); it++)
    {
      
      auto new_behavior {get_loc(set_partition, delta(*it, c))};
      
      /*
       * If two states in a set have different behavior,
       * split the set around character c
       */
      if (new_behavior != behavior)
      {
        // Split the set around c
        unordered_set<DFA_State> split_set;
        for (auto x : subset)
        {
          if (get_loc(set_partition, delta(x, c)) == new_behavior)
          {
            split_set.emplace(x);
          }
        }

        for (auto x : split_set)
        {
          subset.erase(x);
        }

        // Add the split set to the set partition
        set_partition.push_back(split_set);
        return;
      }
    }
  }
}

vector<unordered_set<DFA_State>>::iterator
  DFA::get_loc(vector<unordered_set<DFA_State>>& set_partition,
    DFA_State search)
{
  auto ret {set_partition.begin()};
  for (; ret != set_partition.end(); ret++)
  {
    if (ret->find(search) != ret->end())
      return ret;
  }

  return ret;
}

DFA_State DFA::delta(DFA_State state, char character)
{
  // Iterate through the state's transition list
  for (auto t : state_map[state])
  {
    if (t.character == character)
    {
      return t.dst_node_id;
    }
  }

  return DFA::ERROR;
}

DFA_State DFA::delta(unordered_map<DFA_State, 
    list<DFA_Transition>>& s_map, DFA_State state, char character)
{
  for (auto t : s_map[state])
  {
    if (t.character == character)
    {
      return t.dst_node_id;
    }
  }

  return DFA::ERROR;
}


void DFA::print()
{
  for (auto p : state_map)
  {
    cout << "STATE " << p.first << ": ";
    for (auto conn : p.second)
      cout << '(' << conn.character << ", " << conn.dst_node_id << ") ";
    cout << endl;

    if (p.first == start_state)
      cout << "START STATE" << endl;

    if (accepted_set.find(p.first) != accepted_set.end())
    {
      cout << "ACCEPTING STATE " << endl;
    }

    cout << endl;
  }
}
