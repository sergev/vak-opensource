# ifndef cntrl
#    define cntrl(c)    (c & 037)
# endif
# ifndef meta
#    define meta(c)     (c | 0400)
# endif

static struct KeyMap {
	char *tcap;
	char *str;
	short val;
	short index;
};
