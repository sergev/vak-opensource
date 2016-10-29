# $Id: Makefile,v 1.2 2004-10-12 10:22:36 vak Exp $

PROG=	milter-regex
SRCS=	milter-regex.c eval.c parse.y
MAN8=	milter-regex.8
BINDIR=	/usr/local/sbin

CFLAGS+=	-Wall -g -O
LDADD+=		-lmilter -pthread

.include <bsd.prog.mk>
