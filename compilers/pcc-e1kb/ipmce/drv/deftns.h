/*
 *      Interface controller definitions
 */

# ifdef DEBUG
out (port, val)
unsigned port;
{
	if (port & IC_CTRL) {
		printf ("Port %03xh", port);
		if (val & STROBE)
			printf ("  Strobe");
		if (val & INIT)
			printf ("  Init");
		printf ("\n");
	} else
		printf ("Port %03xh  Data %03xh  Inverted %03xh\n",
			port, val & 0xFF, ~val & 0xFF);
}
# else
# define out(port,val) outp ((port), (val))
# endif

/*      Stand-alone tool parameters area        */

# define SA_PARMS       0x3e000L
# define SA_MODE        (SA_PARMS + 0)
# define SA_DIR         (SA_PARMS + 1)
# define SA_CHAN        (SA_PARMS + 2)
# define SA_UNIT        (SA_PARMS + 3)
# define SA_MEM         (SA_PARMS + 5)
# define SA_BLOCK       (SA_PARMS + 6)
# define SA_LENGTH      (SA_PARMS + 7)


/*      Default parameters      */

# define DP_DISK        1L
# define DP_WRITE       0L
# define DP_CHAN        0L
# define DP_UNIT        0L
# define DP_MEM         0x40000L
