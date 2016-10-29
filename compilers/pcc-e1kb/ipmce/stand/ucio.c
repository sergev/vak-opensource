# include "svsb.h"
# include "ucio.h"

# define in             _in_
# define out            _out_

# define odd(c)         (_nbits_ (c) & 1)

# define outack(u,a)    out (UCREG(u), oreg[u]=UC_RDY|(a)?oreg[u]|UC_ACK:oreg[u]&~UC_ACK)
# define outdata(u,a)   out (UCREG(u), oreg[u]=UC_STROBE|oreg[u]&~UC_PDATA|(a)&UC_PDATA)
# define outclr(u,a)    out (UCREG(u), oreg[u]&=~(a))

int oreg [u];

ucrecv (u)
{
	int c, ireg;
loop:
	ireg = in (UCREG (u));
	if (! (ireg & UC_STROBE)) {             /* wait for strobe */
		idle (1);
		goto loop;
	}
	outclr (u, UC_RDY);                     /* clear 'ready' */

	c = ireg & UC_PDATA;                    /* get data & parity */
	if (! odd (c)) {
		outack (u, 0);                  /* bad parity */
		printf (MSG ("uc%d: receive error %x\n",
			"УК %d: ошибка приема %x\n"), u, c);
		idle (1);
		goto loop;
	}
	outack (u, 1);                          /* receive ok */
	return (c & UC_DATA);
}

ucsend (u, d)
{
	int ireg;

	d &= UC_DATA;
	if (! odd (d))
		d |= UC_PARITY;
	outdata (u, d);
loop:
	ireg = in (UCREG (u));
	if (! (ireg & UC_RDY)) {        /* wait for 'ready' */
		idle (1);
		goto loop;
	}
	outclr (u, UC_STROBE);          /* clear strobe */
	if (! (ireg & UC_ACK)) {
		outdata (u, d);         /* retry */
		printf (MSG ("uc%d: send error\n",
			"УК %d: ошибка передачи\n"), u);
		idle (1);
		goto loop;
	}
}
