#ifndef SCANNER_H
#define SCANNER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#define TOKEN_AUTO           258
#define TOKEN_BREAK          259
#define TOKEN_CASE           260
#define TOKEN_CHAR           261
#define TOKEN_CONST          262
#define TOKEN_CONTINUE       263
#define TOKEN_DEFAULT        264
#define TOKEN_DO             265
#define TOKEN_DOUBLE         266
#define TOKEN_ELSE           267
#define TOKEN_ENUM           268
#define TOKEN_EXTERN         269
#define TOKEN_FLOAT          270
#define TOKEN_FOR            271
#define TOKEN_GOTO           272
#define TOKEN_IF             273
#define TOKEN_INLINE         274
#define TOKEN_INT            275
#define TOKEN_LONG           276
#define TOKEN_REGISTER       277
#define TOKEN_RESTRICT       278
#define TOKEN_RETURN         279
#define TOKEN_SHORT          280
#define TOKEN_SIGNED         281
#define TOKEN_SIZEOF         282
#define TOKEN_STATIC         283
#define TOKEN_STRUCT         284
#define TOKEN_SWITCH         285
#define TOKEN_TYPEDEF        286
#define TOKEN_UNION          287
#define TOKEN_UNSIGNED       288
#define TOKEN_VOID           289
#define TOKEN_VOLATILE       290
#define TOKEN_WHILE          291
#define TOKEN_ALIGNAS        292
#define TOKEN_ALIGNOF        293
#define TOKEN_ATOMIC         294
#define TOKEN_BOOL           295
#define TOKEN_COMPLEX        296
#define TOKEN_GENERIC        297
#define TOKEN_IMAGINARY      298
#define TOKEN_NORETURN       299
#define TOKEN_STATIC_ASSERT  300
#define TOKEN_THREAD_LOCAL   301
#define TOKEN_FUNC_NAME      302
#define TOKEN_IDENTIFIER     303
#define TOKEN_I_CONSTANT     304
#define TOKEN_F_CONSTANT     305
#define TOKEN_STRING_LITERAL 306
#define TOKEN_ELLIPSIS       307
#define TOKEN_RIGHT_ASSIGN   308
#define TOKEN_LEFT_ASSIGN    309
#define TOKEN_ADD_ASSIGN     310
#define TOKEN_SUB_ASSIGN     311
#define TOKEN_MUL_ASSIGN     312
#define TOKEN_DIV_ASSIGN     313
#define TOKEN_MOD_ASSIGN     314
#define TOKEN_AND_ASSIGN     315
#define TOKEN_XOR_ASSIGN     316
#define TOKEN_OR_ASSIGN      317
#define TOKEN_RIGHT_OP       318
#define TOKEN_LEFT_OP        319
#define TOKEN_INC_OP         320
#define TOKEN_DEC_OP         321
#define TOKEN_PTR_OP         322
#define TOKEN_AND_OP         323
#define TOKEN_OR_OP          324
#define TOKEN_LE_OP          325
#define TOKEN_GE_OP          326
#define TOKEN_EQ_OP          327
#define TOKEN_NE_OP          328
#define TOKEN_SEMICOLON 329
#define TOKEN_LBRACE    330
#define TOKEN_RBRACE    331
#define TOKEN_COMMA     332
#define TOKEN_COLON     333
#define TOKEN_ASSIGN    334
#define TOKEN_LPAREN    335
#define TOKEN_RPAREN    336
#define TOKEN_LBRACKET  337
#define TOKEN_RBRACKET  338
#define TOKEN_DOT       339
#define TOKEN_AMPERSAND 340
#define TOKEN_NOT       341
#define TOKEN_TILDE     342
#define TOKEN_MINUS     343
#define TOKEN_PLUS      344
#define TOKEN_STAR      345
#define TOKEN_SLASH     346
#define TOKEN_PERCENT   347
#define TOKEN_LT        348
#define TOKEN_GT        349
#define TOKEN_CARET     350
#define TOKEN_PIPE      351
#define TOKEN_QUESTION  352

void init_scanner(FILE *input);
int yylex(void);

// Get current lexeme
char *get_yytext(void);

#ifdef __cplusplus
}
#endif

#endif // SCANNER_H
