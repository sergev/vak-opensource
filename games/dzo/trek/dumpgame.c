#include <fcntl.h>

#include "trek.h"

/***  THIS CONSTANT MUST CHANGE AS THE DATA SPACES CHANGE ***/
#define VERSION 2

struct dump {
    void *area;
    int count;
};

struct dump Dump_template[] = {
    // clang-format off
    { &Ship, sizeof(Ship) },
    { &Now, sizeof(Now) },
    { &Param, sizeof(Param) },
    { &Etc, sizeof(Etc) },
    { &Game, sizeof(Game) },
    { Sect, sizeof(Sect) },
    { Quad, sizeof(Quad) },
    { &Move, sizeof(Move) },
    { Event, sizeof(Event) },
    { 0, 0 },
    // clang-format on
};

/*
**  DUMP GAME
**
**      This routine dumps the game onto the file "trek.dump".  The
**      first two bytes of the file are a version number, which
**      reflects whether this image may be used.  Obviously, it must
**      change as the size, content, or order of the data structures
**      output change.
*/

void dumpgame(int _)
{
    int version;
    register int fd;
    register struct dump *d;
    register int i;

    if ((fd = creat("trek.dump", 0644)) < 0) {
failed: printf("не могу запомнить\n");
        return;
    }
    version = VERSION;
    if (write(fd, &version, sizeof version) != sizeof version) {
        goto failed;
    }

    /* output the main data areas */
    for (d = Dump_template; d->area; d++) {
        if (write(fd, &d->area, sizeof d->area) != sizeof d->area) {
            goto failed;
        }
        i = d->count;
        if (write(fd, d->area, i) != i) {
            goto failed;
        }
    }

    close(fd);
}

/*
**  RESTORE GAME
**
**      The game is restored from the file "trek.dump".  In order for
**      this to succeed, the file must exist and be readable, must
**      have the correct version number, and must have all the appro-
**      priate data areas.
**
**      Return value is zero for success, one for failure.
*/

int restartgame()
{
    register int fd;
    int version;

    if ((fd = open("trek.dump", 0)) < 0 || read(fd, &version, sizeof version) != sizeof version ||
        version != VERSION || readdump(fd)) {
        printf("не могу перезапустить\n");
        close(fd);
        return (1);
    }

    close(fd);
    return (0);
}

/*
**  READ DUMP
**
**      This is the business end of restartgame().  It reads in the
**      areas.
**
**      Returns zero for success, one for failure.
*/

int readdump(int fd1)
{
    register int fd;
    register struct dump *d;
    register int i;
    void *junk;

    fd = fd1;

    for (d = Dump_template; d->area; d++) {
        if (read(fd, &junk, sizeof junk) != (sizeof junk))
            return (1);
        if (junk != d->area)
            return (1);
        i = d->count;
        if (read(fd, d->area, i) != i)
            return (1);
    }

    /* make quite certain we are at EOF */
    return (read(fd, &junk, 1));
}
