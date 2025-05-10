#include "scanner.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global variables
static FILE *input_file;
static char yytext[1024]; // Buffer for current lexeme
static int yyleng = 0;    // Length of current lexeme
static int next_char;     // Lookahead character

// Function prototypes
static void consume_char(void);
static void unget_char(void);
static int is_keyword(const char *str);
static int check_type(void);
static void skip_whitespace(void);
static void skip_comment(void);
static int scan_identifier(void);
static int scan_number(void);
static int scan_string(void);
static int scan_operator(void);

// Initialize scanner with input file
void init_scanner(FILE *input)
{
    input_file = input;
    yyleng     = 0;
    yytext[0]  = '\0';
    next_char  = input_file ? fgetc(input_file) : EOF;
}

// Main lexer function
int yylex(void)
{
    if (next_char == EOF) {
        return 0; // End of input
    }

    skip_whitespace();

    if (next_char == EOF) {
        return 0;
    }

    yyleng    = 0;
    yytext[0] = '\0';

    // Check for comments
    if (next_char == '/') {
        consume_char();
        if (next_char == '*') {
            skip_comment();
            return yylex(); // Recurse after skipping comment
        } else if (next_char == '/') {
            while (next_char != '\n' && next_char != EOF) {
                consume_char();
            }
            return yylex(); // Recurse after skipping line comment
        } else {
            unget_char();
            next_char = '/';
        }
    }

    // Scan tokens
    if (isalpha(next_char) || next_char == '_') {
        return scan_identifier();
    } else if (isdigit(next_char) || (next_char == '0' && (next_char == 'x' || next_char == 'X'))) {
        return scan_number();
    } else if (next_char == '"' || next_char == 'L' || next_char == 'u' || next_char == 'U') {
        return scan_string();
    } else {
        return scan_operator();
    }
}

// Consume a character and add to yytext
static void consume_char(void)
{
    if (yyleng < sizeof(yytext) - 1) {
        yytext[yyleng++] = next_char;
        yytext[yyleng]   = '\0';
    }
    next_char = fgetc(input_file);
}

// Push back the current character
static void unget_char(void)
{
    if (next_char != EOF) {
        ungetc(next_char, input_file);
        if (yyleng > 0) {
            yyleng--;
            yytext[yyleng] = '\0';
        }
    }
}

// Check if yytext is a keyword
static int is_keyword(const char *str)
{
    static const struct {
        const char *name;
        int token;
    } keywords[] = { { "auto", AUTO },
                     { "break", BREAK },
                     { "case", CASE },
                     { "char", CHAR },
                     { "const", CONST },
                     { "continue", CONTINUE },
                     { "default", DEFAULT },
                     { "do", DO },
                     { "double", DOUBLE },
                     { "else", ELSE },
                     { "enum", ENUM },
                     { "extern", EXTERN },
                     { "float", FLOAT },
                     { "for", FOR },
                     { "goto", GOTO },
                     { "if", IF },
                     { "inline", INLINE },
                     { "int", INT },
                     { "long", LONG },
                     { "register", REGISTER },
                     { "restrict", RESTRICT },
                     { "return", RETURN },
                     { "short", SHORT },
                     { "signed", SIGNED },
                     { "sizeof", SIZEOF },
                     { "static", STATIC },
                     { "struct", STRUCT },
                     { "switch", SWITCH },
                     { "typedef", TYPEDEF },
                     { "union", UNION },
                     { "unsigned", UNSIGNED },
                     { "void", VOID },
                     { "volatile", VOLATILE },
                     { "while", WHILE },
                     { "_Alignas", ALIGNAS },
                     { "_Alignof", ALIGNOF },
                     { "_Atomic", ATOMIC },
                     { "_Bool", BOOL },
                     { "_Complex", COMPLEX },
                     { "_Generic", GENERIC },
                     { "_Imaginary", IMAGINARY },
                     { "_Noreturn", NORETURN },
                     { "_Static_assert", STATIC_ASSERT },
                     { "_Thread_local", THREAD_LOCAL },
                     { "__func__", FUNC_NAME },
                     { NULL, 0 } };

    for (int i = 0; keywords[i].name != NULL; i++) {
        if (strcmp(str, keywords[i].name) == 0) {
            return keywords[i].token;
        }
    }
    return 0;
}

// Check identifier type (mocked, as in Flex)
static int check_type(void)
{
    // Assume no symbol table; always return IDENTIFIER
    return IDENTIFIER;
}

// Skip whitespace
static void skip_whitespace(void)
{
    while (isspace(next_char)) {
        consume_char();
    }
}

// Skip multi-line comment
static void skip_comment(void)
{
    consume_char(); // Consume '*'
    while (next_char != EOF) {
        if (next_char == '*') {
            consume_char();
            if (next_char == '/') {
                consume_char();
                return;
            }
        } else {
            consume_char();
        }
    }
    fprintf(stderr, "Error: unterminated comment\n");
    exit(1);
}

// Scan identifier or keyword
static int scan_identifier(void)
{
    while (isalnum(next_char) || next_char == '_') {
        consume_char();
    }
    int token = is_keyword(yytext);
    return token ? token : check_type();
}

// Scan number (integer or floating-point)
static int scan_number(void)
{
    int is_float = 0;

    // Handle hexadecimal prefix
    if (next_char == '0' && (tolower(next_char) == 'x')) {
        consume_char(); // '0'
        consume_char(); // 'x' or 'X'
        while (isxdigit(next_char)) {
            consume_char();
        }
        if (next_char == '.') {
            is_float = 1;
            consume_char();
            while (isxdigit(next_char)) {
                consume_char();
            }
        }
        if (tolower(next_char) == 'p') {
            is_float = 1;
            consume_char();
            if (next_char == '+' || next_char == '-') {
                consume_char();
            }
            while (isdigit(next_char)) {
                consume_char();
            }
        }
    } else {
        // Decimal or octal
        while (isdigit(next_char)) {
            consume_char();
        }
        if (next_char == '.') {
            is_float = 1;
            consume_char();
            while (isdigit(next_char)) {
                consume_char();
            }
        }
        if (tolower(next_char) == 'e') {
            is_float = 1;
            consume_char();
            if (next_char == '+' || next_char == '-') {
                consume_char();
            }
            while (isdigit(next_char)) {
                consume_char();
            }
        }
    }

    // Handle suffixes
    while (tolower(next_char) == 'u' || tolower(next_char) == 'l' || tolower(next_char) == 'f') {
        consume_char();
    }

    return is_float ? F_CONSTANT : I_CONSTANT;
}

// Scan string literal
static int scan_string(void)
{
    if (next_char == 'L' || next_char == 'u' || next_char == 'U') {
        consume_char(); // Consume prefix
    }
    if (next_char != '"') {
        fprintf(stderr, "Error: expected string literal\n");
        exit(1);
    }
    consume_char(); // Consume opening quote
    while (next_char != '"' && next_char != '\n' && next_char != EOF) {
        if (next_char == '\\') {
            consume_char();
            if (next_char != EOF) {
                consume_char();
            }
        } else {
            consume_char();
        }
    }
    if (next_char == '"') {
        consume_char(); // Consume closing quote
    } else {
        fprintf(stderr, "Error: unterminated string\n");
        exit(1);
    }
    return STRING_LITERAL;
}

// Scan operators and punctuation
static int scan_operator(void)
{
    int c = next_char;
    consume_char();
    int c2 = next_char;

    // Multi-character operators
    if (c == '.' && c2 == '.' && fgetc(input_file) == '.') {
        consume_char();
        consume_char();
        return ELLIPSIS;
    }
    if (c == '>' && c2 == '>') {
        consume_char();
        if (next_char == '=') {
            consume_char();
            return RIGHT_ASSIGN;
        }
        return RIGHT_OP;
    }
    if (c == '<' && c2 == '<') {
        consume_char();
        if (next_char == '=') {
            consume_char();
            return LEFT_ASSIGN;
        }
        return LEFT_OP;
    }
    if (c == '+' && c2 == '=') {
        consume_char();
        return ADD_ASSIGN;
    }
    if (c == '-' && c2 == '=') {
        consume_char();
        return SUB_ASSIGN;
    }
    if (c == '*' && c2 == '=') {
        consume_char();
        return MUL_ASSIGN;
    }
    if (c == '/' && c2 == '=') {
        consume_char();
        return DIV_ASSIGN;
    }
    if (c == '%' && c2 == '=') {
        consume_char();
        return MOD_ASSIGN;
    }
    if (c == '&' && c2 == '=') {
        consume_char();
        return AND_ASSIGN;
    }
    if (c == '^' && c2 == '=') {
        consume_char();
        return XOR_ASSIGN;
    }
    if (c == '|' && c2 == '=') {
        consume_char();
        return OR_ASSIGN;
    }
    if (c == '+' && c2 == '+') {
        consume_char();
        return INC_OP;
    }
    if (c == '-' && c2 == '-') {
        consume_char();
        return DEC_OP;
    }
    if (c == '-' && c2 == '>') {
        consume_char();
        return PTR_OP;
    }
    if (c == '&' && c2 == '&') {
        consume_char();
        return AND_OP;
    }
    if (c == '|' && c2 == '|') {
        consume_char();
        return OR_OP;
    }
    if (c == '<' && c2 == '=') {
        consume_char();
        return LE_OP;
    }
    if (c == '>' && c2 == '=') {
        consume_char();
        return GE_OP;
    }
    if (c == '=' && c2 == '=') {
        consume_char();
        return EQ_OP;
    }
    if (c == '!' && c2 == '=') {
        consume_char();
        return NE_OP;
    }

    // Single-character tokens
    switch (c) {
    case ';':
        return ';';
    case '{':
        return '{';
    case '}':
        return '}';
    case ',':
        return ',';
    case ':':
        return ':';
    case '=':
        return '=';
    case '(':
        return '(';
    case ')':
        return ')';
    case '[':
        return '[';
    case ']':
        return ']';
    case '.':
        return '.';
    case '&':
        return '&';
    case '!':
        return '!';
    case '~':
        return '~';
    case '-':
        return '-';
    case '+':
        return '+';
    case '*':
        return '*';
    case '/':
        return '/';
    case '%':
        return '%';
    case '<':
        return '<';
    case '>':
        return '>';
    case '^':
        return '^';
    case '|':
        return '|';
    case '?':
        return '?';
    default:
        return 0; // Unknown character
    }
}

// Get current lexeme
char *get_yytext(void)
{
    return yytext;
}
