/*
 * A regular expression matching program
 */

#include <iostream>
#include <exception>

#include "DFA.h"
#include "NFA.h"
#include "Regex_Parser.h"

using namespace std;

int main()
{
  // Prints the program's title
  void print_title();
  
//  print_title();
  
  string input_regex {""};
 
  // Main program loop:
  // Accept regular expressions from user until they enter "quit"
  while (true)
  {    
    cout << "Enter a regular expression. Type \"quit\" to quit: ";
    getline(cin, input_regex);

    // Parse the regular expression and create an NFA
    unique_ptr<NFA> nfa_pt;
    try
    {
      nfa_pt = Regex_Parser::regex_to_nfa(input_regex);
    }
    catch (std::runtime_error& e)
    {
      cerr << "Invalid Regex: " << e.what() << endl;
      input_regex.clear();
      continue; 
    }

    if (input_regex == "quit")
    {
      break;
    }

    // create minimized DFA
    DFA dfa{*nfa_pt};
    dfa.minimize();

    // Ask for strings for the DFA to accept
    string to_accept;
    cout << "Enter the strings to be accepted by \"" << input_regex;
    cout << "\" (one per line). Type \"quit\" to quit." << endl;
      
    getline(cin, to_accept);
    while (to_accept != "quit")
    {
      bool accepted {dfa.accept(to_accept)};

      if (accepted)
      {
        cout << "accepted" << endl;
      }
      else
      {
        cout << "not accepted" << endl;
      }
      
      getline(cin, to_accept);
    } 
  }
  
  return 0;
}

void print_title()
{
  cout << "Regex_Matcher - a regular expression matching program" << endl <<
    endl;
 
  cout << ". For this program, a regular expression is defined recursively as: "
    << endl;

  cout << "\t- a single character" << endl;
  cout << "\t- a bracket expression" << endl;
  cout << "\t- a regular expression in parentheses \"(Expr)\"" << endl;
  cout << "\t- the closure of a regular expression \"Expr*\"" << endl;
  cout << "\t- the concatenation of two regular expressions \"ExprExpr\"" <<
    endl; 
  cout << "\t- the alternation of two regular expressions \"Expr|Expr\"" <<
    endl << endl;


  cout << ". \"a single character\" means:" << endl;
  cout << "\t- ascii values 33-126 (\'!\'-\'~\')" << endl;
  cout << "\t- the following escape sequences: " << 
    "'\\|', '\\*', '\\(', '\\)', '\\[', '\\]', '\\s' (space), and '\\\\'" << 
    endl;
  cout << ". Literal blank characters in the regex are ignored." << 
   " Use the escape sequence '\\s' to include blanks." << endl << endl;
  cout << ". Other whitespace characters (tab, newline, etc) are invalid" <<
    endl << endl;
  cout << ". The interpretation of the regular expressions follows" <<
    " the following order of operations" << endl << endl;
  
  cout << "\t- ()" << endl;
  cout << "\t- *" << endl;
  cout << "\t- concatenation" << endl;
  cout << "\t- |" << endl << endl;
  cout << "Ex: \"a*\" matches \"\", \"a\", \"aa\"" <<
    " and does not match \"b\", \"aaaab\", \"aaabaaa\"" << endl << endl;

  cout << "Bracket Expressions:" << endl;
  cout << "A bracket expression is a list of characters enclosed " <<
    "by square brackets []" << endl;
  cout << "Ranges of characters can be included in a bracket expression" << 
    endl;
  cout << "Ex: [a-zJKL] matches all lowercase letters and";
  cout << " the uppercase letters J, K, and L" << endl << endl;

  cout << "If the first character of the bracket expression is \'^\', ";
  cout << "then the bracket expression will match all characters except ";
  cout << "the characters in the bracket expression" << endl;

  cout << "Metacharacters lose their meaning in bracket expressions." << endl;
  cout << "To include the literal ']', place it first in the list." << endl;
  cout << "To include the literal '-', place it last in the list." << endl;
  cout << "To include the literal '^', place it anywhere except first." << 
    endl;
  cout << endl;
}
