/*
 * $Id: allowdirectportio.c,v 1.3 2003/06/19 10:29:21 das Exp $
 * Main source file for allowdirectportio.
 *
 * Copyright (C) 2003 Dwight A. Schauer
 *
 *  This file is part of the allowdirectportio package.
 *
 *  allowdirectportio is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  allowdirectportio is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with allowdirectporio; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <sys/io.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sysexits.h>
#include <stdlib.h>

#define PROGRAM_VERSION		"allowdirectportio $Revision: 1.3 $"

#define NUM_IOPORTS_LPT		3	/* LPT ports have 3 I/O ports */

/* define port structure for accessable ports */
typedef struct {
	const char *name;
	unsigned long base;
	unsigned long count;
} tPortIO;

/* declare ports that are to be accessed */
tPortIO IOport[] =
{
	{"LPT1", 0x378, NUM_IOPORTS_LPT},
	{"LPT2", 0x278, NUM_IOPORTS_LPT},
/*	{"LPT3", 0xECF0, 16}, */
/*	{"LPT4", 0xECE0, 16}, */
	{NULL,0},
};

void drop_privleges (void)
{
	/* drop privleges back to those of the user that invoked the wrapper */
	seteuid (getuid ());
}

void usage (int argc, char **argv)
{
	int i;

	drop_privleges();

	if (argc > 1 && strcmp (argv[1], "--version") == 0) {
		printf ("%s\n", PROGRAM_VERSION);
		exit (EXIT_SUCCESS);
	}
	printf("Usage:\n\t%s COMMAND [ARG]...\n\n", argv[0]);
	printf("Run COMMAND, allowing direct I/O access to the following ports:\n");

	/* cycle through list of ports */
	for (i = 0; IOport[i].name != NULL; i++)
	{
		printf ("\t%-12s 0x%03lX - 0x%03lX\n",
			IOport[i].name, IOport[i].base,
			IOport[i].base + (IOport[i].count-1));
	}

	printf("Options:\n");
	printf("\t--help       display this help and exit\n");
	printf("\t--version    output version information and exit\n");

	printf("\nReport bugs to <bugs-allowdirectportio@teegra.net>\n");
	exit (EX_USAGE);
}

int main (int argc, char **argv)
{
	int i;

	if (argc <= 1 || argv[1][0] == '-') {
		usage (argc, argv);
	}

	if (iopl (3)) {
		/* If unable to unrestrict the access, then output appropiate error */
		fprintf (stderr,
			"Unable to raise I/O privlege level.\n%s\n",
			strerror(errno));
		return EX_NOPERM;
	}
	printf ("I/O privelge level raised from 0 to 3\n");

	/* cycle through list of ports */
	for (i = 0; IOport[i].name != NULL; i++) {
		/* Attempt to unrestrict access to the port in question.
		 * To succeed at this, this process must be own owned by
		 * root, so the executable must be setuid root when
		 * invoked by a non-root user. */
		if (ioperm (IOport[i].base, IOport[i].count, 1)) {
			/* If unable to unrestrict the access, then output appropiate error */
			fprintf (stderr,
				"Unable to set permissions for %s\n%s\n",
				IOport[i].name, strerror(errno));
			return EX_NOPERM ;
		}
		printf ("Access allowed to %-12s 0x%03lX - 0x%03lX\n",
			IOport[i].name, IOport[i].base,
			IOport[i].base + (IOport[i].count - 1));
	}

	drop_privleges();

	/* Run wrapped executable with arguments */
	/* Exec is needed so that permissions are inherited */
	return execvp (argv[1], argv+1);
}
