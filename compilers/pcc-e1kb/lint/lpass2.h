/*	@(#)lpass2.h	1.3	*/
/*	3.0 SID #	1.2	*/
typedef struct sty STYPE;
struct sty { ATYPE t; STYPE *next; };

typedef struct sym {
#ifdef FLEXNAMES
	char *name;
#else
	char name[LCHNM];
#endif
	char nargs;
	int decflag;
	int fline;
	STYPE symty;
	int fno;
	int mno;
	int use;
	short more;
	} STAB;
