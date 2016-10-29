/*
 * Portable Forth interpreter.
 *
 * Copyright (C) 1990-92 Serge Vakulenko, <vak@kiae.su>.
 */

# include <setjmp.h>

# define MEMSZ          4096    /* max size of allot'ated memory */
# define DEFSZ          512     /* max size of compiled function */
# define STACKSZ        256     /* max size of data stack */
# define NAMESZ         32      /* max length of ident name */
# define DICTSZ         256     /* max size of dictionary */
# define NESTSZ         512     /* max depth of function calls nesting */
# define LOOPSZ         32      /* max loop nesting */
# define OUTSZ          8       /* max output file nesting */
# define INSZ           8       /* max input file nesting */

/* types of idents */

# define FGENINT        1	/* constant */
# define FGENFLT        2       /* float constant */
# define FSUBR          3       /* compiled function */
# define FHARDWARE      4       /* builtin function */
# define FSTRING        5       /* ." */
# define FCSTRING       6       /* .( */
# define FQUOTE         7       /* " */
# define FCQUOTE        8       /* ascii */
				/* */
# define FREPEAT        10      /* repeat */
# define FDO            11      /* do */
# define FLOOP          12      /* loop */
# define FPLOOP         13      /* +loop */
# define FCONDIF        14      /* if */
# define FCONDELSE      15      /* else */
# define FCONDTHEN      16      /* then */
# define FBEGIN         17      /* begin */
# define FUNTIL         18      /* until */
# define FWHILE         19      /* while */
# define FCOUNTI        20      /* i */
# define FCOUNTJ        21      /* j */
# define FCOUNTK        22      /* k */
# define FLEAVE         23      /* leave */
# define FIFDO          24      /* ?do */
# define FEXIT          25      /* exit */

# define FETCHWORD(a)   (memory.v[(a)/sizeof(value)])
# define STOREWORD(a,w) (memory.v[(a)/sizeof(value)]=(w))
# define FETCHBYTE(a)   ((int) (unsigned char) memory.c[a])
# define STOREBYTE(a,w) (memory.c[a]=(w))

# define compilation    FETCHWORD(compindex).i
# define base           FETCHWORD(baseindex).i
# define span           FETCHWORD(spanindex).i
# define hld            FETCHWORD(hldindex).i
# define pad            (MEMSZ-1)

# define ftoi(z)        (_x.r = (z), _x.i)
# define itof(z)        (_x.i = (z), _x.r)
# define vtoi(z)        ((z).i)
# define itov(z)        (_x.i = (z), _x)

# define push(i)	((stackptr < stack+STACKSZ) ? *stackptr++ = (i) : stackerr (1))
# define fpush(f)	((stackptr < stack+STACKSZ) ? *stackptr++ = ftoi (f) : stackerr (1))

# define pop()		((stackptr > stack) ? *--stackptr : (integer) stackerr (0))
# define fpop()		((stackptr > stack) ? itof (*--stackptr) : (real) stackerr (0))

/*
 * Size of integer must be equal to size of real.
 * All known machines have longs and floats of the same size,
 * so we will use these types.
 */

typedef long     	integer;
typedef unsigned long  	uinteger;
typedef float   	real;
typedef int     	(*funcptr) ();

typedef union value {
	integer         i;
	real            r;
	union value     *v;
	funcptr         p;
} value;

typedef struct ident {
	int             type;
	int             immed;
	value           val;
	char            name [NAMESZ];
} ident;

struct table {
	char            *name;
	funcptr         func;
	int             type;
	int             immed;
};

struct loop {
	int		type;
	integer         cnt;
	integer         low;
	integer         up;
	value		*ptr;
};

union memory {
	char            c [MEMSZ];
	value           v [MEMSZ/sizeof(value)];
};

enum {
	binary		= 2,
	octal		= 8,
	decimal		= 10,
	hex		= 16,
};

extern  ident		dict [];
extern  ident		*dictptr;
extern  ident		*lowdictptr;
extern  jmp_buf		errjmp;
extern  value		defbuf [];
extern  value		*defptr;
extern  union memory	memory;
extern  integer		here;
extern  integer		lowmem;
extern	integer		stack [];
extern  integer		*stackptr;
extern  struct table	table [];
extern	integer		compindex;
extern	integer		baseindex;
extern	integer		spanindex;
extern	integer		hldindex;
extern			outstack [];
extern			outptr;
extern			instack [];
extern			inptr;
extern			tty;
extern			boln;

extern	value		*execptr;
extern			execcnt;

extern union value	_x;

extern  ident		*find (), *enter ();
extern  char		*getword ();
extern  integer		allot ();
extern  integer		alloc ();
extern			execute ();
extern			genint ();
extern  char		*malloc ();
