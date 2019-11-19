#ifndef DFA_STATE_H
#define DFA_STATE_H

#include <algorithm>
#include <string>
#include <iostream>
#include <vector>

class DFA_State;

// declare std::hash<DFA_State>
namespace std
{
  template<>
  struct hash<DFA_State>
  {
    public:
      size_t operator()(const DFA_State& s) const;
  };
}

/*
 * A class representing a DFA State
 */
class DFA_State
{
  private:

    // The state's id implemented as a sorted string
    std::string state;
  
  public:
    
    /*
     * Default constructor
     */
    DFA_State() { state = ""; };
    
    /*
     * Construct a DFA_State from a set of nfa states
     */
    DFA_State(const std::unordered_set<unsigned>& set);

    /*
     * Test for equality
     */
    bool operator==(const DFA_State& other) const
    {
      return state == other.state;
    }

    /*
     * Test for inequality
     */
    bool operator!=(const DFA_State& other) const
    {
      return state != other.state;
    }

    /*
     * Output state to stream
     */
    friend std::ostream& operator<<(std::ostream& stream, const DFA_State& s)
    {
      stream << s.state;
      return stream;
    }

    // Give the hash function access to private members
    friend size_t std::hash<DFA_State>::operator()(const DFA_State& s) const;
};

#endif
