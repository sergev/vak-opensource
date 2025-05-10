#ifndef SCANNER_H
#define SCANNER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

enum {
    TOKEN_EOF,              // End of file reached
    TOKEN_UNKNOWN,          // Unrecognized symbol
    TOKEN_AUTO,             // auto
    TOKEN_BREAK,            // break
    TOKEN_CASE,             // case
    TOKEN_CHAR,             // char
    TOKEN_CONST,            // const
    TOKEN_CONTINUE,         // continue
    TOKEN_DEFAULT,          // default
    TOKEN_DO,               // do
    TOKEN_DOUBLE,           // double
    TOKEN_ELSE,             // else
    TOKEN_ENUM,             // enum
    TOKEN_EXTERN,           // extern
    TOKEN_FLOAT,            // float
    TOKEN_FOR,              // for
    TOKEN_GOTO,             // goto
    TOKEN_IF,               // if
    TOKEN_INLINE,           // inline
    TOKEN_INT,              // int
    TOKEN_LONG,             // long
    TOKEN_REGISTER,         // register
    TOKEN_RESTRICT,         // restrict
    TOKEN_RETURN,           // return
    TOKEN_SHORT,            // short
    TOKEN_SIGNED,           // signed
    TOKEN_SIZEOF,           // sizeof
    TOKEN_STATIC,           // static
    TOKEN_STRUCT,           // struct
    TOKEN_SWITCH,           // switch
    TOKEN_TYPEDEF,          // typedef
    TOKEN_UNION,            // union
    TOKEN_UNSIGNED,         // unsigned
    TOKEN_VOID,             // void
    TOKEN_VOLATILE,         // volatile
    TOKEN_WHILE,            // while
    TOKEN_ALIGNAS,          // _Alignas
    TOKEN_ALIGNOF,          // _Alignof
    TOKEN_ATOMIC,           // _Atomic
    TOKEN_BOOL,             // _Bool
    TOKEN_COMPLEX,          // _Complex
    TOKEN_GENERIC,          // _Generic
    TOKEN_IMAGINARY,        // _Imaginary
    TOKEN_NORETURN,         // _Noreturn
    TOKEN_STATIC_ASSERT,    // _Static_assert
    TOKEN_THREAD_LOCAL,     // _Thread_local
    TOKEN_FUNC_NAME,        // __func__
    TOKEN_IDENTIFIER,       // identifier (e.g., "variable", "_abc123")
    TOKEN_I_CONSTANT,       // integer/character constant (e.g., "123", "0x1a", "'a'", "L'\n'")
    TOKEN_F_CONSTANT,       // floating-point constant (e.g., "3.14", "1e-10", "0x1.2p3")
    TOKEN_STRING_LITERAL,   // string literal (e.g., "hello", L"world")
    TOKEN_ELLIPSIS,         // ...
    TOKEN_RIGHT_ASSIGN,     // >>=
    TOKEN_LEFT_ASSIGN,      // <<=
    TOKEN_ADD_ASSIGN,       // +=
    TOKEN_SUB_ASSIGN,       // -=
    TOKEN_MUL_ASSIGN,       // *=
    TOKEN_DIV_ASSIGN,       // /=
    TOKEN_MOD_ASSIGN,       // %=
    TOKEN_AND_ASSIGN,       // &=
    TOKEN_XOR_ASSIGN,       // ^=
    TOKEN_OR_ASSIGN,        // |=
    TOKEN_RIGHT_OP,         // >>
    TOKEN_LEFT_OP,          // <<
    TOKEN_INC_OP,           // ++
    TOKEN_DEC_OP,           // --
    TOKEN_PTR_OP,           // ->
    TOKEN_AND_OP,           // &&
    TOKEN_OR_OP,            // ||
    TOKEN_LE_OP,            // <=
    TOKEN_GE_OP,            // >=
    TOKEN_EQ_OP,            // ==
    TOKEN_NE_OP,            // !=
    TOKEN_SEMICOLON,        // ;
    TOKEN_LBRACE,           // {
    TOKEN_RBRACE,           // }
    TOKEN_COMMA,            // ,
    TOKEN_COLON,            // :
    TOKEN_ASSIGN,           // =
    TOKEN_LPAREN,           // (
    TOKEN_RPAREN,           // )
    TOKEN_LBRACKET,         // [
    TOKEN_RBRACKET,         // ]
    TOKEN_DOT,              // .
    TOKEN_AMPERSAND,        // &
    TOKEN_NOT,              // !
    TOKEN_TILDE,            // ~
    TOKEN_MINUS,            // -
    TOKEN_PLUS,             // +
    TOKEN_STAR,             // *
    TOKEN_SLASH,            // /
    TOKEN_PERCENT,          // %
    TOKEN_LT,               // <
    TOKEN_GT,               // >
    TOKEN_CARET,            // ^
    TOKEN_PIPE,             // |
    TOKEN_QUESTION,         // ?
};

void init_scanner(FILE *input);
int yylex(void);

// Get current lexeme
char *get_yytext(void);

#ifdef __cplusplus
}
#endif

#endif // SCANNER_H
