#include "grammar.tab.hh"
#include "scanner.hh"

namespace Demo {

class Parser {
    Scanner scanner;
    int result;

public:
    // Invoke the parser.
    void parse();

    // Print the results.
    void print();

    // Allow grammar.yy to update our data.
    friend class Grammar;
};

} // namespace Demo
