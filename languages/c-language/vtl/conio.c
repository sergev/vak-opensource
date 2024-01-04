/*
  Console I/O
*/

#include "conio.h"

#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

static int init = 0;
static struct termios tios_org;

static int conio_open(void);
static int conio_close(void);

/*
  kbhit
*/
int kbhit(void)
{
    fd_set rfds;
    struct timeval tout;

    if (!init)
        conio_open();

    tout.tv_sec = 0;
    tout.tv_usec = 0;
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    if (select(1, &rfds, NULL, NULL, &tout) == 1)
        return 1;

    return 0;
}

/*
  getch
*/
int getch(void)
{
    unsigned char c;

    if (!init)
        conio_open();

    if (read(0, &c, 1) <= 0)
        return -1;

    return (int)c;
}

/*
  putch
*/
int putch(int c)
{
    unsigned char cc;

    if (!init)
        conio_open();

    cc = (unsigned char)c;
    write(1, &cc, 1);

    return 0;
}

/*
  Initialize
*/

static int conio_open(void)
{
    struct termios tios;

    if (tcgetattr(1, &tios) < 0)
        return 1;
    tios_org = tios;
    tios.c_iflag &= ~(ISTRIP | INLCR | ICRNL | IXON | IXOFF);
    tios.c_oflag &= ~OPOST;
    tios.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN);
    tios.c_cc[VMIN] = 1;
    tios.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &tios) < 0)
        return 1;

    init = 1;
    atexit((void (*)(void))conio_close);

    return 0;
}

/*
  Recover
*/
static int conio_close(void)
{
    if (tcsetattr(0, TCSANOW, &tios_org) < 0)
        return 1;

    return 0;
}
