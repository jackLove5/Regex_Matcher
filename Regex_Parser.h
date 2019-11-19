#ifndef REGEX_PARSER
#define REGEX_PARSER

#include <unordered_map>
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
    static size_t word_location;
    
    // The stream of input strings
    static std::vector<std::string> input_stream;

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
 
  public:

    /*
     * Converts the regular expression to an NFA
     */
    static NFA* regex_to_nfa(const std::string& regex);
};

#endif
