/*
 * Regex_Parser implementation file
 */

#include <iostream>
#include <string>
#include <unordered_set>
#include <algorithm>
#include <memory>
#include <exception>
#include <sstream>
#include <string.h>

#include "NFA.h"
#include "Regex_Parser.h"

using namespace std;

static const char ALPHABET_BEGIN {' '};
static const char ALPHABET_END {'~'};

size_t Regex_Parser::parse_location {0};

char* Regex_Parser::input;

unique_ptr<NFA> Regex_Parser::regex_to_nfa(const std::string& regex)
{
  parse_location = 0;

  auto cpy {regex};
  remove_spaces(cpy);
  input = new char[cpy.length() + 1];
  
  strcpy(input, cpy.c_str());

  // Parse the regex
  unique_ptr<NFA> result = goal();

  delete [] input;
  return result;
}

void Regex_Parser::remove_spaces(string& str)
{
  auto part_end {std::stable_partition(str.begin(), str.end(), 
      [](char c){ return c != ' ';})};

  str = string(str.begin(), part_end);
}

/*
 * Regular Expression Syntax Parser
 * Simultaneously validates the regex syntax and constructs
 * the resulting NFA
 */

// Goal -> Expr
unique_ptr<NFA> Regex_Parser::goal()
{
  return expr();
}

// Expr -> TermE'
unique_ptr<NFA> Regex_Parser::expr()
{
  auto nfa {term()};
  auto operand {e_prime()};
  if (operand != nullptr)
  {
    // Scanned two terms. Take the alternation of the two operands
    nfa->disjunction(operand);
  }

  return nfa;
}

// E' -> |TermE'
// E' -> ""
unique_ptr<NFA> Regex_Parser::e_prime()
{
  if (input[parse_location] == '|')
  {
    // E' -> |TermE'
    parse_location++;

    auto op1 = term();
    auto op2 = e_prime();
    if (op2 != nullptr)
    {
      // Scanned two terms. Take the alternation of the two operands
      op1->disjunction(op2);
    }

    return op1;
  }
  else
  {
    // E' -> ""
    return nullptr;
  }
}

// Term -> closureT'
unique_ptr<NFA> Regex_Parser::term()
{
  auto nfa {closure()};
  unique_ptr<NFA> operand = t_prime();
  if (operand != nullptr)
  {
    // Scanned another closure. Concatenate the operands
    nfa->concatenate(operand);
  }

  return nfa;
}

/*
 * Special characters: [, ], *, (, ), \, |
 */
bool Regex_Parser::is_special(char c)
{
  return c == '(' || c == ')' || c == '[' || 
    c == ']' || c == '*' || c == '|' || c == '\\';
}


bool Regex_Parser::is_escapable(char c)
{
  return is_special(c) || c == 's';
}

// T' -> closureT'
// T' -> ""
unique_ptr<NFA> Regex_Parser::t_prime()
{
  char c = input[parse_location];
  if (c == '(' || c == '[' || c == '\\' ||
      (c <= ALPHABET_END && c >= ALPHABET_BEGIN && !is_special(c)))
  {
    unique_ptr<NFA> op1 = closure();
    unique_ptr<NFA> op2 = t_prime();

    if (op2 != nullptr)
    {
      // Scanned another closure. Concatenate the operands
      op1->concatenate(op2);
    }

    return op1;
  }
  else if (c == '|' || c == '\0' || c == ')')
  {
    // T' -> ""
    return nullptr;
  }
  else
  {
    string msg = string("Invalid character ") + input[parse_location];
    throw std::runtime_error(msg);
  }
}

// closure -> FactorF
unique_ptr<NFA> Regex_Parser::closure()
{
  auto nfa {factor()};
  f(nfa);
  return nfa;
}

// Factor -> (Expr)
// Factor -> character
// Factor -> bracket
unique_ptr<NFA> Regex_Parser::factor()
{
  char c {input[parse_location]};

  if (c == '(')
  {
    parse_location++;
    unique_ptr<NFA> ret {expr()};

    c = input[parse_location];
    if (c == ')')
    {
      parse_location++;
      return ret;
    }
    else
    {
     throw std::runtime_error("Expected closing \')\'");
    }
  }
  else if (c == '[')
  {
    return bracket();
  }
  else
  {
    return character();
  }
}

// f -> *
// f -> ""
void Regex_Parser::f(unique_ptr<NFA>& nfa)
{
  char c {input[parse_location]};
  if (c == '*')
  {
    nfa->closure();
    parse_location++;
  }
}

// character -> non_escapable_ascii
// character -> \escapable_ascii            
unique_ptr<NFA> Regex_Parser::character()
{
  char new_char = input[parse_location];
  if (new_char == '\\')
  {
    parse_location++;
    char esc_char {input[parse_location]};
    if (!is_escapable(esc_char))
    {
      string msg = string("Invalid escape character ") + esc_char; 
      throw std::runtime_error(msg);
    }
    else
    {
      if (is_special(esc_char))
      {
        // Character to be escaped is a metacharacter ex: \* -> '*'
        new_char = esc_char;
      }
      else
      {
        // Character to be escaped is not a metacharacter ex: \s -> ' '
        switch (esc_char)
        {
          case 's':
            new_char = ' ';
          break;
    
          default:
            new_char = esc_char;
        }
      }
    }
  }
  else if (new_char > ALPHABET_END || new_char < ALPHABET_BEGIN ||
      is_special(new_char))
  {
    string msg = string("Invalid \'") + new_char + string("\' character");
    throw std::runtime_error(msg);
  }

  parse_location++;
  return std::make_unique<NFA>(new_char);
}

// bracket -> [bracket_prime
unique_ptr<NFA> Regex_Parser::bracket()
{
  parse_location++;
  return bracket_prime();
}

// bracket_prime -> element_list]
// bracket_prime -> ^element_list]
unique_ptr<NFA> Regex_Parser::bracket_prime()
{
  unique_ptr<NFA> ret = nullptr;
  unordered_set<char> set;

  bool complement = input[parse_location] == '^';
  if (complement)
  {
    parse_location++;
  }

  element_list(set);
  if (input[parse_location] != ']')
  {
    throw std::runtime_error("Expected closing \']\'");
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
        ret = std::make_unique<NFA>(c);
      }
      else
      {
        unique_ptr<NFA> op1 = std::make_unique<NFA>(c);
        ret->disjunction(op1);
      }
    }
  }
 
  return ret;
}

// element_list -> begin more
void Regex_Parser::element_list(unordered_set<char>& set)
{
  begin(set);
  more(set);
}

// begin -> ]b_prime
// begin -> element
void Regex_Parser::begin(unordered_set<char>& set)
{
  char c {input[parse_location]};
  if (c == ']')
  {
    parse_location++;
    
    char end {b_prime()};
    for (; c <= end; c++)
    {
      set.emplace(c);
    }
  }
  else
  {
    element(set);
  }
}

// b_prime -> - ascii
// b_prime -> ""
char Regex_Parser::b_prime()
{
  if (input[parse_location] == '-')
  {
    parse_location++;
    char end {input[parse_location]};
    
    if (end < ']' || end > ALPHABET_END)
    {
      string msg = string("Invalid range ]-") + end;
      throw std::runtime_error(msg);
    }
    else
    {
      return end;
    }
  }
  else
  {
    // b_prime -> ""
    return ']';
  } 
}

// element -> ascii element_prime
// element -> -]
void Regex_Parser::element(unordered_set<char>& set)
{
  char c {input[parse_location]};
  if (c >= ALPHABET_BEGIN && c <= ALPHABET_END && c != '-' && c != ']')
  {
    parse_location++;
    char end {element_prime(c)};
    for (; c <= end; c++)
    {
      set.emplace(c);
    }
  }
  else if (c == '-')
  {
    if (input[parse_location + 1] == ']')
    {
      parse_location++;
      set.emplace('-');
    }
    else
    {
      throw std::runtime_error("Invalid \'-\' placement in bracket expression");
    }
  }
  else
  {
    throw std::runtime_error("Expected bracket element");
  }
}

// element_prime -> - ascii
// element_prime -> - -]
// element_prime -> ""
char Regex_Parser::element_prime(char start)
{
  char c {input[parse_location]};
  if (c == '-')
  {
    parse_location++;
    char end {input[parse_location]};
    parse_location++;
    
    if (end == '-' && input[parse_location] != ']')
    {
      throw std::runtime_error("Invalid \'-\' placement in bracket expression");
    }
    else if (end >= start && end <= ALPHABET_END && end != ']')
    {
      return end;
    }
    else
    {
      string msg = string("Invalid range ") + start + "-" + end;
      throw std::runtime_error(msg);
    }
  }
  else
  {
    // element_prime -> ""
    return start;
  }
}

// more -> element more
// more -> ""
void Regex_Parser::more(unordered_set<char>& set)
{
  char c {input[parse_location]};
  if (c == ']')
  {
    // more -> ""
  }
  else
  {
    element(set);
    more(set);
  }
}
