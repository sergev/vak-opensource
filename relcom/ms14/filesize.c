#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

int main (int argc, char **argv)
{
	struct stat st;

	if (argc != 2) {
		fprintf (stderr, "Usage: filesize file\n");
		exit (-1);
	}
	if (stat (argv[1], &st) < 0) {
		perror (argv[1]);
		exit (-1);
	}
	if ((st.st_mode & S_IFMT) != S_IFREG) {
		fprintf (stderr, "%s: %s is not the regular file\n",
			argv[0], argv[1]);
		exit (-1);
	}
	printf ("%ld\n", (long) st.st_size);
	return (0);
}
