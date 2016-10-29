/*
 * Комплексный тест БК интерфейса Эльбрус-Б - PC AT.
 */

# include "svsb.h"
# include "ucio.h"

# define OK             0xa5            /* reply statys OK */
# define BAD            0xff            /* reply statys BAD */

# define BUFSZ          256             /* size of UC input-output buffer */

# define in             _in_            /* put word into SVSB HW register */
# define out            _out_           /* get word from SVSB HW register */

# define ODD(c)         (_nbits_ (c) & 1) /* if word has odd number of bits */
# define HALT(c)        _halt_ (c)      /* stop SVSB processor */
# define CLOCK()        _in_ (0x1d)

# define OUTACK(u)      { out ((u), oreg &= ~UC_RDY); out ((u), oreg = UC_RDY | oreg | UC_ACK); }
# define OUTNACK(u)     { out ((u), oreg &= ~UC_RDY); out ((u), oreg = UC_RDY | oreg & ~UC_ACK); }
# define OUTDATA(u,a)   out ((u), oreg = UC_STROBE | oreg & ~UC_PDATA | (a) & UC_PDATA)
# define OUTCLR(u,a)    out ((u), oreg &= ~(a))

# define IDLE()         _out_ (0x14, 0)

int uchan;              /* univ. channel number */

int oreg;               /* contents of UC output register */
int ireg;               /* contents of UC input register */

int ubuf [BUFSZ];
int u1buf [BUFSZ];
int u2buf [BUFSZ];

main ()
{
	uchan = uprobe ();      /* опознавание универсального канала */
	uchan = UCREG (uchan);

	for (;;) {
		uctest ();
	}
}

uprobe ()
{
	int i2, i3;

	i2 = in (UCREG (2));
	i3 = in (UCREG (3));

	for (;;) {
		IDLE ();
		if (i2 != in (UCREG (2)))
			return (2);
		if (i3 != in (UCREG (3)))
			return (3);
	}
}

uctest ()
{
	int i;

	/*
	 * I. Waiting for strobe.
	 *    Read UC input register and write it
	 *    to output register until strobe is set.
	 */
	for (;;) {
		IDLE ();
		ireg = in (uchan);
		if (ireg & UC_STROBE)
			break;
		oreg = ireg;
		out (uchan, oreg);
	}

	/*
	 * II. Set STROBE bit and 0 bit of data.
	 */
	oreg = UC_STROBE | 1;
	out (uchan, oreg);

	/*
	 * III. Waiting for READY bit in input register.
	 */
	for (;;) {
		ireg = in (uchan);
		if (ireg & UC_RDY)
			break;
	}

	/*
	 * IV. Set READY bit and 1 bit of data.
	 */
	oreg = UC_RDY | 2;
	out (uchan, oreg);

	/*
	 * V. Receive BUFSZ words of from UC.
	 *    Put them into ubuf.
	 *    Check if this is counter code.
	 *    Send reply through UC.
	 */
	for (i=0; i<BUFSZ; ++i)
		ubuf [i] = recv (0);
	reply (checkcount (ubuf));

	/*
	 * VI. Receive BUFSZ words with bad parity.
	 *     Check if this is counter code.
	 *     Send reply through UC.
	 */
	for (i=0; i<BUFSZ; ++i)
		ubuf [i] = recv (1);
	reply (checkcount (ubuf));

	/*
	 * VII. Send BUFSZ words of counter code to UC.
	 */
	for (i=0; i<BUFSZ; ++i)
		send (ubuf [i], 0);

	/*
	 * VIII. Send BUFSZ words of counter with bad parity.
	 */
	for (i=0; i<BUFSZ; ++i)
		send (ubuf [i], 1);

	/*
	 * IX. Send/receive 2*BUFSZ words.
	 *     Compare received arrays and send status byte.
	 */
	/* genrandom (ubuf); */
	sendrecv (ubuf, u1buf);
	reply (1);
	sendrecv (ubuf, u2buf);
	reply (! bcmp ((char *) u1buf, (char *) u2buf, BUFSZ * sizeof (int)));
}

recv (invpar)
{
	int c;
loop:
	IDLE ();
	ireg = in (uchan);
	if (! (ireg & UC_STROBE))               /* wait for strobe */
		goto loop;

	c = ireg & UC_PDATA;                    /* get data & parity */
	if (invpar) {                           /* invert parity */
		if (ODD (c)) {
			OUTACK (uchan);         /* bad parity */
			HALT (0xdada);
		}
		OUTNACK (uchan);                /* receive ok */
	} else {
		if (! ODD (c)) {
			OUTNACK (uchan);        /* bad parity */
			HALT (0xdada);
		}
		OUTACK (uchan);                 /* receive ok */
	}
	return (c & UC_DATA);
}

send (d, negpar)
{
	d &= UC_DATA;
	if (ODD (d) == negpar)          /* compute parity - make it odd */
		d |= UC_PARITY;
	OUTDATA (uchan, d);
loop:
	IDLE ();
	ireg = in (uchan);
	if (! (ireg & UC_RDY))          /* wait for 'ready' */
		goto loop;
	OUTCLR (uchan, UC_STROBE);

	if (! negpar == ! (ireg & UC_ACK))
		HALT (0xcaca);
}

sendrecv (outbuf, inbuf)
int outbuf [], inbuf [];
{
	int ic, oc, d, c;

	ic = 0;
	oc = 0;

	d = outbuf [oc++] & UC_DATA;
	if (! ODD (d))
		d |= UC_PARITY;
	OUTDATA (uchan, d);
loop:
	IDLE ();
	ireg = in (uchan);

	if (oc < BUFSZ && (ireg & UC_RDY)) {    /* wait for 'ready' */
		OUTCLR (uchan, UC_STROBE);
		if (! (ireg & UC_ACK))   /* bad parity */
			HALT (0xbaba);

		d = outbuf [oc++] & UC_DATA;
		if (! ODD (d))
			d |= UC_PARITY;
		OUTDATA (uchan, d);
	}

	if (ic < BUFSZ && (ireg & UC_STROBE)) { /* wait for strobe */
		c = ireg & UC_PDATA;            /* get data & parity */

		if (! ODD (c))
			HALT (0xfafa);          /* bad parity */

		OUTACK (uchan);                 /* receive ok */
		inbuf [ic++] = c & UC_DATA;
	}

	if (ic < BUFSZ || oc < BUFSZ)
		goto loop;

	while (! (in (uchan) & UC_RDY))       /* wait for 'ready' */
		IDLE ();
	OUTCLR (uchan, UC_STROBE);

	if (! (ireg & UC_ACK))                  /* bad parity */
		HALT (0xbaba);
}

genrandom (buf)
int buf [];
{
	int i;

	srand (CLOCK ());
	for (i=0; i<BUFSZ; ++i)
		buf [i] = rand () & 0x3ff;
}

checkcount (buf)
int buf [];
{
	int i, c;

	for (i=0; i<BUFSZ; ++i) {
		c = i & 0xff | i<<8 & 0x300;
		if (buf [i] != c)
			return (0);
	}
	return (1);
}

reply (ok)
{
	if (ok)
		send (OK, 0);
	else {
		send (BAD, 0);
		HALT (0xbad);
	}
}
