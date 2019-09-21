#include <iostream>

#include "Compact_DFA.h"
#include "NFA.h"
#include "Regex_Parser.h"
#include "DFA_Table.h"

/*
 * A regular expression matching program
 */

using namespace std;

int main()
{
  void print_title();
  
  print_title();
  
  string input_regex;
  getline(cin, input_regex);

  while (input_regex != "quit")
  {
    // Parse the regular expression and create an NFA
    NFA* nfa_pt = Regex_Parser::regex_to_nfa(input_regex);

    if (nfa_pt == nullptr)
    {
      // Parsing failed
      cerr << "Error. Invalid regex format" << endl;
    }
    else
    {
      // create minimized DFA
      DFA_Table table(*nfa_pt);
      Compact_DFA dfa{table.get_compact_dfa(*nfa_pt)};
      dfa.minimize();

      // Print the resulting minimized DFA
      // cout << "Minimized DFA:" << endl;
      // dfa.print();

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

      delete nfa_pt;
    }

    cout << "Enter a regular expression. Type \"quit\" to quit: ";
    getline(cin, input_regex);
  }
  
  return 0;
}

void print_title()
{
  cout << "Regex_Matcher, a regular expression matching program" << endl <<
    endl;
 
  cout << ". For this program, a regular expression is defined recursively as: "
    << endl;

  cout << "\t- a single character" << endl;
  cout << "\t- a regular expression in parentheses \"(Expr)\"" << endl;
  cout << "\t- the closure of a regular expression \"Expr*\"" << endl;
  cout << "\t- the concatenation of two regular expressions \"ExprExpr\"" <<
    endl;
  
  cout << "\t- the disjunction of two regular expressions \"Expr+Expr\"" <<
    endl << endl;

  cout << ". \"a single character\" means:" << endl;
  cout << "\t- any printable ascii character (including spaces)" << endl;
  cout << "\t- the following escape sequences: " << 
    "\'\\+\', \'\\*\', \'\\(\',\'\\)\', and \'\\\\\'" << endl << endl;
  
  cout << ". The interpretation of the regular expressions follows" <<
    " the following order of operations" << endl;
  
  cout << "\t- ()" << endl;
  cout << "\t- *" << endl;
  cout << "\t- concatenation" << endl;
  cout << "\t- +" << endl << endl;
  cout << "Ex: \"a*\" matches \"\", \"a\", \"aa\"" <<
    " and does not match \"b\", \"aaaab\", \"aaabaaa\"" << endl;

  cout << "Ex: \"(c+C)(\\+\\+)\\+*\" matches \"c++\", \"C+++\", \"c++++\"" << 
    " and does not match \"c+\", \"c\", \"C#\"" << endl << endl;

  cout << "Enter a regular expression. Type \"quit\" to quit: ";

}
