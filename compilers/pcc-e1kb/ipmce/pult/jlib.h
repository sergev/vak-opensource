# include "scr.h"

# define JBOX                   BOX

# define Jmputstr(r,c,s)	(Jmove(r,c),Jputstr(s))
# define Jmputch(r,c,s)		(Jmove(r,c),Jputch(s))

# define getch()                (VSync (), KeyGet ())

# define Jbold          VSetBold
# define Jdim           VSetDim
# define Jnorm          VSetNormal
# define Jrbold         VSetRBold
# define Jrdim          VSetRDim
# define Jrev           VStandOut
# define Junrev         VStandEnd

# define Jclear         VClear
# define Jclearbox      VClearBox
# define Jclose         VClose
# define Jdrawbox       VDrawBox
# define Jfillbox       VFillBox
# define Jfreebox       VFreeBox
# define Jgetbox        VGetBox
# define Jmove          VMove
# define Jmprintf       mprintw
# define Jopen          VOpen
# define Jprintf        printw
# define Jputch         VPutChar
# define Jputstr        VPutString
# define Jungetbox      VUngetBox
