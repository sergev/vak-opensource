/*	@(#)stdio.h	2.7	*/
#ifndef _NFILE
#define _NFILE	20

#if besm
#define BUFSIZ  3072
#endif
#if u370
#define BUFSIZ	4096
#endif
#if vax || u3b || u3b5 || svsb
#define BUFSIZ	1024
#endif
#if pdp11
#define BUFSIZ	512
#endif

/* buffer size for multi-character output to unbuffered files */
#define _SBFSIZ 8

typedef struct {
#if vax || u3b || u3b5
	int	_cnt;
	unsigned char	*_ptr;
#else
	unsigned char	*_ptr;
	int	_cnt;
#endif
	unsigned char	*_base;
	char	_flag;
	char	_file;
} FILE;

/*
 * _IOLBF means that a file's output will be buffered line by line
 * In addition to being flags, _IONBF, _IOLBF and _IOFBF are possible
 * values for "type" in setvbuf.
 */
#define _IOFBF		0000
#define _IOREAD		0001
#define _IOWRT		0002
#define _IONBF		0004
#define _IOMYBUF	0010
#define _IOEOF		0020
#define _IOERR		0040
#define _IOLBF		0100
#define _IORW		0200

#ifndef NULL
#define NULL		0
#endif
#ifndef EOF
#define EOF		(-1)
#endif

#define stdin		(&_iob[0])
#define stdout		(&_iob[1])
#define stderr		(&_iob[2])

#define _bufend(p)	_bufendtab[(p)->_file]
#define _bufsiz(p)	(_bufend(p) - (p)->_base)

#ifndef lint
#define getc(p)		(--(p)->_cnt < 0 ? _filbuf(p) : (int) *(p)->_ptr++)
#define putc(x, p)	(--(p)->_cnt < 0 ? \
			_flsbuf((unsigned char) (x), (p)) : \
			(int) (*(p)->_ptr++ = (unsigned char) (x)))
#define getchar()	getc(stdin)
#define putchar(x)	putc((x), stdout)
#define clearerr(p)	((void) ((p)->_flag &= ~(_IOERR | _IOEOF)))
#define feof(p)		((p)->_flag & _IOEOF)
#define ferror(p)	((p)->_flag & _IOERR)
#define fileno(p)	(p)->_file
#endif

extern FILE	_iob[_NFILE];
extern FILE	*fopen(), *fdopen(), *freopen(), *popen(), *tmpfile();
extern long	ftell();
extern void	rewind(), setbuf();
extern char	*ctermid(), *cuserid(), *fgets(), *gets(), *tempnam(), *tmpnam();
extern unsigned char *_bufendtab[];

#define L_ctermid	9
#define L_cuserid	9
#define P_tmpdir	"/usr/tmp/"
#define L_tmpnam	(sizeof(P_tmpdir) + 15)
#endif
