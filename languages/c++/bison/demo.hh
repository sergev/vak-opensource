#include "grammar.tab.hh"

namespace Demo {

void add_upper();
void add_lower();
void add_word(const std::string &word);
void add_newline();
void add_char();

int get_next_token(Demo::Grammar::semantic_type *const lval,
                   Demo::Grammar::location_type *location);

} // namespace Demo
