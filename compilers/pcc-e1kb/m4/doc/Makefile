SRCS    = m4.ms
MACROS  = -ms#U
TROFF   = hroff

m4.lp:  ${SRCS}
	${TROFF} ${MACROS} ${SRCS} > m4.lp

clean:
	rm -f m4.lp *.b core a.out
