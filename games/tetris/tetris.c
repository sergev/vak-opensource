/*
 * Tetris for BSD/386
 *
 * (C) Copyright 1995, Vadim Antonov
 * All Rights Reserved.
 *
 * Permission is granted hereby for the unlimited
 * usage, modification and redistribution.
 *
 * THIS SOFTWARE IS PROVIDED AS-IS.
 * The author cannot be held liable for any damage or
 * loss suffered as a consequence of the use of this
 * software; including but not limited to repetitive
 * stress injury and/or moral damage acquired because
 * of inability to win the game.
 */
#define USE_POSIXTTY

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>

#ifdef USE_SGTTY
#include <sgtty.h>
#else
#ifdef USE_POSIXTTY
#include <termios.h>
#else
#include "Other terminal interfaces aren't supported"
#endif
#endif

/* PC console colors */
enum pccolors {
	BLACK, RED, GREEN, BROWN, BLUE, MAGENTA, CYAN, WHITE,
	GREY, LT_RED, LT_GREEN, YELLOW, LT_BLUE, LT_MAGENTA, LT_CYAN, HI_WHITE
};

#ifndef DEBUG
#  define SCORE_4	"/usr/games/lib/tetris.scores"
#  define SCORE_5	"/usr/games/lib/pentis.scores"
#else
#  define SCORE_4	"tetris.scores"
#  define SCORE_5	"pentis.scores"
#endif

#define BORDER_COL	GREEN
#define DOT_COL		BROWN

#define TOPS		10

#define PITY		25
#define PITWIDTH	12
#define PITDEPTH	24

#define TOPSLINE        12      /* tops start at that line */

#define NPIECES		6

struct tops {
	char    name[10];
	int     score;
	int     class;
} tops[TOPS];

struct coord {
	int     x, y;
	int     color;
};

#define END		127

#define NTYPES_4	7
#define NTYPES_5	24

struct tet {
	char    color;
	char    dx, dy;
	struct cc { char x, y; } p[NPIECES];
} tet[NTYPES_5] = {
	/* OOOO */
	{ HI_WHITE,   0, 3, { {0,0}, {0,1}, {0,2}, {0,3}, {END,END} } },

	/* O    O    O  OO OO */
	/* OOO OOO OOO OO   OO */
	{ YELLOW,     1, 2, { {0,0}, {1,0}, {1,1}, {1,2}, {END,END} } },
	{ LT_MAGENTA, 1, 2, { {0,1}, {1,0}, {1,1}, {1,2}, {END,END} } },
	{ LT_CYAN,    1, 2, { {0,2}, {1,0}, {1,1}, {1,2}, {END,END} } },
	{ LT_GREEN,   1, 2, { {0,0}, {0,1}, {1,1}, {1,2}, {END,END} } },
	{ LT_RED,     1, 2, { {0,1}, {0,2}, {1,0}, {1,1}, {END,END} } },

	/* OO */
	/* OO */
	{ LT_BLUE,    1, 1, { {0,0}, {0,1}, {1,0}, {1,1}, {END,END} } },

	/* OOOOO */
	{ WHITE,      0, 4, { {0,0}, {0,1}, {0,2}, {0,3}, {0,4}, {END,END} } },

	/* O     O     O     O  OOO OO   */
	/* OOOO OOOO OOOO OOOO OO    OOO */
	{ BROWN,      1, 3, { {0,0}, {1,0}, {1,1}, {1,2}, {1,3}, {END,END} } },
	{ CYAN,       1, 3, { {0,1}, {1,0}, {1,1}, {1,2}, {1,3}, {END,END} } },
	{ MAGENTA,    1, 3, { {0,2}, {1,0}, {1,1}, {1,2}, {1,3}, {END,END} } },
	{ BLUE,       1, 3, { {0,3}, {1,0}, {1,1}, {1,2}, {1,3}, {END,END} } },
	{ GREEN,      1, 3, { {0,1}, {0,2}, {0,3}, {1,0}, {1,1}, {END,END} } },
	{ RED,        1, 3, { {0,0}, {0,1}, {1,1}, {1,2}, {1,3}, {END,END} } },

	/* OO  O O  OO */
	/* OOO OOO OOO */
	{ LT_MAGENTA, 1, 2, { {0,0}, {0,1}, {1,0}, {1,1}, {1,2}, {END,END} } },
	{ LT_GREEN,   1, 2, { {0,0}, {0,2}, {1,0}, {1,1}, {1,2}, {END,END} } },
	{ LT_CYAN,    1, 2, { {0,1}, {0,2}, {1,0}, {1,1}, {1,2}, {END,END} } },

	/* O    O    O O    O  O    O   */
	/* OOO OOO OOO OOO OOO OOO  O   */
	/* O   O   O    O   O    O  OOO */
	{ LT_RED,     2, 2, { {0,0}, {1,0}, {1,1}, {1,2}, {2,0}, {END,END} } },
	{ MAGENTA,    2, 2, { {0,1}, {1,0}, {1,1}, {1,2}, {2,0}, {END,END} } },
	{ GREEN,      2, 2, { {0,2}, {1,0}, {1,1}, {1,2}, {2,0}, {END,END} } },
	{ CYAN,       2, 2, { {0,0}, {1,0}, {1,1}, {1,2}, {2,1}, {END,END} } },
	{ YELLOW,     2, 2, { {0,1}, {1,0}, {1,1}, {1,2}, {2,1}, {END,END} } },
	{ RED,        2, 2, { {0,0}, {1,0}, {1,1}, {1,2}, {2,2}, {END,END} } },
	{ LT_BLUE,    2, 2, { {0,0}, {1,0}, {2,0}, {2,1}, {2,2}, {END,END} } },
};

char pit[PITDEPTH+1][PITWIDTH];
char pitcnt[PITDEPTH];
int score, lines, interval, class;
int shownext;
int pentix, colored;

int mytopsline = -1;
int nexttopsscore = 0;

char user[10], ucuser[10];

#define NCLASSES 20

int classlines[NCLASSES] = {
	10, 20, 30, 40, 50,
	80, 100, 120, 140, 160,
	200, 250, 300, 350, 400,
	500, 600, 700, 800, 10000000
};

struct cc cur = {-1, -1};
int curcol = -1;
short  ospeed;

void t_setmode(), t_resetmode();

/*
 * Switch the color
 */
void color(int c)
{
	if( !colored ||  c == curcol )
		return;
	curcol = c;
	if (c < 8)
		printf("\33[0;3%dm", c);
	else
		printf("\33[1;3%dm", c & 7);
}

/*
 * Restore color
 */
void restorecolor()
{
	printf("\33[m");
}

/*
 * Position the cursor
 */
void pos(int x, int y)
{
	if( cur.x != x || cur.y != y ) {
		printf("\033[%d;%dH", x+1, y+1);
		cur.x = x;
		cur.y = y;
	}
}

/*
 * Erase the screen
 */
void erase()
{
	printf("\033c");
}

/*
 * Position the piece
 */
void ppos(struct coord p)
{
	pos(p.x, 2*p.y + PITY);
}

/*
 * Output piece coordinates given its center and angle
 */
void pdots(struct tet *t, struct cc c, int a, struct coord *res)
{
	struct cc org;
	struct coord tmp;
	int yw, xw;
	int i;

	if( a & 1 ) {   /* 90 deg */
		xw = t->dy;
		yw = t->dx;
	} else {
		xw = t->dx;
		yw = t->dy;
	}
	org = c;
	org.x -= (xw+1) / 2;
	org.y -= yw / 2;
	if( org.y < 0 )
		org.y = 0;
	if( org.y + yw >= PITWIDTH && c.y <= PITWIDTH )
		org.y = PITWIDTH-1 - yw;
	for( i = 0 ; t->p[i].x != END ; i++ ) {
		switch( a ) {
		case 0:
			res[i].x = t->p[i].x;
			res[i].y = t->p[i].y;
			break;
		case 3:
			res[i].x = xw - t->p[i].y;
			res[i].y = t->p[i].x;
			break;
		case 2:
			res[i].x = xw - t->p[i].x;
			res[i].y = yw - t->p[i].y;
			break;
		case 1:
			res[i].x = t->p[i].y;
			res[i].y = yw - t->p[i].x;
		}
		res[i].x += org.x;
		res[i].y += org.y;
		res[i].color = t->color;
	}
	res[i].x = res[i].y = END;

	do {
		xw = 0;
		for( i = 0 ; res[i+1].x != END ; i++ ) {
			if( res[i].x < res[i+1].x )
				continue;
			if( res[i].x == res[i+1].x && res[i].y <= res[i+1].y )
				continue;
			xw++;
			tmp = res[i];
			res[i] = res[i+1];
			res[i+1] = tmp;
		}
	} while( xw );
}

/*
 * Draw the piece
 */
void draw(struct coord *p)
{
	for( ; p->x != END ; p++ ) {
		if( p->x < 0 )
			continue;
		ppos(*p);
		color(p->color);
		printf("[]");
		cur.y += 2;
	}
}

/*
 * Move the piece
 */
void move(struct coord *old, struct coord *new, int dot)
{
	for(;;) {
		if( old->x == END )
			goto draw;
		if( new->x == END )
			goto clear;
		if( old->x > new->x )
			goto draw;
		if( old->x < new->x )
			goto clear;
		if( old->y > new->y )
			goto draw;
		if( old->y == new->y ) {
			old++;
			if( colored && old[-1].color != new->color )
				goto draw;
			new++;
			continue;       /* old & new at the same place */
		}
clear:          if( old->x >= 0 ) {
			ppos(*old);
			color(DOT_COL);
			printf(dot? ". ": "  ");
			cur.y += 2;
		}
		old++;
		continue;

draw:           if( new->x == END )
			break;
		if( new->x >= 0 ) {
			ppos(*new);
			color(new->color);
			printf("[]");
			cur.y += 2;
		}
		new++;
	}
}

/*
 * Medal colors
 */
int medcol(int place)
{
	switch( place ) {
	case 0:
		return YELLOW;
	case 1:
		return HI_WHITE;
	case 2:
		return LT_RED;
	}
	return WHITE;
}

/*
 * Show the top ten
 */
void showtop()
{
	int f = open(pentix? SCORE_5 : SCORE_4, 0);
	int i, j;

	if( f == -1 || read(f, tops, sizeof tops) != sizeof tops )
		bzero(tops, sizeof tops);
	close(f);

	/*
	 * Join in the current player
	 */
	for( i = 0 ; i < TOPS ; i++ ) {
		if( tops[i].name[0] == 0 || tops[i].score < score )
			break;
	}
	mytopsline = i;
	if( i == 0 )
		 nexttopsscore = 10000000;
	else
		 nexttopsscore = tops[i-1].score;

	for( i = j = 0 ; i < TOPS ; i++ ) {
		pos(TOPSLINE+i, 0);
		color(medcol(i));
		printf("%2d ", i+1);
		if( mytopsline == i )
			printf("%8.8s %2d %7d", ucuser, class+1, score);
		else if( tops[j].name[0] ) {
			printf("%8.8s %2d %7d", tops[j].name,
					tops[j].class+1, tops[j].score);
			j++;
		}
	}
}

/*
 * Save the score
 */
void savescore()
{
	char *name = pentix? SCORE_5 : SCORE_4;
	int i, j, f;

	f = open(name, 2);
	if( f == -1 || read(f, tops, sizeof tops) != sizeof tops ) {
		bzero(tops, sizeof tops);
		f = creat(name, 0666);
	}

	/*
	 * Join in the current player
	 */
	for( i = 0 ; i < TOPS ; i++ ) {
		if( tops[i].name[0] == 0 || tops[i].score < score )
			break;
	}

	if( i == TOPS ) {
		close(f);
		return;
	}
	for( j = TOPS-1 ; j > i ; j-- )
		tops[j] = tops[j-1];
	strcpy(tops[i].name, user);
	tops[i].class = class;
	tops[i].score = score;
	(void) lseek(f, 0l, 0);
	if (write(f, tops, sizeof tops) < 0)
		/*ignore*/;
	close(f);
}

/*
 * Draw the class in large friendly figures
 */
void drawclass()
{
	int i = class+1;
	static char *msd[3][3] = {
		{ "    ", "    ", " __ " },
		{ "    ", "   |", " __|" },
		{ "    ", "   |", "|__ " },
	};
	static char *lsd[3][10] = {
{ " __ ", "    ", " __ ", " __ ", "    ", " __ ", " __ ", " __ ", " __ ", " __ " },
{ "|  |", "   |", " __|", " __|", "|__|", "|__ ", "|__ ", "   |", "|__|", "|__|" },
{ "|__|", "   |", "|__ ", " __|", "   |", " __|", "|__|", "   |", "|__|", "   |" },
	};

	color(LT_GREEN);
	pos(5, 5);
	printf("%s %s", msd[0][i/10], lsd[0][i%10]);
	cur.y += 9;
	pos(6, 5);
	printf("%s %s", msd[1][i/10], lsd[1][i%10]);
	cur.y += 9;
	pos(7, 5);
	printf("%s %s", msd[2][i/10], lsd[2][i%10]);
	cur.y += 9;
}

/*
 * Switch to the next class
 */
void nextclass()
{
	if( class >= NCLASSES-1 )
		return;
	class++;
	interval -= interval / 5;
	drawclass();
	if( mytopsline < TOPS ) {
		color(medcol(mytopsline));
		pos(TOPSLINE+mytopsline, 12);
		printf("%2d", class+1);
	}
}

/*
 * The piece reached the bottom
 */
void scarp(struct coord *c)
{
	int i, nfull, j, k;
	struct coord z;

	nfull = 0;
	for( ; c->x != END ; c++ ) {
		if( c->x <= 0 ) {
			savescore();
			color(YELLOW);
			pos(9, PITY);
			printf("                        ");
			pos(10, PITY);
			printf(" ===== GAME OVER ====== ");
			pos(11, PITY);
			printf("                        ");
			pos(25, 0);
			restorecolor();
			t_resetmode();
			exit(0);
		}
		pit[c->x][c->y] = colored? c->color : 1;
		if( ++pitcnt[c->x] == PITWIDTH )
			nfull++;
	}
			/* Remove the full lines */
	if( nfull ) {
		/* Clear upper nfull lines */
		for( i = 0 ; i < nfull ; i++ ) {
			for( j = 0 ; j < PITWIDTH ; j++ ) {
				if( pit[i][j] ) {
					z.x = i; z.y = j;
					ppos(z);
					color(DOT_COL);
					printf(". ");
				}
			}
		}

		/* Move lines down */
		k = nfull;
		for( i = nfull ; i < PITDEPTH && k > 0; i++ ) {
			if( pitcnt[i-k] == PITWIDTH ) {
				k--, i--;
				continue;
			}
			for( j = 0 ; j < PITWIDTH ; j++ ) {
				if( pit[i][j] != pit[i-k][j] ) {
					z.x = i; z.y = j;
					ppos(z);
					if( pit[i-k][j] ) {
						color(pit[i-k][j]);
						printf("[]");
					} else {
						color(DOT_COL);
						printf(". ");
					}
				}
			}
		}

		/* Now fix the pit contents */
		for( i = PITDEPTH-1 ; i > 0 ; i-- ) {
			if( pitcnt[i] != PITWIDTH )
				continue;
			bcopy(pit[0], pit[0]+PITWIDTH, PITWIDTH * i);
			bzero(pit[0], PITWIDTH);
			bcopy(pitcnt, pitcnt+1, i);
			pitcnt[0] = 0;
			i++;
		}

		/* Update line count */
		pos(1, 7);
		lines += nfull;
		color(HI_WHITE);
		printf("%7d", lines);
		cur.y += 7;
		if( pitcnt[PITDEPTH-1] == 0 )
			score += 100 * class * class;
		if( lines >= classlines[class] )
			nextclass();
	}

	score += shownext? (class/2 + 1) : class;
	pos(2, 7);
	color(HI_WHITE);
	printf("%7d", score);
	cur.y += 7;
	if( score > nexttopsscore )
		showtop();
	else if( mytopsline < TOPS ) {
		color(medcol(mytopsline));
		pos(TOPSLINE+mytopsline, 15);
		printf("%7d", score);
	}
}

/*
 * The main routine
 */
int main(ac, av)
	int ac;
	char **av;
{
	int ptype, nextptype;            /* Piece type */
	int angle, nextangle, anew;      /* Angle */
	struct cc c, cnew, z;
	struct coord *cp;
	struct coord old[NPIECES], new[NPIECES], chk[NPIECES];
	struct coord nextold[NPIECES], nextnew[NPIECES];
	int i, j, sclass;
	char cc;
	long restusec;
	struct timeval tv;
	fd_set fds;
	long t;
	char *p;
	int rotf;       /* direction of rotation */

	time(&t);
	srand((int)t);
	shownext = 0;
	sclass = 0;
	rotf = 0;
	p = getlogin();
	if( p == 0 )
		p = "@#$!!#!";
	strncpy(user, p, 9);
	strcpy(ucuser, user);
	p = ucuser;
	while( *p ) {
		if( 'a' <= *p && *p <= 'z' )
			*p -= 'a' - 'A';
		p++;
	}

	while( (i = getopt(ac, av, "cpnr0123456789")) != EOF ) {
		switch( i ) {
		case 'p':
			pentix ^= 1;
			break;
		case 'n':
			shownext ^= 1;
			break;
		case 'c':
			colored ^= 1;
			break;
		case 'r':
			rotf ^= 1;
			break;
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			sclass = (sclass*10) + (i-'0');
			break;
		default:
			fprintf(stderr, "Usage: %s [-cpnr] [-CLASS]\n", av[0]);
			fprintf(stderr, "\t-c - turn on color\n");
			fprintf(stderr, "\t-p - throw pentagramms in\n");
			fprintf(stderr, "\t-n - start with hints display on\n");
			fprintf(stderr, "\t-r - change the direction of rotation\n");
			fprintf(stderr, "\t-CLASS - start from the given class\n");
			exit(1);
		}
	}
	if( sclass > 0 )
		sclass--;
	interval = 500;
	for( i = sclass ; i-- ; )
		interval -= interval / 5;

	t_setmode();

	/* Draw the pit */
	erase();
	pos(1, 0);
	color(WHITE);
	printf("Lines:       ");
	color(HI_WHITE);
	printf("0");
	cur.y += 14;
	pos(2, 0);
	color(WHITE);
	printf("Score:       ");
	color(HI_WHITE);
	printf("0");
	cur.y += 14;
	score = 0;
	lines = 0;
	class = sclass;
	pos(4, 5);
	color(WHITE);
	printf("C L A S S");
	cur.y += 9;
	drawclass();
	pos(3, PITY + 12 + 2*PITWIDTH);
	color(WHITE);
	printf("N E X T");
	cur.y += 7;

	pos(10, 0);
	printf("  === The Top Ten ===");
	pos(11, 0);
	  /*   ^dd nnnnnnnn dd ddddddd */
	printf(" #     name cl   score");
	showtop();

	color(WHITE);
	pos(10, PITY + 5 + 2*PITWIDTH);
	printf("    COMMANDS");
	pos(12, PITY + 5 + 2*PITWIDTH);
	printf("q - quit");
	pos(13, PITY + 5 + 2*PITWIDTH);
	printf("4 - toggle hint display");
	pos(14, PITY + 5 + 2*PITWIDTH);
	printf("5 or space - drop");
	pos(15, PITY + 5 + 2*PITWIDTH);
	printf("6 - increase class");
	pos(16, PITY + 5 + 2*PITWIDTH);
	printf("7 - move left");
	pos(17, PITY + 5 + 2*PITWIDTH);
	printf("8 - rotate");
	pos(18, PITY + 5 + 2*PITWIDTH);
	printf("9 - move right");

	color(BORDER_COL);
	for( i = 0 ; i < PITDEPTH ; i++ ) {
		pos(i, PITY-2);
		printf("##");
		color(DOT_COL);
		for( j = 0 ; j < PITWIDTH ; j++ ) {
			printf(". ");
			pit[i][j] = 0;
		}
		pitcnt[i] = 0;
		color(BORDER_COL);
		printf("##");
		cur.y += 2*PITWIDTH + 4;
	}
	pos(PITDEPTH+1, PITY-2);
	for( j = 0 ; j < PITWIDTH+2 ; j++ )
		printf("##");
	cur.y += 2*PITWIDTH + 4;
	for( j = 0 ; j < PITWIDTH ; j++ )
		pit[PITDEPTH][j] = 1;

	if( !pentix )
		nextptype = ((rand()>>2) & 07777) % NTYPES_4;
	else {
		nextptype = ((rand()>>2) & 07777) % (2*NTYPES_4 + NTYPES_5);
		if( nextptype >= NTYPES_4 )
			nextptype -= NTYPES_4;
		if( nextptype >= NTYPES_4 )
			nextptype -= NTYPES_4;
	}
	nextangle = (rand()>>3) % 03;
	for( i = 0 ; i < NPIECES ; i++ )
		nextold[i].x = nextold[i].y = END;

newpiece:
	ptype = nextptype;
	angle = nextangle;
	if( !pentix )
		nextptype = ((rand()>>2) & 07777) % NTYPES_4;
	else {
		nextptype = ((rand()>>2) & 07777) % (2*NTYPES_4 + NTYPES_5);
		if( nextptype >= NTYPES_4 )
			nextptype -= NTYPES_4;
		if( nextptype >= NTYPES_4 )
			nextptype -= NTYPES_4;
	}
	nextangle = (rand()>>3) % 03;

	c.y = PITWIDTH/2 - 1;
	for( c.x = -2 ;; c.x++ ) {
		pdots(&tet[ptype], c, angle, new);
		for( cp = new ; cp->x != END ; cp++ ) {
			if( cp->x >= 0 )
				goto ok;
		}
	}
ok:
	draw(new);
	bcopy(new, old, sizeof old);

	if( shownext ) {
		z.x = 6;
		z.y = PITWIDTH + 7;
		pdots(&tet[nextptype], z, nextangle, nextnew);
		move(nextold, nextnew, 0);
		bcopy(nextnew, nextold, sizeof nextold);
	}

	restusec = interval * 1000;
	for(;;) {
		cnew = c;
		anew = angle;
		pos(0, 0);
		fflush(stdout);

		FD_ZERO(&fds);
		FD_SET(0, &fds);
		tv.tv_sec = 0;
		tv.tv_usec = restusec;

		if( select(1, &fds, 0, 0, &tv) == 0 ) {
			restusec = interval * 1000;
			cnew.x++;
			pdots(&tet[ptype], cnew, anew, chk);
			for( cp = chk ; cp->x != END ; cp++ ) {
				if( cp->x < 0 )
					continue;
				if( pit[cp->x][cp->y] ) {
					scarp(new);
					goto newpiece;
				}
			}
			goto check;
		}
		if (read(0, &cc, 1) < 0)
			exit(0);
		restusec /= 2;
		switch( cc ) {
		case 'q':
		case 'Q':
			restorecolor();
			pos(25, 0);
			savescore();
			t_resetmode();
			exit(0);
		case '4':
			if( shownext ) {
				for( i = 0 ; i < NPIECES ; i++ )
					nextold[i].x = nextold[i].y = END;
				move(nextnew, nextold, 0);
				shownext = 0;
			} else {
				z.x = 6;
				z.y = PITWIDTH + 7;
				pdots(&tet[nextptype], z, nextangle, nextnew);
				draw(nextnew);
				bcopy(nextnew, nextold, sizeof nextold);
				shownext = 1;
			}
			continue;

		case '6':
			nextclass();
			continue;

		case '8':
			if( rotf ) {
				if( ++anew > 3 )
					anew = 0;
			} else {
				if( --anew < 0 )
					anew = 3;
			}
			break;

		case '7':
			if( cnew.y <= 0 )
				goto out;
			cnew.y--;
			break;

		case '9':
			if( cnew.y >= PITWIDTH-1 )
				goto out;
			cnew.y++;
			break;

		case ' ':
		case '5':
			for(;;) {
				cnew.x++;
				pdots(&tet[ptype], cnew, anew, chk);
				for( cp = chk ; cp->x != END ; cp++ ) {
					if( cp->x < 0 )
						continue;
					if( pit[cp->x][cp->y] ) {
						cnew.x--;
						pdots(&tet[ptype], cnew, anew, chk);
						move(new, chk, 1);
						scarp(chk);
						goto newpiece;
					}
				}
				score += 1 + (class/5);
			}
			/* NOTREACHED */

		}
		pdots(&tet[ptype], cnew, anew, chk);
	check:
		for( cp = chk ; cp->x != END ; cp++ ) {
			if( cp->y < 0 || cp->y >= PITWIDTH )
				goto out;
		}
		for( cp = chk ; cp->x != END ; cp++ ) {
			if( cp->x < 0 )
				continue;
			if( pit[cp->x][cp->y] )
				goto out;
		}
		c = cnew;
		angle = anew;
		bcopy(new, old, sizeof old);
		bcopy(chk, new, sizeof new);
		move(old, new, 1);
	out:    ;
	}
}

/*
 * Set/reset terminal modes
 */
#ifdef USE_SGTTY
static struct sgttyb t_saved_mode, t_mode;

void t_setmode()
{
	ioctl(0, TIOCGETP, &t_saved_mode);
	t_mode = t_saved_mode;
	t_mode.sg_flags &= ~(ECHO|XTABS|CRMOD);
        t_mode.sg_flags |= CBREAK;
	ospeed = mode.sg_ospeed;
        ioctl(0, TIOCSETP, &t_mode);
}

void t_resetmode()
{
	ioctl(2, TIOCSETP, &t_saved_mode);
}

#else
#ifdef USE_POSIXTTY
static struct termios t_saved_mode, t_mode;

void t_setmode()
{
	(void) tcgetattr(0, &t_saved_mode);
	t_mode = t_saved_mode;
	t_mode.c_lflag &= ~(ICANON|ECHO|ISIG|IEXTEN);
        t_mode.c_iflag &= ~(ICRNL|INLCR|IXON|IXOFF);
        t_mode.c_oflag &= ~OPOST;
        t_mode.c_cc[VMIN] = 1;
        t_mode.c_cc[VTIME] = 0;
        (void) tcsetattr(0, TCSADRAIN, &t_mode);
#ifdef CBAUD
	ospeed = t_mode.c_cflag & CBAUD;
#else
	ospeed = t_mode.c_ospeed;
#endif
}

void t_resetmode()
{
	(void) tcsetattr(0, TCSADRAIN, &t_saved_mode);
}
#else

/* NOT SUPPORTED */

#endif	/* USE_POSIXTTY */
#endif  /* USE_SGTTY */
