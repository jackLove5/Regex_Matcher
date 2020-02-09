#ifndef REGEX_PARSER
#define REGEX_PARSER

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>

/*
 * A class that implements a regular expression parser.
 * Parses the LL(1) regex grammar using a top-down
 * recursive decent parser.
 * Also constructs an equivalent NFA from the regex while
 * parsing.
 */
class Regex_Parser
{
  private:
    
    /*
     * Private constructor
     */
    Regex_Parser() {}

    /*
     * The current location in the parse
     */
    static size_t parse_location;

    /*
     * The input regex
     */
    static char* input;
    
    /*
     * Returns true iff c is a special character
     */
    static bool is_special(char c);
    
    /*
     * Returns true iff c is a escapable
     */
    static bool is_escapable(char c);

    /* 
     * Remove spaces from a string
     */
    static void remove_spaces(std::string& str);

   /*
    * Parser functions
    */ 
    static std::unique_ptr<NFA> goal();
    static std::unique_ptr<NFA> expr();
    static std::unique_ptr<NFA> e_prime();
    static std::unique_ptr<NFA> term();
    static std::unique_ptr<NFA> t_prime();
    static std::unique_ptr<NFA> closure();
    static std::unique_ptr<NFA> factor();
    static void f(std::unique_ptr<NFA>& nfa);
    static std::unique_ptr<NFA> character();
    static std::unique_ptr<NFA> bracket();
    static std::unique_ptr<NFA> bracket_prime();
    static void element_list(std::unordered_set<char>& set);
    static void begin(std::unordered_set<char>& set);
    static char b_prime();
    static void element(std::unordered_set<char>& set);
    static char element_prime(char start);
    static void more(std::unordered_set<char>& set);

  public:

    /*
     * Converts the regular expression to an NFA
     */
    static std::unique_ptr<NFA> regex_to_nfa(const std::string& regex);
};

#endif
