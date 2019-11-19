/*
 * DFA_State implementation file
 */

#include <unordered_set>
#include <string>
#include <algorithm>
#include <iostream>

#include "DFA_State.h"

class DFA_State;

// Implement std::hash<DFA_State>
namespace std
{
  size_t hash<DFA_State>::operator()(const DFA_State& s) const
  {
    return std::hash<std::string>()(s.state);
  }
}

DFA_State::DFA_State(const std::unordered_set<unsigned>& set)
{
  std::vector<unsigned> v(set.begin(), set.end());
  std::sort(v.begin(), v.end());
  std::string s;

  for (auto x : v)
    s += std::to_string(x);

  state = s;
}
