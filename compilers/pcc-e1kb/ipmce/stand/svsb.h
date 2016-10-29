# define SA_BASADDR 1                   /* базовый адоес загрузки */

# define TSTADDR 0x3fff0
# define DOSADDR ((int *) TSTADDR+2)
# define TTYADDR ((int *) TSTADDR+0)
# define MSGADDR ((int *) TSTADDR+0xf)

# define _ttyout (*TTYADDR)             /* буфер вывода на консоль */
# define _ttyin  (*(TTYADDR+1))         /* буфер ввода с консоли */

# define _dosout (*DOSADDR)             /* заявка на обмен с DOS */
# define _dosin  (*(DOSADDR+1))         /* ответ на обмен с DOS */

# define _latmsg (*MSGADDR)             /* язык диагностики */

# define MSG(l,r) (_latmsg ? (l) : (r))

# define ctrl(c) (c & 037)

# define BOOTVERSION    "1.0"
# define LCOPYRIGHT     "Copyright (C) 1990 Demos/*"
# define RCOPYRIGHT     "(C) 1990 Демос/*"

# define idle(rup) { if (_ttyin) {\
	if (_ttyin == ctrl('^')) { _ttyin = 0; _rtt (); }\
	if (rup) { _ttyin = 0; _flush_ (); }\
	} asm ("wmod 14h"); }
