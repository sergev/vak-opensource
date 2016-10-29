# include <stdio.h>
# ifdef MSDOS
# include <stdlib.h>
# endif
# include <signal.h>
/*# endif*/
# include <ctype.h>
# include <string.h>
# include "cap.h"
# include "key.h"
# include "scr.h"
# include "menu.h"
# include "choice.h"
# include "ut.h"

void init();

static struct KeyMap keymap [] = {
	"kl",           0,              meta ('l'),     0,
	"kr",           0,              meta ('r'),     0,
	"ku",           0,              meta ('u'),     0,
	"kd",           0,              meta ('d'),     0,
	"kN",           0,              meta ('N'),     0,
	"kP",           0,              meta ('P'),     0,
	"kh",           0,              meta ('h'),     0,
	"kH",           0,              meta ('H'),     0,
	"kS",           0,              meta ('S'),     0,
	"kI",           0,              meta ('I'),     0,
	"kD",           0,              meta ('D'),     0,
	"k1",           0,              meta ('a'),     0,
	"k2",           0,              meta ('b'),     0,
	"k3",           0,              meta ('c'),     0,
	"f1",           0,              meta ('1'),     0,
	"f2",           0,              meta ('2'),     0,
	"f3",           0,              meta ('3'),     0,
	"f4",           0,              meta ('4'),     0,
	"f5",           0,              meta ('5'),     0,
	"f6",           0,              meta ('6'),     0,
	"f7",           0,              meta ('7'),     0,
	"f8",           0,              meta ('8'),     0,
	"f9",           0,              meta ('9'),     0,
	"f0",           0,              meta ('0'),     0,
	"f.",           0,              meta ('.'),     0,
	"f-",           0,              meta ('-'),     0,
	"kB",           0,              meta ('B'),     0,
	0,              0,              0,              0,
};

# define NAME_LENGTH 40
extern long far *pcur_tick;
extern char batch_file[NAME_LENGTH];
extern void Draw_StateB (), get_batch_nwg (), load_batch (), run_sas ();
int     is_name = 0;

void main (argc, argv)
char **argv;

{
	int	 submenu = 's';
	char	flag, *p;

	init ();
	VMove (0, 0);
	VDrawBox(0, 0, LINES - 1, COLS);
	VMPutString (LINES-1, 0, "\3\16 Пультовой сервис ДВС версия 9.13            (C) ИТМ и ВТ АН СССР, 5-1=4 1990 \17\2");
	Draw_StateB ();

	if (argc == 2) {
		if ((p = strchr(argv[1], '/')) != NULL)
			flag = p [1];
		else
			flag = 0;
		*p = 0;
		strncpy (batch_file, argv[1], NAME_LENGTH);
		(void) extension (batch_file, "sas");
		switch (flag) {
		case 'l' :
			is_name = 2;
			load_batch ();
			break;
		case 'r' :
			is_name = 3;
			run_sas ();
			break;
		}
		is_name = 1;
	} else
		get_batch_nwg ();

	for (;;) {
		VSync ();
		submenu = runmenu(submenu);
	}
}


extern void ptinitialize (), inst_timer ();

void ignore (sig) {

	signal (SIGINT, ignore);
	VRedraw ();
}

void init ()
{
	char buf [1024];

# ifdef NDEBUG
	signal (SIGINT, ignore);
# endif

# ifndef MSDOS
	signal (SIGINT, SIG_IGN);
# endif

	if (! CapInit (buf)) {
		outerr ("cannot read termcap\n");
		exit (1);
	}
	if (! VInit ()) {
		outerr ("cannot initialize terminal\n");
		exit (1);
	}
	VSetPalette (10, 0, 0, 7, 15, 0, 15, 7, 14, 0, 15, 1);
	if (COLS != 80 || LINES != 25) {
		VMove (LINES-1, 0);
		VClearLine ();
		VSync ();
		VClose ();
		outerr ("must be 80 columns & 25 lines on terminal\n");
		exit (1);
	}
	KeyInit (keymap, VFlush);
	VOpen ();
	ptinitialize();
# ifdef MSDOS
	inst_timer ();
# endif

}

