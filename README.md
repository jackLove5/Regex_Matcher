# Regex_Matcher
A regular expression matching program

This program accepts regular expressions provided in a particular format.
The program then prompts the user for strings and attempts to match them to the provided regular expression.


This project was inspired by chapters 2 and 3 of "Engineering a Compiler" (Cooper, Torczon).
These chapters present common algorithms/techniques used in scanning and parsing. I would eventually like to use these classes to make a full-blown scanner generator for a small compiler project.

## Usage Notes/Specifics:
* In this program, a regular expression is defined as
  - A single character (ASCII values 33-126 and escape sequences "\\(", "\\)", "\\\*", "\\|", "\\[", "\\]", "\\s" (space), and "\\\\")
  - A bracket expression
  - A regex in parentheses, (Expr)
  - The concatenation of two regexs, ExprExpr
  - The closure of a regex, Expr\*
  - The disjunction of two regexs, Expr|Expr
* Literal blank characters in the regex are ignore. Use the escape sequence "\\s" to include blanks.
* Other whitespace characters (tab, newline, etc) are invalid.  

* Order of precedence for regex operators (from highest to lowest)
  - parentheses
  - closure
  - concatenation
  - alternation
* Ex: the regular expression "a|bc\*" will match "a", "b", "bc", "bcc", etc

* A bracket expression is a list of characters enclosed by square brackets
  - Ranges of characters can be included in a bracket expression
  - Ex: [a-zJKL] will match all lowercase letters and the uppercase letters J, K, and L
  - If the first character of the bracket expression is ^, then the expression will match all characters not included in the bracket expression
  - Metacharacters lose their meaning in bracket expressions
  - To include a literal ], place it first in the list
  - To include a literal -, place it last in the list
  - To include a literal ^, place it anywhere except first

## Program flow Overview:
1. Parses the regular expression using a recursive descent parser
2. Builds an NFA from the regex during the parse
3. Builds a DFA from the NFA using subset construction
4. Minimizes the DFA using Hopcroft's algorithm
5. Validates user input using the minimized DFA
