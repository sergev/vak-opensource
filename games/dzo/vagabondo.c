/*
 * Cерегей Вершинин (C) 1986
 *
 * Перевод на C и куча других правок:
 * Vladimir Oleynik (C) 2004 <dzo@simtreas.ru>
 */

#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <termios.h>

#define VISIBLE 0100
#define INCAN   0200

#define RATING  0
#define ENERGY  1
#define WEAPON  2
#define PANOPLY 3
#define GOLD    4
#define SCORES  5

static int scores[SCORES];

static int er = 4, x, y, wr, sh;

static char k[9][16][32];
static char f[9][16][32];
static char *kaxy;
static char *faxy;
static char la[9];

static char fa[6+6+1];
static char fv[6+6];
static int newsack;

static char akt[4], aktf[4];
static int kax[4], kay[4];

static const char *msg;

static int
ir (int a, int b)
{
  return (rand () % (b - a + 1)) + a;
}


static void
kr (int y, int x)
{
  printf ("\033[%d;%dH", y + 2, x * 2 + 1);
}


static void
bk (void)
{
  kr (16, 0);
  printf ("\033[J");
}


static int
tsa (char s1, char s2)
{
  int i = 0;

  while (fa[i] && fa[i] != s1 && fa[i] != s2)
	i++;
  newsack = 1;
  return i;
}

static void
newsack_show(void)
{
	int i;

	kr (9, 33);
	for (i = 0; i < 6; i++)
		printf ("%2c", fa[i]);
	if(sh) {
		kr (10, 33);
		for (; fa[i]; i++)
			printf ("%2c", fa[i]);
	}
	newsack = 0;
}

static void
beast (int a, int x, int y)
{
  int j;
  char c;
  char ff;

  c = k[a][x][y];
  ff = f[a][x][y];
  if ((ff & 4) != 0)
	if ((c != akt[0]) && (c != akt[1]) && (c != akt[2])) {
	  for (j = 0; j < 3; j++) {
		if (akt[j] == ' ')
		  break;
	  }
	  akt[j] = c;
	  aktf[j] = ff;
	  kax[j] = x;
	  kay[j] = y;
	}
}

static void
z (void)
{
  msg = "На этой штуке заклятие\n";
}

static int
peur (int a, char u)
{
  int i, j;

  if (u != 'J') {
	if (*kaxy == '%') {
	  for (i = 0; i < 4; i++)
		akt[i] = ' ';
	  if (u == '.' && a != 0) {
		scores[ENERGY] -= 2;
		if(sh == 1 && la[a] == 1) {
			la[a] = 2;
			scores[RATING]++;
		}
		a--;
		for (i = 0; i < 16; i++)
			for (j = 0; j < 32; j++) {
				u = k[a][i][j];
				if (u == 'm') {
					k[a][i][j] = ir ('D', 'Z');
					f[a][i][j] = ir (6, 7);
				} else if(u == 'e' || u == 'f') {
					k[a][i][j] = 'J';
				} else if(u == 'j' && sh == 1) {
					k[a][i][j] = 'A';
				} else if(u == '~') {
					k[a][i][j] = 'X';
					f[a][i][j] = 0;
				} else if(u == 'g') {
					k[a][i][j] = '@';
					f[a][i][j] = 0;
				}
			}
		u = 'J';
	  } else if (u == '5' && a != 8) {
		a++;
		if(sh == 0 && la[a] == 0) {
			la[a] = 1;
			scores[RATING] += 3;
		}
		u = 'J';
	  } else {
	    msg = " Туда лестница не ведет\n";
	  }
	} else {
	  msg = " Без лестницы?\n";
	}
  }
  if (u == 'J') {
	printf ("\033[H\033[J  %d уровень подземелья %d\n\
								  Рейтинг\n\
								  Энергия\n\
								  Оружие\n\
								  Защита\n\
								  В банке\n\n\
								  Вредность\n\n\
								  Рюкзак:",
								      a, er - 3);
	for (i = 0; i < SCORES; i++) {
	  kr (i, 37);
	  printf ("%4d", scores[i]);
	}
	kr (6, 38);
	printf ("\b%3d", wr);
	newsack_show();
	kr (-1, 0);
	for (i = 0; i < 16; i++)
	  for (j = 0; j < 32; j++)
		if (f[a][i][j] & VISIBLE) {
		  kr (i, j);
		  putchar (k[a][i][j]);
		}
  }
  return a;
}

static void
stp (int a, int c, int d)
{
  if (k[a][x + c][y + d] != '!' && k[a][x + c][y + d] != '-') {
	x += c;
	y += d;
  } else {
	msg = " Н Е Л Ь З Я ! ! !\n";
  }
}

static void
xx (int a, char c)
{
  int x, y;

  x = ir (1, 14);
  y = ir (1, 30);
  k[a][x][y] = c;
  f[a][x][y] = ir (0, 0377) & ~VISIBLE;
}

static char
remap_item(int c)
{
	switch(c) {
		case '\'':
			return '"';
		case '<':
			return '>';
		default:
			return (char) c;
	}
}

static const char *
clear_hmem(int a)
{
    int l, i, j;
    char w;

    for(l = 0; l < 9; l++)
	   for (i = 1; i < 15; i++)
	       for (j = 1; j < 31; j++) {
		 w = k[l][i][j];
		 if(w != '-' && w != '!' && w != '.')
		       f[l][i][j] &= ~VISIBLE;
	       }
    peur (a, 'J');
    return "'Очищение' памяти...\n";
}

static struct termios tts;

static void
set_tty_old_mode (void)
{
  tcsetattr (0, TCSANOW, &tts);
}

static void
sigend (int sig)
{
  signal (sig, SIG_IGN);
  set_tty_old_mode ();
  _exit (1);
}

static void
set_tty_raw_input (void)
{
  struct termios ttn;

  tcgetattr (0, &tts);
  ttn = tts;
  ttn.c_cc[VMIN] = 1;
  ttn.c_cc[VTIME] = 0;
  ttn.c_lflag &= ~(ISIG | ECHO | ECHOE | ECHOK | ICANON);
  signal (SIGINT, sigend);
  signal (SIGQUIT, sigend);
  tcsetattr (0, TCSADRAIN, &ttn);
}


static void
fs(void)
{
  fflush (stdout);
  sleep (1);
}


int
main ()
{
  int a, fnr, pb, ebr;
  int i, j, j1, j2;
  int a_prev, e1, e2;
  char e3, w, ex;
  int zkl = 0, wep = 0, lt = 0, uf = 0, ps = 0, ib = 0;
  int b = 0, c = 0;
  int newscan, skills = 0;
  char *fabc;

  srand (time (0));

  set_tty_raw_input ();
  atexit (set_tty_old_mode);

  fa[0] = ']';
  fa[1] = '(';
  fa[2] = '>';
  fa[3] = '.';
  fa[4] = '.';
  fa[5] = '.';
  scores[ENERGY] = 25;
  fnr = 400;

repeat:
  printf ("\033[H\033[J");
  kr (9, 1);
  fs();
  for (a = 0; a <= 8; a++) {
	la[a] = 0;
	for (j = 0; j < 32; j++) {
	  k[a][0][j] = '-';
	  f[a][0][j] = VISIBLE;
	}
	for (i = 1; i < 15; i++) {
	  k[a][i][0] = '!';
	  f[a][i][0] = VISIBLE;
	  for (j = 1; j < 31; j++) {
		k[a][i][j] = '.';
		f[a][i][j] = 0;
	  }
	  k[a][i][31] = '!';
	  f[a][i][31] = VISIBLE;
	}
	for (j = 0; j < 32; j++) {
	  k[a][15][j] = '-';
	  f[a][15][j] = VISIBLE;
	}
  }
  printf ("            Узкая и склизкая дыра");
  fs();
  for (a = 1; a <= 8; a++) {
	i = ir (1, 5);
	while (i < 15) {
	  j1 = 1;
	  j2 = ir (0, 6);
	  while (j2 < 31) {
		for (j = j1; j <= j2; j++) {
		  k[a][i][j] = '-';
		}
		j1 = j2 + ir (1, 5);
		j2 += ir (3, 10);
	  }
	  i += ir (0, 7);
	}
  }
  printf ("\n              Темный коридор ...");
  fs();
  for (a = 1; a <= 8; a++) {
	j = ir (1, 6);
	while (j < 31) {
	  j1 = 1;
	  j2 = ir (0, 6);
	  while (j2 < 15) {
		for (i = j1; i <= j2; i++)
		  if (k[a][i][j] == '.')
			k[a][i][j] = '!';
		  else
			k[a][i][j] = '#';
		j1 = j2 + ir (1, 5);
		j2 += ir (2, 7);
	  }
	  j += ir (2, 5);
	}
  }
  printf ("\n              С К Е Л Е Т Ы . . .");
  fs();
  for (j = 0; j < er; j++)
	for (i = 040; i <= 0176; i++) {
	  a = ir (0, 8);
	  xx (a, remap_item(i));
	}
  printf ("\n              ТРАХ    А-А-А . . .");
  fs();
  for (a = 0; a <= 8; a++) {
	xx (a, '^');
	xx (a, '*');
	xx (a, '%');
  }
  xx (8, '%');
  xx (8, ',');
  ebr = 0;
  sh = 0;
  for (i = 0; i < 4; i++)
	akt[i] = ' ';
  fa[6] = 0;
  fa[7] = 0;
  fa[8] = 0;
  fa[9] = 0;
  fa[10] = 0;
  fa[11] = 0;
  a_prev = a = 0;
  peur (a, 'J');
  msg = "Прогнивший пол провалился... ";
  e1 = x = 1;
  e2 = y = 1;
  e3 = '@';
  wr = wr / 2 + 1;
  pb = 0;
  while (1) {
	newscan = 0;
	bk();
	if(msg) {
		printf (msg);
		msg = 0;
	}
	if (scores[ENERGY] < 5)
	  printf ("Силы на исходе\n");
	if (scores[ENERGY] < 0) {
	  printf ("   Вот Вы и стали еще одной жертвой этого подземелья.\n");
	  break;
	}
	if (a == 0)
	  if (sh)
		if (x == 1 && y == 1) {
		  if (scores[GOLD] >= 0) {
			printf ("Как, Вы вернулись?! Ну и ну !!!\nА дальше пойдете? ");
			w = getchar ();
			if (w == 'y' || w == 'd') {
			  er++;
			  scores[RATING] += 50;
			  goto repeat;
			}
			break;
		  } else {
			printf ("А расплачиваться кто будет?\n");
		  }
		}
	if (lt) {
	  if (fnr < 10)
	    printf ("Фонарь гаснет\n");
	  fnr--;
	  if (fnr == 0) {
	    scores[RATING]--;
	    lt = 0;
	  }
	}
	if (uf > (200 + wr)) {
	  scores[ENERGY] -= 3;
	  uf = 50;
	  if(ir (1, 3) == 1) {
	     printf ( "\n%s", clear_hmem (a) );
	  }
	  printf ("Отдыхать надо иногда\n");
	} else {
		if (uf > 200)
			printf ("Отдохнуть-бы\n");
	}
	if((scores[RATING] / 25) > skills) {
		skills++;
		switch(skills) {
			case 1:
				msg = "идентифицировать заклятье у предметов";
				break;
			case 2:
				msg = "переписывать папирусы";
				break;
			case 3:
				msg = "определять питательность свертков";
				break;
			case 4:
				msg = "распознавать зверей";
				break;
			case 5:
				msg = "переделывать 'ВП'";
				break;
			case 6:
				msg = "избавляться от призраков";
				break;
		}
		if(msg) {
			printf ("Вы научились %s!\n", msg);
			msg = 0;
		}
	}

	kaxy = &k[a][x][y];
	faxy = &f[a][x][y];
	for (i = -1; i <= 1; i++) {
	  for (j = -1; j <= 1; j++) {
		b = x + i;
		c = y + j;
		fabc = &f[a][b][c];
		switch (k[a][b][c]) {
		case '%':
		  msg = "Лестница. ";
		  break;
		case '*':
		  msg = "Золото. ";
		  break;
		case ',':
		  if (a == 8) {
			if(sh == 0 ) {
				sh = 1;
				newsack = 1;
				fa[6] = '.';
				fa[7] = '.';
				fa[8] = '.';
				fa[9] = '.';
				fa[10] = '.';
				fa[11] = '.';
			}
			msg = "ЗОЛОТОЙ ШАР!!! ";
		  } else {
			msg = (skills && (*fabc & INCAN)) ?
			"Волшебная кирка (проклято). " : "Волшебная кирка. ";
		  }
		  break;
		case '+':
		case '$':
		  if(skills == 0 || (skills == 1 && (*fabc & INCAN) == 0))
			msg = "Пузырек с надписью 'Drink me!'\n";
		  else if(skills == 1)
			msg = "Пузырек с надписью 'Drink me!' (проклято)\n";
		  else if(skills > 1) {
			if((*fabc & 6) == 0)
				msg = "Пузырек пьяного магического зрения\n";
			else if((*fabc & 6) == 2)
				msg = "Боченок варенья. ";
			else if((*fabc & 6) == 4)
				msg = "Пузырек пьяного магического зрения\n";
			else
				msg = "Боченок пива. ";
		  }
		  break;
		case ' ':
		  if (i == 0 && j == 0) {
			i = a;
			if (sh) {
			  if (a > 0)
				a--;
			} else {
			  if (a < 8)
				a++;
			}
			if(i == a) {
				*kaxy = 'J';
				i = 0;
			} else {
				scores[ENERGY] -= 3;
				wr += 7;
				peur (a, 'J');
				newscan = 1;
				msg = "Ой, как больно!...\n";
			}
		  } else {
			msg = "Черная дыра\n";
		  }
		  break;
		case '=':
		  msg = "Кольцо. ";
		  break;
		case '^':
		  if (i == 0 && j == 0) {
			scores[ENERGY] -= ir (3, 15);
			wr += ir (0, 5);
			*faxy = 4;
			msg = "Ведьмин студень!\n";
		  }
		  break;
		case '#':
		  if (i == 0 && j == 0)
			*kaxy = ir (0, 1) ? '!' : '-';
		  break;
		case '(':
		case ')':
		  msg = (skills && (*fabc & INCAN)) ?
			"Доспехи (прокляты). " : "Доспехи. ";
		  break;
		case '[':
		case ']':
		  msg = (skills && (*fabc & INCAN)) ?
			"Оружие (проклято). " : "Оружие. ";
		  break;
		case '?':
		  if(skills == 0 || (skills == 1 && (*fabc & INCAN) == 0))
			msg = "Свисток. ";
		  else if(skills == 1)
			msg = "Свисток (проклят). ";
		  else if(skills > 1) {
			if((*fabc & 3) == 1)
				msg = "Свистулька. ";
			else if((*fabc & 3) == 2)
				msg = "Магический Тромбон. ";
			else if((*fabc & 3) == 0)
				msg = "Свисток от телепортера\n";
			else
				msg = "Свисток гномов. ";
		  }
		  break;
		case ':':
		case ';':
		  msg = "ЕДА!!! ";
		  break;
		case '/':
		case '\\':
		  if(skills == 0 || (skills == 1 && (*fabc & INCAN) == 0))
			msg = "'ВП'. ";
		  else if(skills == 1)
			msg = "'ВП' (проклята). ";
		  else if(skills > 1) {
			if((*fabc & 3) == 3)
				msg = "'ВП' очистки мозгов. ";
			else if((*fabc & 3) == 2)
				msg = "Рубильник света.\n";
			else if((*fabc & 3) == 1)
				msg = "Рубильник от телепортера.\n";
			else
				msg = "'ВП' против зверей.\n";
		  }
		  break;
		case '>':
		  msg = (skills && (*fabc & INCAN)) ?
			"Батареи (прокляты). " : "Батареи. ";
		  break;
		case 'X':
		  if (i == 0 && j == 0) {
			ib = ir(2, 5);
			*faxy = 4;
			msg = "Хроно-синкластический инфундибулум\n";
		  }
		  break;
		case '"':
		  if(*kaxy != '"') {
			newscan = 1;
			x = b;
			y = c;
		  } else {
			if (sh)
				*kaxy = '.';
			msg = "Комариная плешь\n";
		  }
		  break;
		case '&':
		  if(skills == 0 || (skills == 1 && (*fabc & INCAN) == 0))
			msg = "Папирус. ";
		  else if(skills == 1)
			msg = "Папирус (проклят). ";
		  else if(skills > 1) {
			if((*fabc & 6) == 0)
				msg = "Папирус снятия заклятья\n";
			else if((*fabc & 6) == 2)
				msg = "Лозунг с проклятьем. ";
			else if((*fabc & 6) == 4)
				msg = a != 8 ?
				      "Указатель лестницы. " :
				      "Указатель на золотой шар. ";
			else
				msg = "Молитва. ";
		  }
		  break;
		case 'C':
		case 'H':
		case 'T':
		case 'J':
		  scores[ENERGY] -= ir (2, 8);
		  k[a][b][c] = '.';
		  msg = "\007Холодная рука схватила Вас за горло...\n";
		  break;
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'I':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'U':
		case 'V':
		case 'W':
		case 'Y':
		case 'Z':
		  if (i == 0 && j == 0) {
			if ((*faxy & 6) == 0) {
			  *faxy |= 6;
			  msg = " Сейчас он Вам покажет...\n";
			} else if ((*faxy & 6) == 2) {
			  j1 = ir (-1, 1) + x;
			  j2 = ir (-1, 1) + y;
			  if (k[a][j1][j2] != '!' && k[a][j1][j2] != '-') {
			    w = *kaxy;
			    *kaxy = '.';
			    k[a][j1][j2] = w;
			    f[a][j1][j2] = *faxy;
			  }
			} else if (zkl == 0) {
			  if (wep == 0) {
			    j1 = ir (2, 15);
			    if (scores[PANOPLY] > j1)
				scores[PANOPLY] -= j1;
			    else
				scores[ENERGY] -= j1;
			    msg = "\007Защищайтесь же!!!\n";
			  } else {
			    i = ir (2, 10) + sh;
			    scores[RATING] += i;
			    scores[WEAPON] -= i / 2;
			    scores[PANOPLY] -= 2 + 2 * sh;
			    if (scores[PANOPLY] < 0) {
				scores[ENERGY] += scores[PANOPLY];
				scores[PANOPLY] = 0;
			    }
			    if (scores[WEAPON] < 0) {
				scores[RATING] += scores[WEAPON];
				scores[WEAPON] = 0;
			    }
			    wep = 0;
			    if ((*faxy & 1) == 0) {
				for(i = 0; i < 3; i++)
					if(akt[i] == *kaxy)
						break;
				akt[i] = ' ';
				*kaxy = 'm';
				msg = "\007ГОТОВ!\n";
			    } else {
				*faxy &= ~1;
				msg = "Сильная зверюга!\n";
			    }
			  }
			}
		  } else {
			beast (a, b, c);
			if(skills < 4)
				msg = "З В Е Р Ь !\n";
			else if((*fabc & 6) == 0)
				msg = "З В Е Р Ь ! (спящий)\n";
			else if((*fabc & 6) == 2)
				msg = "Пока живая еда. ";
			else
				msg = "З В Е Р Ь !\n";
		  }
		  break;
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '0':
		  if (i == 0 && j == 0) {
			j1 = ir (1, 7);
			if((*faxy & 1))
				j1 = -j1;
			scores[ENERGY] += j1;
			if (j1 > 0) {
			  *kaxy = 'z';
			  msg = "Вкусно !\n";
			} else {
			  *kaxy = 'y';
			  msg = "Тьфу...\n";
			}
		  } else {
			if(skills < 3)
				msg = "Сверток. Съедим?\n";
			else if((*fabc & 1))
				msg = "Сверток какой-то дряни\n";
			else
				msg = "Тормозок. Съедим?\n";
		  }
		  break;
		case '-':
		case '.':
		case '!':
		  break;
		case 'a':
		  if (i == 0 && j == 0) {
			wr = wr / 10 + 1;
			*kaxy = 'j';
			msg = "'Алилуйя'... Вредность убавилась.\n";
		  } else {
			msg = "Алтарь. ";
		  }
		  break;
		case 'b':
		  msg = "Надпись 'Здесь был Ося' (здесь Ося и остался)\n";
		  break;
		case 'c':
		  msg = "Груда камней. ";
		  break;
		case 'd':
		  msg = "Метла Бабы Яги (сломана)\n";
		  break;
		case 'e':
		  msg = "Череп. ";
		  break;
		case 'f':
		  msg = "Полуистлевший скелет\n";
		  break;
		case 'g':
		  msg = "Сгоревшая матерь\n";
		  break;
		case 'h':
		  msg = "Кусочек Ноева ковчега\n";
		  break;
		case 'i':
		  msg = "Симметрида. ";
		  break;
		case 'j':
		  msg = "Черный ящик. ";
		  break;
		case 'k':
		  msg = "Лужа мышинного масла\n";
		  break;
		case 'l':
		  msg = "IBM PC/XT, Made in Japan\n";
		  break;
		case 'm':
		  msg = "Дохлая крыса. ";
		  break;
		case 'n':
		  msg = "Зуда. ";
		  break;
		case 'o':
		  msg = "Пустышка. ";
		  break;
		case 'p':
		  msg = "Куча сепулек. ";
		  break;
		case 'r':
		  msg = "Надпись 'Здесь вы и останетесь!'\n";
		  break;
		case 's':
		  msg = "Указатель:     'М','Ж' ==> 158м.\n";
		  break;
		case 't':
		  msg = "Зачитанный журнал 'Радио'\n";
		  break;
		case 'u':
		  msg = "Зачитанный журнал 'Playboy'\n";
		  break;
		case 'v':
		  msg = "Артефакт. ";
		  break;
		case 'w':
		  msg = "Надпись 'Здесь-то мы его и съели'\n";
		  break;
		case 'x':
		  msg = "Надпись 'Привет сивому'\n";
		  break;
		case 'y':
		  msg = "Что-то очень мерзкое\n";
		  break;
		case 'z':
		  msg = "Следы пикника. ";
		  break;
		case 'q':
		  msg = "Скелет. ";
		  break;
		case '~':
		  msg = "Чер-те что. ";
		  break;
		case '1':
		case 'A':
		case 'B':
		  msg = "Бродячий торговый автомат\n";
		  break;
		case '|':
		  msg = "Осцилограф 'ИО-4Б'\n";
		  break;
		case '`':
		  msg = "Мышелот (в собственном соку)\n";
		  break;
		case '@':
		  j1 = tsa ('.', '.');
		  if (fa[j1]) {
			wr++;
			fa[j1] = '@';
		  }
		  msg = "Призрак. ";
		  break;
		default:
		  if (i == 0 && j == 0) {
			*kaxy = remap_item(ir (040, 0176));
			*faxy = ir (1, 0377);
		  } else {
			msg = "Мешок с надписью 'Загляни, понравится !'\n";
		  }
		}
		if(newscan)
			break;
		if(msg) {
			printf (msg);
			msg = 0;
		}
	  }
	  if(newscan)
		break;
	}
	if(newscan)
		continue;
	ex = *kaxy;
	*kaxy = '@';
	if(a_prev == a) {
		kr (e1, e2);
		putchar (e3);
	}
	if (lt) {
	  char inv;

	  for (i = -1; i <= 1; i++)
		for (j = -1; j <= 1; j++) {
			j1 = x + i;
			j2 = y + j;
			inv = k[a][j1][j2];
			if ((inv == '^' || inv == 'X')
					&& (f[a][j1][j2] & 6) != 4) {
				inv = '.';
			} else {
				f[a][j1][j2] |= VISIBLE;
			}
			kr (j1, j2);
			putchar (inv);
		}
	} else {
	  kr (x, y);
	  putchar('@');
	  *faxy |= VISIBLE;
	}
	e3 = ex;
	*kaxy = ex;
	e1 = x;
	e2 = y;
	a_prev = a;
	for (i = 0; i < SCORES; i++) {
	  kr (i, 37);
	  printf ("%4d", scores[i]);
	}
	kr (6, 38);
	printf ("\b%3d", wr);
	if (newsack)
	  newsack_show();
	kr (-1, 0);
//l3:
	w = getchar ();
	if(w == 033) {
	    w = getchar ();
	    if(w == '[' || w == 'O')
		w = getchar ();
	}
	bk();
	switch (w) {
	case '1':
	  stp (a, 1, -1);
	  break;
	case '2':
	case 'B':
	  stp (a, 1, 0);
	  break;
	case '3':
	  stp (a, 1, 1);
	  break;
	case '4':
	case 'D':
	  stp (a, 0, -1);
	  break;
	case '5':
	  a = peur(a, '5');
	  break;
	case '6':
	case 'C':
	  stp (a, 0, 1);
	  break;
	case '7':
	  stp (a, -1, -1);
	  break;
	case '8':
	case 'A':
	  stp (a, -1, 0);
	  break;
	case '9':
	  stp (a, -1, 1);
	  break;
	case '0':
	  uf = 0;
	  break;
	case '.':
	  a = peur(a, '.');
	  break;
	case 'q':
	  i = tsa ('$', '+');
	  if (fa[i]) {
		fa[i] = '.';
		wr--;
		if ((fv[i] & 6) == 0) {
		  for (i = 1; i < 15; i++)
			for (j = 1; j < 31; j++)
			  if (k[a][i][j] != '.') {
				kr (i, j);
				w = k[a][i][j];
				if (w != '!' && w != '-' && w != '#' && w != '.')
				  w = (char) ir (042, 0176);
				putchar (w);
			  }
		} else if ((fv[i] & 6) == 2) {
		  scores[ENERGY] += 20;
		  uf = 0;
		  msg = "Чувствуете прилив сил?\n";
		} else if ((fv[i] & 6) == 4) {
		  for (i = 1; i < 15; i++)
			for (j = 1; j < 31; j++)
			  if (k[a][i][j] != '.') {
				kr (i, j);
				putchar (k[a][i][j]);
				f[a][i][j] |= VISIBLE;
			  }
		} else {
		  ebr = 20;
		  msg = "Трезвость - норма нашей жизни!\n";
		}
	  } else {
		msg = "Пить нечего\n";
	  }
	  break;
	case 'd':
	  printf ("Что выбросить? ");
	  w = getchar ();
	  i = tsa (w, w);
	  if (fa[i]) {
		if ((fv[i] & INCAN) == 0) {
		  if (w != '.') {
			wr--;
			fa[i] = '.';
			if (*kaxy != '"') {
			  *kaxy = w;
			  *faxy = fv[i];
			}
		  }
		} else {
		    z();
		}
	  } else {
		msg = "Нету\n";
	  }
	  break;
	case 'z':
	  i = tsa ('*', '*');
	  if (fa[i]) {
		scores[RATING]++;
		wr--;
		fa[i] = '.';
		scores[GOLD] += ir (10, 70);
		msg = "Там!\n";
	  } else {
		msg = "Где золото-то?\n";
	  }
	  break;
	case 's':
	  j1 = tsa ('?', '?');
	  if (fa[j1]) {
		i = fv[j1] & 3;
		if (i == 1)
		  msg = "\007";
		else if (i == 2) {
		  for (i = -1; i <= 1; i++)
			for (j = -1; j <= 1; j++)
			  if ((x + i) >= 1 && (x + i) <= 14 && (y + j) >= 1
				  && (y + j) <= 30)
			    k[a][x + i][y + j] = ' ';
		  fa[j1] = '.';
		} else if (i == 0) {
		  if(fv[j1] == 0) {
			fa[j1] = '.';
			msg = "\007Б А М - М - М ! ! !  Свисток рассыпался в прах.\n";
		  } else {
			i = fv[j1] >> 1;
			fv[j1] = i & ~3;
			msg = "\007Б А М - М - М ! ! !\n";
		  }
		  x = ir (1, 14);
		  y = ir (1, 30);
		} else {
		  /* i = 3 */
		  if(fv[j1] == 3) {
			fa[j1] = '.';
			msg = "\007Уничтожение зверя. Свисток выхватили гномы и убежали.\n";
		  } else {
			i = fv[j1] >> 1;
			fv[j1] = i | 3;
			msg = "\007Уничтожение зверя\n";
		  }
		  for (i = 0; i < 4; i++)
			akt[i] = ' ';
		  for (i = -1; i <= 1; i++)
			for (j = -1; j <= 1; j++)
			  if (k[a][x + i][y + j] >= 'A' && k[a][x + i][y + j] <= 'Z')
				k[a][x + i][y + j] = '*';
		}
	  } else {
		msg = "Однако свисток нужен\n";
	  }
	  break;
	case 'e':
	  i = tsa (':', ';');
	  if (fa[i]) {
		fa[i] = '.';
		scores[ENERGY] += 18;
		wr--;
		msg = "Спасибо!\n";
	  } else {
		msg = "Еда юк!\n";
	  }
	  break;
	case 't':
	  i = tsa ('.', '.');
	  if (fa[i]) {
		if (*kaxy == '"') {
		  msg = " Н Е Л Ь З Я ! ! !\n";
		} else {
		  wr++;
		  fa[i] = *kaxy;
		  fv[i] = *faxy;
		  *kaxy = '.';
		  msg = "Берем\n";
		}
	  } else {
		msg = "Рюкзак полон\n";
	  }
	  break;
	case 'p':
	  i = tsa ('(', ')');
	  if (fa[i]) {
		if ((fv[i] & INCAN) == 0) {
		  fa[i] = '.';
		  scores[PANOPLY] += 11;
		  msg = "Доспехи надеты\n";
		} else {
		  z ();
		}
	  } else {
		msg = "Доспех нету\n";
	  }
	  break;
	case 'l':
	  if (fnr > 0) {
		lt = 1;
		msg = "Фонарь включен\n";
	  } else {
		msg = "Батареи сели\n";
	  }
	  break;
	case 'o':
	  lt = 0;
	  msg = "Фонарь выключен\n";
	  break;
	case '@':
	  if(skills < 6) {
		msg = "Вы не умеете это делать\n";
		break;
	  }
	  for(;;) {
		i = tsa ('@', '@');
		if(fa[i] == 0)
			break;
		fa[i] = '.';
		if(msg == NULL)
			msg = "Рюкзак очищен от призраков\n";
	  }
	  break;
	case 'W':
	  if(skills < 5) {
		msg = "Вы не умеете это делать\n";
		break;
	  }
	  i = tsa ('/', '\\');
	  if (fa[i]) {
		printf("\033[H\033[J"
			"1. В 'ВП' для очистки памяти\n"
			"2. В рубильник для включения света\n"
			"3. В рубильник от телепортера\n"
			"4. В 'ВП' от зверей\n");
		switch(getchar ()) {
			case '1':
				w = 3;
				break;
			case '2':
				w = 2;
				break;
			case '3':
				w = 1;
				break;
			case '4':
				w = 0;
				break;
			default:
				w = fv[i] & 3;
				msg = "Это на каком языке?\n";
				break;
		}
		fv[i] &= ~3;
		fv[i] |= w;
		peur (a, 'J');
	  } else {
		msg = "Нет 'ВП' для манипуляции\n";
	  }
	  break;
	case 'w':
	  i = tsa ('/', '\\');
	  if (fa[i]) {
		if ((fv[i] & 3) == 3) {
		  msg = clear_hmem ( a );
		} else if ((fv[i] & 3) == 2) {
		  for (j = 1; j < 15; j++)
			for (j1 = 1; j1 < 31; j1++)
			  f[a][j][j1] |= VISIBLE;
		  fa[i] = '.';
		  peur (a, 'J');
		  msg = "Да будет свет...\n";
		} else if ((fv[i] & 3) == 1) {
		  if (sh) {
			a -= 3;
			if (a < 0)
			  a = 0;
		  } else {
			a += 3;
			if (a > 8)
			  a = 8;
		  }
		  fa[i] = '.';
		  wr--;
		  peur (a, 'J');
		  msg = "'ВП' потерялась\n";
		} else {
		  for (j1 = 0; j1 < 4; j1++) {
			if (akt[j1] != ' ') {
			  akt[j1] = ' ';
			  k[a][kax[j1]][kay[j1]] = remap_item(ir (044, 077));
			}
		  }
		  msg = "Превращение догоняющего зверя\n";
		}
	  } else {
		msg = "Махать нечем !\n";
	  }
	  break;
	case '/':
	case '?': {
	  char c1, fv1;
	  int sh_fa;

	  if(sh) {
		for (i = 0; i < 6; i++)
			if(fa[i] == '.')
				break;
		if(i < 6) {
			sh_fa = i;
			for (i = 6; fa[i]; i++)
				if(fa[i] != '.')
					break;
			if(fa[i]) {
				c1 = fa[i];
				fa[i] = fa[sh_fa];
				fa[sh_fa] = c1;
				c1 = fv[i];
				fv[i] = fv[sh_fa];
				fv[sh_fa] = c1;
				goto resack;
			} else {
				sh_fa = 5;
			}
		} else {
			for (i = 11; i >= 6; i--)
				if(fa[i] != '.')
					break;
			sh_fa = i;
		}
	  } else {
		sh_fa = 5;
	  }
	  c1 = fa[sh_fa];
	  fv1 = fv[sh_fa];
	  for (i = 0; i < sh_fa; i++) {
		fa[sh_fa - i] = fa[sh_fa - 1 - i];
		fv[sh_fa - i] = fv[sh_fa - 1 - i];
	  }
	  fa[0] = c1;
	  fv[0] = fv1;
resack:
	  newsack = 1;
	  msg = "Перестройка в рюкзаке\n";
	  break;
	  }
	case 'f':
	  wep = 1;
	  msg = "У-Р-Р-Р-А-А ! ! !\n";
	  break;
	case 'n':
	  printf
		("\033[H\033[J@LOR\n\nERRORS DETEKTED:  0\n\nFREE MEMORY:  2256 WORDS\n");
	  do {
		w = getchar ();
		putchar (w);
		if (w == 'n')
		  goto l999;
	  } while (w != 'p');
	  peur (a, 'J');
	  break;
	case 'X':
	case 'x':
	  printf ("Закончить изволите? ");
	  w = getchar ();
	  if (w == 'y' || w == 'd')
		goto l1;
	  break;
	case 'm':
	  i = tsa ('[', ']');
	  if (fa[i]) {
		if ((fv[i] & INCAN) == 0) {
		  fa[i] = '.';
		  scores[WEAPON] += 15;
		  msg = "Оружие приготовлено\n";
		} else {
		  z ();
		}
	  } else {
		msg = "Оружия нету\n";
	  }
	  break;
	case 'b':
	  i = tsa ('<', '>');
	  if (fa[i]) {
		if ((fv[i] & INCAN) == 0) {
		  fa[i] = '.';
		  wr--;
		  fnr = 400;
		  msg = "Батареи заменены\n";
		} else {
		  z ();
		}
	  } else {
		msg = "Батарей нету\n";
	  }
	  break;
	case 'j':
	  peur (a, 'J');
	  break;
	case 'R':
	  if(skills < 2) {
		msg = "Вы не умеете это делать\n";
		break;
	  }
	  i = tsa ('&', '&');
	  if (fa[i]) {
		printf("\033[H\033[J"
			"1. Снятие заклятья\n"
			"2. Установка заклятья\n"
			"3. Поиск лестницы\n"
			"4. Молитва\n");
		switch(getchar ()) {
			case '1':
				w = 0;
				break;
			case '2':
				w = 2;
				break;
			case '3':
				w = 4;
				break;
			case '4':
				w = 6;
				break;
			default:
				w = fv[i];
				msg = "Это на каком языке?\n";
				break;
		}
		fv[i] = w;
		peur (a, 'J');
	  } else {
		msg = "Нет папируса для перезаписи\n";
	  }
	  break;
	case 'r':
	  i = tsa ('&', '&');
	  if (fa[i]) {
		if ((fv[i] & 6) == 0) {
		  for (j = 0; fa[j]; j++)
			fv[j] &= ~INCAN;
		  msg = "'APCHXYZZYURR!!!'... Заклятье снято\n";
		} else if ((fv[i] & 6) == 2) {
		  for (j = 0; fa[j]; j++)
			fv[j] |= INCAN;
		  msg = "Надпись гласит: 'Слава КПСС! Будте вы прокляты!'\n";
		} else if ((fv[i] & 6) == 4) {
		  w = ((a == 8 && sh == 0) ? ',' : '%');
		  j1 = 1;
		  while (j1 < 15) {
			j2 = 1;
			while (j2 < 31)
			  if (k[a][j1][j2] == w)
				break;
			  else
				j2++;
			if (k[a][j1][j2] == w)
			  break;
			else
			  j1++;
		  }
		  if (j1 == 15 && j2 == 31) {
			msg = w == ',' ? "Золотой шар украден!\n" : "Лестницу унесли\n";
		  } else {
			printf ("%s - %d %d %d\n",
					(w == ',' ? "Золотой шар" : "Лестница"),
					a, j1, j2);
			fs();
		  }
		} else {
		  fa[i] = '.';
		  wr = wr / 10 + 1;
		  msg = "'Алилуйя'... Вредность убавилась.\n";
		}
	  } else {
		msg = "А читать-то и нечего\n";
	  }
	  break;
	case 'i':
	  i = tsa ('=', '=');
	  if (fa[i]) {
		if ((fv[i] & INCAN) == 0) {
		  msg = "Силовое поле включено!\n";
		  zkl = 1;
		  fa[i] = '.';
		} else {
		  z ();
		}
	  } else {
		msg = "Кольца нету\n";
	  }
	  break;
	case 'v':
	  if (zkl) {
		i = tsa ('.', '.');
		if (fa[i]) {
		  fa[i] = '=';
		  fv[i] = INCAN;
		  zkl = 0;
		} else {
		  msg = "Рюкзак полон\n";
		}
	  } else {
		msg = "Кольца нету\n";
	  }
	  break;
	case 'S':
	  if (*kaxy == '1' || *kaxy == 'A' || *kaxy == 'B') {
		printf ("Чего продаете? ");
		w = getchar ();
		i = tsa (w, w);
		if (fa[i]) {
			switch(w) {
				case '$':
				case '%':
				case '&':
				case '(':
				case ')':
				case '+':
				case ',':
				case '/':
				case ':':
				case ';':
				case '=':
				case '>':
				case '?':
				case '[':
				case '\\':
				case ']':
					fa[i] = '.';
					if((fv[i] & INCAN)) {
						scores[GOLD]++;
						msg = "На этом предмете заклятье. Получите свой золотой!\n";
					} else {
						scores[GOLD] += ir (10, 100);
						msg = "Берем!\n";
					}
					break;
				case '*':
					fa[i] = '.';
					scores[GOLD]++;
					msg = "Клиент всегда прав. Получите свой золотой!\n";
					break;
				default:
					msg = "Это не покупаем!\n";
					break;
			}
		} else {
			msg = "Но ведь у Вас этого нет.\n";
		}
	  } else {
		msg = "Подойди ближе к автомату!\n";
	  }
	  break;
	case 'a':
	  if (*kaxy == '1' || *kaxy == 'A' || *kaxy == 'B') {
		if (scores[GOLD] > 0) {
		  printf ("Чего изволите? ");
		  w = getchar ();
		  i = tsa ('.', '.');
		  if (fa[i]) {
			if(w == '<')
				fa[i] = ',';
			else
				fa[i] = remap_item(w);
			fv[i] = ir (0, 0377);
			scores[GOLD] -= ir (10, 200);
			*kaxy = 'k';
			msg = "П О Л У Ч И Т Е !\n";
		  }
		} else {
		  msg = "Подаю только по пятницам!\n";
		}
	  } else {
		msg = "Подойди ближе к автомату!\n";
	  }
	  break;
	case 'k':
	  i = tsa (',', ',');
	  if (fa[i]) {
		printf ("Направление? ");
		w = getchar ();
		if(w == 033) {
			w = getchar ();
		if(w == '[' || w == 'O')
			w = getchar ();
		}
		bk ();
		j1 = x;
		j2 = y;
		switch (w) {
		case '1':
		  j1 = x + 1;
		  j2 = y - 1;
		  break;
		case '2':
		case 'B':
		  j1 = x + 1;
		  break;
		case '3':
		  j1 = x + 1;
		  j2 = y + 1;
		  break;
		case '4':
		case 'D':
		  j2 = y - 1;
		  break;
		case '5':
		  *kaxy = ' ';
		  break;
		case '6':
		case 'C':
		  j2 = y + 1;
		  break;
		case '7':
		  j1 = x - 1;
		  j2 = y - 1;
		  break;
		case '8':
		case 'A':
		  j1 = x - 1;
		  break;
		case '9':
		  j1 = x - 1;
		  j2 = y + 1;
		  break;
		case '.':
		  if (a > 0) {
			k[a - 1][x][y] = ' ';
			msg = "Кусок свода обрушился и раскололся о вашу глупую голову\n";
		  }
		  break;
		default:
		  msg = " Н Е Л Ь З Я ! ! !\n";
		}
		if ((j1 >= 1 && j1 <= 14 && j2 >= 1 && j2 <= 30)
			&& ((w = k[a][j1][j2]) == '!' || w == '-')) {
		  k[a][j1][j2] = 'c';
		  scores[ENERGY]--;
		} else {
		  msg = "Ну, чего размахался?\n";
		}
	  } else {
		msg = "А стенку вы будете лбом прошибать...\n";
	  }
	  break;
	case 'y':
	  if ((*faxy & 6) == 2 && *kaxy >= 'A' && *kaxy <= 'Z') {
		*kaxy = ':';
	  } else
		msg = " Н Е Л Ь З Я ! ! !\n";
	  break;
	case 'h':
	  printf ("\033[H\033[J\
Ладно, я кое-что подскажу. Итак: здесь творится черт знает что,\n\
но на восьмом уровне лежит золотой шар. Только достав его, Вы\n\
сможете выйти из подземелья, придя на то же самое место, откуда Вы вышли.\n\
Вы можете использовать команды:\n\
a - купить (на золото)\n\
b - заменить батареи\n\
d - выбросить предмет\n\
e - поесть\n\
f - приготовиться к сражению\n\
h - помощь (этот текст)\n\
i - надеть кольцо\n\
j - обновить экран\n\
k - сломать стенку (киркой)\n\
l - включить фонарь\n\
m - приготовить оружие\n\
n - спец-клавиша (для продолжения нажать p)\n\
o - выключить фонарь\n\
p - надеть доспехи\n\
q - пить\n\
r - читать папирус\n\
s - свистнуть\n\
t - взять предмет, на котором стоишь\n\
u - вызвать джина (только в безнадежном случае!)\n\
						    ...    дальше? ");
	  w = getchar ();
	  printf ("\n\
v - снять кольцо\n\
w - взмахнуть волшебной палочкой\n\
x - закончить\n\
y - поджарить убегающего зверя\n\
z - перевести деньги в банк на счет пещеры\n\
/ - переложить вещи в рюкзаке\n\
S - продать предмет\n\
R - переписать папирус (нужно умение)\n\
W - переделать 'ВП' (нужно умение)\n\
@ - избавиться от призраков (нужно умение)\n\
    П Е Р Е Д В И Ж Е Н И Е:\n\
\n\
	7 8 9\n\
	4   6    - движение по уровню\n\
	1 2 3\n\
    5 - вниз\n\
    . - вверх по лестнице\n\
    0 - отдыхать.\n\
\n\
Использовать можно лишь вещи, лежащие в рюкзаке.\n\
Ну, что, пойдем дальше? ");
	  w = getchar ();
	  peur (a, 'J');
	  scores[RATING] -= 2;
	  break;
	case 'u':
	  if (sh == 0) {
		printf ("Что, влип? Ладно, попробую перенести отсюда.\n\
Только дороговато это встанет...\n\
Ты готов? ");
		w = getchar ();
		if (w != 'n') {
		  for (i = 0; i < SCORES; i++)
			scores[i] -= ir (0, 15);
		  for (i = 0; fa[i]; i++)
			fa[i] = '.';
		  *kaxy = '@';
		  x = ir (1, 14);
		  y = ir (1, 30);
		  j1 = ir (0, 8);
		  if (j1 > a)
			j1 = a;
		  a = j1;
		  peur (a, 'J');
		}
	  } else {
		msg = "Джинн в отгуле\n";
	  }
	  break;
	default:
	  msg = "Что-что?\n";
	}

	if (ebr > 0) {
	  j1 = x + ir (-1, 1);
	  j2 = y + ir (-1, 1);
	  if (j1 >= 1 && j1 <= 14 && j2 >= 1 && j2 <= 30) {
		x = j1;
		y = j2;
		ebr--;
	  }
	}
	uf += wr;
	ps += (scores[WEAPON] + scores[PANOPLY]) / 10;
	if (ps > 200) {
	  ps = 0;
	  scores[RATING]++;
	  scores[ENERGY]--;
	}
	if (scores[ENERGY] > (50 + scores[RATING] / 2))
	  scores[ENERGY] = 50 + scores[RATING] / 2;
	if (scores[ENERGY] > 150)
	  scores[ENERGY]--;
	if (ib) {
	  if (pb <= 0) {
		ib--;
		y = pb = ir (1, 30);
		x = ir (1, 14);
		for (i = 0; i < 4; i++)
		  akt[i] = ' ';
		a = peur (ir (0, 8), 'J');
	  } else {
		pb--;
	  }
	}
	for (i = 0; i < 4; i++)
	  if (akt[i] != ' ')
		if (kax[i] != x || kay[i] != y) {
		  j1 = kax[i];
		  if (x > j1)
			j1++;
		  else if (x < j1)
			j1--;
		  j2 = kay[i];
		  if (y > j2)
			j2++;
		  else if (y < j2)
			j2--;
		  if ((k[a][j1][j2] == '!' || k[a][j1][j2] == '-')
			  && (aktf[i] & 6) == 4) {
			akt[i] = ' ';
		  } else {
			k[a][kax[i]][kay[i]] = '.';
			kr (kax[i], kay[i]);
			putchar('.');
			k[a][j1][j2] = akt[i];
			kr (j1, j2);
			putchar (akt[i]);
			f[a][j1][j2] = aktf[i];
			kax[i] = j1;
			kay[i] = j2;
		  }
		}
  }
l1:
  i = scores[RATING] + (scores[ENERGY] + scores[WEAPON] + scores[PANOPLY] + (scores[GOLD] / 5)) / 3;
  printf ("\nВаш счет: %d\n", i);
l999:
  return 0;
}
