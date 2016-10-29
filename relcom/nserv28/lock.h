# if defined (SYSV) || defined (SVR4) || defined (XENIX)
# include <unistd.h>
#    define LOCK(fd)    (lockf (fd, F_TLOCK, 0L) >= 0)
# else
# include<sys/file.h>
#    define LOCK(fd)    (flock (fd, (LOCK_EX|LOCK_NB) ) >= 0)
# endif
