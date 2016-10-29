/*
 * $Header: compack.c,v 2.0 92/08/05 08:42:39 vak Exp $
 * $Log:	compack.c,v $
 * Revision 2.0  92/08/05  08:42:39  vak
 * 
 * 
 * Revision 1.3  92/06/19  22:19:45  usenet
 * Fix errors. Rudnev
 * 
 * Revision 1.2  92/06/19  22:11:39  usenet
 * 
 * Revision 1.2  92/06/19  22:08:30  usenet
 * News Server
 * 
 */
/*
 * File compressor/decompressor with 7-bit encoding.
 * Usage:
 *	compack (op, get, put)
 *	int op;
 *	int (*get) ();
 *	void (*put) (int c);
 * Parameters:
 *	op	=0 - compress input to output
 *		=1 - uncompress
 *		=2 - compress and convert to 7 bit (pack)
 *		=3 - unpack and uncompress
 *	get	- function to get next character from input stream
 *	put	- function to output character
 *
 * Uses 12-bit compress and 13-to-16 method of encoding.
 *
 * Author: Serge Vakulenko.
 */
/* #define TEST */

#define BITS		12	/* max bits/code for 16-bit machine */
#define HSIZE		5003	/* 80% occupancy */
#define MAXSTACK        4000    /* size of output stack */

/* Defines for third byte of header */
#define BIT_MASK	0x1f
#define BLOCK_MASK	0x80

/* Masks 0x40 and 0x20 are free.  I think 0x20 should mean that there
 * is a fourth header byte (for expansion).
 */
#define INIT_BITS	9	/* initial number of bits/code */

#define MAXCODE(n_bits)		((1 << (n_bits)) - 1)

#define htab                    __htab__
#define codetab                 __codetab__

#define tab_prefix              codetab

#define CHECK_GAP 10000		/* ratio check interval */

/* the next two codes should not be changed lightly, as they must not
 * lie within the contiguous general code space.
 */
#define FIRST	257		/* first free entry */
#define CLEAR	256		/* table clear output code */

#define GET()		(*getbyte)()
#define PUT(c)		(*putbyte)(c)
#define DECGET()	(*inbyte)()
#define ENCPUT(c)	(*outbyte)(c)

#define ESC	'~'

typedef unsigned char uchar;

uchar tab_suffix [1<<BITS];     /* last char in this entry */
uchar de_stack [MAXSTACK];

static uchar magic_header[] = "\037\235";	/* 1f 9d */

static char enctab [91] = {
	'!',	'"',	'#',	'$',	'%',	'&',	'\'',	'(',
	')',	'*',	'+',	',',	'-',	'.',	'/',	'0',
	'1',	'2',	'3',	'4',	'5',	'6',	'7',	'8',
	'9',	':',	';',	'<',	'=',	'>',	'?',	'@',
	'A',	'B',	'C',	'D',	'E',	'F',	'G',	'H',
	'I',	'J',	'K',	'L',	'M',	'N',	'O',	'P',
	'Q',	'R',	'S',	'T',	'U',	'V',	'W',	'X',
	'Y',	'Z',	'[',	']',	'^',	'_',	'a',	'b',
	'c',	'd',	'e',	'f',	'g',	'h',	'i',	'j',
	'k',	'l',	'm',	'n',	'o',	'p',	'q',	'r',
	's',	't',	'u',	'v',	'w',	'x',	'y',	'z',
	'{',	'|',	'}',
};

static char dectab [93] = {
	0,	1,	2,	3,	4,	5,	6,	7,
	8,	9,	10,	11,	12,	13,	14,	15,
	16,	17,	18,	19,	20,	21,	22,	23,
	24,	25,	26,	27,	28,	29,	30,	31,
	32,	33,	34,	35,	36,	37,	38,	39,
	40,	41,	42,	43,	44,	45,	46,	47,
	48,	49,	50,	51,	52,	53,	54,	55,
	56,	57,	58,	0,	59,	60,	61,	0,
	62,	63,	64,	65,	66,	67,	68,	69,
	70,	71,	72,	73,	74,	75,	76,	77,
	78,	79,	80,	81,	82,	83,	84,	85,
	86,	87,	88,	89,	90,
};

static n_bits;			/* number of bits/code */
static maxbits = BITS;		/* user settable max # bits/code */
static maxcode;			/* maximum code, given n_bits */
static maxmaxcode = 1 << BITS;	/* should NEVER generate this code */

long htab [HSIZE];
unsigned short codetab [HSIZE];

static free_ent = 0;		/* first unused entry */

/* block compression parameters -- after all codes are used up, and
 * compression rate changes, start over.
 */
static block_compress = BLOCK_MASK;
static clear_flg = 0;
static long ratio = 0;

static long checkpoint = CHECK_GAP;

static offset;
static size;
static long in_count = 1;	/* length of input */
static long bytes_out;		/* length of compressed output */

static int (*getbyte) ();       /* get byte from input stream (GET) */
static void (*putbyte) ();      /* put byte to output stream (PUT) */
static int (*inbyte) ();        /* get byte from input stream (DECGET) */
static void (*outbyte) ();      /* put byte to output stream (ENCPUT) */

static char outputbuf [BITS];

static uchar lmask[9] = {
	0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0x00,
};

static uchar rmask[9] = {
	0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff,
};

static enccnt;			/* number of output blocks in current line */
static char encblock [13];
static enclen;

static char decblock [13*4];
static deccnt;
static declen;
static deceof;

static int decfill ();
static void pack ();
static void encflush ();
static void unpack ();
static void conv ();
static int compress ();
static void output ();
static int decompress ();
static int getcode ();
static void cl_block ();
static void cl_hash ();

extern int strlen ();
extern int printf ();

/*
 * Read input line upto \n, but no more tnah n chracters.
 * Return 1 if OK, 0 on end of file.
 * N must be >0.
 */
static int decgets (p, n)
char *p;
int n;
{
	register c;
	register char *s;

	s = p;
	--n;
	while (--n > 0) {
		c = DECGET ();
		if (c < 0) {
			*s = 0;
			return (s != p);
		}
		if ((*s++ = c) == '\n')
			break;
	}
	*s = 0;
	return (1);
}

static int decstart ()
{
	char buf [80];

	declen = deccnt = deceof = 0;
	/* search for header line */
	while (decgets (buf, sizeof buf))
		if (! strncmp (buf, "start ", 6))
			return (0);
	return (-1);
}

static int decget ()
{
	if (deceof)
		return (-1);
	if (deccnt >= declen) {
		if (! decfill ()) {
			deceof = 1;
			return (-1);
		}
		deccnt = 0;
	}
	return ((uchar) decblock [deccnt++]);
}

static int decfill ()
{
	char buf [80], rec [13];
	register i, m, len;
	register char *p;

	declen = 0;
loop:
	if (! decgets (buf, sizeof (buf)))
		return (0);			/* premature EOF */
	if (! strncmp (buf, "end\n", 4))
		return (0);			/* legal EOF */
	for (len=strlen(buf), p=buf; len>3; len-=16, p+=16) {
		if (*p == ESC) {
			m = *++p - 'A';
			if (m<=0 || m>12)	/* invalid record */
				return (0);
			++p;
			len -= 2 + (m*8 + 12) / 13 * 2;
		} else
			m = 13;
		pack (p, rec);
		for (i=0; i<m; ++i)
			decblock [declen++] = rec[i];
	}
	if (! declen)
		goto loop;
	return (1);
}

static void pack (p, s)
char *p;
register char *s;
{
	int w [8];
	char b [16];
	register i, c;

	for (i=0; i<16; ++i) {
		c = *p++;
		if (c<'!' || c>'}')
			b[i] = 0;
		else
			b[i] = dectab [c - '!'];
	}
	for (i=0, p=b; i<8; ++i, p+=2)
		w[i] = p[0] * 91 + p[1];
	*s++ = w[0] >> 5;
	*s++ = w[0] << 3 | w[1] >> 10 & 07;
	*s++ = w[1] >> 2;
	*s++ = w[1] << 6 | w[2] >> 7 & 077;
	*s++ = w[2] << 1 | w[3] >> 12 & 01;
	*s++ = w[3] >> 4;
	*s++ = w[3] << 4 | w[4] >> 9 & 017;
	*s++ = w[4] >> 1;
	*s++ = w[4] << 7 | w[5] >> 6 & 0177;
	*s++ = w[5] << 2 | w[6] >> 11 & 03;
	*s++ = w[6] >> 3;
	*s++ = w[6] << 5 | w[7] >> 8 & 037;
	*s = w[7];
}

static void encstart (name)
char *name;
{
	register char *p;

	for (p="start "; *p; ++p)
		ENCPUT (*p);
	for (p=name; *p; ++p)
		ENCPUT (*p);
	ENCPUT ('\n');
	enccnt = enclen = 0;
}

static void encend ()
{
	register char *p;

	if (enclen)
		encflush ();
	if (enccnt)		/* flush last line */
		ENCPUT ('\n');
	for (p="end\n"; *p; ++p)
		ENCPUT (*p);
}

static void encput (c)
int c;
{
	encblock [enclen++] = c;
	if (enclen == 13)
		encflush ();
}

static void encflush ()
{
	char outblock [16];
	register i, m;

	if (! enclen)
		return;

	/* enclen is 1..13 - number of bytes read from file */

	/* clear trailing bytes if n<13 */
	for (i=enclen; i<13; ++i)
		encblock [i] = 0;

	/* unpack 13 bytes to 16 bytes */
	unpack (encblock, outblock);

	/* compute number of bytes in output buffer */
	m = (enclen*8 + 12) / 13 * 2;

	/* mark short block */
	if (enclen != 13) {
		ENCPUT (ESC);
		ENCPUT (enclen + 'A');
	}

	/* output block */
	for (i=0; i<m; ++i)
		ENCPUT (enctab [outblock [i]]);

	/* flush line, 4 blocks per line */
	if (++enccnt == 4) {
		ENCPUT ('\n');
		enccnt = 0;
	}
	enclen = 0;
}

/*
 * Unpack 13 bytes to 16 bytes.
 */
static void unpack (p, s)
register char *p, *s;
{
	conv (p[0] << 5 & 017740 | p[1] >> 3 & 037, s);
	s += 2;
	p += 1;
	conv (p[0] << 10 & 016000 | p[1] << 2 & 01774 | p[2] >> 6 & 3, s);
	s += 2;
	p += 2;
	conv (p[0] << 7 & 017600 | p[1] >> 1 & 0177, s);
	s += 2;
	p += 1;
	conv (p[0] << 12 & 010000 | p[1] << 4 & 07760 | p[2] >> 4 & 017, s);
	s += 2;
	p += 2;
	conv (p[0] << 9 & 017000 | p[1] << 1 & 0776 | p[2] >> 7 & 01, s);
	s += 2;
	p += 2;
	conv (p[0] << 6 & 017700 | p[1] >> 2 & 077, s);
	s += 2;
	p += 1;
	conv (p[0] << 11 & 014000 | p[1] << 3 & 03770 | p[2] >> 5 & 07, s);
	s += 2;
	p += 2;
	conv (p[0] << 8 & 017400 | p[1] & 0377, s);
}

/*
 * Convert 13-bit integer to two numbers in range 0..90.
 */
static void conv (n, s)
int n;
char *s;
{
	register i;

	*s++ = i = n / 91;
	*s = n - i * 91;
}

int compack (op, get, put)
int op;
int (*get) ();
void (*put) ();
{
	register rez;

	switch (op) {
	case 0:		/* compress */
		getbyte = get;
		putbyte = put;
		rez = compress ();
		break;
	case 1:		/* uncompress */
		getbyte = get;
		putbyte = put;
		rez = decompress ();
		break;
	case 2:		/* compress & encode */
		getbyte = get;
		putbyte = encput;
		outbyte = put;
		encstart ("stdin");
		rez = compress ();
		encend ();
		break;
	case 3:		/* decode & uncompress */
		inbyte = get;
		getbyte = decget;
		putbyte = put;
		if (decstart () < 0)
			return (-1);
		rez = decompress ();
		break;
	default:
		return (-1);
	}
	return (rez);
}

/* compress stdin to stdout
 *
 * Algorithm:  use open addressing double hashing (no chaining) on the
 * prefix code / next character combination.  We do a variant of
 * Knuth's algorithm D (vol. 3, sec. 6.4) along with G. Knott's
 * relatively-prime secondary probe.  Here, the modular division first
 * probe is gives way to a faster exclusive-or manipulation.  Also do
 * block compression with an adaptive reset, whereby the code table is
 * cleared when the compression ratio decreases, but after the table
 * fills.  The variable-length output codes are re-sized at this
 * point, and a special CLEAR code is generated for the decompressor.
 * Late addition: construct the table according to file size for
 * noticeable speed improvement on small files.  Please direct
 * questions about this implementation to ames!jaw.
 */
static int compress ()
{
	register long fcode;
	register i, c, ent, disp, hsize_reg, hshift;

	PUT (magic_header[0]);
	PUT (magic_header[1]);
	PUT ((char) (maxbits | block_compress));

	i = 0;
	offset = 0;
	bytes_out = 3;			/* includes 3-byte header mojo */
	clear_flg = 0;
	ratio = 0;
	in_count = 1;
	checkpoint = CHECK_GAP;
	maxcode = MAXCODE (n_bits = INIT_BITS);
	free_ent = ((block_compress) ? FIRST : 256);

	ent = GET ();

	hshift = 0;
	for (fcode = HSIZE; fcode < 65536L; fcode *= 2L)
		hshift++;
	hshift = 8 - hshift;		/* set hash code range bound */

	hsize_reg = HSIZE;
	cl_hash ((long) hsize_reg); /* clear hash table */

	while ((c = GET ()) >= 0) {

		in_count++;

		fcode = (long) (((long) c << maxbits) + ent);
		i = ((c << hshift) ^ ent);		/* xor hashing */

		if (htab [i] == fcode) {
			ent = codetab [i];
			continue;
		} else if ((long) htab [i] < 0)       /* empty slot */
			goto nomatch;
		disp = hsize_reg - i;	/* secondary hash (after G. Knott) */
		if (i == 0)
			disp = 1;
probe:
		if ((i -= disp) < 0)
			i += hsize_reg;

		if (htab [i] == fcode) {
			ent = codetab [i];
			continue;
		}
		if ((long) htab [i] > 0)
			goto probe;
nomatch:
		output (ent);
		ent = c;
		if (free_ent < maxmaxcode) {
			codetab [i] = free_ent++;       /* code -> hashtable */
			htab [i] = fcode;
		} else if ((long) in_count >= checkpoint && block_compress)
			cl_block ();
	}
	/* Put out the final code. */
	output (ent);
	output (-1);
	return (bytes_out);
}

/* Output the given code. Inputs: code:	A n_bits-bit integer.  If ==
 * -1, then EOF.  This assumes that n_bits =< (long)wordsize - 1.
 * Outputs: Outputs code to the file. Assumptions: Chars are 8 bits
 * long. Algorithm: Maintain a BITS character long buffer (so that 8
 * codes will fit in it exactly).  Use the VAX insv instruction to
 * insert each code in turn.  When the buffer fills up empty it and
 * start over.
 */

static void output (code)
int code;
{
	/* On the VAX, it is important to have the register
	 * declarations in exactly the order given, or the asm will
	 * break.
	 */
	register r_off = offset, bits = n_bits;
	register char *bp = outputbuf;

	if (code >= 0) {
		/* byte/bit numbering on the VAX is simulated by the
		 * following code
		 */
		/* Get to the first byte. */
		bp += (r_off >> 3);
		r_off &= 7;

		/* Since code is always >= 8 bits, only need to mask
		 * the first hunk on the left.
		 */
		*bp = (*bp & rmask[r_off]) | (code << r_off) & lmask[r_off];
		bp++;
		bits -= (8 - r_off);
		code >>= 8 - r_off;

		/* Get any 8 bit parts in the middle (<=1 for up to 16
		 * bits).
		 */
		if (bits >= 8) {
			*bp++ = code;
			code >>= 8;
			bits -= 8;
		}
		/* Last bits. */
		if (bits)
			*bp = code;
		offset += n_bits;
		if (offset == (n_bits << 3)) {
			bp = outputbuf;
			bits = n_bits;
			bytes_out += bits;
			do
				PUT (*bp++);
			while (--bits);
			offset = 0;
		}
		/* If the next entry is going to be too big for the
		 * code size, then increase it, if possible.
		 */
		if (free_ent > maxcode || (clear_flg > 0)) {
			/* Write the whole buffer, because the input
			 * side won't discover the size increase until
			 * after it has read it.
			 */
			if (offset > 0) {
				register i;

				for (i=0; i<n_bits; ++i)
					PUT (outputbuf [i]);
				bytes_out += n_bits;
			}
			offset = 0;

			if (clear_flg) {
				maxcode = MAXCODE (n_bits = INIT_BITS);
				clear_flg = 0;
			} else {
				n_bits++;
				if (n_bits == maxbits)
					maxcode = maxmaxcode;
				else
					maxcode = MAXCODE (n_bits);
			}
		}
	} else {
		/* At EOF, write the rest of the buffer. */
		if (offset > 0) {
			register i, n;

			n = (offset + 7) / 8;
			for (i=0; i<n; ++i)
				PUT (outputbuf [i]);
			bytes_out += n;
		}
		offset = 0;
	}
}

/* Decompress stdin to stdout.	This routine adapts to the codes in the
 * file building the "string" table on-the-fly; requiring no table to
 * be stored in the compressed file.  The tables used herein are shared
 * with those of the compress() routine.  See the definitions above.
 */

static int decompress ()
{
	register uchar *stackp;
	register finchar;
	register code, oldcode, incode;

	if (maxbits < INIT_BITS)
		maxbits = INIT_BITS;
	if (maxbits > BITS)
		maxbits = BITS;
	maxmaxcode = 1 << maxbits;

	/* read header */
	if (GET() != (magic_header[0] & 0xff) ||
	    GET() != (magic_header[1] & 0xff))
		return (-1);			/* bad magic */
	maxbits = GET ();			/* set -b from file */
	block_compress = maxbits & BLOCK_MASK;
	maxbits &= BIT_MASK;
	maxmaxcode = 1 << maxbits;
	if (maxbits > BITS)
		return (-1);		/* cannot handle more than BITS bits */

	/* As above, initialize the first 256 entries in the table. */
	maxcode = MAXCODE (n_bits = INIT_BITS);
	for (code = 255; code >= 0; code--) {
		tab_prefix [code] = 0;
		tab_suffix [code] = (uchar) code;
	}
	free_ent = ((block_compress) ? FIRST : 256);

	offset = 0;
	size = 0;

	finchar = oldcode = getcode ();
	if (oldcode == -1)		/* EOF already? */
		return (0);		/* Get out of here */
	PUT ((char) finchar);		/* first code must be 8 bits = char */
	stackp = de_stack;

	while ((code = getcode ()) > -1) {

		if ((code == CLEAR) && block_compress) {
			for (code = 255; code >= 0; code--)
				tab_prefix [code] = 0;
			clear_flg = 1;
			free_ent = FIRST - 1;
			if ((code = getcode ()) == -1) /* O, untimely death! */
				break;
		}
		incode = code;
		/* Special case for KwKwK string. */
		if (code >= free_ent) {
			*stackp++ = finchar;
			code = oldcode;
		}
		/* Generate output characters in reverse order */
		while (code >= 256) {
			*stackp++ = tab_suffix [code];
			code = tab_prefix [code];
		}
		*stackp++ = finchar = tab_suffix [code];

		/* And put them out in forward order */
		do
			PUT (*--stackp);
		while (stackp > de_stack);

		/* Generate the new entry. */
		if ((code = free_ent) < maxmaxcode) {
			tab_prefix [code] = (unsigned short) oldcode;
			tab_suffix [code] = finchar;
			free_ent = code + 1;
		}
		/* Remember previous code. */
		oldcode = incode;
	}
	return (1);
}

/* Read one code from the standard input.  If EOF, return -1. Inputs:
 * stdin Outputs: code or -1 is returned.
 */
static int getcode ()
{
	/* On the VAX, it is important to have the register
	 * declarations in exactly the order given, or the asm will
	 * break.
	 */
	register code;
	static uchar buf [BITS];
	register r_off, bits;
	register uchar *bp = buf;

	if (clear_flg > 0 || offset >= size || free_ent > maxcode) {
		/* If the next entry will be too big for the current
		 * code size, then we must increase the size.  This
		 * implies reading a new buffer full, too.
		 */
		if (free_ent > maxcode) {
			n_bits++;
			if (n_bits == maxbits)
				maxcode = maxmaxcode;  /* won't get any bigger now */
			else
				maxcode = MAXCODE (n_bits);
		}
		if (clear_flg > 0) {
			maxcode = MAXCODE (n_bits = INIT_BITS);
			clear_flg = 0;
		}
		for (size=0; size<n_bits; ++size) {
			register c = GET ();
			if (c < 0)
				break;
			buf [size] = c;
		}
		if (size <= 0)
			return -1;		       /* end of file */
		offset = 0;
		/* Round size down to integral number of codes */
		size = (size << 3) - (n_bits - 1);
	}
	r_off = offset;
	bits = n_bits;

	/* Get to the first byte. */
	bp += (r_off >> 3);
	r_off &= 7;

	/* Get first part (low order bits) */
	code = (*bp++ >> r_off);
	bits -= (8 - r_off);
	r_off = 8 - r_off;		/* now, offset into code word */

	/* Get any 8 bit parts in the middle (<=1 for up to 16 bits). */
	if (bits >= 8) {
		code |= *bp++ << r_off;
		r_off += 8;
		bits -= 8;
	}

	/* high order bits. */
	code |= (*bp & rmask[bits]) << r_off;
	offset += n_bits;

	return code;
}

static void cl_block ()
{					/* table clear for block compress */
	register long rat;

	checkpoint = in_count + CHECK_GAP;

	if (in_count > 0x007fffff) {		/* shift will overflow */
		rat = bytes_out >> 8;
		if (rat == 0) {			/* Don't divide by zero */
			rat = 0x7fffffff;
		} else {
			rat = in_count / rat;
		}
	} else {
		rat = (in_count << 8) / bytes_out;	/* 8 fractional bits */
	}
	if (rat > ratio) {
		ratio = rat;
	} else {
		ratio = 0;
		cl_hash ((long) HSIZE);
		free_ent = FIRST;
		clear_flg = 1;
		output (CLEAR);
	}
}

static void cl_hash (hsize)             /* reset code table */
register long hsize;
{
	register long *htab_p = htab + hsize;
	register long i;
	register long m1 = -1;

	i = hsize - 16;
	do {				/* might use Sys V memset(3) here */
		*(htab_p - 16) = m1;
		*(htab_p - 15) = m1;
		*(htab_p - 14) = m1;
		*(htab_p - 13) = m1;
		*(htab_p - 12) = m1;
		*(htab_p - 11) = m1;
		*(htab_p - 10) = m1;
		*(htab_p - 9) = m1;
		*(htab_p - 8) = m1;
		*(htab_p - 7) = m1;
		*(htab_p - 6) = m1;
		*(htab_p - 5) = m1;
		*(htab_p - 4) = m1;
		*(htab_p - 3) = m1;
		*(htab_p - 2) = m1;
		*(htab_p - 1) = m1;
		htab_p -= 16;
	} while ((i -= 16) >= 0);
	for (i+=16; i>0; --i)
		*--htab_p = m1;
}

#ifdef TEST

#include <stdio.h>

#ifdef __MSDOS__
#include <fcntl.h>

#define IOBUFSZ (1024*4)

char inbuf [IOBUFSZ];
char outbuf [IOBUFSZ];
#endif

get ()
{
	return (getchar ());
}

put (c)
{
	putchar (c);
}

main (argc, argv)
char **argv;
{
#ifdef __MSDOS__
	setmode (0, O_BINARY);
	setmode (1, O_BINARY);
	setvbuf (stdin, inbuf, _IOFBF, sizeof (inbuf));
	setvbuf (stdout, outbuf, _IOFBF, sizeof (outbuf));
#endif
	if (! argv[1]) {
usage:          fprintf (stderr, "Usage: compr [-cdpu] < input > output\n");;
		return (0);
	}
	if (! strcmp (argv[1], "-d")) {
		if (compack (1, get, put) < 0)
			fprintf (stderr, "Cannot decompress\n");
	} else if (! strcmp (argv[1], "-p")) {
		if (compack (2, get, put) < 0)
			fprintf (stderr, "Cannot pack\n");
	} else if (! strcmp (argv[1], "-u")) {
		if (compack (3, get, put) < 0)
			fprintf (stderr, "Cannot unpack\n");
	} else if (argc == 1 || ! strcmp (argv[1], "-c")) {
		if (compack (0, get, put) < 0)
			fprintf (stderr, "Cannot compress\n");
	} else
		goto usage;
	fflush (stdout);
	return (0);
}
#endif /* TEST */
