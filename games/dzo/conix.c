#include <assert.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>

struct cmdline_optdef {
    char opt_short;
    char *opt_long;
    int has_arg;
    char *arg_name;
    int (*handler) (char *, void *);
    char *help;
};

static char *logo_banner[] = {
    "        CCCCCCCCCCCCC      OOOOOOOOO      NNNNNNNN        NNNNNNNN IIIIIIIIII XXXXXXX       XXXXXXX",
    "     CCC::::::::::::C    OO:::::::::OO    N:::::::N       N::::::N I::::::::I X:::::X       X:::::X",
    "   CC:::::::::::::::C  OO:::::::::::::OO  N::::::::N      N::::::N I::::::::I X:::::X       X:::::X",
    "  C:::::CCCCCCCC::::C O:::::::OOO:::::::O N:::::::::N     N::::::N II::::::II X::::::X     X::::::X",
    " C:::::C       CCCCCC O::::::O   O::::::O N::::::::::N    N::::::N   I::::I   XXX:::::X   X:::::XXX",
    "C:::::C               O:::::O     O:::::O N:::::::::::N   N::::::N   I::::I      X:::::X X:::::X",
    "C:::::C               O:::::O     O:::::O N:::::::N::::N  N::::::N   I::::I       X:::::X:::::X",
    "C:::::C               O:::::O     O:::::O N::::::N N::::N N::::::N   I::::I        X:::::::::X",
    "C:::::C               O:::::O     O:::::O N::::::N  N::::N:::::::N   I::::I        X:::::::::X",
    "C:::::C               O:::::O     O:::::O N::::::N   N:::::::::::N   I::::I       X:::::X:::::X",
    "C:::::C               O:::::O     O:::::O N::::::N    N::::::::::N   I::::I      X:::::X X:::::X",
    " C:::::C       CCCCCC O::::::O   O::::::O N::::::N     N:::::::::N   I::::I   XXX:::::X   X:::::XXX",
    "  C:::::CCCCCCCC::::C O:::::::OOO:::::::O N::::::N      N::::::::N II::::::II X::::::X     X::::::X",
    "   CC:::::::::::::::C  OO:::::::::::::OO  N::::::N       N:::::::N I::::::::I X:::::X       X:::::X",
    "     CCC::::::::::::C    OO:::::::::OO    N::::::N        N::::::N I::::::::I X:::::X       X:::::X",
    "        CCCCCCCCCCCCC      OOOOOOOOO      NNNNNNNN         NNNNNNN IIIIIIIIII XXXXXXX       XXXXXXX",
    "",
    "         _____)                   _____                           ___      _      __    _    _",
    "       /         ,  /)      /)   (, /  |              /)  /)    / _  )    '  )  /   ) / /  / /",
    "      /   ___      // _   _(/      /---|  __ __   ___// _(/    / (_ /    ,--'  /   /   /    /",
    "     /     / ) _(_(/_(_(_(_(_   ) /    |_/ (_/ (_(_)(/_(_(_   (____/    /___  (__ /   /    /",
    "    (____ /                    (_/",
    NULL
};



#define STRR(x)  #x
#define STR(x)  STRR(x)

#define CONIX_VERSION  "0.2"

#define BORDER  2

#define MAXSPEED  2

#define MAXLEVEL   30
#define INITBALLS   3
#define MAXBALLS   (MAXLEVEL + 2)
#define MAXEATERS   3

#define INITLIVES  3
#define TIMEOUT  90
#define THRESHOLD  75

#define FPSMIN   12
#define FPSMAX   24

#define TILE_COLOR0_START "\033[34;7m"
#define TILE_COLOR_START "\033[34m"
#define COLOR_END "\033[0m"
#define STR_TILE   TILE_COLOR_START " " COLOR_END
#define STR_BALL   "\033[1;33m"     "O" COLOR_END
#define STR_EATER  "\033[44;31m"     "@" COLOR_END
#define STR_TRAIL  "\033[1;36m"     "#" COLOR_END
#define CONIX_COLOR "\033[1;32m"

#define RANDBIT  (rand() & 0x1)
#define RANDWITHIN(a, b)  ((rand() * ((long) (b)) / ((long) RAND_MAX + 1)) + ((long) (a)))


typedef enum {
    TILE_NONE = 0,
    TILE_SEA,
    TILE_LAND,
} tile_t;


typedef int direction_t;

#define WORDBITS  (sizeof (int) * 8)
#define DIRGETX(d)  ((d) << (WORDBITS - 2) >> (WORDBITS - 2))
#define DIRGETY(d)  (((d) & 0xc) << (WORDBITS - 4) >> (WORDBITS - 2))
#define DIRCHARX(d)  (DIRGETX (d) == 1 ? 'e' : DIRGETX (d) == -1 ? 'w' : '?')
#define DIRCHARY(d)  (DIRGETY (d) == 1 ? 's' : DIRGETY (d) == -1 ? 'n' : '?')
#define DIRSETX(d, n)  (((d) & (-1 ^ 0x3)) | (n & 0x3))
#define DIRSETY(d, n)  (((d) & (-1 ^ 0xc)) | ((n & 0x3) << 2))
#define DIRFLIPX(d)  (DIRSETX (d, (-DIRGETX(d))))
#define DIRFLIPY(d)  (DIRSETY (d, (-DIRGETY(d))))

#define RANDDIR  (RANDBIT ? 1 : -1)


typedef struct object {
    size_t x;
    size_t y;
    size_t old_x;
    size_t old_y;
    direction_t dir;
} object_t;

typedef struct {
    size_t x;
    size_t y;
} coor_t;

typedef struct {
    int init;
    int new;

    /* The game board. */
    size_t width;
    size_t height;
    size_t border;
    tile_t *board;

    /* Balls. */
    size_t nball;
    object_t *balls;
    size_t *ballmap;

    /* Eaters. */
    size_t neater;
    object_t *eaters;
    size_t *eatermap;
    int kill_eaters;

    /* Conix. */
    object_t conix;
    size_t trail_len;
    size_t trail_start_x;
    size_t trail_start_y;
    size_t *trail_x;
    size_t *trail_y;
    int *trailmap;
    int conquered;

    /* Stats. */
    size_t nlive;
    size_t land;
    size_t total;
    size_t score;
    int timeout;

    /* Conquer worklist. */
    coor_t *worklist;
    size_t t_head;
    size_t t_tail;
    size_t p_head;
    size_t p_tail;
} game_t;


#define MAPIDX(g, x, y)  ((y) * (g)->width + (x))
#define TILEAT(g, x, y)  ((g)->board[MAPIDX (g, x, y)])
#define BALLAT(g, x, y)  ((g)->ballmap[MAPIDX (g, x, y)])
#define EATERAT(g, x, y)  ((g)->eatermap[MAPIDX (g, x, y)])
#define CONIXAT(g, x, y)  ((g)->conix.x == (x) && (g)->conix.y == (y))


struct {
    int interactive;
    unsigned level;
    unsigned speed_num;
    unsigned speed_den;
    size_t width;
    size_t height;
    size_t border;
    unsigned fps_min;
    unsigned fps_max;
    unsigned fps_ratio;
    unsigned nball;
    unsigned nlive;
} config = { 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };


static int config_usage (char *, void *);


static int
cmdline_usage (const char *prog_name,
	       struct cmdline_optdef *optdefs, size_t noptdef)
{
    assert (prog_name && optdefs);

    /* Check for existence of options. */
    int has_opts = 0;
    int has_args = 0;
    int max_len = 0;
    size_t n;
    struct cmdline_optdef *optdef_p;
    for (n = noptdef, optdef_p = optdefs; n; n--, optdef_p++) {
	int len = 1;  /* pre-padding. */

	if (optdef_p->opt_short || optdef_p->opt_long) {
	    has_opts++;

	    /* Calculate length of usage line. */
	    len += 2;  /* '-s' */
	    if (optdef_p->opt_long) {
		len += 4 + strlen (optdef_p->opt_long);  /* ', --long' */
		if (optdef_p->has_arg)
		    len++;  /* '=' */
	    }
	    if (optdef_p->has_arg) {
		if (optdef_p->has_arg == 2)
		    len += 2;  /* '[' ']' */
		len += strlen (optdef_p->arg_name);  /* 'ARG' */
	    }
	} else if (optdef_p->help) {
	    has_args++;
	    len += strlen (optdef_p->arg_name);  /* 'ARG' */
	}

	len += 2; /* post-padding. */

	if (len > max_len)
	    max_len = len;
    }

    /* Print usage message. */
    char *prog_name_suffix = strrchr (prog_name, '/');
    fprintf (stderr, "Usage: %s%s",
	     (prog_name_suffix ? prog_name_suffix + 1 : prog_name),
	     (has_opts ? " [OPTION]..." : ""));
    for (n = noptdef, optdef_p = optdefs; n; n--, optdef_p++) {
	if (! (optdef_p->opt_short || optdef_p->opt_long)) {
	    fprintf (stderr, (optdef_p->has_arg == 0 || optdef_p->has_arg == 3 ?
			    " [%s]" : " %s"),
		     optdef_p->arg_name);
	    if (optdef_p->has_arg > 1) {
		fprintf (stderr, "...");
		break;
	    }
	}
    }
    fprintf (stderr, "\n");

    if (has_opts) {
	fprintf (stderr, "Options:\n");

	for (n = noptdef, optdef_p = optdefs; n; n--, optdef_p++) {
	    if (! (optdef_p->opt_short || optdef_p->opt_long))
		continue;

	    int len = fprintf (stderr, " ");

	    if (optdef_p->opt_short)
		len += fprintf (stderr, "-%c", optdef_p->opt_short);
	    else
		len += fprintf (stderr, "  ");

	    if (optdef_p->opt_long) {
		len += fprintf (stderr, optdef_p->opt_short ? ", " : "  ");
		len += fprintf (stderr, "--%s", optdef_p->opt_long);
	    }

	    if (optdef_p->has_arg)
		len += fprintf (stderr, (optdef_p->has_arg == 2 ? "[%s%s]" : "%s%s"),
				(optdef_p->opt_long ? "=" : ""),
				optdef_p->arg_name);

	    char *bol = optdef_p->help;
	    while (bol) {
		fprintf (stderr, "%*c", max_len - len, ' ');
		char *eol = strchr (bol, '\n');
		fprintf (stderr, "%.*s", (int) (eol ? eol - bol : strlen (bol)), bol);
		if ((bol = eol)) {
		    bol++;
		    len = 0;
		    fprintf (stderr, "\n");
		}
	    }
	    fprintf (stderr, "\n");
	}
    }

    if (has_args) {
	fprintf (stderr, "Arguments:\n");

	for (n = noptdef, optdef_p = optdefs; n; n--, optdef_p++) {
	    if (optdef_p->opt_short || optdef_p->opt_long || ! optdef_p->help)
		continue;

	    int len = fprintf (stderr, " ");
	    len += fprintf (stderr, "%s", optdef_p->arg_name);

	    char *bol = optdef_p->help;
	    while (bol) {
		fprintf (stderr, "%*c", max_len - len, ' ');
		char *eol = strchr (bol, '\n');
		fprintf (stderr, "%.*s", (int) (eol ? eol - bol : strlen (bol)), bol);
		if ((bol = eol)) {
		    bol++;
		    len = 0;
		    fprintf (stderr, "\n");
		}
	    }
	    fprintf (stderr, "\n");
	}
    }

    return 0;
}


static int
cmdline_process (int argc, char **argv,
		 struct cmdline_optdef *optdefs,
		 size_t noptdef, void *cfg)
{
    int ret = 0;

    assert (argv && optdefs);

    /* Populate option arrays. */
    char *opts_short = (char *) malloc (noptdef * 3 + 1);
    struct option *opts_long =
	(struct option *) malloc (noptdef * sizeof (struct option));
    char *short_p = opts_short;
    struct option *long_p = opts_long;

    struct cmdline_optdef *optdef_p = optdefs;
    size_t n = noptdef;
    for (optdef_p = optdefs; n; n--, optdef_p++) {
	if (! (optdef_p->opt_short || optdef_p->opt_long))
	    continue;

	if (optdef_p->opt_long) {
	    long_p->name = optdef_p->opt_long;
	    long_p->has_arg = optdef_p->has_arg;
	    long_p->flag = NULL;
	    long_p->val = (optdef_p->opt_short ? optdef_p->opt_short :
			   n - noptdef - 2);
	    long_p++;
	}

	if (optdef_p->opt_short) {
	    *short_p++ = optdef_p->opt_short;
	    switch (optdef_p->has_arg) {
	    case 2:
		*short_p++ = ':';
	    case 1:
		*short_p++ = ':';
	    }
	}
    }
    *short_p = '\0';

    /* Supress error logging to stderr. */
#if 0
    opterr = 0;
#endif

    /* Parse command-line options. */
    while (1) {
	int opt_idx = 0;
	int c = getopt_long (argc, argv, opts_short, opts_long, &opt_idx);

	if (c == -1)
	    break;
	else if (c == '?') {
	    cmdline_usage (argv[0], optdefs, noptdef);
	    ret = -1;
	} else {
	    if (c < 0)
		optdef_p = optdefs - (c + 2);
	    else
		for (n = noptdef, optdef_p = optdefs; n; n--, optdef_p++)
		    if (c == optdef_p->opt_short)
			break;

	    ret = optdef_p->handler (optarg, cfg);
	    if (ret == -2)
		cmdline_usage (argv[0], optdefs, noptdef);
	}

	if (ret)
	    break;
    }

    /* Get non-option argument(s). */
    optdef_p = optdefs;
    n = noptdef;
    while (! ret && optind < argc) {
	while (n && (optdef_p->opt_short || optdef_p->opt_long)) {
	    optdef_p++;
	    n--;
	}

	ret = (n ? optdef_p->handler (argv[optind], cfg) : -1);
	if (ret && opterr)
	    fprintf (stderr, "excess arguments\n");

	optind++;
	if (optdef_p->has_arg < 2) {
	    optdef_p++;
	    n--;
	}
    }

    free (opts_short);
    free (opts_long);

    return ret;
}

#include <sys/ioctl.h>

static int get_terminal_width_height(size_t *width, size_t *height)
{
	struct winsize win = { 0, 0, 0, 0 };
	int ret = ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);
	if (win.ws_row <= 1) win.ws_row = 25;
	if (win.ws_col <= 1) win.ws_col = 40;
	if (height) *height = (size_t) win.ws_row - 1;
	if (width) *width = (size_t) (win.ws_col - 1);

	return ret;
}

static int
config_error (char *msg, char *prog)
{
    assert (msg);

    fprintf (stderr, "Error: %s\n", msg);
    config_usage (NULL, prog);

    return 0;
}

static int
config_version (char *arg, void *data)
{
    assert (! arg);
    fprintf (stderr, "conix: a console-based Xonix, version " CONIX_VERSION
	     " (built: " __DATE__ " " __TIME__")\n");
    exit (0);
    return 0;
}

static int
config_interactive (char *arg, void *data)
{
    assert (! arg);

    config.interactive = 1;

    return 0;
}

static int
config_level (char *arg, void *data)
{
    assert (arg);

    int ret;

    ret = sscanf (arg, "%u", &config.level);
    if (ret < 0)
	return ret;

    if (config.level < 1 || config.level > MAXLEVEL)
	config_error ("level out of range", data);

    return 0;
}

static int
config_speed (char *arg, void *data)
{
    assert (arg);

    int ret;

    ret = sscanf (arg, "%u/%u", &config.speed_num, &config.speed_den);
    if (ret < 0) {
	ret = sscanf (arg, "%u", &config.speed_num);
	config.speed_den = 1;
    }
    if (ret < 0)
	return ret;

    if (config.speed_num == 0 || config.speed_den == 0 ||
	config.speed_num / config.speed_den > MAXSPEED ||
	config.speed_den / config.speed_num > MAXSPEED)
	config_error ("illegal speed multiplier", data);

    return 0;
}

static struct cmdline_optdef optdefs[] = {
    { 'i', "interactive", 0, NULL, config_interactive,
	"Switch to interactive mode" },
    { 'l', "level", 1, "NUM", config_level,
	"Start game at level NUM (1-" STR(MAXLEVEL) ", default: 1)" },
    { 's', "speed", 1, "RATIO", config_speed,
	"Adjust game speed by RATIO (integer or fraction 1/"
	    STR(MAXSPEED) "-" STR(MAXSPEED) ", default: 1)" },
    { 'V', "version", 0, NULL, config_version,
	"Print version information and exit" },
    { 'h', "help", 0, NULL, config_usage,
	"Display this help and exit" },
};
#define NOPTDEF  (sizeof (optdefs) / sizeof (struct cmdline_optdef))

static int
config_usage (char *arg, void *data)
{
    assert (! arg);

    int ret = cmdline_usage (data, optdefs, NOPTDEF);
    if (ret < 0)
	return ret;

    exit (0);
    return 0;
}

static int
tile_print (int inverse, const char *s)
{
    if (inverse)
	printf ("\033[7m");

    printf ("%s", s);

    return 0;
}

static void setogotoxy(size_t x, size_t y)
{
	/* vt-100 */
	int j, i = 0;
	char outcurs[12];

	x = x+1;
	y = y+1;
	outcurs[i++] = '\033';
	outcurs[i++] = '[';
	if ((j = y / 10) != 0) {
		if ((y / 100) != 0) {
			j /= 10;
			outcurs[i++] =  (j + '0');
			j = y - j * 100;
			j /= 10;
			outcurs[i++] =  (j + '0');
		} else {
			outcurs[i++] =  (j + '0');
		}
		y %= 10;
	}
	outcurs[i++] =  (y + '0');
	outcurs[i++] =  (';');
	if ((j = x / 10) != 0) {
		if ((x / 100) != 0) {
			j /= 10;
			outcurs[i++] =  (j + '0');
			j = x - j * 100;
			j /= 10;
			outcurs[i++] =  (j + '0');
		} else {
			outcurs[i++] =  (j + '0');
		}
		x %= 10;
	}
	outcurs[i++] =  (x + '0');
	outcurs[i++] =  ('H');
	outcurs[i]=0;
	printf("%s", outcurs);
}

static void
tile_print_xy (int inverse, size_t x, size_t y, const char *s)
{
    setogotoxy(x, y);
    tile_print (inverse, s);
}

static char *
str_conix (void)
{
    static char *conix[] = {
	CONIX_COLOR "|"  COLOR_END,
	CONIX_COLOR "/"  COLOR_END,
	CONIX_COLOR "-"  COLOR_END,
	CONIX_COLOR "\\" COLOR_END
    };
    static int i = 0;
    char *s = conix[i++];
    if (i == sizeof (conix) / sizeof (char *))
	i = 0;
    return s;
}

static int
game_hit (game_t *g)
{
    assert (g);

    size_t x, y;

    x = g->width / 2;

    y = g->height - (g->border / 2) - 1;
    g->eaters[0].x = x;
    g->eaters[0].y = y;
    g->eaters[0].dir = DIRSETX (DIRSETY (0, -1), -1);
    g->eatermap[MAPIDX (g, x, y)] = 1;
    g->kill_eaters = 1;

    g->conix.x = x;
    g->conix.y = 0;
    g->conix.dir = 0;

    if (g->trail_len)
	g->conquered = 1;
    g->trail_len = 0;

    return 0;
}

static int
game_init (game_t *g, size_t nball)
{
    assert (g);

    size_t x, y, i, j;

    /* Initialize game board. */
    g->init = 1;
    for (y = 0; y < g->height; y++)
	for (x = 0; x < g->width; x++) {
	    g->board[MAPIDX (g, x, y)] =
		((x < g->border || y < g->border || x >= g->width - g->border ||
		  y >= g->height - g->border) ?
		 TILE_LAND : TILE_SEA);
	}

    /* Clear object maps. */
    for (i = 0; i < g->width * g->height; i++)
	g->ballmap[i] = g->eatermap[i] = 0;

    /* Initialize balls. */
    assert (nball <= MAXBALLS);
    g->nball = nball;
    for (i = 0; i < nball; i++) {
	int repeat;
	do {
	    repeat = 0;
	    x = RANDWITHIN (g->border, g->width - 2 * g->border);
	    y = RANDWITHIN (g->border, g->height - 2 * g->border);

	    for (j = 0; j < i; j++) {
		if (g->balls[j].x == x && g->balls[j].y == y) {
		    repeat = 1;
		    break;
		}
	    }
	} while (repeat);

	g->balls[i].x = x;
	g->balls[i].y = y;
	g->balls[i].dir = DIRSETX (DIRSETY (0, RANDDIR), RANDDIR);
	g->ballmap[MAPIDX (g, x, y)] = i + 1;
    }

    /* Initialize eater and conix. */
    g->neater = 1;
    game_hit (g);
    for (i = 0; i < g->width * g->height; i++)
	g->trailmap[i] = 0;
    g->conquered = 0;

    /* Initialize statistics. */
    g->land = 0;
    g->total = (g->width - 2 * g->border) * (g->height - 2 * g->border);

    return 0;
}

static int
game_flush (game_t *g, unsigned nlive)
{
    assert (g);
    g->nlive = nlive;
    g->score = 0;
    return 0;
}

static game_t *
game_new (size_t width, size_t height, size_t border)
{
    size_t ntile = width * height;

    game_t *g = (game_t *) malloc (sizeof (game_t));
    assert (g);
    g->new = 1;

    g->width = width;
    g->height = height;
    g->border = border;

    g->board = (tile_t *) malloc (sizeof (tile_t) * ntile);
    g->ballmap = (size_t *) malloc (sizeof (size_t) * ntile);
    g->eatermap = (size_t *) malloc (sizeof (size_t) * ntile);
    g->balls = (object_t *) malloc (sizeof (object_t) * MAXBALLS);
    g->eaters = (object_t *) malloc (sizeof (object_t) * MAXEATERS);

    g->trail_x = (size_t *) malloc (sizeof (size_t) * ntile);
    g->trail_y = (size_t *) malloc (sizeof (size_t) * ntile);
    g->trailmap = (int *) malloc (sizeof (int) * ntile);

    g->worklist = (coor_t *) malloc (sizeof (coor_t) * width * height);

    assert (g->board && g->ballmap && g->eatermap && g->balls && g->eaters &&
	    g->trail_x && g->trail_y && g->trailmap && g->worklist);

    return g;
}

static int
game_print (game_t *g)
{
    assert (g);

    size_t x, y, i;

    if (g->new)
	g->new = 0;

    printf ("\033[H");

    if (g->init) {
	g->init = 0;

	/* Print game board, just once. */
	for (y = 0; y < g->height; y++) {
	    for (x = 0; x < g->width; x++) {
		switch (g->board[MAPIDX (g, x, y)]) {
		case TILE_SEA:
		    if ((i = BALLAT (g, x, y))) {
			tile_print (0, STR_BALL);
		    } else
			tile_print (0, STR_TILE);
		    break;

		case TILE_LAND:
		    if (CONIXAT (g, x, y))
			tile_print (1, str_conix ());
		    else if ((i = EATERAT (g, x, y))) {
			tile_print (0, STR_EATER);
		    } else
			tile_print (1, STR_TILE);
		    break;

		default:
		    assert (0);
		}
	    }

	    printf ("\n");
	}
    } else {
	/* Update balls. */
	for (i = 0; i < g->nball; i++) {
	    tile_print_xy (0, g->balls[i].old_x, g->balls[i].old_y, STR_TILE);
	    g->balls[i].old_x = g->balls[i].x;
	    g->balls[i].old_y = g->balls[i].y;
	}

	for (i = 0; i < g->nball; i++) {
	    tile_print_xy (0, g->balls[i].x, g->balls[i].y, STR_BALL);
	}

	/* Update eaters. */
	for (i = 0; i < g->neater; i++) {
	    tile_print_xy (1, g->eaters[i].old_x, g->eaters[i].old_y, STR_TILE);
	    g->eaters[i].old_x = g->eaters[i].x;
	    g->eaters[i].old_y = g->eaters[i].y;
	}
	if (g->kill_eaters) {
	    for (i = 1; i < g->neater; i++)
		g->eatermap[MAPIDX (g, g->eaters[i].x, g->eaters[i].y)] = 0;
	    g->neater = 1;
	    g->kill_eaters = 0;
	}
	for (i = 0; i < g->neater; i++) {
	    tile_print_xy (0, g->eaters[i].x, g->eaters[i].y, STR_EATER);
	}

	/* Update conix. */
	if (g->trailmap[MAPIDX (g, g->conix.old_x, g->conix.old_y)])
	    tile_print_xy (0, g->conix.old_x, g->conix.old_y, STR_TRAIL);
	else
	    tile_print_xy (1, g->conix.old_x, g->conix.old_y, STR_TILE);
	g->conix.old_x = g->conix.x;
	g->conix.old_y = g->conix.y;
	if (g->board[MAPIDX (g, g->conix.x, g->conix.y)] == TILE_LAND)
	    tile_print_xy (1, g->conix.x, g->conix.y, str_conix ());
	else
	    tile_print_xy (0, g->conix.x, g->conix.y, str_conix ());

	/* Update conquered area, clear trail map. */
	if (g->conquered) {
	    for (y = 0; y < g->height; y++)
		for (x = 0; x < g->width; x++) {
		    size_t mapidx = MAPIDX (g, x, y);
		    if (g->trailmap[mapidx] == 1) {
			if (g->board[mapidx] == TILE_LAND)
			    tile_print_xy (1, x, y, STR_TILE);
			else
			    tile_print_xy (0, x, y, STR_TILE);
		    }
		    g->trailmap[mapidx] = 0;
		}

	    g->conquered = 0;
	}

    }
    fflush (stdout);
    return 0;
}

static tile_t
game_board (game_t *g, size_t x, size_t y)
{
    assert (g);
    return (x >= 0 && y >= 0 && x < g->width && y < g->height ?
	    g->board[MAPIDX (g, x, y)] : TILE_NONE);
}

static int
game_interference (game_t *g, size_t nobj, object_t *objs, size_t *objmap,
		   tile_t mine)
{
    assert (g);

    size_t i, j;
    int ret = 0;

    /* Compute interference between objects. */
    int flipped;
    int niter = 0;
    do {
	flipped = 0;
	for (i = 0; i < nobj; i++) {

	    unsigned nflip = 0;
	    while (nflip < 8) {
		int x, y;
		int flip = 0;

		/* Calculate interference with walls. */
		x = objs[i].x + DIRGETX (objs[i].dir);
		y = objs[i].y + DIRGETY (objs[i].dir);

		if (game_board (g, objs[i].x, y) != mine) {
		    objs[i].dir = DIRFLIPY (objs[i].dir);
		    flip = 1;
		}
		if (game_board (g, x, objs[i].y) != mine) {
		    objs[i].dir = DIRFLIPX (objs[i].dir);
		    flip = 1;
		}
		if (! flip && game_board (g, x, y) != mine) {
		    objs[i].dir = (RANDBIT ? DIRFLIPX (objs[i].dir) :
				   DIRFLIPY (objs[i].dir));
		    flip = 1;
		}

		/* Calculate interference with other objects. */
		x = objs[i].x + DIRGETX (objs[i].dir);
		y = objs[i].y + DIRGETY (objs[i].dir);

		if ((j = objmap[MAPIDX (g, objs[i].x, y)])) {
		    j--;
		    if (i != j && DIRGETX (objs[i].dir) == DIRGETX (objs[j].dir) &&
			DIRGETY (objs[i].dir) != DIRGETY (objs[j].dir))
		    {
			objs[i].dir = DIRFLIPY (objs[i].dir);
			objs[j].dir = DIRFLIPY (objs[j].dir);
			flip = 1;
		    }
		}

		if ((j = objmap[MAPIDX (g, x, objs[i].y)])) {
		    j--;
		    if (i != j && DIRGETY (objs[i].dir) == DIRGETY (objs[j].dir) &&
			DIRGETX (objs[i].dir) != DIRGETX (objs[j].dir))
		    {
			objs[i].dir = DIRFLIPX (objs[i].dir);
			objs[j].dir = DIRFLIPX (objs[j].dir);
			flip = 1;
		    }
		}

		if (! flip && (j = objmap[MAPIDX (g, x, y)])) {
		    j--;
		    if (i != j && objs[i].dir != objs[j].dir) {
			direction_t tmp_dir = objs[j].dir;
			objs[j].dir = objs[i].dir;
			objs[i].dir = tmp_dir;
			flip = 1;
		    }
		}

		if (flip) {
		    nflip++;
		    flipped = 1;
		    continue;
		}

		break;
	    }

	}

	niter++;
    } while (flipped && niter < 8);

    if (niter == 8)
	return -1;

    /* Perform actual progress. */
    for (i = 0; i < nobj; i++)
	objmap[MAPIDX (g, objs[i].x, objs[i].y)] = 0;

    for (i = 0; i < nobj; i++) {
	size_t x = objs[i].x + DIRGETX (objs[i].dir);
	size_t y = objs[i].y + DIRGETY (objs[i].dir);

	objs[i].old_x = objs[i].x;
	objs[i].old_y = objs[i].y;
	objs[i].x = x;
	objs[i].y = y;
	objmap[MAPIDX (g, x, y)] = i + 1;

	/* Check hit. */
	if (mine == TILE_SEA) {
	    if (g->trail_len &&
		(g->trailmap[MAPIDX (g, objs[i].old_x, y)] ||
		 g->trailmap[MAPIDX (g, x, objs[i].old_y)] ||
		 g->trailmap[MAPIDX (g, x, y)]))
		ret = 1;
	} else if ((g->conix.old_x == objs[i].old_x && g->conix.old_y == y) ||
		   (g->conix.old_x == x && g->conix.old_y == objs[i].old_y) ||
		   (g->conix.old_x == x && g->conix.old_y == y))
	    ret = 1;
    }

    return ret;
}


static int
game_conquer_tentative_enq (game_t *g, size_t x, size_t y)
{
    size_t mapidx = MAPIDX (g, x, y);
    if (g->board[mapidx] == TILE_SEA && ! g->trailmap[mapidx]) {
	g->worklist[g->t_tail++] = (coor_t) { x, y };
	g->trailmap[mapidx] = 2;  /* Color it visited. */
	return 1;
    }

    return 0;
}

static coor_t
game_conquer_tentative_deq (game_t *g)
{
    assert (g->t_head < g->t_tail);
    return g->worklist[g->t_head++];
}

static int
game_conquer_perpetual_enq (game_t *g, size_t x, size_t y)
{
    g->worklist[g->p_tail].x = x;
    g->worklist[g->p_tail].y = y;
    g->p_tail++;
    return 1;
}

static coor_t
game_conquer_perpetual_deq (game_t *g)
{
    assert (g->p_head < g->p_tail);
    return g->worklist[g->p_head++];
}

static int
game_conquer (game_t *g, size_t x, size_t y)
{
    assert (g && g->worklist);
    g->t_head = g->t_tail = g->p_head = g->p_tail = 0;
    int ret = 0;

    /* Enqueue the starting point. */
    game_conquer_tentative_enq (g, x, y);

    /* Traverse and tentatively mark the surrounding. */
    while (g->t_head < g->t_tail) {
	coor_t xy = game_conquer_tentative_deq (g);
	x = xy.x;
	y = xy.y;
	size_t mapidx = MAPIDX (g, x, y);

	/* Check for a ball. */
	if (g->ballmap[mapidx])
	    ret = 1;

	/* Enqueue for perpetual conquer. */
	game_conquer_perpetual_enq (g, x, y);

	/* Add uncolored neighbors to worklist. */
	size_t near_x[] = { x, x + 1, x, x - 1 };
	size_t near_y[] = { y - 1, y, y + 1, y };
	int i;
	for (i = 0; i < 4; i++)
	    game_conquer_tentative_enq (g, near_x[i], near_y[i]);
    }

    /* No balls encountered, perpetualize the conquer. */
    if (! ret) {
	while (g->p_head < g->p_tail) {
	    coor_t xy = game_conquer_perpetual_deq (g);
	    x = xy.x;
	    y = xy.y;
	    size_t mapidx = MAPIDX (g, x, y);

	    g->trailmap[mapidx] = 1;
	    g->board[mapidx] = TILE_LAND;
	    g->land++;

	    g->score += 2;
	}
    }

    return ret;
}

static int
game_conix (game_t *g)
{
    assert (g);

    int x, y, i;
    int ret = 0;

    if (! g->conix.dir) {
	g->conix.old_x = g->conix.x;
	g->conix.old_y = g->conix.y;
	return 0;
    }

    x = g->conix.x + DIRGETX (g->conix.dir);
    y = g->conix.y + DIRGETY (g->conix.dir);

    switch (game_board (g, x, y)) {
    case TILE_LAND:
	if (g->trail_len) {
	    g->conquered = 1;

	    /* Did it hit an eater? */
	    for (i = 0; i < g->neater; i++)
		if (x == g->eaters[i].x && y == g->eaters[i].y) {
		    ret = 1;
		    break;
		}

	    /* Did it hit the starting point? */
	    if (! ret && x == g->trail_start_x && y == g->trail_start_y)
		ret = 1;

	    if (ret)
		return ret;

	    /* Stop conix. */
	    g->conix.dir = 0;

	    /* Conquer. */
	    for (i = 0; i < g->trail_len; i++) {
		size_t t_x = g->trail_x[i];
		size_t t_y = g->trail_y[i];

		g->board[MAPIDX (g, t_x, t_y)] = TILE_LAND;
		g->land++;
		g->score += 2;

		size_t near_x[] = { t_x, t_x + 1, t_x, t_x - 1 };
		size_t near_y[] = { t_y - 1, t_y, t_y + 1, t_y };
		int j;
		for (j = 0; j < 4; j++) {
		    size_t nearidx = MAPIDX (g, near_x[j], near_y[j]);

		    if (g->board[nearidx] == TILE_SEA && ! g->trailmap[nearidx])
			game_conquer (g, near_x[j], near_y[j]);
		}
	    }

	    g->trail_len = 0;
	    /* Don't clear trail map! */
	}
	break;

    case TILE_SEA:
	/* Did it hit a ball? */
	for (i = 0; i < g->nball; i++)
	    if (x == g->balls[i].x && y == g->balls[i].y) {
		ret = 1;
		break;
	    }

	/* Did it hit its own trail? */
	if (! ret && g->trailmap[MAPIDX (g, x, y)])
	    ret = 1;

	if (ret)
	    return 1;

	/* Extend trail. */
	if (! g->trail_len) {
	    g->trail_start_x = g->conix.x;
	    g->trail_start_y = g->conix.y;
	}
	g->trail_x[g->trail_len] = x;
	g->trail_y[g->trail_len] = y;
	g->trail_len++;
	g->trailmap[MAPIDX (g, x, y)] = 1;

	break;

    case TILE_NONE:
	/* Hit the border, stop. */
	g->conix.dir = 0;
	return 0;

    default:
	assert (0);
    }

    g->conix.old_x = g->conix.x;
    g->conix.old_y = g->conix.y;
    g->conix.x = x;
    g->conix.y = y;

    return 0;
}

static int
game_timestep (game_t *g)
{
    int ret = 0;
    int ret2 = 0;

    assert (g);

    /* Advance balls. */
    ret = game_interference (g, g->nball, g->balls, g->ballmap, TILE_SEA);
    if (ret < 0)
	return ret;
    if (! ret2)
	ret2 = ret;

    /* Advance eaters. */
    if (! ret2) {
	ret = game_interference (g, g->neater, g->eaters, g->eatermap, TILE_LAND);
	if (ret < 0)
	    return ret;
	ret2 = ret;
    }

    /* Advance conix. */
    if (! ret2) {
	ret = game_conix (g);
	if (ret < 0)
	    return ret;
	ret2 = ret;
    }

    return ret2;
}

static int key_read (void)
{
    int ret;
    unsigned char c;

    if ((ret = read (0, &c, sizeof (c))) > 0)
	return c;

    return ret;
}

static int key_read_nonblock (void)
{
    static struct timeval tv = { 0, 0 };
    fd_set fds;
    FD_ZERO (&fds);
    FD_SET (0, &fds);
    if (select (1, &fds, NULL, NULL, &tv) > 0)
	return key_read ();
    return 0;
}

static int
game_level (game_t *g, unsigned level, unsigned long tick_usec)
{
    assert (g && tick_usec < 1000000000);

    int ret = 0;
    struct timespec tick = { 0, tick_usec };
    struct timeval tv_start, tv_now, tv_diff, tv_elapsed = { 0, 0 }, tv_new;
    int pause = 0;

    gettimeofday (&tv_start, NULL);
    tv_now = tv_start;
    int fresh = 1;
    int added = 0;
    g->timeout = TIMEOUT;

    while (1) {
	struct timespec left;

	if (! pause) {
	    /* Print game board. */
	    if (! ret)
		game_print (g);

	    /* Advance timeout. */
	    if (! ret) {
		gettimeofday (&tv_now, NULL);
		if (fresh) {
		    fresh = 0;
		    tv_start = tv_now;
		}
		timersub (&tv_now, &tv_start, &tv_diff);
		tv_start = tv_now;
		timeradd (&tv_elapsed, &tv_diff, &tv_new);
		g->timeout -= (tv_new.tv_sec - tv_elapsed.tv_sec);
		tv_elapsed = tv_new;

		if (g->timeout < 0) {
		    g->timeout = TIMEOUT;
		    added = 0;
		}
	    }

	    /* Check level completion. */
	    if (! ret && g->land * 100 / g->total >= THRESHOLD)
		ret = 2;

	    /* Add eater if timeout expired. */
	    if (! ret && ! added && g->timeout == 0) {
		added = 1;
		if (g->neater < MAXEATERS) {
		    size_t x = g->width / 2;
		    size_t y = g->height - (g->border / 2) - 1;
		    while (g->eatermap[MAPIDX (g, x, y)])
			x--;

		    g->eaters[g->neater].x = x;
		    g->eaters[g->neater].y = y;
		    g->eaters[g->neater].old_x = x;
		    g->eaters[g->neater].old_y = y;
		    g->eaters[g->neater].dir = DIRSETX (DIRSETY (0, -1), -1);
		    g->eatermap[MAPIDX (g, x, y)] = 1;
		    g->neater++;
		} else
		    ret = 1;
	    }

	    if (ret == 1) {
		game_hit (g);
		g->nlive--;
	    }

	    fflush (stdout);

	    /* Update status line. */
	    setogotoxy(0, g->height);
	    printf ("Score: %6lu   Lives: %2lu   Full: %2lu%%   "
		    "Time: %2d   Level: %2u",
		    g->score, g->nlive, g->land * 100 / g->total, g->timeout,
		    level);
	    /* Check for no more lives. */
	    if (! g->nlive)
		ret = 3;
	}

	if (ret > 0) {
	    sleep (ret == 1 ? 1 : 2);
	    while (key_read_nonblock () > 0) ;

	    if (ret > 1)
		break;
	    ret = 0;
	    fresh = 1;
	    g->timeout = TIMEOUT;
	    continue;
	} else {
	    nanosleep (&tick, &left);

	    int c;
	    while ((c = key_read_nonblock ()) > 0) {
		int done = 1;

		/* Keypresses should have no effect when pausing. */
		if (pause && c != 'p' && c != 'q')
		    continue;

		direction_t new_conix_dir = 0;
		switch (c) {
		case 'q':
		    printf ("    \033[1mBye-bye!\033[m");
		    fflush (stdout);
		    ret = -1;
		    break;
		case 'p':
		    if ((pause = ! pause)) {
			printf ("    \033[1mPress 'p' to resume\033[m");
			fflush (stdout);
		    } else {
			printf ("\033[1K");
			fresh = 1;
		    }
		    break;

		case '\033':
		    c = key_read ();
		    if (c == '[') {
			c = key_read ();
			switch (c) {
			case 'A':
			    new_conix_dir = DIRSETY (0, -1);
			    break;
			case 'B':
			    new_conix_dir = DIRSETY (0, 1);
			    break;
			case 'C':
			    new_conix_dir = DIRSETX (0, 1);
			    break;
			case 'D':
			    new_conix_dir = DIRSETX (0, -1);
			    break;
			default:
			    done = 0;
			}
		    } else
			done = 0;
		    break;

		case 'h':
		    new_conix_dir = DIRSETX (0, -1);
		    break;
		case 'j':
		    new_conix_dir = DIRSETY (0, 1);
		    break;
		case 'k':
		    new_conix_dir = DIRSETY (0, -1);
		    break;
		case 'l':
		    new_conix_dir = DIRSETX (0, 1);
		    break;
		case ' ':
		    g->conix.dir = 0;
		    break;
		default:
		    done = 0;
		}

		/* Pointing conix at the direction it's already moving will
		 * stop it. */
		if (new_conix_dir)
		    g->conix.dir =
			(new_conix_dir == g->conix.dir ? 0 : new_conix_dir);

		if (done)
		    break;
	    }
	}

	if (ret)
	    break;

	if (! pause)
	    ret = game_timestep (g);
    }

    return ret;
}

static void
sigint_handler (int sig)
{
    exit (0);
}

static struct termios term_orig;

static void
term_reset (void)
{
    tcsetattr (0, TCSANOW, &term_orig);
    printf ("\033[?25h\n");
}

static int
term_init (void)
{
    struct termios term_new;

    /* Get and save current terminal settings. */
    tcgetattr (0, &term_orig);
    term_new = term_orig;

    /* Register SIGINT handler, terminal restoration call. */
    atexit (term_reset);
    signal (SIGINT, sigint_handler);

    /* Disable echoing, switch to noncanonical mode. */
    term_new.c_lflag &= ~(ECHO | ECHONL | ICANON);
    tcsetattr (0, TCSANOW, &term_new);

    /* Hide cursor. */
    printf ("\033[?25l");

    return 0;
}

static int
game_start (game_t *g, unsigned long nball, unsigned level,
	    unsigned long fps_min, unsigned long fps_max)
{
    assert (g && nball && nball <= MAXBALLS && fps_min <= fps_max);

    int ret = 0;

    do {
	unsigned long fps_span = fps_max - fps_min;
	unsigned long fps = fps_min + (level - 1) * fps_span / (MAXLEVEL - 1);
	if (fps > fps_max)
	    fps = fps_max;
	assert (fps);

	game_init (g, nball);
	unsigned nlive = g->nlive;
	ret = game_level (g, level, 1000000000 / fps);
	if (ret == 2) {
	    ret = 0;
	    level++;
	    nball++;

	    /* Bonus life? */
	    if (nlive == g->nlive)
		g->nlive++;

	    /* Game completed? */
	    if (level == MAXLEVEL)
		break;
	}
    } while (! ret);

    return (ret > 0 ? 0 : ret);
}


int
main (int argc, char **argv)
{
    int ret = 0;
    struct {
	size_t width;
	size_t height;
	size_t border;
	unsigned fps_min;
	unsigned fps_max;
	unsigned nball;
	unsigned nlive;
    } settings = {
	0, 0, BORDER,
	FPSMIN, FPSMAX,
	INITBALLS, INITLIVES
    };

    ret = cmdline_process (argc, argv, optdefs, NOPTDEF, argv[0]);
    if (ret < 0)
	return ret;

    /* Adjust speed first. */
    settings.fps_min = settings.fps_min * config.speed_num / config.speed_den;
    settings.fps_max = settings.fps_max * config.speed_num / config.speed_den;

    /* Adjust other settings based on level or particular settings. */
    if (config.level > 1)
	settings.nball += config.level - 1;

    /* Initialize terminal. */
    term_init ();
    get_terminal_width_height(&settings.width, &settings.height);
    if(settings.width < (100 + settings.border * 2) || settings.height < (22 + settings.border * 2)) {
	fprintf(stderr, "The screen size too small for this game :(\n");
	return 1;
    }

    game_t *g = game_new (settings.width, settings.height, settings.border);

    if (config.interactive) {
	g->new = 0;

	char **line = logo_banner;
	int i;
	for (i = 0; i < settings.height; i++) {
	    if (i < settings.border || i >= settings.height - settings.border)
		printf ("%s%*s%s\n", TILE_COLOR0_START, (int) settings.width, "", COLOR_END);
	    else if (i < (settings.border+1) || ! *line)
		printf ("%s%*s%s%*s%s%*s%s\n",
			TILE_COLOR0_START, (int) settings.border, "", COLOR_END,
			(int) (settings.width - settings.border * 2), "",
			TILE_COLOR0_START, (int) settings.border, "", COLOR_END);
	    else {
		int n = settings.width - settings.border * 2 - 100;
		n = n / 2;
		printf ("%s%*s%s"
			"%*s%s%s%s"
			"%*s"
			"%s%*s%s\n",
				TILE_COLOR0_START, (int) settings.border, "", COLOR_END,
				n, "", CONIX_COLOR, *line, COLOR_END,
				(int) settings.width - settings.border * 2 - strlen(*line) - n, "",
				TILE_COLOR0_START, (int) settings.border, "", COLOR_END);
		line++;
	    }
	}

	key_read ();
    }

    while (1) {
	game_flush (g, settings.nlive);
	ret = game_start (g, settings.nball, config.level,
			  settings.fps_min, settings.fps_max);

	if (ret < 0 || ! config.interactive)
	    break;

	printf ("    \033[1mPress 'y' to play again\033[m");
	fflush (stdout);
	int c = key_read ();
	while (key_read_nonblock () > 0) ;
	if (c != 'y')
	    break;
	printf ("\033[1K");
    }

    exit (ret);

    return 0;
}
