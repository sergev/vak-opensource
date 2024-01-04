/*
  VTL-C
  System-dependent module for Linux/FreeBSD/Cygwin

  T. Nakagawa

  2004/05/23
*/

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#define MEMSIZE (1024 * 32)
#define READB(adrs) (*(Lct + (adrs) % MEMSIZE))
#define WRITEB(adrs, data) ((*(Lct + (adrs) % MEMSIZE) = (data)))
#define READW(adrs) \
    ((unsigned short)(*(Lct + (adrs) % MEMSIZE) + *(Lct + ((adrs) + 1) % MEMSIZE) * 256))
#define WRITEW(adrs, data) \
    (*(Lct + (adrs) % MEMSIZE) = (data) % 256, *(Lct + ((adrs) + 1) % MEMSIZE) = (data) / 256)

#define Nbf 0x82
#define Lbf 0x88
#define Svp ((2 + '!') * 2)
#define Pcc ((2 + '#') * 2)
#define Rmd ((2 + '%') * 2)
#define Bnd ((2 + '&') * 2)
#define Rnd ((2 + '\'') * 2)
#define Lmt ((2 + '*') * 2)
#define Obj 0x108

extern unsigned char Lct[];

void initl(void);
unsigned char getchr(void);
void putchr(unsigned char c);

#endif
