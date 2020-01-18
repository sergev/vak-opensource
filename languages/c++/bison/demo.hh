#include "grammar.tab.hh"

namespace Demo {

int get_next_token(Demo::Grammar::semantic_type *const lval,
                   Demo::Grammar::location_type *location);

// Counters.
extern unsigned chars;
extern unsigned words;
extern unsigned lines;
extern unsigned uppercase;
extern unsigned lowercase;

} // namespace Demo
