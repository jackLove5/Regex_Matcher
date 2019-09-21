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
    static std::unordered_map<std::string, char> char_map;
    static size_t word_location;
    static std::vector<std::string> input_stream;
 
    static NFA* goal();
    static NFA* expr();
    static NFA* e_prime(NFA* nfa);
    static NFA* term();
    static NFA* t_prime(NFA* nfa);
    static NFA* closure();
    static NFA* factor();
    static NFA* f(NFA* nfa);
    static std::unordered_map<std::string, char> init_char_map();
    static std::vector<std::string> create_input_stream(const std::string& );
  public:
    static NFA* regex_to_nfa(const std::string& regex);
};

#endif
