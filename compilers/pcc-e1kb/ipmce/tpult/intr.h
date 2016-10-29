# define MAX_H (sizeof (int_tab) / sizeof (int_tab[1]))
# define TICKPERMIN  1092L
# define TICK_CHK (TICKPERMIN / 600)

struct int_h {
	int     flag;
	int     ticks;
	long    wait_tick;      /* wait time */
	void (*exec) ();        /* function to execute */
};

extern struct int_h int_tab [];
extern int stopped;
int dispatch ();
# ifdef __TURBOC__
# define _disable disable
# define _enable  enable
# define _dos_getvect getvect
# define _dos_setvect setvect
# endif

