/*
 * Example of getline() function: a convenient and safe routine
 * for processing the input stream.
 */
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE *stream = fopen(argv[1], "r");
    if (stream == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stream)) != -1) {
        printf("[%zu] '", nread);
        fwrite(line, nread, 1, stdout);
        printf("'\n");
    }

    free(line);
    fclose(stream);
    return EXIT_SUCCESS;
}
