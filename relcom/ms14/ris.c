#include <stdio.h>
#include <ris/ris.h>
#include <ris/ris_info.h>
#include <ris/risa_info.h>

char risreply[256];

int rischeck (char *addr)
{
	int k;

	risreply[0] = 0;
	k = RISA_check ("mailserv", 0, addr, RL_ADDR);
	switch (k) {
	case 1:         return (0);     /* OK */
	case RIS_TEMPE: return (0);     /* RIS temporarily unavailable */
	case RIS_TEMPD: return (0);     /* DNS temporarily unavailable */
	case 0:         return (1);     /* Access denied */
	default:        return (k);     /* System error */
	}
}

#ifdef DEBUG
int main (int argc, char **argv)
{
	int k;

	if (argc != 2) {
		fprintf (stderr, "Usage: ris user@host\n");
		return (-1);
	}
	ris_debug = 1;
	k = rischeck (argv[1]);
	printf ("rischeck (%s) ", argv[1]);
	switch (k) {
	case 0:  printf ("-- OK\n");             break;
	case 1:  printf ("-- access denied\n");  break;
	default: printf ("error, code=%d\n", k); break;
	}
	return (0);
}

void ris_error (char *fmt, char *a, char *b, char *c)
{
	printf (fmt, a, b, c);
	printf ("\n");
}

#else

void ris_error (char *fmt, char *a, char *b, char *c)
{
	sprintf (risreply, fmt, a, b, c);
}
#endif
