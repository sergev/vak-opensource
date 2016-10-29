/*
 * Kalah game: http://en.wikipedia.org/wiki/Kalah
 * Implemented to work on standard ANSI displays.
 *
 * Copyright (C) 1993-2007 Serge Vakulenko <vak@cronyx.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define ANAME		"Genie"	/* name of machine player */
#define BNAME		"You"	/* name of user player */
#define NH		6	/* number of pits */
#define NSTONES		6	/* number of stones in pit */
#define NBLINKS		2	/* number of blinks */

#define FIELDLINE	9
#define PROMPTLINE	17
#define MESGLINE	5

#define CLEAR()		printf ("\33[J")
#define CLEARLINE()	printf ("\33[K")
#define CURSOR(r,c)	printf ("\33[%d;%dH", (r)+1, (c)+1)
#define INVERSE()	printf ("\33[7m")
#define NORMAL()	printf ("\33[m")

int apit [NH+1];		/* machine pits and store */
int bpit [NH+1];		/* man pits and store */

char movestring [80];
char *moveptr;

int msg;			/* 1 if message is on screen */
int level = 3;			/* difficulty level */
int user_level = -1;		/* for playing computer against computer */

/*
 * Print prompt and get reply.
 * Return 0 if reply is 'no'
 * or 1 if reply is 'yes' or empty.
 * Repeat question if reply is not clear.
 */
int yes (char *prompt)
{
	char reply [80], *p;

	for (;;) {
		CURSOR (PROMPTLINE, 0);
		printf (prompt);
		CLEARLINE ();
		fflush (stdout);
		if (! fgets (reply, sizeof(reply), stdin))
			continue;
		for (p=reply; *p==' ' || *p=='\t'; ++p);
		switch (*p) {
		case 0:
		case 'y': case 'Y':
			return (1);
		case 'n': case 'N':
		case 'q': case 'Q':
			return (0);
		}
	}
}

void clear_message ()
{
	CURSOR (MESGLINE, 0);
	CLEARLINE ();
	msg = 0;
}

void message (char *fmt, ...)
{
        va_list ap;

	if (msg)
		clear_message ();
	else
		CURSOR (MESGLINE, 0);
        va_start (ap, fmt);
	vprintf (fmt, ap);
        va_end (ap);
	msg = 1;
}

void draw_game (char *aname, int *ap, char *bname, int *bp)
{
	int i;
	static char *level_name [] = {
		"0 >>", "I >>", "II >>", "III >>", "IV >>",
		"V >>", "VI >>", "VII >>", "VIII >>", "IX >>",
	};

	CURSOR (0, 0);
	CLEAR ();
	msg = 0;
	printf ("\n\t%-15s<< Level %-7s%15s\n",
		aname, level_name [level], bname);
	printf ("\t~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	CURSOR (FIELDLINE-2, 8);
	printf ("   6       5       4       3       2       1");
	CURSOR (FIELDLINE-1, 8);
	printf ("----------------------------------------------");
	for (i=0; i<NH; ++i) {
		CURSOR (FIELDLINE, 8 + 8 * (NH-1-i));
		printf ("( %2d )", ap [i]);
	}
	CURSOR (FIELDLINE+2, 0);
	printf ("( %2d )", ap [NH]);
	CURSOR (FIELDLINE+2, 8+8*NH);
	printf ("( %2d )", bp [NH]);
	CURSOR (FIELDLINE+5, 8);
	printf ("----------------------------------------------");
	CURSOR (FIELDLINE+6, 8);
	printf ("   1       2       3       4       5       6");
	for (i=0; i<NH; ++i) {
		CURSOR (FIELDLINE+4, 8 + 8*i);
		printf ("( %2d )\t", bp [i]);
	}
}

int all_pits_empty (int *pit)
{
	int i;

	for (i=0; i<NH; ++i)
		if (*pit++)
			return (0);
	return (1);
}

void draw_move (int r, int c, int old, int new)
{
	int i;

	switch (r) {
	case 'a':
		r = FIELDLINE;
		c = 8 + 8 * (NH - 1 - c);
		break;
	case 'k':
		r = FIELDLINE+2;
		c = c=='b' ? 8+8*NH : 0;
		break;
	case 'b':
		r = FIELDLINE+4;
		c = 8 + 8*c;
		break;
	default:
		printf ("bad draw_move %c\n", r);
		exit (-1);
	}
	for (i=0; i<NBLINKS; ++i) {
		CURSOR (r, c);
		INVERSE ();
		printf ("( %2d )", old);
		fflush (stdout);
		usleep (200000L / NBLINKS);
		NORMAL ();
		CURSOR (r, c);
		printf ("( %2d )", old);
		fflush (stdout);
		usleep (200000L / NBLINKS);
	}
	CURSOR (r, c);
	printf ("( %2d )", new);
}

void draw_empty (int x, int *pit)
{
	int m;

	m = -1;
	while (++m < NH) {                     /* my pits */
		if (! pit [m])
			continue;
		pit [NH] += pit [m];
		draw_move (x, m, pit [m], 0);
	}
	draw_move ('k', x, pit [NH], pit [NH]);
}

/*
 * Perform a move, change values in mypit[] and herpit[].
 * Parameter move contains a pit number: 0..5.
 * When x is nonzero, draw a move on the screen.
 */
int make_move (int x, int move, int *mypit, int *herpit)
{
	int c;

	c = mypit [move];			/* get stones */
	mypit [move] = 0;
	if (x)
		draw_move (x, move, c, 0);
loop:
	while (++move < NH) {			/* my pits */
		++ mypit [move];
		if (x)
			draw_move (x, move, mypit [move], mypit [move]);
		if (--c < 1) {
			if (mypit [move] == 1 && herpit [NH-1-move]) {
				/* Last pit was empty, capture stones. */
				if (x) {
					if (x == 'a')
						message (rand() & 1 ?
							"I like eating your stones..." :
							"Your stones are so sweet!");
					draw_move (x^3, NH-1-move,
						herpit [NH-1-move], 0);
					draw_move (x, move, 0, 0);
				}
				mypit [NH] += herpit [NH-1-move] + 1;
				herpit [NH-1-move] = 0;
				mypit [move] = 0;
				if (x)
					draw_move ('k', x, mypit [NH], mypit [NH]);
			}
			return (1);
		}
	}
	++mypit [NH];				/* my store */
	if (x)
		draw_move ('k', x, mypit [NH], mypit [NH]);
	if (--c < 1)
		return (0);
	move = -1;
	while (++move < NH) {			/* opposite pits */
		++ herpit [move];
		if (x)
			draw_move (x^3, move, herpit [move], herpit [move]);
		if (--c < 1)
			return (1);
	}
	move = -1;
	goto loop;
}

int user_move ()
{
	for (;;) {
		while (! *moveptr) {
			moveptr = movestring;
			CURSOR (PROMPTLINE, 0);
			printf ("Your move: ");
			CLEARLINE ();
			fflush (stdout);
			if (! fgets (movestring, sizeof(movestring), stdin))
				continue;
			if (msg)
				clear_message ();
		}
		CURSOR (PROMPTLINE, 0);
		CLEARLINE ();
		switch (*moveptr++) {
		case 'r': case 'R':
			draw_game (ANAME, apit, BNAME, bpit);
			continue;

		case 'q': case 'Q':
			exit (0);

		case '1': case '2': case '3': case '4': case '5': case '6':
			return (moveptr [-1] - '1');
		}
	}
}

/*
 * Estimate who is winning.
 */
int estimate (int *mypit, int *herpit)
{
	int score, i, n;
	static int weight[13] = { 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2 };

	/* Approximate a number of stones, available for every player.
	 * Assume we have distributed all stones according to game rules.
	 * Count every player pit. */
	score = 0;
	for (i=0; i<NH; ++i) {
		n = mypit[i] + i;
		score += n/13 + weight [n%13] - i;
	}
	for (i=0; i<NH; ++i) {
		n = herpit[i] + i;
		score -= n/13 + weight [n%13] - i;
	}

	/* Then, add kalahs. */
	score = score/2 + mypit [NH] - herpit [NH];
	return score;
}

/*
 * Given the board position, find the best move for the player
 * after looking ahead depth moves. The move is returned as function value,
 * and its score is returned in "score".
 */
int find_move (int *mypit, int *herpit, int *score, int depth)
{
	int move, best, s, player_changed, i;
	int apit_copy [NH+1], bpit_copy [NH+1];

	/*printf ("find_move (depth=%d) called\n", depth);*/
	/* Initial value is the lower limit of the acceptable range. */
	*score = mypit [NH] - herpit [NH];

	/* Some heuristics. */
	if (mypit [5] == 1) {
		++*score;
		return (5);
	}
	if (mypit [5] == 0 && mypit [4] == 2) {
		++*score;
		return (4);
	}

	/* Run through possible moves, stopping if a cutoff occurs.
	 * A cutoff occurs when your opponent has a better choice
	 * somewhere above you in the tree. */
	best = -1;
	for (move=0; move<NH; ++move) {
		if (! mypit [move])
			continue;

		/* This is a legal move. */
		if (best < 0)
			best = move;

		/* Make a scratch copy of board. */
		memcpy (apit_copy, mypit, sizeof(apit_copy));
		memcpy (bpit_copy, herpit, sizeof(bpit_copy));

		/* Find the value of this board by evaluating if game over
		 * or looking ahead if not. */
		player_changed = make_move (0, move, apit_copy, bpit_copy);
		if (all_pits_empty (apit_copy)) {
			/* Game finished, so just compare kalahs. */
			s = apit_copy [NH] - bpit_copy [NH];
			for (i=0; i<NH; ++i)
				s -= bpit_copy [i];

		} else if (! player_changed) {
			/* Player did not change, so move without
			 * changing depth. */
			find_move (apit_copy, bpit_copy, &s, depth);

		} else if (depth > 0) {
			/* Player changed, so reduce search depth. */
			find_move (bpit_copy, apit_copy, &s, depth - 1);

			/* Good for him is bad for me. */
			s = -s;
		} else {
			/* Depth exhausted, so estimate who is winning. */
			s = estimate (apit_copy, bpit_copy);
		}
		if (s > *score || (s == *score && (rand() & 1))) {
			*score = s;
			best = move;
		}
	}
	/*printf ("find_move (depth=%d) returned %d, score %d\n", depth, best, *score);*/
	return best;
}

void random_message ()
{
	static char *tab [] = {
		"My turn: I'm thinking.",
		"Well, well...",
		"Now I will beat you.",
		"Surrender!",
		"Oh-oh.",
		"You have no chance.",
		"You have tired me.",
		"Do you think you are a Grandmaster?",
	};
	int n;

	n = (unsigned) rand() % (sizeof(tab) / sizeof(tab[0]));
	message (tab [n]);
}

void play_game ()
{
	int i, s;

	for (i=0; i<NH; ++i)
		apit [i] = bpit [i] = NSTONES;
	apit [NH] = bpit [NH] = 0;
	draw_game (ANAME, apit, BNAME, bpit);
	for (;;) {
		moveptr = movestring;
		*moveptr = 0;
		do {
			if (all_pits_empty (bpit)) {
				draw_empty ('a', apit);
				return;
			}
			if (user_level < 0) {
				/* Human player. */
				i = user_move ();
				if (! bpit [i]) {
					message ("Cell %d is empty.", i);
					continue;
				}
			} else {
				/* Computer against computer. */
				CURSOR (PROMPTLINE, 0);
				printf ("Thinking...");
				CLEARLINE ();
				fflush (stdout);
				i = find_move (bpit, apit, &s, user_level);
				CURSOR (PROMPTLINE, 0);
				printf ("Your move is %d", i+1);
				CLEARLINE ();
				fflush (stdout);
				usleep (1000000);
			}
		} while (! make_move ('b', i, bpit, apit));

		do {
			if (all_pits_empty (apit)) {
				draw_empty ('b', bpit);
				return;
			}
			random_message ();
			fflush (stdout);
			usleep (2000000);
			i = find_move (apit, bpit, &s, level);
			message ("My move is %d", i+1);
		} while (! make_move ('a', i, apit, bpit));
	}
}

void usage ()
{
	printf ("Kalah Genie game, Copyright (C) Serge Vakulenko\n");
	printf ("Usage:\n");
	printf ("\tkalah [level] [user-level]\n");
	printf ("Level:\n");
	printf ("\t0\t- trivial\n");
	printf ("\t1\t- easy\n");
	printf ("\t2\t- novice\n");
	printf ("\t3\t- intermediate (default)\n");
	printf ("\t4\t- expert\n");
	printf ("\t5\t- master\n");
	printf ("\t6\t- genius\n");
	printf ("\t...\n");
	printf ("\t9\t- Kasparov\n");
	exit (-1);
}

int main (int argc, char **argv)
{
	if (argc > 3)
		usage ();
	if (argc > 2) {
		if (argv[2][0] < '0' || argv[2][0] > '9')
			usage ();
		user_level = atoi (argv[2]);
		if (user_level < 0 || user_level > 9)
			usage ();
	}
	if (argc > 1) {
		if (argv[1][0] < '0' || argv[1][0] > '9')
			usage ();
		level = atoi (argv[1]);
		if (level < 0 || level > 9)
			usage ();
	}
	srand ((int) time ((long) 0));

	for (;;) {
		play_game ();
		if (apit [NH] != bpit [NH])
			message ("%s won.\n",
				apit [NH] > bpit [NH] ? ANAME : BNAME);
		if (user_level >= 0 || ! yes ("Next game? "))
			break;
	}
	CURSOR (PROMPTLINE, 0);
	CLEAR ();
	return (0);
}
