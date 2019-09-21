#include <deque>

#include "NFA.h"

using namespace std;

/*
 * NFA implementation file
 */

static const char EPSILON {-1};

/*
 * Constructs an nfa from a single character
 */
NFA::NFA(char c)
{
  start_state_id = 0;
  fa_transition transition(c, 1);
  
  // Record transition from start state
  state_map.push_back({transition});
  
  // Final state has no outgoing transitions 
  state_map.push_back({});
  
  final_state_id = transition.dst_node_id;
}

/*
 * Constructs an NFA corresponding to the concatenation of both operands'
 * regular expressions
 */
void NFA::concatenate(const NFA* other)
{
  unsigned size_offset {static_cast<unsigned>(state_map.size())};
  unsigned other_start {other->start_state_id + size_offset};

  /* Create a new epsilon transition from the final state 
   * to other's first state */
  fa_transition transition(-1, other_start);
  state_map.at(final_state_id).push_front(transition);

  // Copy all of other's states
  for (int i = 0; i < other->state_map.size(); i++)
  {
    // Add the state to the adjacency list
    state_map.push_back(other->state_map.at(i));
    
    // Update all of the state's transitions
    for (auto& conn : state_map.back())
    {
      conn.dst_node_id += size_offset;
    }
  }

  final_state_id = other->final_state_id + size_offset;
}

/*
 * Construct an NFA corresponding to the disjunction of both operands'
 * regular expressions
 */
void NFA::disjunction(const NFA* other)
{
  unsigned size_offset {static_cast<unsigned>(state_map.size())};
  unsigned other_start {other->start_state_id + size_offset};

  // Append other's states
  for (int i = 0; i < other->state_map.size(); i++)
  {
    state_map.push_back(other->state_map.at(i));
    for (auto& conn : state_map.back())
    {
      conn.dst_node_id += size_offset;
    }

  }
 
  // Connect the new start state to each operand's start state
  fa_transition start_transition_1(EPSILON, start_state_id);
  fa_transition start_transition_2(EPSILON, other_start);

  // new start state
  state_map.push_back({start_transition_1, start_transition_2});

  start_state_id = state_map.size() - 1;

  // Connect each operand's end state to the new end state
  unsigned new_final_state_id {static_cast<unsigned>(state_map.size())};
  fa_transition end_transition(EPSILON, new_final_state_id);

  state_map.at(final_state_id).push_front(end_transition);
  state_map.at(other->final_state_id + size_offset).push_front(end_transition);

  // new end state
  state_map.push_back({});

  final_state_id = new_final_state_id;
}

/*
 * Construct the NFA corresponding to the closure of the current NFA's
 * regular language
 */
void NFA::closure()
{
  unsigned new_start_state_id {static_cast<unsigned>(state_map.size())};
  unsigned new_final_state_id {static_cast<unsigned>(state_map.size()) + 1};

  // Connect final to start
  state_map.at(final_state_id).push_front({EPSILON, start_state_id});

  // Connect new_start to start
  state_map.push_back({ {EPSILON, start_state_id} });
 
  // Connect final to new_final
  state_map.push_back({});
  state_map.at(final_state_id).push_front({EPSILON, new_final_state_id});
  
  // Connect new_start to new_final
  state_map.at(new_start_state_id).push_front({EPSILON, new_final_state_id});

  start_state_id = new_start_state_id;
  final_state_id = new_final_state_id;
}

/*
 * NFA's transition function
 * If a transition over character c exists
 * between states x and y, delta(x, c) = y
 * returns -1 if no transition from x exists over c
 */
int NFA::delta(unsigned state, char character) const
{
  for (auto t : state_map.at(state))
  {
    if (t.character == character)
    {
      return t.dst_node_id;
    }
  }
  
  return -1;
}

/**
 * Returns the set of states reachable by 0 or more epsilon transitions
 */
unordered_set<unsigned> NFA::epsilon_closure(unsigned init_state) const
{
  unordered_set<unsigned> result;

  deque<unsigned> work_list {init_state};
  while (!work_list.empty())
  {
    auto current_state {work_list.front()};
    work_list.pop_front();

    result.emplace(current_state);
    for (auto t : state_map[current_state])
    {
      // if an epsilon transition connects to a new state, enqueue the new state
      if (t.character == EPSILON && result.find(t.dst_node_id) == result.end())
        work_list.push_back(t.dst_node_id);
    }
  }

  return result;
}
