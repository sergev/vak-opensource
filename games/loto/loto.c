#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define rnd()  ((rand()&077777) / (32.0*1024))
#define NPLAY 6
#define NCUBE 5

char name [NPLAY] [100];
int num, player, nround, delta;
int scale [NPLAY];
int cube [NCUBE];
int touched [NPLAY];
int weight [7] = { 0, 10, 2, 3, 4, 5, 6, };

char *z1 [] = {
	"первого",
	"второго",
	"третьего",
	"четвертого",
	"пятого",
	"шестого",
	"седьмого",
	"восьмого",
	"девятого",
	"десятого",
};

void printround ()
{
	int i;

	printf ("\nЭтап %d.%d\t\t", nround, player);
	for (i=0; i<num; i++)
		printf ("%7.7s\t", name[i]);
	printf ("\nВ игре %s.\t       -", name [player]);
	for (i=0; i<num; i++)
		printf ("--------");
	printf ("-\n\t\t\t");
	for (i=0; i<num; i++)
		printf ("%7d\t", scale[i]);
	printf ("\n\n");
}

int getcha ()
{
	int c;

	c = getchar ();
	if (c == EOF) exit (0);
	return (c);
}

int getch ()
{
	int c, s;

	while ((c = getcha ()) == ' ' || c=='\t');
	s = c;
	while (c != '\n')
		c = getcha ();
	return (s);
}

void printrez ()
{
	int i, av;

	printf ("\nСчет:\n\n");
	av = 0;
	for (i=0; i<num; i++)
		av += scale [i];
	av /= num;
	for (i=0; i<num; i++)
		printf ("\t%6d\t%s\n", scale[i]-av, name[i]);
}

int yes ()
{
	int c;

	for (;;) {
		printf ("Продолжать? ");
		c = getch ();
		if (c=='\n' || c=='y' || c=='Y')
			return (1);
		if (c=='n' || c=='N')
			return (0);
		if (c=='k' || c=='K')
			exit (0);
		if (c=='p' || c=='P') {
			printrez ();
			printround ();
		}
	}
}

void printscale ()
{
	int i;

	printf ("\n");
	for (i=0; i<num; i++)
		printf ("%7.7s\t", name[i]);
	printf ("\n");
	for (i=0; i<num; i++)
		printf ("--------");
	printf ("---\n");
	for (i=0; i<num; i++)
		printf ("%7d\t", scale[i]);
	printf ("\n");
}

void checkwin ()
{
	int i;

	for (i=0; i<num; i++)
		if (scale [i] >= 1100)
			break;
	if (i >= num) return;

	printscale ();

	printf ("\n\n\nКонец игры.\n");

	printrez ();

	exit (0);
}

int cubes (int sum)
{
	int k, s=0, n1=0, n5=0;
	int top [NCUBE], newcube [NCUBE];

	for (k=0; k<NCUBE; k++)
		newcube[k] = 0;

	for (k=0; k<NCUBE; k++) {
		if (cube[k]) {
			switch (top[k] = 1 + 6 * rnd ()) {
			case 1:
				n1++;
				break;
			case 5:
				n5++;
				break;
			default:
				newcube[k] = 1;
				break;
			}
		}
	}

	if (top[0]==top[1] && top[1]==top[2] && top[2]==top[3] && top[3]==top[4]) {
		newcube[0] = newcube[1] = newcube[2] = newcube[3] = newcube[4] = 0;
		s += 40 * weight [top [0]];
	} else if (top[0]==top[1] && top[1]==top[2] && top[2]==top[3]) {
		newcube[0] = newcube[1] = newcube[2] = newcube[3] = 0;
		s += 30 * weight [top [0]];
	} else if (top[0]==top[1] && top[1]==top[2] && top[2]==top[4]) {
		newcube[0] = newcube[1] = newcube[2] = newcube[4] = 0;
		s += 30 * weight [top [0]];
	} else if (top[0]==top[1] && top[1]==top[3] && top[3]==top[4]) {
		newcube[0] = newcube[1] = newcube[3] = newcube[4] = 0;
		s += 30 * weight [top [0]];
	} else if (top[0]==top[2] && top[2]==top[3] && top[3]==top[4]) {
		newcube[0] = newcube[2] = newcube[3] = newcube[4] = 0;
		s += 30 * weight [top [0]];
	} else if (top[1]==top[2] && top[2]==top[3] && top[3]==top[4]) {
		newcube[1] = newcube[2] = newcube[3] = newcube[4] = 0;
		s += 30 * weight [top [1]];
	} else if (top[0]==top[1] && top[1]==top[2]) {
		newcube[0] = newcube[1] = newcube[2] = 0;
		s += 20 * weight [top [0]];
	} else if (top[0]==top[1] && top[1]==top[3]) {
		newcube[0] = newcube[1] = newcube[3] = 0;
		s += 20 * weight [top [0]];
	} else if (top[0]==top[1] && top[1]==top[4]) {
		newcube[0] = newcube[1] = newcube[4] = 0;
		s += 20 * weight [top [0]];
	} else if (top[0]==top[2] && top[2]==top[3]) {
		newcube[0] = newcube[2] = newcube[3] = 0;
		s += 20 * weight [top [0]];
	} else if (top[0]==top[2] && top[2]==top[4]) {
		newcube[0] = newcube[2] = newcube[4] = 0;
		s += 20 * weight [top [0]];
	} else if (top[0]==top[3] && top[3]==top[4]) {
		newcube[0] = newcube[3] = newcube[4] = 0;
		s += 20 * weight [top [0]];
	} else if (top[1]==top[2] && top[2]==top[3]) {
		newcube[1] = newcube[2] = newcube[3] = 0;
		s += 20 * weight [top [1]];
	} else if (top[1]==top[2] && top[2]==top[4]) {
		newcube[1] = newcube[2] = newcube[4] = 0;
		s += 20 * weight [top [1]];
	} else if (top[1]==top[3] && top[3]==top[4]) {
		newcube[1] = newcube[3] = newcube[4] = 0;
		s += 20 * weight [top [1]];
	} else if (top[2]==top[3] && top[3]==top[4]) {
		newcube[2] = newcube[3] = newcube[4] = 0;
		s += 20 * weight [top [2]];
	}

	s += 10*n1 + 5*n5;

	printf ("\t\t\t");
	for (k=0; k<NCUBE; k++)
		if (cube[k])
			printf ("%3d ", top[k]);
		else
			printf ("  . ");
	printf ("\t\tВыигрыш: %-6d\n", s);
	printf ("\t\t\t---------------------\t\tЗа этап: %-6d\n", sum+s);
	printf ("\t\t\t");
	for (k=0; k<NCUBE; k++)
		if (cube[k])
			if (newcube[k])
				printf ("  + ");
			else
				printf ("  - ");
		else printf ("    ");
	printf ("\n");

	for (k=0; k<NCUBE; k++)
		cube[k] = newcube[k];

	return (s);
}

int step ()
{
	int sum=0, amount=0, beg=1, k;

	for (;;) {
		if (beg) {
			for (k=0; k<NCUBE; k++)
				cube[k] = 1;
			beg = 0;
		} else if (!yes ())
			return (sum);
		sum += amount = cubes (sum);
		if (! amount) {
			printf ("\7Все пропало.\n");
			return (0);
		}
		for (k=0; k<NCUBE; k++)
			if (cube[k]) break;
		if (k >= NCUBE) {
			printf ("\7Призовая игра!\n");
			beg = 1;
		}
	}
}

void checkforw ()
{
	int i;
	int forw [NCUBE];

	for (i=0; i<NCUBE; i++)
		forw [i] = 0;
oncemore:
	for (i=0; i<NCUBE; i++) {
		if (i == player)
			continue;
		if (!touched [i])
			continue;
		if (forw [i])
			continue;
		if (! (scale[player] <= scale[i] &&
			scale[player] + delta > scale[i]))
			continue;
		scale[i] -= 50;
		forw[i] = 1;
		delta += 50;
		goto oncemore;
	}
}

void checkback ()
{
	int i;
	int forw [NCUBE];

	for (i=0; i<NCUBE; i++)
		forw [i] = 0;
oncemore:
	for (i=0; i<NCUBE; i++) {
		if (i == player)
			continue;
		if (!touched [i])
			continue;
		if (forw [i])
			continue;
		if (! (scale[player] >= scale[i] &&
			scale[player] + delta <= scale[i]))
			continue;
		scale[i] += 50;
		forw[i] = 1;
		delta -= 50;
		goto oncemore;
	}
}

void changes ()
{
	checkforw ();

	if (scale [player] >= 400 && scale [player] < 500) {
		if (scale [player] + delta < 500)
			delta -= 100;
	} else if (scale [player] >= 900 && scale [player] < 1000) {
		if (scale [player] + delta < 1000)
			delta -= 100;
	}

	if (delta < 0)
		checkback ();

	if (delta) {
		scale [player] += delta;
		touched [player] = 1;
	}
}

int main ()
{
	int i;

	srand ((int) time ((long *) 0));

	for (i=0; i<NPLAY; i++) {
		printf ("Введите имя %s игрока: ", z1[i]);
		gets (name[i]);
		if (! name[i][0]) break;
		name[i][7] = '\0';
	}
	if (! (num = i)) exit (0);

	for (nround=1; ; nround++) {
		for (player=0; player<num; player++) {
			printround ();
			delta = step ();
			if (delta) {
				changes ();
				checkwin ();
			}
		}
	}
}
