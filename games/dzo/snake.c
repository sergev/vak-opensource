/*
	Old game - snake. (version vt200-like only)
	Vladimir Oleynik <dzo@simtreas.ru> (C) 1987-2003

	Have mashine`s snake with "AI".
	Battlefiled is "geographic`s sphere as box map".
	Rabbit have "weight" growing after eated bush.
	If can`t moving your snake, you may begin eat itself.
	But computer`s snake always eat itself if can`t moving.


	Keys:

	<4> or <7> or <Left>  - left
	<5> or <2> or <Down>  - down
	<6> or <9> or <Right> - right
	<9> or <Up>           - up
	<0> or <Space>        - eat itself
	<Ctrl-C>              - quick exit
*/


#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>


#define MAX_LENGHT 100

#define X_MAX 80
#define Y_MAX 24


#define LEFT       0
#define DOWN       1
#define RIGHT      2
#define UP         3
#define EAT_ITSELF 4

#define EMPTY   0
#define SNAKES  1
#define RABBIT  2
#define BUSH    3

#define MAX_LEVEL    3
#define MAX_LEVEL_C '3'

#define MAX_BUSH 21


static char field[X_MAX-1][Y_MAX-1];    /* Battlefield */
static int  level;                      /* Game speed level */

typedef struct POZ {
	unsigned char x;
	unsigned char y;
} POZ;

typedef struct SNAKE {
	POZ  poz[MAX_LENGHT];   /* snake coordinate circular buffer */
	POZ *p_poz_h;           /* pointer head */
	POZ *p_poz_t;           /* pointer tail */

	const char *win_lose;
	const char *body_color;
	POZ  head;                      /* head coordinates snake */
	unsigned char length;           /* snake`s length */
	unsigned char score_x_pos;
	signed char   up_grow;          /* up growing snake */
} SNAKE;

/* human`s snake */
static SNAKE human = {
	{ {0, 0} },     /* empty snake coordinate circular buffer */
	human.poz,      /* pointer head to start circular buffer  */
	human.poz,      /* pointer tail to start circular buffer  */

	"WON",
	"3mO",
	{ 38, 10 },     /* head coordinate human`s snake */
	0,              /* not setted snake`s length */
	5,              /* score x position */
	3               /* begin up growing snake to set lenght to 3 */
};

/* mashine`s snake */
static SNAKE ai = {
	{ {0, 0} },     /* empty snake coordinate circular buffer */
	ai.poz,         /* pointer head to start circular buffer  */
	ai.poz,         /* pointer tail to start circular buffer  */

	"losed",
	"6mM",
	{ 38, 14 },     /* head coordinate mashine`s snake */
	0,              /* not setted snake`s length */
	66,             /* score x position */
	3               /* begin up growing snake to set lenght to 3 */
};

static int   direction;                 /* direction motion human`s snake */

static POZ rabbit;                      /* rabbit`s coordinate */
static char   rabbit_weight;            /* rabbit "weight" */
static POZ bushes[MAX_BUSH];            /* bushes coordinate */

	/* terminal modes settings */
struct termios old_term_modes, new_term_modes;


/* size optimizations */
static void fflusho(void) { fflush(stdout); }


/* SIGINT handler */
static void sigend(int sig)
{
	int l;

	signal(SIGINT, SIG_IGN);
	if(sig!=0)
		printf("\033[H\033[J");
	l = human.length - ai.length;

	/* compute score */
	if(l>0)
		l *= level+1;
	 else
		l *= (MAX_LEVEL+1)-level;
	printf("\nScore %d\n", l);
	printf("\033[0m");
	fflusho();
	tcsetattr(0, TCSANOW, &old_term_modes);
	_exit(0);
}

static void exit_game(int sig)
{
    sigend(0);
}

/* Input without stopind */
static void get_and_delay(void)
{
	int c;
	struct timeval tv;
	fd_set readfds;

	tv.tv_sec = 0;
	/* maximum delay milliseconds: level=0 delay=180... level=3 delay=120 */
	tv.tv_usec = (9l-level)*20000;

	FD_ZERO(&readfds);
	FD_SET(0, &readfds);

	fflusho();

	if(select(1, &readfds, NULL, NULL, &tv) > 0) {
		/* ready for input */
		while(1) {
			static const char key_moves[] = "0 52B47D69C8A";
			const  char *p;

			c = getchar();
			if(c==3)        /* Ctrl-C */
				exit_game(SIGINT);
			if(c==033 || c=='[' || c == 'O')
				continue;
			p = strchr(key_moves, c);
			if(p) {
			    static const char dir_moves[] = {
				EAT_ITSELF, EAT_ITSELF, /* "0 "  */
				DOWN, DOWN, DOWN,       /* "52B" */
				LEFT, LEFT, LEFT,       /* "47D" */
				RIGHT,RIGHT,RIGHT,      /* "69C" */
				UP,   UP, EAT_ITSELF }; /* "8A\0" */

			    direction = dir_moves[(p-key_moves)];
			}
			return;
		}
	}
}


/* Cursor positioning */
static __attribute__ ((stdcall)) void
curs (int x, int y)
{
  printf("\033[%d;%dH", y+1, x+1);
}


/* random generate 1...n */
static __attribute__ ((stdcall)) int
rand_n(int n)
{
	return rand() % n + 1;
}

/* Generate rabbit (t==-RABBIT) and bushs (bushes[t]) */
static __attribute__ ((stdcall)) void
rndrb(int t)
{
	int x, y;

	for(;;) {
		x = rand_n(X_MAX-2);
		y = rand_n(Y_MAX-2);
		if(field[x][y] == EMPTY) {
			/* empty place on field */
			curs(x, y);
			if(t == -RABBIT) {
				/* generate rabbit */
				rabbit.x = x;
				rabbit.y = y;
				field[x][y] = RABBIT;
				/* weight */
				rabbit_weight = rand_n(5);
				printf("\033[35m%c", rabbit_weight + '0');
			} else {
				/* generate bush */
				bushes[t].x = x;
				bushes[t].y = y;
				field[x][y] = BUSH;
				printf("\033[32m*");
			}
			return;
		}
	}
}

/* Set head snake to place on field */
static __attribute__ ((stdcall)) void
shs(SNAKE *psnake)
{
	POZ *p;

	if(psnake->length >= MAX_LENGHT) {
		printf("\033[H\033[JYou %s!\n", psnake->win_lose);
		exit_game(0);
	}

	/* replace head "image" to body "image" */
	p = psnake->p_poz_h - 1;
	if(p < psnake->poz)
		p = psnake->poz + MAX_LENGHT - 1;
	curs(p->x, p->y);
	printf("\033[3%s", psnake->body_color);

	/* set new head position to field */
	p    = psnake->p_poz_h;
	p->x = psnake->head.x;
	p->y = psnake->head.y;
	field[p->x][p->y] = SNAKES;

	/* set head "image" */
	curs(p->x, p->y);
	printf("\033[31m@");

	/* circular incrementing head pointer */
	if((p - psnake->poz) == (MAX_LENGHT-1))
		psnake->p_poz_h = psnake->poz;
	 else
		psnake->p_poz_h++;
}


/* Move snake`s tail */
static __attribute__ ((stdcall)) void
move_tail(SNAKE *psnake)
{
	signed char up_grow = psnake->up_grow;

	if(up_grow) {
		/* update score */
		signed char delta1;

		curs(psnake->score_x_pos, 0);
		delta1 = up_grow > 0 ? 1 : -1;
		psnake->up_grow -= delta1;
		psnake->length  += delta1;
		printf(" %-2d", psnake->length);
	}
	if(up_grow <= 0) {
		POZ *p  = psnake->p_poz_t;

		field[p->x][p->y] = EMPTY;
		curs(p->x, p->y);
		putchar(' ');
		if((p - psnake->poz) == (MAX_LENGHT-1))
			psnake->p_poz_t = psnake->poz;
		 else
			psnake->p_poz_t++;
	}
}


/* moving head */
static __attribute__ ((stdcall)) int
move_h(int mdirection, SNAKE *psnake)
{
	unsigned char k;
	int x;
	int y;

	x = psnake->head.x;
	y = psnake->head.y;

	switch(mdirection) {
		case DOWN:
			if(y != (Y_MAX-2))
				y++;
			 else
				y = 1;
			 break;
		case LEFT:
			if(x != 1)
				x--;
			 else
				x = X_MAX-2;
			break;
		case RIGHT:
			if(x != (X_MAX-2))
				x++;
			 else
				x = 1;
			break;
		case UP:
			if(y != 1)
				y--;
			 else
				y = Y_MAX-2;
			break;
		default: /* case EAT_ITSELF: -  eating itself in progress */
			if(psnake->length > 2) {
				psnake->up_grow = -1;
				move_tail(psnake);
				return 0;
			}
			return 1;
	}

	k = field[x][y];
	if( k == SNAKES || k == BUSH )
		return 1;
	psnake->head.x = x;
	psnake->head.y = y;
	if(k == RABBIT) {
		/* eat rabbit, add body */
		psnake->up_grow += rabbit_weight;
	}
	move_tail(psnake);
	/* set head */
	shs(psnake);
	if(k == RABBIT) {
		/* Generate new rabbit */
		rndrb(-RABBIT);
	}
	return 0;
}

/* moving head mashine`s snake */
static __attribute__ ((stdcall)) int
movem(int mdirection)
{
	return move_h(mdirection, &ai);
}

#define ABS(X)          (X < 0 ? -X : X)

/* AI - mashine`s "intellect" */
static void mm_AI(void)
{
	int d;

	/* Calculate minimum moving */
	d = rabbit.x - ai.head.x;
	if(ABS(d) > (X_MAX/2-1))
		d = -d;
	if(d > 0) {
		if( !movem(RIGHT) )
			return;
	} else {
		if(d && !movem(LEFT))
			return;
	}

	d = rabbit.y - ai.head.y;
	if(ABS(d) > (Y_MAX/2-1))
		d = -d;
	if(d > 0) {
		if( !movem(DOWN) )
			return;
	} else {
		if(d && !movem(UP))
			return;
	}

	/* Can`t moving to optimum direction, probing all directions */
	for(d = LEFT; d <= EAT_ITSELF; d++)
		if( !movem(d) )
			return;
	/* Can`t moving and shorting, exchange head<->tail */
	{
		POZ *ph, *pt, pch, pct;

		ph = ai.p_poz_h - 1;
		if(ph < ai.poz)
			ph = ai.poz + MAX_LENGHT - 1;
		pch = *ph;
		curs(pch.x, pch.y);
		printf("\033[3%s", ai.body_color);
		pt  = ai.p_poz_t;
		pct = *pt;
		curs(pct.x, pct.y);
		*ph = pct;
		ai.head = pct;
		*pt = pch;
		printf("\033[31m@");
	}
}

static void draw_box(void)
{
	int i;

	/* clear and set bold */
	printf("\033[H\033[J\033[1m");

	for(i=0; i <= (Y_MAX-1); i++) {
		int c = '*';

		curs(0, i);
		if(i > 0 && i < (Y_MAX-1)) {
			c = 'I';
			putchar(c);
			curs(X_MAX-1, i);
		} else {
			int j;

			putchar(c);
			for(j=0; j < (X_MAX-2); j++)
				putchar('=');
		}
		putchar(c);
	}
}

/* rabbit`s jump */
static __attribute__ ((stdcall)) void
mover(int mdirection)
{
	int x;
	int y;
	unsigned char k;

	x = rabbit.x;
	y = rabbit.y;

	switch(mdirection) {
		case DOWN:
			if(y != (Y_MAX-2))
				y++;
			 else
				y = 1;
			 break;
		case LEFT:
			if(x != 1)
				x--;
			 else
				x = X_MAX-2;
			break;
		case RIGHT:
			if(x != (X_MAX-2))
				x++;
			 else
				x = 1;
			break;
		case UP:
			if(y != 1)
				y--;
			 else
				y = Y_MAX-2;
			break;
	}

	k = field[x][y];
	if(k == EMPTY || k == BUSH) {
		field[rabbit.x][rabbit.y] = EMPTY;
		curs(rabbit.x, rabbit.y);
		putchar(' ');
		rabbit.x = x;
		rabbit.y = y;
		field[x][y] = RABBIT;
		curs(x, y);
		/* rabbit grow +1/log(1+rabbit_weight) if eat bush */
		if(k == BUSH && rabbit_weight < 9 && rand_n(rabbit_weight) == 1)
			rabbit_weight++;
		printf("\033[35m%c", rabbit_weight + '0');
	}
}

/* rabbit`s intellect */
static __attribute__ ((stdcall)) void
ri(void)
{
	int hypotenuse[MAX_BUSH], d, n, i;

	/* Find direction for rabbit jumping */
	for(i = 0; i < MAX_BUSH; i++) {
		d = bushes[i].x - rabbit.x;
		hypotenuse[i] = d*d;
		d = bushes[i].y - rabbit.y;
		hypotenuse[i] += d*d;
	}
	d = X_MAX*X_MAX + Y_MAX*Y_MAX; /* max hypotenuse */
	/* find min hypotenuse */
	for(n = i = 0; i < MAX_BUSH; i++)  {
		if(hypotenuse[i] < d) {
			d = hypotenuse[i];
			n = i;
		}
	}
	/* n is near bush for rabbit */
	d = bushes[n].x - rabbit.x;
	if(ABS(d) > (X_MAX/2-1))
		d = -d;
	if(d > 0) {
		mover(RIGHT);
	} else {
		if(d)
			mover(LEFT);
	}
	d = bushes[n].y - rabbit.y;
	if(ABS(d) > (Y_MAX/2-1))
		d = -d;
	if(d > 0) {
		mover(DOWN);
	} else {
		if(d)
			mover(UP);
	}
	if(bushes[n].x == rabbit.x  &&  bushes[n].y == rabbit.y)
		rndrb(n);       /* generate new random bush */
}


int main(void)
{
	int i;

	signal(SIGINT, sigend);
	signal(SIGQUIT, sigend);

	tcgetattr(0, &old_term_modes);
	new_term_modes = old_term_modes;
	new_term_modes.c_cc[VMIN] = 1;
	new_term_modes.c_cc[VTIME] = 0;
	new_term_modes.c_lflag &= ~(ECHO | ECHOE | ECHOK | ICANON);
	tcsetattr(0, TCSADRAIN, &new_term_modes);

	srand(time(0));

	draw_box();

	for(i=0; i<2; i++) {
		SNAKE *snakes = i==0 ? &human : &ai;
		/* set score */
		move_tail(snakes);
		/* set body "image" to place for score */
		snakes->poz[MAX_LENGHT - 1].x = snakes->score_x_pos - 1;
		/* Set initial snake - head only */
		shs(snakes);
	}

	/* Generation bushes */
	for(i=0; i < MAX_BUSH; i++)
		rndrb(i);

	/* Generate first rabbit */
	rndrb(-RABBIT);
	i = 4;  /* rabit wait 3 move before jumping */
	/* Game loop */
	for(;;) {
		if((human.length/(MAX_LENGHT/MAX_LEVEL)) > level)
			level++;
		if(! --i) {
			/* rabbit jumping */
			ri();
			i = 4;
		}
		/* Mashine`s moving */
		mm_AI();
		/* Setup human`s moving */
		curs(human.head.x, human.head.y);
		get_and_delay();
		/* human`s moving */
		move_h(direction, &human);
	}
}
