#ifndef DFA_TABLE
#define DFA_TABLE
#include <unordered_set>
#include <unordered_map>

#include "NFA.h"
#include "Compact_DFA.h"

/*
 * Boost's hash combine function.
 */
template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

/*
 * Generate hash values for unordered_sets
 */
struct Set_Hash
{
  size_t operator() (const std::unordered_set<unsigned>& set) const
  {
    size_t seed = 0;
    for (auto x: set)
      hash_combine(seed, x);

    return seed;
  }
};

// DFA states represented as sets of NFA states
typedef std::unordered_set<unsigned> DFA_Table_State;

/*
 *  Maps DFA states and characters to destination DFA states
 *  A transition exists from state x to state y over character c 
 *  iff DFA_Table[x][c] == y
 */
typedef std::unordered_map<DFA_Table_State,
               std::unordered_map<char, DFA_Table_State>, Set_Hash> T_Table;

/*
 * Represents a DFA.
 * Implemented using an adjacency matrix.
 * Used for converting a NFAs to DFAs
 */
class DFA_Table
{
  private:
    T_Table table;

  public:
    DFA_Table(const NFA& nfa);
    Compact_DFA get_compact_dfa(const NFA& nfa);
};

#endif
