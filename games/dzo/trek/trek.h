/*
**  Global Declarations
**
**      Virtually all non-local variable declarations are made in this
**      file.  Exceptions are those things which are initialized, which
**      are defined in "externs.c", and things which are local to one
**      program file.
**
**      So far as I know, nothing in here must be preinitialized to
**      zero.
**
**      You may have problems from the loader if you move this to a
**      different machine.  These things actually get allocated in each
**      source file, which UNIX allows; however, you may (on other
**      systems) have to change everything in here to be "extern" and
**      actually allocate stuff in "externs.c"
*/

#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

extern jmp_buf jmpbuf;

#define FLOAT double
#include <math.h>

/* external function definitions */
extern FLOAT franf(void); /* floating random number function */

/*********************  GALAXY  **************************/

/* galactic parameters */
#define NSECTS 10 /* dimensions of quadrant in sectors */
#define NQUADS 8  /* dimension of galazy in quadrants */
#define NINHAB 32 /* number of quadrants which are inhabited */

struct quad /* definition for each quadrant */
{
    int bases;       /* number of bases in this quadrant */
    int klings;      /* number of Klingons in this quadrant */
    int holes;       /* number of black holes in this quadrant */
    int scanned;     /* star chart entry (see below) */
    int stars;       /* number of stars in this quadrant */
    int qsystemname; /* starsystem name (see below) */
};

#define Q_DISTRESSED 0200
#define Q_SYSTEM     077

/*  systemname conventions:
 *      1 -> NINHAB     index into Systemname table for live system.
 *      + Q_DISTRESSED  distressed starsystem -- systemname & Q_SYSTEM
 *                      is the index into the Event table which will
 *                      have the system name
 *      0               dead or nonexistent starsystem
 *
 *  starchart ("scanned") conventions:
 *      0 -> 999        taken as is
 *      -1              not yet scanned ("...")
 *      1000            supernova ("///")
 *      1001            starbase + ??? (".1.")
 */

/* ascii names of systems */
extern char *Systemname[NINHAB];

/* quadrant definition */
extern struct quad Quad[NQUADS][NQUADS];

/* defines for sector map  (below) */
#define EMPTY      '.'
#define STAR       '*'
#define BASE       '#'
#define ENTERPRISE 'E'
#define QUEENE     'Q'
#define KLINGON    'K'
#define INHABIT    '@'
#define HOLE       ' '

/* current sector map */
extern char Sect[NSECTS][NSECTS];

/************************ DEVICES ******************************/

#define NDEV 16 /* max number of devices */

/* device tokens */
#define WARP     0  /* warp engines */
#define SRSCAN   1  /* short range scanners */
#define LRSCAN   2  /* long range scanners */
#define PHASER   3  /* phaser control */
#define TORPED   4  /* photon torpedo control */
#define IMPULSE  5  /* impulse engines */
#define SHIELD   6  /* shield control */
#define COMPUTER 7  /* on board computer */
#define SSRADIO  8  /* subspace radio */
#define LIFESUP  9  /* life support systems */
#define SINS     10 /* Space Inertial Navigation System */
#define CLOAK    11 /* cloaking device */
#define XPORTER  12 /* transporter */
#define SHUTTLE  13 /* shuttlecraft */

/* device names */
struct device {
    char *name;   /* device name */
    char *person; /* the person who fixes it */
    char *ending; /* для русских диагностик */
};

extern struct device Device[NDEV];

/***************************  EVENTS  ****************************/
#define NEVENTS 12 /* number of different event types */

#define E_LRTB   1  /* long range tractor beam */
#define E_KATSB  2  /* Klingon attacks starbase */
#define E_KDESB  3  /* Klingon destroys starbase */
#define E_ISSUE  4  /* distress call is issued */
#define E_ENSLV  5  /* Klingons enslave a quadrant */
#define E_REPRO  6  /* a Klingon is reproduced */
#define E_FIXDV  7  /* fix a device */
#define E_ATTACK 8  /* Klingon attack during rest period */
#define E_SNAP   9  /* take a snapshot for time warp */
#define E_SNOVA  10 /* supernova occurs */

#define E_GHOST  0100 /* ghost of a distress call if ssradio out */
#define E_HIDDEN 0200 /* event that is unreportable because ssradio out */
#define E_EVENT  077  /* mask to get event code */

struct event {
    int x, y;       /* coordinates */
    FLOAT date;     /* trap stardate */
    int evcode;     /* event type */
    int systemname; /* starsystem name */
};

/* systemname conventions:
 *      1 -> NINHAB     index into Systemname table for reported distress calls
 *
 * evcode conventions:
 *      1 -> NEVENTS-1  event type
 *      + E_HIDDEN      unreported (SSradio out)
 *      + E_GHOST       actually already expired
 *      0               unallocated
 */

#define MAXEVENTS 25 /* max number of concurrently pending events */

extern struct event Event[MAXEVENTS]; /* dynamic event list; one entry per pending event */

/*****************************  KLINGONS  *******************************/

struct kling {
    int x, y;      /* coordinates */
    int power;     /* power left */
    FLOAT dist;    /* distance to Enterprise */
    FLOAT avgdist; /* average over this move */
    char srndreq;  /* set if surrender has been requested */
};

#define MAXKLQUAD 9 /* maximum klingons per quadrant */

/********************** MISCELLANEOUS ***************************/

/* condition codes */
#define GREEN  0
#define DOCKED 1
#define YELLOW 2
#define RED    3

/* starbase coordinates */
#define MAXBASES 9 /* maximum number of starbases in galaxy */

/*  distress calls  */
#define MAXDISTR 5 /* maximum concurrent distress calls */

/* phaser banks */
#define NBANKS 6 /* number of phaser banks */

struct xy {
    int x, y; /* coordinates */
};

/*
 *      note that much of the stuff in the following structs CAN NOT
 *      be moved around!!!!
 */

/* information regarding the state of the starship */
struct dzoShip {
    char shldup;    /* shield up flag */
    char cloaked;   /* set if cloaking device on */
    FLOAT warp;     /* warp factor */
    FLOAT warp2;    /* warp factor squared */
    int brigfree;   /* space left in brig */
    int crew;       /* ship's complement */
    int energy;     /* starship's energy */
    int shield;     /* energy in shields */
    char torped;    /* torpedoes */
    char cloakgood; /* set if we have moved */
    FLOAT reserves; /* life support reserves */
    FLOAT warp3;    /* warp factor cubed */
    int distressed; /* number of distress calls */
    int quadx;      /* quadrant x coord */
    int quady;      /* quadrant y coord */
    int sectx;      /* sector x coord */
    int secty;      /* sector y coord */
    int cond;       /* condition code */
    char sinsbad;   /* Space Inertial Navigation System condition */
    char *shipname; /* name of current starship */
    char ship;      /* current starship */
};
extern struct dzoShip Ship;
/* sinsbad is set if SINS is working but not calibrated */

/* game related information, mostly scoring */
struct dzoGame {
    int killk;       /* number of klingons killed */
    int deaths;      /* number of deaths onboard Enterprise */
    char negenbar;   /* number of hits on negative energy barrier */
    char killb;      /* number of starbases killed */
    int kills;       /* number of stars killed */
    char skill;      /* skill rating of player */
    char length;     /* length of game */
    char killed;     /* set if you were killed */
    char killinhab;  /* number of inhabited starsystems killed */
    char tourn;      /* set if a tournament game */
    char passwd[15]; /* game password */
    char snap;       /* set if snapshot taken */
    char helps;      /* number of help calls */
    int captives;    /* total number of captives taken */
};
extern struct dzoGame Game;
/* per move information */
struct dzoMove {
    char free;    /* set if a move is free */
    char endgame; /* end of game flag */
    FLOAT Move_pad1;
    FLOAT time;   /* time used this move */
    char shldchg; /* set if shields changed this move */
    char newquad; /* set if just entered this quadrant */
    char resting; /* set if this move is a rest */
};
extern struct dzoMove Move;

/* parametric information */
struct dzoParam {
    int bases;    /* number of starbases */
    int klings;   /* number of klingons */
    FLOAT date;   /* stardate */
    FLOAT time;   /* time left */
    int brigfree; /* max possible number of captives */
    int crew;     /* size of ship's complement */
    int energy;   /* starship's energy */
    int shield;   /* energy in shields */
    char torped;  /* photon torpedos */
    char Param_PAD;
    FLOAT reserves;          /* life support reserves */
    FLOAT resource;          /* Federation resources */
    FLOAT damfac[NDEV];      /* damage factor */
    FLOAT dockfac;           /* docked repair time factor */
    FLOAT regenfac;          /* regeneration factor */
    int stopengy;            /* energy to do emergency stop */
    int shupengy;            /* energy to put up shields */
    int klingpwr;            /* Klingon initial power */
    int warptime;            /* time chewer multiplier */
    FLOAT phasfac;           /* Klingon phaser power eater factor */
    char moveprob[6];        /* probability that a Klingon moves */
    FLOAT movefac[6];        /* Klingon move distance multiplier */
    FLOAT eventdly[NEVENTS]; /* event time multipliers */
    FLOAT navigcrud[2];      /* navigation crudup factor */
    int cloakenergy;         /* cloaking device energy per stardate */
    FLOAT damprob[NDEV];     /* damage probability */
    FLOAT hitfac;            /* Klingon attack factor */
    int klingcrew;           /* number of Klingons in a crew */
    FLOAT srndrprob;         /* surrender probability */
    int energylow;           /* low energy mark (cond YELLOW) */
};

extern struct dzoParam Param;

/* Sum of damage probabilities must add to 1000 */

/* other information kept in a snapshot */
struct dzoNow {
    int bases;  /* number of starbases */
    int klings; /* number of klingons */
    FLOAT date; /* stardate */
    FLOAT time; /* time left */
    char Now_PAD1[10 + sizeof(FLOAT)];
    FLOAT resource;                  /* Federation resources */
    int distressed;                  /* number of currently distressed quadrants */
    struct event *eventptr[NEVENTS]; /* pointer to event structs */
    struct xy base[MAXBASES];        /* locations of starbases */
};
extern struct dzoNow Now;

/* Other stuff, not dumped in a snapshot */
struct dzoEtc {
    struct kling klingon[MAXKLQUAD];                        /* sorted Klingon list */
    int nkling;                                             /* number of Klingons in this sector */
                                                            /* < 0 means automatic override mode */
    char fast;                                              /* set if speed > 300 baud */
    struct xy starbase;                                     /* starbase in current quadrant */
    char snapshot[sizeof Quad + sizeof Event + sizeof Now]; /* snapshot for time warp */
    char statreport; /* set to get a status report on a srscan */
};
extern struct dzoEtc Etc;

/*
 *      eventptr is a pointer to the event[] entry of the last
 *      scheduled event of each type.  Zero if no such event scheduled.
 */

/* Klingon move indicies */
#define KM_OB 0 /* Old quadrant, Before attack */
#define KM_OA 1 /* Old quadrant, After attack */
#define KM_EB 2 /* Enter quadrant, Before attack */
#define KM_EA 3 /* Enter quadrant, After attack */
#define KM_LB 4 /* Leave quadrant, Before attack */
#define KM_LA 5 /* Leave quadrant, After attack */

/* you lose codes */
#define L_NOTIME   1  /* ran out of time */
#define L_NOENGY   2  /* ran out of energy */
#define L_DSTRYD   3  /* destroyed by a Klingon */
#define L_NEGENB   4  /* ran into the negative energy barrier */
#define L_SUICID   5  /* destroyed in a nova */
#define L_SNOVA    6  /* destroyed in a supernova */
#define L_NOLIFE   7  /* life support died (so did you) */
#define L_NOHELP   8  /* you could not be rematerialized */
#define L_TOOFAST  9  /* pretty stupid going at warp 10 */
#define L_STAR     10 /* ran into a star */
#define L_DSTRCT   11 /* self destructed */
#define L_CAPTURED 12 /* captured by Klingons */
#define L_NOCREW   13 /* you ran out of crew */

/******************  COMPILE OPTIONS  ***********************/

/* Trace info */
/* # define        xTRACE          1 */
/* int     Trace;                    */

void dumpme(int);
char *bmove(char *a, char *b, int l);
int damaged(int dev);
void out(int dev);
char *systemname(struct quad *q1);
void lose(int why);
void unschedule(struct event *e1);
int ranf(int max);
void initquad(int f);
void dock(int);
void undock(int);
void compkldist(int f);
void klmove(int fl);
void damage(int dev1, FLOAT dam);
void attack(int resting);
void warp(int fl, int c, FLOAT d);
void warp1(int fl);
void killk(int ix, int iy);
void killb(int qx, int qy);
void kills(int x, int y, int f);
void killd(int x, int y, int f);
void autover(void);
int check_out(int device);
int kalc(int tqx, int tqy, int tsx, int tsy, FLOAT *dist);
void compkldist(int f);
long score(void);
char cgetc(void);
struct event *schedule(int type, FLOAT offset, char x, char y, char z);
void reschedule(struct event *e1, FLOAT offset);
struct event *xsched(int ev1, int factor, int x, int y, int z);
void xresched(struct event *e1, int ev1, int factor);
int sequal(char *a, char *b);
int dumpssradio(void);
int readdump(int fd1);
int events(int warp);
void nova(int x, int y);
void snova(int x, int y);
void sector(int *x, int *y);
char *concat(char *s1, char *s2, char *s3);
int length(char *s);
void tsyserr(const char *s, ...);
int getcodi(int *co, FLOAT *di);
void shield(int f);
FLOAT move(int ramflag, int course, FLOAT time, FLOAT speed);
struct kling *selectklingon(void);
void win(void);
void lose(int why);
void reset(int);
void setup(void);
void play(int);
void checkcond(void);
void ram(int ix, int iy);
void abandon(int);
void capture(int);
void computer(int);
void dcrept(int);
void destruct(int);
void help(int);
void impulse(int);
void lrscan(int);
void phaser(int);
void dumpgame(int);
void rest(int);
void shell(int);
void srscan(int f);
void torped(int);
void visual(int);
void setwarp(int);
int restartgame(void);
char *mrok(int n);
int randcourse(int n);
