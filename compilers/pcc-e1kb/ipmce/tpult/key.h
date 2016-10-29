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

extern void KeyUnget(), KeyInit();
extern KeyGet();

# ifdef MSDOS
# ifdef NDEBUG
# define KB_READ 0x07
# else
# define KB_READ 0x08
# endif
# define KB_STAT 0x0b

# define getch() (bdos (KB_READ, 0, 0) & 0xFF)
# define kbhit() (bdos (KB_STAT, 0, 0) & 0x01)

# endif
