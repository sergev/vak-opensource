#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "parser.tab.hh"
#include "location.hh"

namespace Demo {

class Scanner : public yyFlexLexer{
public:
    Scanner(std::istream *in)
        : yyFlexLexer(in)
    {
    }

    virtual ~Scanner()
    {
    }

    virtual int yylex() { return 0; }

    int get_next_token(Demo::Parser::semantic_type *const lval,
                       Demo::Parser::location_type *location);
private:
    Demo::Parser::semantic_type *yylval = nullptr;
};

} /* end namespace Demo */
