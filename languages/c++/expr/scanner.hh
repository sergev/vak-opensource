#ifndef yyFlexLexer
#   define yyFlexLexer Scanner_FlexLexer
#   include <FlexLexer.h>
#endif

namespace Demo {

class Scanner : public yyFlexLexer {
public:
    Grammar::token::yytokentype get_next_token(Grammar::semantic_type *const lval,
                                               Grammar::location_type *loc);
};

} // namespace Demo
