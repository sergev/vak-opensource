# include "regio.h"
# include "jlib.h"

static long ymask1 [16] = {
	0x00000001,	0x00000100,	0x00010000,	0x01000000,
	0x00000002,	0x00000200,	0x00020000,	0x02000000,
	0x00000004,	0x00000400,	0x00040000,	0x04000000,
	0x00000008,	0x00000800,	0x00080000,	0x08000000,
};

static long ymask2 [16] = {
	0x00000010,	0x00001000,	0x00100000,	0x10000000,
	0x00000020,	0x00002000,	0x00200000,	0x20000000,
	0x00000040,	0x00004000,	0x00400000,	0x40000000,
	0x00000080,	0x00008000,	0x00800000,	0x80000000,
};

static long ypat1 [16] = {
	0x00000001,	0x00000002,	0x00000004,	0x00000008,
	0x00008000,	0x00004000,	0x00002000,	0x00001000,
	0x00010000,	0x00020000,	0x00040000,	0x00080000,
	0x80000000,	0x40000000,	0x20000000,	0x10000000,
};

static long ypat2 [16] = {
	0x00000010,	0x00000020,	0x00000040,	0x00000080,
	0x00000800,	0x00000400,	0x00000200,	0x00000100,
	0x00100000,	0x00200000,	0x00400000,	0x00800000,
	0x08000000,	0x04000000,	0x02000000,	0x01000000,
};

extern halting;				/* процессор стоит */

extern long ptrecvl ();

win7 ()
{
	register i;
	struct iommu mmu;

	loadmmu (&mmu);
	Jmprintf (2, 4, "i   раф   рам    кр  рсп рзз рфо");
	Jmprintf (2, 44, "i   раф   рам    кр  рсп рзз рфо");
	for (i=0; i<16; ++i) {
		Jmove (4+i, 4);
		prmmu (&mmu, i);
		Jmove (4+i, 44);
		prmmu (&mmu, i+16);
	}
	Jmprintf (21, 34, "рмс     размер");
	Jmprintf (22, 34, " %x      %lx", mmu.pm,
		mmu.pm & 8 ? 1L<<(11+(mmu.pm&7)) : 1024L);
}

win8 ()
{
	register i;
	struct iommu mmu;

	loadmmu (&mmu);
	Jmprintf (2, 4, "i   раф   рам    кр  рсп рзз рфо");
	Jmprintf (2, 44, "i   раф   рам    кр  рсп рзз рфо");
	for (i=0; i<16; ++i) {
		Jmove (4+i, 4);
		prmmu (&mmu, i+32);
		Jmove (4+i, 44);
		prmmu (&mmu, i+32+16);
	}
	Jmprintf (21, 34, "рмс     размер");
	Jmprintf (22, 34, " %x      %lx", mmu.pm,
		mmu.pm & 8 ? 1L<<(11+(mmu.pm&7)) : 1024L);
}

win5 ()
{
	register i, n;
	register char *p;
	struct ioiru o;
	static char *iirbit [32] = {
		"СПСИ4", "ОСПАМК", "КОМЧЦП", "ошчамк", "ПГЦ", "ПТ4К", "ПТ5К", "ЗКОМРК",
		"НУАК", "кк", "ПШР", "СПСИ5", "", "ЗАЩЗ", "ОСПАМЧ", "ЗПЧЦП",
		"ошчамч", "СБСТЗ", "", "", "АВП", "ДНОЛЬ", "ПТЗЧ", "ошп",
		"ошл", "НЗЗ", "ошоп", "ошнрсп", "сброб3", "сброб2", "сброб1", "сброб0",
	};
	static char *eirbit [16] = {
		"ЗАПРОС", "ПЕ33ТМ", "ПОС2", "ПОС0", "ПОП", "1ПОМК3", "1ПОМК2", "1ПОМК1",
		"1ПОМК0", "2ПОМК3", "2ПОМК2", "2ПОМК1", "2ПОМК0", "ПОБ", "ИСЕС", "КОЕС",
	};

	loadiru (&o);
	Jmputch (1, 14, '3');
	Jmputch (1, 34, '2');
	Jmputch (1, 54, '1');
	Jmputch (1, 72, '0');
	Jmprintf (2, 10, "2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1");
	Jmprintf (3, 4, "рпр   ");
	for (i=31; i>=0; --i)
		Jputch (o.iir >> i & 1 ? '1' : '0'), Jputch (' ');
	Jclearbox (4, 10, 6, 63);
	for (i=0; i<32; ++i)
		if (o.iir >> i & 1)
			for (n=0, p=iirbit[i]; *p; ++p, ++n)
				Jmputch (4+n, 72-2*i, *p);
	Jmputch (10, 22, '1');
	Jmputch (10, 40, '0');
	Jmprintf (11, 10, "6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1");
	Jmprintf (12, 4, "грм");
	Jmove (12, 10);
	for (i=15; i>=0; --i)
		Jputch (o.eim >> i & 1 ? '1' : '0'), Jputch (' ');
	Jmprintf (13, 4, "грвп");
	Jmove (13, 10);
	for (i=15; i>=0; --i)
		Jputch (o.eir >> i & 1 ? '1' : '0'), Jputch (' ');
	Jclearbox (14, 10, 6, 31);
	for (i=0; i<16; ++i)
		if ((o.eir | o.eim) >> i & 1)
			for (n=0, p=eirbit[i]; *p; ++p, ++n)
				Jmputch (14+n, 40-2*i, *p);

	Jmprintf (16, 63, "исес  %x", o.scx);
	Jmprintf (17, 63, "коес  %x", o.scr);
	Jmprintf (18, 63, "у0ес  %x", o.scz);
	Jmprintf (17, 46, "поб   %04x", o.bci);

	for (i=0; i<4; ++i) {
		Jmprintf (11+i, 46, "в УК %d  %05lx", i, o.ucs [i]);
		Jmprintf (11+i, 63, "из УК %d  %05lx", i, o.ucr [i]);
	}
	Jmprintf (21, 5, "чс    %04x ", (int) o.clock.l);
	prhalf (o.clock.r);
	Jmprintf (22, 5, "тм         ");
	prhalf (o.tm);
}

win3 ()
{
	register i;
	struct iobwr bwr;

	loadbwr (&bwr);
	for (i=0; i<8; ++i) {
		Jmprintf (3+i, 5, "  баз%x  ", i);
		prreg (bwr.a[i]);
		Jprintf ("        брз%x  ", i);
		prword (&bwr.w[i]);
		Jprintf ("        %02x", bwr.wt[i] & 0xff);
	}
}

win4 ()
{
	register i, line;
	struct iolcm b;
	static char *chnam [] = { "уск%d    ", "аск%d    ", "сск%d    ", "        " };

	loadlcm (&b, 0);
	line = 3;
	for (i=0; i<32; i+=2) {
		Jmprintf (line, 3, chnam [i%8/2], i/8);
		prlcm (i, b.w, b.wt);
		Jprintf ("    ");
		prlcm (i+1, b.w, b.wt);
		++line;
		if (i % 8 == 6)
			++line;
	}
}

win6 ()
{
	register i, line;
	struct iolcm b;

	loadlcm (&b, 1);
	line = 5;
	for (i=0; i<32; i+=2) {
		Jmprintf (line++, 3, i%4 ? "    п   " : "бк%d и   ", i/4);
		prlcm (i, b.w, b.wt);
		Jprintf ("    ");
		prlcm (i+1, b.w, b.wt);
	}
}

win2 ()
{
	register i;
	struct iobcr bcr;

	loadbcr (&bcr);
	for (i=0; i<8; ++i) {
		Jmprintf (3+i, 5, "  бас%x  ", i);
		prreg (bcr.a[i]);
		Jprintf ("      брс%x  ", i);
		prcmd (bcr.w[i].l);
		Jprintf ("  ");
		prcmd (bcr.w[i].r);
		Jprintf ("      %02x", bcr.wt[i] & 0xff);
	}
}

win0 ()
{
	register i;
	struct ioregs regs;

	loadregs (&regs);
	Jmprintf (4, 5, "счас ");
	prreg (regs.pc);
	Jmprintf (6, 5, "  рк ");
	prcmd (regs.cr);
	Jmprintf (8, 5, " ркп ");
	prcmd (regs.crp);
	for (i=0; i<8; ++i) {
		Jmprintf (3+i, 30, "  м%x ", i);
		prreg (regs.r[i]);
		Jprintf ("  м%x ", i+8);
		prreg (regs.r[i+8]);
		Jprintf ("  ис%x ", i);
		prreg (regs.s[i]);
	}
	Jmprintf (12, 10, "см  ");
	prword (&regs.a);
	Jmprintf (12, 40, "рмр  ");
	prword (&regs.y);
	Jmprintf (14, 10, "    6         5         4");
	Jprintf ("         3         2         1        0");
	Jmprintf (15, 10, "4321098765432109876543210");
	Jprintf ("987654321098765432109876543210987654321");
	Jmprintf (16, 7, "см ");
	for (i=31; i>=0; --i)
		Jputch (regs.a.l >> i & 1 ? '1' : '0');
	for (i=31; i>=0; --i)
		Jputch (regs.a.r >> i & 1 ? '1' : '0');
}

static struct word getacc ()
{
	struct word a;
	register n;

	/* load accumulator */
	a.r = ptrecvb (0, 0) & 077;
	a.r |= (long) (ptrecvb (0, 1) & 077) << 6;
	a.r |= (long) (ptrecvb (0, 2) & 077) << 12;
	a.r |= (long) (ptrecvb (0, 3) & 077) << 18;
	a.r |= (long) (ptrecvb (0, 4) & 077) << 24;
	n = ptrecvb (0, 5) & 077;
	a.r |= (long) n << 30;
	a.l = n >> 2;
	a.l |= (long) (ptrecvb (0, 6) & 077) << 4;
	a.l |= (long) (ptrecvb (0, 7) & 077) << 10;
	a.l |= (long) (ptrecvb (0, 033) & 037) << 16;
	a.l |= (long) (ptrecvb (0, 075) & 7) << 21;
	a.l |= (long) ptrecvb (0, 076) << 24;
	return (a);
}

struct word getyoung ()
{
	struct word y;
	register long a, b;
	register i;

	y.l = y.r = 0;
	a = ptrecvl (0, 060);
	b = ptrecvl (0, 064);
	for (i=0; i<16; ++i) {
		if (a & ymask1 [i])
			y.r |= ypat1 [i];
		if (a & ymask2 [i])
			y.l |= ypat1 [i];
		if (b & ymask1 [i])
			y.r |= ypat2 [i];
		if (b & ymask2 [i])
			y.l |= ypat2 [i];
	}
	return (y);
}

static loadregs (r)
register struct ioregs *r;
{
	register n;
	register long h;

	/* enable access to cpu registers */
	stop ();
	ptenable ();
	/* регистры */
	for (n=0; n<16; ++n)
		r->r[n] = ptrecvl (n, 0130);
	/* спец. регистры */
	for (n=0; n<8; ++n)
		r->s[n] = ptrecvl (n+030, 0130);
	/* сумматор */
	r->a = getacc ();
	/* РМР */
	r->y = getyoung ();
	/* РК */
	r->cr = ptrecvl (0, 0120);
	/* РКП */
	r->crp = ptrecvl (0, 0124);
	/* СЧАС */
	h = ptrecvl (0, 0150);
	r->pc = h&0177 | h>>1&037600L | h>>2 & 07740000L | h>>3 & 01770000000L;
	/* disable access to cpu registers */
	if (! halting)
		run ();
}

static loadiru (r)
register struct ioiru *r;
{
	register n, q, i;
	register long h;

	/* enable access to cpu registers */
	stop ();
	ptenable ();

	h = 0;
	for (n=0; n<8; ++n)
		h |= (long) (ptrecvb (037, 040+n) & 0xf) << 4*n;
	r->iir = h;

	h = 0;
	for (n=0; n<8; ++n)
		h |= (long) (ptrecvb (036, 040+n) & 0xf) << 4*n;
	r->tm = h;

	h = 0;
	for (n=0; n<8; ++n)
		h |= (long) (ptrecvb (035, 040+n) & 0xf) << 4*n;
	r->clock.r = h;
	h = 0;
	for (n=0; n<4; ++n)
		h |= (long) (ptrecvb (035, 050+n) & 0xf) << 4*n;
	r->clock.l = h;

	q = 0;
	for (n=0; n<4; ++n)
		q |= (ptrecvb (047, 050+n) & 0xf) << 4*n;
	r->eir = q;

	q = 0;
	for (n=0; n<4; ++n)
		q |= (ptrecvb (046, 050+n) & 0xf) << 4*n;
	r->eim = q;

	q = 0;
	for (n=0; n<4; ++n)
		q |= (ptrecvb (043, 050+n) & 0xf) << 4*n;
	r->bci = q;

	r->scx = ptrecvb (044, 050) & 0xf;
	r->scr = ptrecvb (044, 051) & 0xf;
	r->scz = ptrecvb (044, 052) & 0xf;

	for (i=0; i<4; ++i) {
		h = 0;
		for (n=0; n<5; ++n)
			h |= (long) (ptrecvb (070+i, 040+n) & 0xf) << 4*n;
		r->ucr [i] = h & 0x3ffff;
		h = 0;
		for (n=0; n<5; ++n)
			h |= (long) (ptrecvb (074+i, 040+n) & 0xf) << 4*n;
		r->ucs [i] = h & 0x3ffff;
	}
	r->ucs[0] = ~r->ucs[0] & 0x3ffff;
	r->ucs[1] = ~r->ucs[1] & 0x3ffff;
	/* disable access to cpu registers */
	if (! halting)
		run ();
}

static loadbcr (b)
register struct iobcr *b;
{
	register n;
	register long a;

	/* enable access to cpu registers */
	stop ();
	ptenable ();
	for (n=0; n<8; ++n) {
		b->w[n].r = ptrecvl (n, 0230);
		b->w[n].l = ptrecvl (n, 0234);
		b->wt[n] = ptrecvb (n, 0242);
		a = ptrecvl (n, 0154);
		b->a[n] = a&0x7f | a>>1&0x3f80 | a>>2&0x1fc000 |
			a>>3 & 0xfe00000;
	}
	/* disable access to cpu registers */
	if (! halting)
		run ();
}

static loadbwr (b)
register struct iobwr *b;
{
	register n, i;
	register long a;
	int c;

	/* enable access to cpu registers */
	stop ();
	ptenable ();
	for (n=0; n<8; ++n) {
		b->w[n].r = 0;
		for (i=0; i<8; ++i)
			b->w[n].r |= (long) (ptrecvb (n, 040+i) & 0xf) << (4*i);
		b->w[n].l = 0;
		for (i=0; i<4; ++i)
			b->w[n].l |= (long) (ptrecvb (n, 050+i) & 0xf) << (4*i);
		b->w[n].l |= (long) ptrecvb (n, 056) << 16;
		b->w[n].l |= (long) inverbyte (ptrecvb (n, 057)) << 24;
		b->wt[n] = ptrecvb (n, 070);
		a = ptrecvl (n, 0164);
		b->a[n] = a&0x7f | a>>1&0x3f80 | a>>2&0x1fc000 |
			a>>3 & 0xfe00000;
	}
	/* disable access to cpu registers */
	if (! halting)
		run ();
}

static loadlcm (b, hi)
register struct iolcm *b;
{
	register n;

	/* enable access to cpu registers */
	stop ();
	ptenable ();
	for (n=0; n<32; ++n)
		load1lcm (&b->w[n], &b->wt[n], hi ? n+32 : n);
	/* disable access to cpu registers */
	if (! halting)
		run ();
}

load1lcm (w, t, n)
register struct word *w;
char *t;
register n;
{
	register i;

	w->r = 0;
	for (i=0; i<8; ++i)
		w->r |= (long) (ptrecvb (0200|n, 040+i) & 0xf) << (4*i);
	w->l = 0;
	for (i=0; i<4; ++i)
		w->l |= (long) (ptrecvb (0200|n, 050+i) & 0xf) << (4*i);
	w->l |= (long) ptrecvb (0200|n, 056) << 16;
	w->l |= (long) inverbyte (ptrecvb (0200|n, 057)) << 24;
	*t = ptrecvb (0200|n, 070);
}

static loadmmu (b)
register struct iommu *b;
{
	register n, i;

	/* enable access to cpu registers */
	stop ();
	ptenable ();
	for (n=0; n<64; ++n) {
		b->ma[n] = 0;
		for (i=0; i<5; ++i)
			b->ma[n] |= (long) (ptrecvb (n+0100, 040+i) & 0xf) << (4*i);
		b->ma[n] &= 0x3ffff;
		b->pa[n] = 0;
		for (i=0; i<4; ++i)
			b->pa[n] |= (ptrecvb (n+0100, 050+i) & 0xf) << (4*i);
		b->t[n] = ptrecvb (n+0100, 070) >> 5 & 3;
	}
	load1 (&b->en, 051, 057);
	load1 (&b->ro, 052, 057);
	load1 (&b->us, 053, 057);
	b->pm = ptrecvb (042, 050) & 0xf;
	/* disable access to cpu registers */
	if (! halting)
		run ();
}

static load1 (w, a, b)
register struct word *w;
{
	register i;

	w->r = ptrecvb (a, b);
	w->r |= (long) inverbyte (ptrecvb (a, --b)) << 8;
	b -= 2;
	for (i=0; i<4; ++i)
		w->r |= (long) inverbyte (ptrecvb (a, --b) & 0xf) << (4*i + 12);
	w->l = 0;
	for (i=0; i<8; ++i)
		w->l |= (long) inverbyte (ptrecvb (a, --b) << 4 & 0xf0) << (4*i);
}

beep ()
{
	putchar ('\7');
}

static inverbyte (c)
register c;
{
	register r;

	for (r=0; c; c>>=1, r<<=1)
		r |= c & 1;
	return (r);
}

prword (w)
register struct word *w;
{
	Jprintf ("%04x %04x %04x %04x", (int) (w->l >> 16),
		(int) w->l, (int) (w->r >> 16), (int) w->r);
}

prhalf (h)
register unsigned long h;
{
	Jprintf ("%04x %04x", (int) (h >> 16), (int) h);
}

static prcmd (c)
register long c;
{
	register cmd = c>>20 & 0xff;

	if (cmd == 0x3f)
		Jprintf ("%x %04x %03x", (int) (c >> 28) & 0xf,
			(int) (c >> 12) & 0xffff, (int) c & 0xfff);
	else
		Jprintf ("%x %02x %05lx", (int) (c >> 28) & 0xf,
			(int) (c >> 20) & 0xff, c & 0xfffff);
}

static prlcm (i, w, t)
struct word *w;
char *t;
{
	Jprintf ("мпк%02x  ", i);
	prword (w + i);
	Jprintf ("  %02x", t [i] & 0xff);
}

static prmmu (m, i)
register struct iommu *m;
{
	Jprintf ("%2x  %04x  %05lx  %x   %x   %x   %x", i,
		m->pa[i], m->ma[i], m->t[i], getbit (&m->en, i),
		getbit (&m->ro, i), getbit (&m->us, i));
}

static prreg (r)
register long r;
{
	Jprintf ("%03x %04x", (int) (r>>16 & 0xfff), (int) r);
}

getbit (w, n)
register struct word *w;
register n;
{
	if (n < 32)
		return (w->r >> n & 1);
	else
		return (w->l >> (n-32) & 1);
}
