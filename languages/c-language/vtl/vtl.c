/*
  Very Tiny Language

  T. Nakagawa

  2004/05/23
  2004/06/26
*/

#include "system.h"

static int fndln(unsigned short *ptr);
static unsigned short nxtln(unsigned short ptr);
static void getln(unsigned short lbf);
static int getnm(unsigned short *ptr, unsigned short *n);
static int num(unsigned short ptr);
static void ordr(unsigned short ptr);
static void expr(unsigned short *ptr, unsigned short *val);
static void factr(unsigned short *ptr, unsigned short *val);
static void term(unsigned short *ptr, unsigned short *val);
static void getvr(unsigned short *ptr, unsigned char *c, unsigned short *adr);
static void putl(unsigned short *ptr, unsigned char d);
static void crlf(void);
static void putnm(unsigned short x);
static void putstr(char *str);

int main(void) {
  unsigned short ptr;
  unsigned short n;

  initl();
  putstr("VTL-C\r\n");

  for (; ; ) {
    ptr = Lbf + 2;
    getln(ptr);
    if (!getnm(&ptr, &n)) {
      unsigned short line;

      line = Lbf;
      WRITEW(line, 0);
      WRITEW(Pcc, 0);
      for (; ; ) {
	ordr(ptr);
	if (READW(Pcc) == 0 || READW(Pcc) == READW(line)) {
	  /* no branch */
	  if (line == Lbf) break;	/* For direct mode */
	  line = nxtln(line);
	  if (line == READW(Bnd)) break;	/* When executed to the last line */
	} else {
	  /* branch */
	  WRITEW(Svp, READW(line) + 1);
	  if (fndln(&line)) break;
	}
	WRITEW(Pcc, READW(line));
	ptr = line + 2 + 1;
      }
    } else {
      if (n == 0) {
	/* LIST */
	for (ptr = Obj; ptr != READW(Bnd); ) {
	  putnm(READW(ptr));
	  ptr += 2;
	  putl(&ptr, '\0');
	  crlf();
	}
      } else {
	unsigned short cur, src, dst, tmp;
	unsigned short m;

	/* DELETE */
	WRITEW(Pcc, n);
	if (!fndln(&cur) && READW(cur) == n) {
	  src = nxtln(cur);
	  for (dst = cur; src != READW(Bnd); WRITEB(dst++, READB(src++))) ;
	  WRITEW(Bnd, dst);
	}
	/* INSRT */
	if (READB(ptr) == '\0') continue;
	for (m = 3, tmp = ptr; READB(tmp) != '\0'; m++, tmp++) ;
	if (READW(Bnd) + m < READW(Lmt)) {
	  src = READW(Bnd);
	  WRITEW(Bnd, READW(Bnd) + m);
	  for (dst = READW(Bnd); src != cur; WRITEB(--dst, READB(--src))) ;
	  WRITEW(src, n);
	  src += 2;
	  while (WRITEB(src++, READB(ptr++)) != '\0') ;
	  continue;
	}
      }
    }
    putstr("\r\nOK");
  }

  return 0;
}


/*
  Searches for the smallest line with a line number greater than or equal to *Pcc,
  and if found, puts its starting address in *ptr and returns 0, otherwise returns 1.
*/
static int fndln(unsigned short *ptr) {
  for (*ptr = Obj; *ptr != READW(Bnd); *ptr = nxtln(*ptr)) {
    if (READW(*ptr) >= READW(Pcc)) return 0;
  }
  return 1;
}


/*
  Find the beginning of the next line in *ptr and return its position
*/
static unsigned short nxtln(unsigned short ptr) {
  for (ptr += 2; READB(ptr++) != '\0'; ) ;
  return ptr;
}


/*
  Read one line and store it at the address starting from lbf
*/
static void getln(unsigned short lbf) {
  int p;
  unsigned char c;

  for (p = 0; ; ) {
    c = getchr();
    if (c == '\b') {
      if (p > 0) p--;
    } else if (c == '\r') {
      WRITEB(lbf + p, '\0');
      putchr('\n');
      return;
    } else if (c == 0x15 || p + 1 == 74) {
      crlf();
      p = 0;
    } else if (c <= 0x1f) {
    } else {
      WRITEB(lbf + p++, c);
    }
  }
}


/*
  Returns 0 if **ptr is not a number.
  If it is a number, convert the number to binary, put it in *n, advance *ptr to
  the next address of the number, and return 1.
*/
static int getnm(unsigned short *ptr, unsigned short *n) {
  if (!num(*ptr)) return 0;
  *n = 0;
  do {
    *n *= 10;
    *n += READB((*ptr)++) - '0';
  } while (num(*ptr));
  return 1;
}


/*
  Returns 1 if *ptr is a number, otherwise returns 0
*/
static int num(unsigned short ptr) {
  return ('0' <= READB(ptr) && READB(ptr) <= '9');
}


/*
  Execute statement starting with ptr
*/
static void ordr(unsigned short ptr) {
  unsigned char c;
  unsigned short adr;

  getvr(&ptr, &c, &adr);	/* Find address of lvalue */
  ptr++;	/* Skip '=' in assignment */

  if (READB(ptr) == '"') {
    ptr++;
    putl(&ptr, '"');
    if (READB(ptr) != ';') crlf();
  } else {
    unsigned short val;

    expr(&ptr, &val);	/* Calculate rvalue */

    if (c == '$') {
      putchr(val & 0xff);
    } else if ((c -= '?') == 0) {
      putnm(val);
    } else {
      unsigned short r;

      WRITEW(adr, val);

      r = READW(Rnd);
      r += val;
      r = (r << 8) + (r >> 8);
      WRITEW(Rnd, r);
    }
  }
  return;
}


/*
  Calculate the value starting with *ptr and put it in *val
*/
static void expr(unsigned short *ptr, unsigned short *val) {
  unsigned char c;

  factr(ptr, val);
  while ((c = READB(*ptr)) != '\0' && c != ')') {
    term(ptr, val);
  }
  (*ptr)++;
  return;
}


/*
  Read the value element starting from *ptr and put it in *val
*/
static void factr(unsigned short *ptr, unsigned short *val) {
  unsigned char c;

  if (READB(*ptr) == '\0') {
    *val = 0;
    return;
  }

  if (getnm(ptr, val)) return;

  c = READB((*ptr)++);
  if (c == '?') {
    unsigned short tmp;

    tmp = Lbf;
    getln(tmp);
    expr(&tmp, val);
  } else if (c == '$') {
    *val = getchr();;
  } else if (c == '(') {
    expr(ptr, val);
  } else {
    unsigned short adr;

    (*ptr)--;
    getvr(ptr, &c, &adr);
    *val = READW(adr);	/* get the value of a variable or array */
  }
  return;
}


/*
  Read the operator and value elements starting from *ptr and put the operation
  result in *val
*/
static void term(unsigned short *ptr, unsigned short *val) {
  unsigned char c;
  unsigned short val2;

  c = READB((*ptr)++);
  factr(ptr, &val2);
  if (c == '*') {
    *val *= val2;
  } else if (c == '+') {
    *val += val2;
  } else if (c == '-') {
    *val -= val2;
  } else if (c == '/') {
    WRITEW(Rmd, *val % val2);
    *val /= val2;
  } else if (c == '=') {
    *val = (*val == val2);
  } else if (c == '>') {
    *val = (*val >= val2);
  } else {
    *val = (*val < val2);
  }
  return;
}


/*
  Put the characters of the variable/array starting from *ptr into *c and return
  the address *adr
*/
static void getvr(unsigned short *ptr, unsigned char *c, unsigned short *adr) {
  unsigned short val;

  *c = READB((*ptr)++);
  if (*c == ':') {
    expr(ptr, &val);
    *adr = READW(Bnd) + val * 2;
  } else {
    *adr = ((*c & 0x3f) + 2) * 2;
  }
  return;
}


/*
  Display string str ending with letter d
*/
static void putl(unsigned short *ptr, unsigned char d) {
  while (READB(*ptr) != d) putchr(READB((*ptr)++));
  (*ptr)++;
  return;
}


/*
  Begin on a new line
*/
static void crlf(void) {
  putchr('\r');
  putchr('\n');
  return;
}


/*
  Display the number x
*/
static void putnm(unsigned short x) {
  unsigned short ptr;
  unsigned char y;

  ptr = Nbf + 5;
  WRITEB(ptr, '\0');
  do {
    y = x % 10;
    x /= 10;
    WRITEB(--ptr, y + '0');
  } while (x != 0);
  putl(&ptr, '\0');
  return;
}


/*
  Display string str and line break
*/
static void putstr(char *str) {
  while (*str != '\0') putchr(*(str++));
  crlf();
  return;
}
