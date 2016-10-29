/*$Header: devconf.h,v 1.2 91/01/13 22:05:27 mike Exp $*/

/*$Log:	devconf.h,v $
 * Revision 1.2  91/01/13  22:05:27  mike
 * Конфигурация блочных устройств для инициализации bdevsw.
 * 
 * Revision 1.1  90/10/23  11:24:10  drun
 * Initial revision
 * */

#include <sys/tmconf.h>
#include <sys/rpconf.h>
#include <sys/elconf.h>
#include <sys/bdconf.h>

# define NTMQ           NTE
# define NRPQ           NPDISK
# define NELQ           NELDISK
# define NDEMQ          NPDISK
# define NBDQ           NBDISK
# define NRPBUFQ        1
