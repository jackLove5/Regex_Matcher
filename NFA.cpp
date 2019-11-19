/*
 * NFA implementation file
 */

#include <deque>
#include <climits>

#include "NFA.h"
#include "NFA_Transition.h"

using namespace std;

const char NFA::EPSILON {-1};
const unsigned NFA::ERROR {UINT_MAX};

NFA::NFA(char c)
{
  start_state_id = 0;
  NFA_Transition transition(c, 1);
  
  // Record transition from start state
  state_map.push_back({transition});
  
  // Final state has no outgoing transitions 
  state_map.push_back({});
  
  final_state_id = transition.dst_node_id;
}

void NFA::concatenate(const NFA* other)
{
  unsigned size_offset {static_cast<unsigned>(state_map.size())};
  unsigned other_start {other->start_state_id + size_offset};

  /* Create a new epsilon transition from the final state 
   * to other's first state */
  NFA_Transition transition(EPSILON, other_start);
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
  NFA_Transition start_transition_1(EPSILON, start_state_id);
  NFA_Transition start_transition_2(EPSILON, other_start);

  // new start state
  state_map.push_back({start_transition_1, start_transition_2});

  start_state_id = state_map.size() - 1;

  // Connect each operand's end state to the new end state
  unsigned new_final_state_id {static_cast<unsigned>(state_map.size())};
  NFA_Transition end_transition(EPSILON, new_final_state_id);

  state_map.at(final_state_id).push_front(end_transition);
  state_map.at(other->final_state_id + size_offset).push_front(end_transition);

  // new end state
  state_map.push_back({});

  final_state_id = new_final_state_id;
}

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

unsigned NFA::delta(unsigned state, char character) const
{
  for (auto t : state_map.at(state))
  {
    if (t.character == character)
    {
      return t.dst_node_id;
    }
  }
  
  return ERROR;
}

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
