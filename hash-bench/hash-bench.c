#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "GeneralHashFunctions.h"

const char version[] = "1.0";
const char copyright[] = "Copyright (C) 2006 Serge Vakulenko";

char *progname;

hash_function_t func = 0;

/* options descriptor */
static struct option longopts[] = {
	{ "rs",		no_argument,	(int*) &func,	(int) RSHash, },
	{ "js",		no_argument,	(int*) &func,	(int) JSHash, },
	{ "pjw",	no_argument,	(int*) &func,	(int) PJWHash, },
	{ "elf",	no_argument,	(int*) &func,	(int) ELFHash, },
	{ "bkdr",	no_argument,	(int*) &func,	(int) BKDRHash, },
	{ "sdbm",	no_argument,	(int*) &func,	(int) SDBMHash, },
	{ "djb",	no_argument,	(int*) &func,	(int) DJBHash, },
	{ "dek",	no_argument,	(int*) &func,	(int) DEKHash, },
	{ "ap",		no_argument,	(int*) &func,	(int) APHash, },
	{ 0,		0,		0,		0, },
};

void usage ()
{
	fprintf (stderr, "Hash bench, Version %s, %s\n", version, copyright);
	fprintf (stderr, "Usage:\n");
        fprintf (stderr, "\t%s [--rs | --js | --pjw | --elf | --bkdr | --sdbm |\n",
                progname);
        fprintf (stderr, "\t\t\t--djb | --dek | --ap] [file]\n");
	exit (-1);
}

void bench ()
{
	char word [512], *p;
	unsigned int hash;

	while (fgets (word, sizeof(word), stdin)) {
		p = word;
		strsep (&p, "/ \t\f\r\n");
		if (! *word)
			continue;
		hash = func (word, strlen (word));
		printf ("%s  %08x\n", word, hash);
	}
}

int main (int argc, char **argv)
{
	progname = *argv;
	while (getopt_long (argc, argv, "", longopts, 0) >= 0)
		continue;
	argc -= optind;
	argv += optind;

	if (! func || argc > 1)
		usage ();

	if (argc > 0 && ! freopen (argv[0], "r", stdin)) {
		perror (argv[0]);
		exit (-1);
	}
	bench ();
	return (0);
}
