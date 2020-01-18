#ifndef yyFlexLexer
#   define yyFlexLexer Scanner_FlexLexer
#   include <FlexLexer.h>
#endif

class Scanner : public yyFlexLexer {
public:
    // Types of input tokens.
    enum class Token {
        END,
        NUMBER,
        NAME,
        STRING,
    };

    Scanner() {}
    virtual ~Scanner() {}

    // This routine scans the input and returns a next token.
    Token get_next_token();
};
