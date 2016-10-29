#include <stdlib.h>
#include <signal.h>
#include <sys/gpanel.h>

#ifndef SDL
struct sgttyb origtty, newtty;
#endif

/*
 * Terminate the game when ^C pressed.
 */
void quit(int sig)
{
    signal(SIGINT, SIG_IGN);
#ifndef SDL
    if (newtty.sg_flags != 0)
        ioctl(0, TIOCSETP, &origtty);
#endif
    gpanel_close();
    exit(0);
}

/*
 * Return 1 when any key is pressed on console.
 */
int get_input()
{
#ifdef SDL
    extern int gpanel_input(void);

    return gpanel_input();
#else
    if (newtty.sg_flags == 0) {
        ioctl(0, TIOCGETP, &origtty);

        newtty = origtty;
        newtty.sg_flags &= ~(ECHO|XTABS);
        newtty.sg_flags |= CBREAK;
        ioctl(0, TIOCSETP, &newtty);

        signal(SIGINT, quit);
    }
    int nchars = 0;
    ioctl(0, FIONREAD, &nchars);
    if (nchars <= 0)
        return 0;

    char c;
    read(0, &c, 1);
    return 1;
#endif
}
