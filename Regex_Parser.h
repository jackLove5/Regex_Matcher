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
    Regex_Parser() {}
    
    // Maps input strings to characters ("\+" maps to '+')
    static std::unordered_map<std::string, char> char_map;
    
    // The current location in the parse
    static size_t parse_location;
    
    // The input regex
    static char* input;

    /*
     * Fill the map that resolves input strings to characters
     */
    static std::unordered_map<std::string, char> init_char_map();
    
    /*
     * Turns the regular expression into an input stream represented by
     * a vector of strings
     */
    static std::vector<std::string> create_input_stream(const std::string& );

    /*
     * Returns true iff c is a special character
     */
    static bool is_special(char c);

   /*
    * Parser functions
    */ 
    static NFA* goal();
    static NFA* expr();
    static NFA* e_prime(NFA* nfa);
    static NFA* term();
    static NFA* t_prime(NFA* nfa);
    static NFA* closure();
    static NFA* factor();
    static NFA* f(NFA* nfa);
    static NFA* character();
    static NFA* bracket();
    static NFA* bracket_prime();
    static void element_list(std::unordered_set<char>& set);
    static void f_tail(std::unordered_set<char>& set);
    static void element(std::unordered_set<char>& set);
    static char element_prime(char c);
    static void first(std::unordered_set<char>& set);

  public:

    /*
     * Converts the regular expression to an NFA
     */
    static NFA* regex_to_nfa(const std::string& regex);
};

#endif
