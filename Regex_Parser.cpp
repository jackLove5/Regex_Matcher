/*
 * Regex_Parser implementation file
 */

#include <iostream>
#include <string>
#include <unordered_set>
#include <string.h>

#include "NFA.h"
#include "Regex_Parser.h"

using namespace std;

static const char ALPHABET_BEGIN {' '};
static const char ALPHABET_END {'~'};

size_t Regex_Parser::parse_location {0};

char* Regex_Parser::input;

NFA* Regex_Parser::regex_to_nfa(const std::string& regex)
{
  parse_location = 0;

  input = new char[regex.length() + 1];
  
  strcpy(input, regex.c_str());

  // Parse the regex
  NFA* result = goal();

  delete [] input;
  
  // Report results
  if (result != nullptr && parse_location == regex.length())
  {
    return result;
  }
  else
  {
    return nullptr;
  }
}

/*
 * Regular Expression Syntax Parser
 * Simultaneously validates the regex syntax and constructs
 * the resulting NFA
 */

// Goal -> Expr
NFA* Regex_Parser::goal()
{
  return expr();
}

// Expr -> TermE'
NFA* Regex_Parser::expr()
{
  auto nfa {term()};
  if (nfa != nullptr)
  {
    NFA* operand {e_prime(nfa)};
    if (operand != nullptr)
    {
      if (operand != nfa)
      {
        nfa->disjunction(operand);      
        delete operand;
      }
    }
    else
    {
      delete nfa;
      return nullptr;
    }
  }

  return nfa;
}

// E' -> |TermE'
// E' -> ""
NFA* Regex_Parser::e_prime(NFA* nfa)
{
  if (input[parse_location] == '|')
  {
    // E' -> |TermE'
    parse_location++;

    auto op1 = term();
    if (op1 == nullptr)
    {
      return nullptr;
    }

    auto op2 = e_prime(nfa);
    if (op2 == nullptr)
    {
      delete op1;
      return nullptr;
    }
    
    if (op2 != nfa)
    {
      op1->disjunction(op2);
      delete op2;
    }

    return op1;
  }
  else if (input[parse_location] == ')' || input[parse_location] == '\0')
  {
    // E' -> ""
    return nfa;
  }
  else
  {
    // error
    return nullptr;
  }
}

// Term -> closureT'
NFA* Regex_Parser::term()
{
  auto nfa {closure()};

  if (nfa == nullptr)
  {
    return nullptr;
  }

  NFA* operand = t_prime(nfa);
  if (operand == nullptr)
  {
    delete nfa;
    return nullptr;
  }

  // If we scanned a new closure, concatenate the operands
  if (operand != nfa)
  {
    nfa->concatenate(operand);
    delete operand;
  }

  return nfa;
}

/*
 * [, ], *, (, ), \, |
 */
bool Regex_Parser::is_special(char c)
{
  return c == '(' || c == ')' || c == '[' || 
    c == ']' || c == '*' || c == '|' || c == '\\';
}

// T' -> closureT'
// T' -> ""

// Returns pointer to new dfa if it scanned a new closure
// Returns nfa if it scanned a terminal symbol
// Returns nullptr if it encountered a syntax error
NFA* Regex_Parser::t_prime(NFA* nfa)
{
  char c = input[parse_location];
  if (c == '(' || c == '[' || c == '\\' ||
      (c <= ALPHABET_END && c >= ALPHABET_BEGIN && !is_special(c)))
  {
    NFA* op1 = closure();
    if (op1 == nullptr)
    {
      return nullptr;
    }

    NFA* op2 = t_prime(nfa);
    if (op2 == nullptr)
    {
      delete op1;
      return nullptr;
    }

    // Scanned a new closure. Concatenate the operands
    if (op2 != nfa)
    {
      op1->concatenate(op2);
      delete op2;
    }

    return op1;
  }
  else if (c == '|' || c == '\0' || c == ')')
  {
    return nfa;
  }
  else
  {
    return nullptr;
  }
}

// closure -> FactorF
NFA* Regex_Parser::closure()
{
  auto nfa {factor()};
  if (nfa == nullptr)
  {
    return nullptr;
  }

  return f(nfa);
}

// f -> *
// f -> ""
NFA* Regex_Parser::f(NFA* nfa)
{
  char c {input[parse_location]};

  if (c == '*')
  {
    nfa->closure();
    parse_location++;
    return nfa;
  }
  
  if (c == '(' || c == '|' || c == '\0' || c == '\\' || c == '[' ||
      c == ')' || (c <= ALPHABET_END && c >= ALPHABET_BEGIN && !is_special(c)))
  {
    return nfa;
  }
  else
  {
    return nullptr;
  }
}


// Factor -> (Expr)
// Factor -> character
// Factor -> bracket
NFA* Regex_Parser::factor()
{
  char c {input[parse_location]};

  if (c == '(')
  {
    parse_location++;
    NFA* ret {expr()};
    if (ret == nullptr)
    {
      return nullptr;
    }

    c = input[parse_location];
    if (c == ')')
    {
      parse_location++;
      return ret;
    }
    else
    {
      delete ret;
      return nullptr;
    }
  }
  else if (c == '\\' || 
      (c <= ALPHABET_END && c >= ALPHABET_BEGIN && !is_special(c)))
  {
    return character();
  }
  else if (c == '[')
  {
    return bracket();
  }
  else
  {
    return nullptr;
  }
}

// character -> non_escapable_ascii
// character -> \escapable_ascii            
NFA* Regex_Parser::character()
{
  char new_char = input[parse_location];
  if (input[parse_location] == '\\')
  {
    parse_location++;
    if (!is_special(input[parse_location]))
    {
      return nullptr;
    }
    else
    {
      new_char = input[parse_location];
    }
  }

  parse_location++;
  return new NFA(new_char);
}

// bracket -> [bracket_prime
NFA* Regex_Parser::bracket()
{
  parse_location++;
  return bracket_prime();
}

// bracket_prime -> element_list]
// bracket_prime -> ^element_list]
NFA* Regex_Parser::bracket_prime()
{
  NFA* ret = nullptr;
  unordered_set<char> set;

  bool complement = input[parse_location] == '^';
  if (complement)
  {
    parse_location++;
  }

  element_list(set);
  if (set.find('\0') != set.end() || input[parse_location] != ']')
  {
    return nullptr;
  }

  parse_location++;
  // Build an NFA from the characters in the bracket
  for (char c {ALPHABET_BEGIN}; c <= ALPHABET_END; c++)
  {
    if ((complement && set.find(c) == set.end()) || 
        (!complement && set.find(c) != set.end()))
    {
      if (ret == nullptr)
      {
        ret = new NFA(c);
      }
      else
      {
        NFA* op1 = new NFA(c);
        ret->disjunction(op1);
        delete op1;
      }
    }
  }
  
  return ret;
}

// element_list -> first f_tail
// element_list -> element
void Regex_Parser::element_list(unordered_set<char>& set)
{
  char c {input[parse_location]};
  if (c == ']')
  {
    first(set);
    f_tail(set);
  }
  else if (c >= ALPHABET_BEGIN && c <= ALPHABET_END)
  {
    element(set);
  }
}

// f_tail -> element
// f_tail -> ""
void Regex_Parser::f_tail(unordered_set<char>& set)
{
  char c {input[parse_location]};
  if (c >= ALPHABET_BEGIN && c <= ALPHABET_END && c != ']')
  {
    element(set);
  }
  else if (c == ']')
  {
    // f_tail -> ""
  }
  else
  {
    set.emplace('\0');
  }
}

// element -> ascii element_prime element
// element -> ""
void Regex_Parser::element(unordered_set<char>& set)
{
  char c {input[parse_location]};
  if (c >= ALPHABET_BEGIN && c <= ALPHABET_END && c != ']' && c != '-')
  {
    char start_char {c};
    parse_location++;
    char end_char {element_prime(start_char)};
    
    if (end_char == '\0' || end_char < start_char)
    {
      set.emplace('\0');
    }
    else
    {
      for (char c {start_char}; c <= end_char; c++)
      {
        set.emplace(c);
      }
    }

    element(set);
  }
  else if (c == ']')
  {
    // element -> ""
  }
  else
  {
    set.emplace('\0');
  }

}

// element_prime -> - ascii
// element_prime -> ""
char Regex_Parser::element_prime(char start)
{
  char c {input[parse_location]};

  if (c == '-')
  {
    parse_location++;
    char ret {input[parse_location]};
    
    if (ret >= ALPHABET_BEGIN && ret <= ALPHABET_END && ret != '-' && ret != ']')
    {
      parse_location++;
      return ret;
    }
    else
    {
      return '\0';
    }
  }
  else if (c >= ALPHABET_BEGIN && c <= ALPHABET_END)
  {
    // element -> ""
    return start;
  }
  else
  {
    return '\0';
  }
}

// first -> ]element_prime
void Regex_Parser::first(unordered_set<char>& set)
{
  parse_location++;
  char start_char {']'};
  char end_char {element_prime(start_char)};

  if (end_char != '\0' && end_char >= start_char)
  {
    for (char c {start_char}; c <= end_char; c++)
    {
      set.emplace(c);
    }
  }
  else
  {
    set.emplace('\0');
  }
}
