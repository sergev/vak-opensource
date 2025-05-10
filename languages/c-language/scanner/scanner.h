#include <stdio.h>

// Token codes (assumed to match y.tab.h from Flex specification)
#define AUTO           258
#define BREAK          259
#define CASE           260
#define CHAR           261
#define CONST          262
#define CONTINUE       263
#define DEFAULT        264
#define DO             265
#define DOUBLE         266
#define ELSE           267
#define ENUM           268
#define EXTERN         269
#define FLOAT          270
#define FOR            271
#define GOTO           272
#define IF             273
#define INLINE         274
#define INT            275
#define LONG           276
#define REGISTER       277
#define RESTRICT       278
#define RETURN         279
#define SHORT          280
#define SIGNED         281
#define SIZEOF         282
#define STATIC         283
#define STRUCT         284
#define SWITCH         285
#define TYPEDEF        286
#define UNION          287
#define UNSIGNED       288
#define VOID           289
#define VOLATILE       290
#define WHILE          291
#define ALIGNAS        292
#define ALIGNOF        293
#define ATOMIC         294
#define BOOL           295
#define COMPLEX        296
#define GENERIC        297
#define IMAGINARY      298
#define NORETURN       299
#define STATIC_ASSERT  300
#define THREAD_LOCAL   301
#define FUNC_NAME      302
#define IDENTIFIER     303
#define I_CONSTANT     304
#define F_CONSTANT     305
#define STRING_LITERAL 306
#define ELLIPSIS       307
#define RIGHT_ASSIGN   308
#define LEFT_ASSIGN    309
#define ADD_ASSIGN     310
#define SUB_ASSIGN     311
#define MUL_ASSIGN     312
#define DIV_ASSIGN     313
#define MOD_ASSIGN     314
#define AND_ASSIGN     315
#define XOR_ASSIGN     316
#define OR_ASSIGN      317
#define RIGHT_OP       318
#define LEFT_OP        319
#define INC_OP         320
#define DEC_OP         321
#define PTR_OP         322
#define AND_OP         323
#define OR_OP          324
#define LE_OP          325
#define GE_OP          326
#define EQ_OP          327
#define NE_OP          328

void init_scanner(FILE *input);
int yylex(void);

// Get current lexeme
char *get_yytext(void);
