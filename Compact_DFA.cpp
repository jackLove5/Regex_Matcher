#include "Compact_DFA.h"
#include <iostream>

/*
 * Compact_DFA implementation file
 */

using namespace std;

static const char ALPHABET_START {' '};
static const char ALPHABET_END {'~'};

/*
 * Checks if a given string can be accepted by the DFA
 * returns true iff the DFA recognizes the input string
 */
bool Compact_DFA::accept(const string& to_accept)
{
  unsigned curr_state {start_state_id};

  // Traverse the DFA
  for (auto c : to_accept)
  {
    if (delta(curr_state, c) != -1)
    {
      curr_state = static_cast<unsigned>(delta(curr_state, c));
    }
    else
    {
      // Reached error state
      return false;
    }
  }

  // Did not finish at an accepting state
  return accepted_set.find(curr_state) != accepted_set.end();
}

/*
 * Minimizes the DFA
 */
void Compact_DFA::minimize()
{
  // Do Hopcroft's algorithm and get the resulting set partition
  auto set_partition {hopcroft()};

  // Initialize new DFA's state map
  vector<list<fa_transition>> new_state_map;
  for (auto set : set_partition)
  {
    new_state_map.push_back({});
  }
  
  unsigned new_start_state_id {0};
  unordered_set<unsigned> new_accepted_set;
  
  /*
   * Fill transitions
   * Examines all transitions in the old DFA exactly once
   */
  for (size_t i{0}; i < set_partition.size(); i++)
  {
    for (auto element: set_partition.at(i))
    {
      // Add to the new DFA's accepted set if needed
      if (accepted_set.find(element) != accepted_set.end())
      {
        new_accepted_set.emplace(i);
      }

      // Update the new DFA's starting state id if needed
      if (element == start_state_id)
      {
        new_start_state_id = static_cast<unsigned>(i);
      }

      // Add transitions to the new DFA
      for (auto t : state_map.at(element))
      {
        unsigned dst_state = static_cast<unsigned>(
            get_loc(set_partition, t.dst_node_id));

        if (delta(new_state_map, static_cast<unsigned>(i), t.character) == -1)
        {
          new_state_map.at(i).push_front({t.character, dst_state});
        }
      }
    }
  }

  // Replace all fields
  start_state_id = new_start_state_id;
  state_map = new_state_map;
  accepted_set = new_accepted_set;
}

/*
 * Perform Hopcroft's algorithm
 * returns the resulting set partition
 */
vector<unordered_set<unsigned>> Compact_DFA::hopcroft()
{
  /*
   * Initialize the set partition with set of accepting states
   * and set of not accepted states.
   */
  unordered_set<unsigned> accepting_states;
  unordered_set<unsigned> not_accepting_states;

  for (size_t i {0}; i < state_map.size(); i++)
  {
    if (accepted_set.find(static_cast<unsigned>(i)) == accepted_set.end())
    {
      not_accepting_states.emplace(i);
    }
    else
    {
      accepting_states.emplace(i);
    }
  }

  vector<unordered_set<unsigned>> set_partition;

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
  vector<unordered_set<unsigned>> copy;
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

/*
 * Checks if all the states in a particular subset have
 * the same behavior in response to all characters in the alphabet.
 * If all the elements in the subset don't have the same behavior,
 * the subset gets split
 */
void Compact_DFA::split(vector<unordered_set<unsigned>>& set_partition,
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
    int behavior = get_loc(set_partition, delta(*it, c));
    it++;
    
    for (; it != subset.end(); it++)
    {
      
      int new_behavior {get_loc(set_partition, delta(*it, c))};
      
      /*
       * If two states in a set have different behavior,
       * split the set around character c
       */
      if (new_behavior != behavior)
      {
        // Split the set around c
        unordered_set<unsigned> split_set;
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

/*
 * Returns the index of the set containing the value search
 */
int Compact_DFA::get_loc(const vector<unordered_set<unsigned>>& set_partition,
    int search)
{
  for (size_t i{0}; i < set_partition.size(); i++)
  {
    if (set_partition.at(i).find(search) != set_partition.at(i).end())
      return static_cast<int>(i);
  }

  return -1;
}

/*
 * DFA's transition function
 * If a transition over character c exists
 * between states x and y, delta(x, c) = y
 * returns -1 if no transition from x exists over c
 */
int Compact_DFA::delta(unsigned state, char character)
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

/*
 * Overload that accepts any DFA's state map
 */
int Compact_DFA::delta(const vector<list<fa_transition>>& s_map,
    unsigned state, char character)
{
  for (auto t : s_map.at(state))
  {
    if (t.character == character)
    {
      return t.dst_node_id;
    }
  }

  return -1;
}

/*
 * Print a description of the DFA
 */
void Compact_DFA::print()
{
  for (unsigned i = 0; i < state_map.size(); i++)
  {
    cout << "STATE " << i << ": ";
    for (auto conn : state_map[i])
      cout << '(' << conn.character << ", " << conn.dst_node_id << ") ";
    cout << endl;
    
    if (i == start_state_id)
      cout << "START STATE" << endl;

    if (accepted_set.find(i) != accepted_set.end())
    {
      cout << "ACCEPTING STATE " << endl;
    }

    cout << endl;
  }
}
