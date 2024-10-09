/*
**  CALL THE SHELL
*/
#include <stdlib.h>

#include "trek.h"

void shell(int _)
{
#ifdef M_XENIX
    register int pid;
    register int sav2, sav3;
    if (!(pid = fork())) {
        setuid(getuid());
        nice(0);
        execl("/bin/csh", "-", 0);
        tsyserr("не могу запустить /bin/csh");
    }
    sav2 = signal(2, 1);
    sav3 = signal(3, 1);
    while (wait(&i) != pid)
        ;
    signal(2, sav2);
    signal(3, sav3);
#else
    system("");
#endif
}
