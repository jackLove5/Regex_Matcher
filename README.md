# Regex_Matcher
A regular expression matching program

This program accepts regular expressions provided in a specified format.
The program then prompts the user for strings and attempts to match them to the provided regular expression.


This project was inspired by chapters 2 and 3 of "Engineering a Compiler" (Cooper, Torczon).
These chapters present common algorithms/techniques used in scanning and parsing. I would eventually like to use these classes to make a full-blown scanner generator for a small compiler project.

This is my first attempt at implementing finite-state machines in C++.
Consequently, I made some design decisions that felt pretty awkward such as using actual unordered_sets to represent DFA states during the subset construction of the DFA.
## Usage Notes/Specifics:
* In this program, a regular expression is defined as
  - A single character (ASCII values 32-126 and escape sequences "\\(", "\\)", "\\\*", "\\+", "\\\\")
  - A regex in parentheses, (Expr)
  - The concatenation of two regexs, ExprExpr
  - The closure of a regex, Expr\*
  - The disjunction of two regexs, Expr+Expr
* Order of precedence for regex operators (from highest to lowest)
  - ()
  - *
  - concatenation
  - +
* Ex: the regular expression "a+bc\*" will match "a", "b", "bc", "bcc", etc
* Note: Avoid adding spaces to the regular expression unless you would like them to be interpreted
* Ex: the regular expression " \*" will match "", " ", "  ", etc

## Program flow Overview:
1. Parses the regular expression using a hand coded, top-down, recursive decent parser
2. Builds an NFA from the regex during the parse
3. Builds a DFA from the NFA using subset construction
4. Minimizes the DFA using Hopcroft's algorithm
5. Validates user input using the minimized DFA
