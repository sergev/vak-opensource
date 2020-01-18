#include "grammar.tab.hh"

namespace Demo {

class Parser {
    // Counters.
    unsigned chars, words, lines, uppercase, lowercase;

    // Read next token from input.
    int get_next_token(Grammar::semantic_type *const lval,
                       Grammar::location_type *location);

public:
    // Invoke the parser.
    void parse();

    // Print the results.
    void print();

    // Allow grammar.yy to update our data.
    friend class Grammar;
};

} // namespace Demo
