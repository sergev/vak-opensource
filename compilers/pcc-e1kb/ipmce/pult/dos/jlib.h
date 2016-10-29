# ifndef cntrl
#    define cntrl(c)    (c & 037)
# endif
# ifndef meta
#    define meta(c)     (c | 0400)
# endif

typedef struct {
	short y, x;
	short ny, nx;
	short *mem;
} JBOX;

extern JBOX *Jgetbox ();

# define Jmputstr(r,c,s)	(Jmove(r,c),Jputstr(s))
# define Jmputch(r,c,s)		(Jmove(r,c),Jputch(s))
