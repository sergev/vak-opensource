/*
 *      Сделано в МФТИ.
 *
 *      Чет Фев 18 17:22:37 MSK 1988    Вакуленко С. В.
 */

# include "mfile2"

# define A      INTAREG         /* STAREG */
# define B      INBREG          /* SBREG */
# define C      FORCC
# define AWD    SNAME|SOREG|SCON|STARNM|STARREG
# define TI     TINT|TLONG|TSHORT
# define TU     TUNSIGNED|TULONG|TUSHORT
# define TC     TCHAR|TUCHAR
# define TF     TFLOAT|TDOUBLE
# define TFIXED TI|TU|TPOINT
# define TWORD  TI|TU|TF|TPOINT
# define TP     TPTRTO|TWORD|TSTRUCT

struct optab table[] = {

ASSIGN,         B|FOREFF,
	B,              TANY,
	SZERO,          TANY,
		0,              RLEFT|RRIGHT,
		" vtm ,AL;",

ASSIGN,         B|FOREFF,
	B,              TANY,
	SCON,           TANY,
		0,              RLEFT|RRIGHT,
		" vtm CR,AL;",

ASSIGN,         FOREFF,
	B,              TANY,
	B,              TANY,
		0,              RLEFT|RRIGHT,
		" mtj AL,AR;",

ASSIGN,         A|C|FOREFF,
	B,              TANY,
	A,              TANY,
		0,              RLEFT|RRIGHT|RESCC,
		" ati AL;",

ASSIGN,         FOREFF,
	AWD,            TANY,
	SZERO,          TANY,
		0,             RNULL,
		" ztx AL;",

ASSIGN,         A|C|FOREFF,               /* unsign -> int */
	AWD,            TI,
	A,              TU,
		0,             RLEFT|RRIGHT|RESCC,
		" aax #.[51:1]; atx AL;",

ASSIGN,         A|C|FOREFF,               /* int,unsign -> point */
	AWD,            TPOINT,
	A,              TI|TU,
		0,              RLEFT|RRIGHT|RESCC,
		" aax #.[27:1]; atx AL;",

ASSIGN,         A|C|FOREFF,               /* int -> float */
	AWD,            TF,
	A,              TI,
		0,              RLEFT|RRIGHT|RESCC,
		" ean 434h; atx AL;",

ASSIGN,         A|C|FOREFF,             /* float -> int */
	AWD,            TI,
	A,              TF,
		0,              RLEFT|RRIGHT|RESCC,
		" int; atx AL;",

ASSIGN,         FOREFF,                 /* TANY -> char */
	SNAME,          TC,
	A,              TANY,
		NBREG,          RNULL,
		" aax #0xff; xts #AL; asn 1024+61; sti A1; alx cshift,A1;\
 xts AL; aax cmask,A1; aox ,15; atx AL;",

ASSIGN,         FOREFF,                 /* TANY -> char */
	STRRG,          TC,
	A,              TANY,
		NBREG,          RNULL,
		" aax #0xff; xts ZP; asn 1024+61; sti A1; alx cshift,A1;\
 xts AL; aax cmask,A1; aox ,15; atx AL;",

ASSIGN,         A|C,                    /* TANY -> char */
	SNAME,          TC,
	A,              TANY,
		NBREG,          RRIGHT,
		" aax #0xff; atx ,15; xts #AL; asn 1024+61; sti A1;\
 alx cshift,A1; xts AL; aax cmask,A1; aox ,15; stx AL;",

ASSIGN,         A|C,                    /* TANY -> char */
	STRRG,          TC,
	A,              TANY,
		NBREG,          RRIGHT,
		" aax #0xff; atx ,15; xts ZP; asn 1024+61; sti A1;\
 alx cshift,A1; xts AL; aax cmask,A1; aox ,15; stx AL;",

ASSIGN,         A|C|FOREFF,
	AWD,            TANY,
	A,              TANY,
		0,              RLEFT|RRIGHT|RESCC,
		" atx AL;",

ASSIGN,         A|C|FOREFF,
	SFLD,           TANY,
	A,              TANY,
		0,              RLEFT|RRIGHT|RESCC,
		" aax #.[S:1]; aln 1024+H; xts AL; aax #~.[S+H:H+1]; aox ,15; atx AL;",

UNARY AND,      A|C,
	AWD,            TANY,
	SANY,           TANY,
		NAREG,          RESC1,
		" vtm AL,14; ita 14;",

REG,            A|C,
	SANY,           TANY,
	B,              TANY,
		NAREG,          RESCC|RESC1,
		" ita AR;",

REG,            B,
	SANY,           TANY,
	A,              TANY,
		NBREG,          RESC1,
		" ati A1;",

REG,            FORCC,
	SANY,           TANY,
	A,              TANY,
		0,              RESCC,
		"",

REG,            INTEMP,
	SANY,           TWORD,
	A,              TWORD,
		NTEMP,          RESC1,
		" atx A1;",

REG,            FORARG,
	SANY,           TANY,
	B,              TANY,
		0,              RNULL,
		" its AR;",

REG,            FORARG,
	SANY,           TANY,
	A,              TANY,
		0,              RNULL,
		" atx ,15;",

UNARY MUL,      A|C,
	A,              TWORD,
	SANY,           TWORD,
		NAREG|NASL,     RESC1|RESCC,
		" atia;",

UNARY MUL,      A|C,
	SANY,           TWORD,
	AWD,            TWORD,
		NAREG,          RESC1|RESCC,
		" xta AR;",

UNARY MUL,      A|C,
	A,              TANY,
	SANY,           TC,
		NAREG|NASL,     RLEFT|RESCC,
		" aex; atia; asy 1024; aax #0xff;",

OPLTYPE,	FOREFF,
	SANY,           TANY,
	SANY,           TANY,
		0,              RNULL,
		"",             /* this entry throws away computations
				which don't do anything */

OPLTYPE,        A|C,
	SANY,           TANY,
	SZERO,          TANY,
		NAREG,          RESC1|RESCC,
		" xta;",

OPLTYPE,        A|C,
	SANY,           TANY,
	AWD,            TWORD,
		NAREG,          RESC1|RESCC,
		" xta AR;",

OPLTYPE,        A|C,
	SANY,           TANY,
	SOREG,          TC,
		NAREG,          RESC1|RESCC,
		" xta AR; aax #0xff;",

OPLTYPE,        A|C,
	SANY,           TANY,
	SCON,           TC,
		NAREG,          RESC1|RESCC,
		" xta AR;",

OPLTYPE,        A|C,
	SANY,           TANY,
	SNAME|SWADD,    TC,
		NAREG,          RESC1|RESCC,
		" xta AR; aax #0xff;",

OPLTYPE,        A|C,
	SANY,           TANY,
	SNAME,          TC,
		NAREG,          RESC1|RESCC,
		" xta #AR; aex; atia; asy 1024; aax #0xff;",

OPLTYPE,        B,
	SANY,           TANY,
	SCON,           TANY,
		NBREG,          RESC1,
		" vtm CR,A1;",

OPLTYPE,	INTEMP,
	SANY,           TANY,
	A,              TWORD,
		NTEMP,          RESC1,
		" atx A1;",

OPLTYPE,	FORARG,
	SANY,           TANY,
	AWD,            TWORD,
		0,              RNULL,
		" xts AR;",

FLD,            A|C,
	SANY,           TANY,
	SFLD,           TWORD,
		NAREG,          RESC1|RESCC,
		" xta AL; aln 1024+64-H-S; asn 1024+64-S;",

INCR,           FOREFF,
	B,              TANY,
	SCON,           TINT,
		0,              RNULL,
		" utm CR,AL;",

INCR,           A|C,
	B,              TANY,
	SCON,           TINT,
		NAREG,          RESC1|RESCC,
		" ita AL; utm CR,AL;",

INCR,           FOREFF,
	AWD,            TI,
	SCON,           TINT,
		NAREG,          RESC1|RESCC,
		" xta UL; aadx #1; atx AL;",

INCR,           A|C,
	AWD,            TI,
	SCON,           TINT,
		NAREG,          RESC1|RESCC,
		" xta UL; atx ,15; aadx #1; stx AL;",

INCR,           FOREFF,
	AWD,            TP,
	SCON,           TINT,
		NAREG,          RESC1|RESCC,
		" xta UL; aadx #1; aax #.[27:1]; atx AL;",

INCR,           A|C,
	AWD,            TP,
	SCON,           TINT,
		NAREG,          RESC1|RESCC,
		" xta UL; atx ,15; aadx #1; aax #.[27:1]; stx AL;",

INCR,           FOREFF,
	AWD,            TU,
	SCON,           TINT,
		NAREG,          RESC1|RESCC,
		" xta UL; ntr; arx AR; ntr 200h; atx AL;",

INCR,           A|C,
	AWD,            TU,
	SCON,           TINT,
		NAREG,          RESC1|RESCC,
		" xta UL; atx ,15; ntr; arx AR; ntr 200h; stx AL;",

INCR,           FOREFF,
	AWD,            TPTRTO|TC,
	SONE,           TINT,
		NAREG,          RESC1|RESCC,
		" xta UL; arx #'2; atx AL;",

INCR,           A|C,
	AWD,            TPTRTO|TC,
	SONE,           TINT,
		NAREG,          RESC1|RESCC,
		" xta UL; atx ,15; arx #'2; stx AL;",

INCR,           FOREFF,
	AWD,            TPTRTO|TC,
	SCON,           TINT,
		NAREG,          RESC1|RESCC,
		" xta UL; arx #Zc; atx AL;",

INCR,           A|C,
	AWD,            TPTRTO|TC,
	SCON,           TINT,
		NAREG,          RESC1|RESCC,
		" xta UL; atx ,15; arx #Zc; stx AL;",

DECR,           FOREFF,
	B,              TANY,
	SCON,           TINT,
		0,              RNULL,
		" utm -CR,AL;",

DECR,           A|C,
	B,              TANY,
	SCON,           TINT,
		NAREG,          RESC1|RESCC,
		" ita AL; utm -CR,AL;",

DECR,           FOREFF,
	AWD,            TI,
	SCON,           TINT,
		NAREG,          RESC1|RESCC,
		" xta UL; asux #1; atx AL;",

DECR,           A|C,
	AWD,            TI,
	SCON,           TINT,
		NAREG,          RESC1|RESCC,
		" xta UL; atx ,15; asux #1; stx AL;",

DECR,           FOREFF,
	AWD,            TP,
	SCON,           TINT,
		NAREG,          RESC1|RESCC,
		" xta UL; asux #1; aax #.[27:1]; atx AL;",

DECR,           A|C,
	AWD,            TP,
	SCON,           TINT,
		NAREG,          RESC1|RESCC,
		" xta UL; atx ,15; asux #1; aax #.[27:1]; stx AL;",

DECR,           FOREFF,
	AWD,            TU,
	SONE,           TINT,
		NAREG,          RESC1|RESCC,
		" xta UL; ntr; arx #-1; ntr 200h; atx AL;",

DECR,           A|C,
	AWD,            TU,
	SONE,           TINT,
		NAREG,          RESC1|RESCC,
		" xta UL; atx ,15; ntr; arx #-1; ntr 200h; stx AL;",

DECR,           FOREFF,
	AWD,            TU,
	SCON,           TINT,
		NAREG,          RESC1|RESCC,
		" xta UL; ntr; arx #-(CR); ntr 200h; atx AL;",

DECR,           A|C,
	AWD,            TU,
	SCON,           TINT,
		NAREG,          RESC1|RESCC,
		" xta UL; atx ,15; xts AR; aex #-1; arx #1; ntr; arx ,15;\
 ntr 200h; stx AL;",

DECR,           FOREFF,
	AWD,            TPTRTO|TC,
	SONE,           TINT,
		NAREG,          RESC1|RESCC,
		" xta UL; arx #.[62=27]; aax #.[62=27]; atx AL;",

DECR,           A|C,
	AWD,            TPTRTO|TC,
	SONE,           TINT,
		NAREG,          RESC1|RESCC,
		" xta UL; atx ,15; arx #.[62=27]; aax #.[62=27]; stx AL;",

DECR,           FOREFF,
	AWD,            TPTRTO|TC,
	SCON,           TINT,
		NAREG,          RESC1|RESCC,
		" xta UL; arx #Zm; aax #.[62=27]; atx AL;",

DECR,           A|C,
	AWD,            TPTRTO|TC,
	SCON,           TINT,
		NAREG,          RESC1|RESCC,
		" xta UL; atx ,15; arx #Zm; aax #.[62=27]; stx AL;",

EQ,             FORCC,
	A,              TANY,
	AWD,            TWORD,
		0,              RESCC,
		" aex AR;ZI",

NE,             FORCC,
	A,              TANY,
	AWD,            TWORD,
		0,              RESCC,
		" aex AR;ZI",

LE,             FORCC,
	A,              TPTRTO|TC,
	AWD,            TPTRTO|TC,
		0,              RESCC,
		" aln 3+1024; yta 10h; xts AR; aln 3+1024; yta 10h; xsua ,15;ZI",

LT,             FORCC,
	A,              TPTRTO|TC,
	AWD,            TPTRTO|TC,
		0,              RESCC,
		" aln 3+1024; yta 10h; xts AR; aln 3+1024; yta 10h; xsua ,15;ZI",

GE,             FORCC,
	A,              TPTRTO|TC,
	AWD,            TPTRTO|TC,
		0,              RESCC,
		" aln 3+1024; yta 10h; xts AR; aln 3+1024; yta 10h; xsua ,15;ZI",

GT,             FORCC,
	A,              TPTRTO|TC,
	AWD,            TPTRTO|TC,
		0,              RESCC,
		" aln 3+1024; yta 10h; xts AR; aln 3+1024; yta 10h; xsua ,15;ZI",

LE,             FORCC,
	A,              TWORD,
	AWD,            TWORD,
		0,              RESCC,
		" asux AR;ZI",

LT,             FORCC,
	A,              TWORD,
	AWD,            TWORD,
		0,              RESCC,
		" asux AR;ZI",

GE,             FORCC,
	A,              TWORD,
	AWD,            TWORD,
		0,              RESCC,
		" asux AR;ZI",

GT,             FORCC,
	A,              TWORD,
	AWD,            TWORD,
		0,              RESCC,
		" asux AR;ZI",

ULE,            FORCC,
	A,              TANY,
	AWD,            TWORD,
		0,              RESCC,
		" aex UR; xts AR; apkx ,15;ZI",

ULT,            FORCC,
	A,              TANY,
	AWD,            TWORD,
		0,              RESCC,
		" aex UR; xts AR; apkx ,15;ZI",

UGE,            FORCC,
	A,              TANY,
	AWD,            TWORD,
		0,              RESCC,
		" aex UR; xts AR; apkx ,15;ZI",

UGT,            FORCC,
	A,              TANY,
	AWD,            TWORD,
		0,              RESCC,
		" aex UR; xts AR; apkx ,15;ZI",

CCODES,         A|C,
	SANY,           TANY,
	SANY,           TANY,
		NAREG,          RESC1,
		" xta #1;ZN",

UNARY MINUS,    A|C,
	A,              TU,
	SANY,           TU,
		0,              RLEFT|RESCC,
		" aex #-1; ntr; arx #1; ntr 200h;",

UNARY MINUS,    A|C,
	A,              TWORD,
	SANY,           TANY,
		0,              RLEFT|RESCC,
		" xsua;",

COMPL,          A|C,
	A,              TI|TU,
	SANY,           TANY,
		0,              RLEFT|RESCC,
		" aex #-1;",

PLUS,           A|C,
	B,              TP,
	SCON,           TANY,
		NAREG,          RESC1,
		" uta CR,AL;",

PLUS,           B,
	B,              TP,
	SCON,           TANY,
		NBREG,          RESC1,
		" vtm <,AL>CR,A1;",

ASG MUL,        A|C,
	A,              TI|TF,
	AWD,            TI|TF,
		0,              RLEFT|RESCC,
		" amux AR;",

ASG MUL,        A|C,
	A,              TI|TU,
	AWD,            TI|TU,
		0,              RLEFT|RESCC,
		" xts AR; vjm u_mul_u,14;Zx",

ASG DIV,        A|C,
	A,              TI,
	AWD,            TI,
		0,              RLEFT|RESCC,
		" xts AR; ecn 400h-34h; stx ,15; adx 1,15; int 68h;",

ASG DIV,        A|C,
	A,              TI|TU,
	AWD,            TI|TU,
		0,              RLEFT|RESCC,
		" xts AR; vjm u_div_u,14;Zx",

ASG DIV,        A|C,
	A,              TF,
	AWD,            TF,
		0,              RLEFT|RESCC,
		" adx AR;",

ASG MOD,        A|C,
	A,              TI,
	AWD,            TI,
		0,              RLEFT|RESCC,
		" atx ,15; xts UR; ecn 400h-34h; stx ,15; adx 1,15; int 68h;\
 amux AR; xsua ,15;",

ASG MOD,        A|C,
	A,              TI|TU,
	AWD,            TI|TU,
		0,              RLEFT|RESCC,
		" xts AR; vjm u_mod_u,14;Zx",

ASG LS,         A|C,
	A,              TANY,
	SCCON,          TANY,
		0,              RLEFT|RESCC,
		" aln CR+1024;",

ASG LS,         A|C,
	A,              TANY,
	AWD,            TANY,
		0,              RLEFT|RESCC,
		" aln [AR]1024;",

ASG RS,         A|C,
	A,              TANY,
	SCCON,          TANY,
		0,              RLEFT|RESCC,
		" asn CR+1024;",

ASG RS,         A|C,
	A,              TANY,
	AWD,            TANY,
		0,              RLEFT|RESCC,
		" asn [AR]1024;",

ASG PLUS,       A|C,
	A,              TPTRTO|TC,
	SONE,           TI,
		0,              RLEFT|RESCC,
		" arx #'2;",

ASG PLUS,       A|C,
	A,              TPTRTO|TC,
	AWD,            TI,
		0,              RLEFT|RESCC,
		" xts AR; aax #.[27:1]; asn 1024+3; yta 10h; arx ,15;\
 aax #.[62=27];",

ASG PLUS,       A|C,
	A,              TI|TU,
	AWD,            TPTRTO|TC,
		NAREG|NASL,     RESC1|RESCC,
		" aax #.[27:1]; asn 1024+3; yta 10h; arx AR; aax #.[62=27];",

ASG PLUS,       A|C,
	A,              TI|TF,
	AWD,            TI|TF,
		0,              RLEFT|RESCC,
		" aadx AR;",

ASG PLUS,       A,
	A,              TI|TU,
	AWD,            TI|TU,
		0,              RLEFT,
		" ntr; arx AR; ntr 200h;",

ASG PLUS,       A|C,
	A,              TI|TU,
	AWD,            TI|TU,
		0,              RLEFT|RESCC,
		" ntr; arx AR; ntr 200h; aex;",

ASG PLUS,       A|C,
	A,              TPOINT|TI|TU,
	AWD,            TPOINT|TI|TU,
		0,              RLEFT|RESCC,
		" aadx AR; aax #.[27:1];",

ASG PLUS,       INBREG|FOREFF,
	B,              TANY,
	SCON,           TANY,
		0,              RLEFT,
		" utm CR,AL;",

ASG PLUS,       INBREG|FOREFF,
	B,              TANY,
	AWD,            TANY,
		0,              RLEFT,
		" utm [AR],AL;",

ASG PLUS,       A|C,
	B,              TANY,
	SCON,           TANY,
		NAREG,          RESC1|RESCC,
		" utm CR,AL; ita AL;",

ASG MINUS,      A|C,
	A,              TPTRTO|TC,
	AWD,            TPTRTO|TC,
		0,              RLEFT|RESCC,
		" aln 1024+3; yta 10h; xts AR; aln 1024+3; yta 10h; xsua ,15;",

ASG MINUS,       A|C,
	A,              TPTRTO|TC,
	SONE,           TI,
		0,              RLEFT|RESCC,
		" arx #.[62=27]; aax #.[62=27];",

ASG MINUS,      A|C,
	A,              TPTRTO|TC,
	AWD,            TI|TU,
		0,              RLEFT|RESCC,
		" xts AR; aax #.[27:1]; xsua; asn 1024+3; yta 10h; arx ,15;\
 aax #.[62=27];",

ASG MINUS,      A|C,
	A,              TP,
	AWD,            TP,
		0,              RLEFT|RESCC,
		" asux AR; aax #.[27:1];",

ASG MINUS,      A|C,
	A,              TI|TF,
	AWD,            TI|TF,
		0,              RLEFT|RESCC,
		" asux AR;",

ASG MINUS,      A|C,
	A,              TP,
	AWD,            TI,
		0,              RLEFT|RESCC,
		" asux AR; aax #.[27:1];",

ASG MINUS,      A|C,
	A,              TP,
	AWD,            TU,
		0,              RLEFT|RESCC,
		" xts AR; aax #.[27:1]; xsua ,15; aax #.[27:1];",

ASG MINUS,      A,
	A,              TI|TU,
	AWD,            TI|TU,
		0,              RLEFT,
		" xts AR; aex #-1; arx #1; ntr; arx ,15; ntr 200h;",

ASG MINUS,      A|C,
	A,              TI|TU,
	AWD,            TI|TU,
		0,              RLEFT|RESCC,
		" xts AR; aex #-1; arx #1; ntr; arx ,15; ntr 200h; aex;",

ASG MINUS,      INBREG|FOREFF,
	B,              TANY,
	SCON,           TANY,
		0,              RLEFT,
		" utm -CR,AL;",

ASG MINUS,      A|C,
	B,              TANY,
	SCON,           TANY,
		NAREG,          RESC1|RESCC,
		" utm -CR,AL; ita AL;",

ASG AND,        A|C,
	A,              TANY,
	AWD,            TANY,
		0,              RLEFT|RESCC,
		" aax AR;",

ASG OR,         A|C,
	A,              TANY,
	AWD,            TANY,
		0,              RLEFT|RESCC,
		" aox AR;",

ASG ER,         A|C,
	A,              TANY,
	AWD,            TANY,
		0,              RLEFT|RESCC,
		" aex AR;",

UNARY CALL,     A|C,
	SINLINE,        TANY,
	SANY,           TANY,
		NAREG,          RESC1|RESCC,    /* обязательно сумматор */
		"IL",

UNARY CALL,     A|C,
	SCON,           TANY,
	SANY,           TANY,
		NAREG,          RESC1|RESCC,    /* обязательно сумматор */
		" vjm CL,14;Zx",

UNARY CALL,     A|C,
	AWD,            TANY,
	SANY,           TANY,
		NAREG,          RESC1|RESCC,    /* обязательно сумматор */
		" vjm [AL],14;Zx",

SCONV,          A|C,            /* int -> int */
	A,              TI,
	SANY,           TI,
		0,              RLEFT|RESCC,
		"",

SCONV,          A|C,    /* int,unsign -> char */
	A,              TI|TU,
	SANY,           TC,
		0,              RLEFT|RESCC,
		" aax #0xff;",

SCONV,          A|C,    /* unsign -> int */
	A,              TU,
	SANY,           TI,
		0,              RLEFT|RESCC,
		" aax #.[51:1];",

SCONV,          A|C,        /* float -> int  */
	A,              TF,
	SANY,           TI,
		0,              RLEFT|RESCC,
		" int;",

SCONV,          A|C,        /* int -> float */
	A,              TI,
	SANY,           TF,
		0,              RLEFT|RESCC,
		" ean 434h;",

PCONV,          A|C,                    /* any -> *char */
	A,              TANY,
	SANY,           TPTRTO|TC,
		0,              RLEFT|RESCC,
		" aax #.[62=27];",

PCONV,          A|C,                    /* any -> *int */
	A,              TANY,
	SANY,           TP,
		0,              RLEFT|RESCC,
		" aax #.[27:1];",

STASG,          FOREFF,
	SNAME,          TANY,
	SCON,           TANY,
		NBREG,          RNULL,
		" vtm 1-ZZ,A1;: xta CR+ZZ-1,A1; atx ZZ-1+AL,A1; vlm .-1,A1;",

STASG,          FOREFF,
	SNAME,          TANY,
	B,              TANY,
		NBREG,          RNULL,
		" vtm ZZ-1,A1;: xta <,A1>,AR; atx AL,A1; utm -1,A1; vpzm .-2,A1;",

STASG,          FOREFF,
	SOREG,          TANY,
	SCON,           TANY,
		NBREG,          RNULL,
		" vtm ZZ-1,A1;: xta CR,A1; atx <,A1>AL; utm -1,A1; vpzm .-2,A1;",

STASG,          FOREFF,
	SOREG,          TANY,
	B,              TANY,
		NBREG,          RNULL,
		" vtm ZZ-1,A1;: xta <,A1>,AR; atx <,A1>AL; utm -1,A1; vpzm .-2,A1;",

INIT,	FOREFF,
	SCON,           TANY,
	SANY,           TANY,
		0,              RNOP,
		" .word CL;",

INIT,	FOREFF,
	SNAME,          TANY,
	SANY,           TPOINT,
		0,              RNOP,
		" .word AL;",

	/* for the use of fortran only */

GOTO,	FOREFF,
	SCON,           TANY,
	SANY,           TANY,
		0,              RNOP,
		" uj CL;",

GOTO,	FOREFF,
	SNAME,          TANY,
	SANY,           TANY,
		0,              RNOP,
		" uj [CL];",

GOTO,	FOREFF,
	SNAME,          TI|TU|TC|TPOINT,
	SANY,           TANY,
		0,              RNOP,
		" uj CL;",

	/* Default actions for hard trees ... */

# define DF(x) FORREW,SANY,TANY,SANY,TANY,REWRITE,x,""

UNARY MUL, DF( UNARY MUL ),
INCR, DF(INCR),
DECR, DF(INCR),
FLD, DF(FLD),
ASSIGN, DF(ASSIGN),
COMOP, DF(COMOP),
INIT, DF(INIT),
STASG, DF(STASG),

OPLEAF, DF(NAME),

OPUNARY, DF(UNARY MINUS),

OPLOG,	FORCC,
	SANY,	TANY,
	SANY,	TANY,
		REWRITE,	BITYPE,
		"",

OPLOG,	DF(NOT),

ASG OPANY, DF(ASG PLUS),

OPANY, DF(BITYPE),

FREE, FREE, FREE, FREE, FREE, FREE, FREE, FREE,
	"Help: I'm in trouble!\n" };
