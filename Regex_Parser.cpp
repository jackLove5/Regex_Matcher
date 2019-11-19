/*
 * Regex_Parser implementation file
 */

#include <iostream>

#include "NFA.h"
#include "Regex_Parser.h"

using namespace std;

static const char ALPHABET_START {' '};
static const char ALPHABET_END {'~'};

std::unordered_map<std::string, char> Regex_Parser::char_map {init_char_map()};

size_t Regex_Parser::word_location {0};

std::vector<std::string> Regex_Parser::input_stream {};

std::unordered_map<std::string, char> Regex_Parser::init_char_map()
{
  /*
   * Map single character strings to the correct character ("c" maps to 'c')
   * Map escape sequences to the correct character ("\+" maps to '+')
   */
  std::unordered_map<std::string, char> result;
  for (char c = ALPHABET_START; c <= ALPHABET_END; c++)
  {
    std::string s {""};
    if (c == '+' || c == '*' || c == '(' || c == ')' || c == '\\')
      s += '\\';
    
    s += c;
    result[s] = c;
  }

  return result;
}

vector<std::string> Regex_Parser::create_input_stream(const std::string& regex)
{
  vector<std::string> input_stream;
  size_t i {0};
  for (; i < regex.size() - 1; i++)
  {
    string s {""};

    s += regex[i];
    if (regex[i] == '\\')
    {
      s += regex[i+1];
      i++;
    }

    if (char_map.find(s) == char_map.end() && s != "(" && s != ")" && s != "+"
        && s != "*")
    {
      cerr << "fail at character\"" << s << "\"" << endl;
      return {{"eof"}};
    }
    else
    {
      input_stream.push_back(s);
    }
  }

  if (i == regex.size() - 1)
  {
    std::string s {""};
    s += regex[i];

    if (char_map.find(s) == char_map.end() && s != "(" && s != ")" && s != "+"
      && s != "*")
    {
      cerr << "fail at character\"" << s << "\"" << endl;
      return {{"eof"}};
    }
    else
    {
      input_stream.push_back(s);
    }
  }

  input_stream.push_back("eof");
  return input_stream;
}

NFA* Regex_Parser::regex_to_nfa(const std::string& regex)
{

  word_location = 0;
  
  // Turn the regex into a vector of strings
  input_stream = create_input_stream(regex); 

  // Parse the regex
  NFA* result = goal();

  // Report results
  if (result != nullptr && input_stream.at(word_location) == "eof")
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

// E' -> +TermE'
// E' -> ""
NFA* Regex_Parser::e_prime(NFA* nfa)
{
  auto str {input_stream.at(word_location)};
  if (str == "+")
  {
    // E' -> +TermE'
    word_location++;
    auto op1 = term();
    auto op2 = e_prime(nfa);
    if (op1 == nullptr || op2 == nullptr)
    {
      if (op1 != nullptr)
        delete op1;
      if (op2 != nullptr && op2 != nfa)
        delete op2;
      return nullptr;
    }
    
    if (op2 != nfa)
    {
      op1->disjunction(op2);
      delete op2;
    }

    return op1;
  }
  else if (str == ")" || str == "eof")
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
  if (nfa != nullptr)
  {
    NFA* operand = t_prime(nfa);
    if (operand != nullptr)
    {
      // If it scanned a new closure, concatenate them
      if (operand != nfa)
      {
        nfa->concatenate(operand);
        delete operand;
      }  
    }
    else
    {
      delete nfa;
      nfa = nullptr;
    }
  }

  return nfa;
}

// T' -> closureT'
// T' -> ""

// Returns pointer to new dfa if it scanned a new closure
// Returns nfa if it scanned a terminal symbol
// Returns nullptr if it encountered a syntax error
NFA* Regex_Parser::t_prime(NFA* nfa)
{
  auto str {input_stream.at(word_location)};
  if (str == "(" || char_map.find(str) != char_map.end())
  {
    NFA* op1 = closure();
    NFA* op2 = t_prime(nfa);
    if (op1 == nullptr || op2 == nullptr)
    {
      if (op1 != nullptr)
        delete op1;
      if (op2 != nullptr && op2 != nfa)
        delete op2;

      return nullptr;
    }

    if (op2 != nfa)
    {
      op1->concatenate(op2);
      delete op2;
    }
    return op1;
  }
  else if (str == "+" || str == "eof" || str == ")")
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
  if (nfa != nullptr)
  {
    return f(nfa);
  }
  else
  {
    delete nfa;
    return nullptr;
  }
}

// f -> *
// f -> ""
NFA* Regex_Parser::f(NFA* nfa)
{
  auto str {input_stream.at(word_location)};

  if (str == "*")
  {
    nfa->closure();
    word_location++;
    return nfa;
  }
  
  if (str == "(" || str == "+" || str == "eof" ||
      str == ")" || char_map.find(str) != char_map.end())
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
NFA* Regex_Parser::factor()
{
  auto str {input_stream.at(word_location)};
  if (str == "(")
  {
    word_location++;
    NFA* ret {expr()};
    if (ret != nullptr)
    {
      str = input_stream.at(word_location);
      if (str == ")")
      {
        word_location++;
        return ret;
      }
      else
      {
        delete ret;
        return nullptr;
      }
    }
    else
    {
      return nullptr;
    }
  }
  else
  {
    if (char_map.find(str) != char_map.end())
    {
      word_location++;
      // Construct trivial DFA from a single character
      return new NFA(char_map[str]);
    }
    else
    {
      return nullptr;
    }
  }
}
