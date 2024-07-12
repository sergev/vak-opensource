#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

//
// COMPARE compares two strings for equality.
//
// Assume that t is all lower case.
// Ignore blanks and decase s during comparison.
// s0 points to next character after successful comparison.
//
bool compare(char **s0, char *t)
{
    char *s = *s0;

    while (*t) {
        while (isspace(*s)) {
            ++s;
        }
        int ch = *s++;
        if (isupper(ch)) {
            ch = tolower(ch);
        }
        if (ch != *t++) {
            return (false);
        }
    }
    *s0 = s;
    return (true);
}

bool consume(char **p, char uppercase)
{
    while (isspace(**p)) {
        ++(*p);
    }
    if (**p != uppercase && **p != tolower(uppercase)) {
        return false;
    }
    (*p)++;
    return true;
}

//
// IS_PROGRAM checks to see whether the current record is an END statement.
//
bool is_program(char *s)
{
    if (*s == 0) {
        return (true);
    }

    // Search for "PROGRAM" statement somewhere in the card image
    if (!consume(&s, 'P'))
        return false;
    if (!consume(&s, 'R'))
        return false;
    if (!consume(&s, 'O'))
        return false;
    if (!consume(&s, 'G'))
        return false;
    if (!consume(&s, 'R'))
        return false;
    if (!consume(&s, 'A'))
        return false;
    if (!consume(&s, 'M'))
        return false;
    return true;
}

//
// GET_NAME creates a file name based on the name of the current module.
//
void get_name(char *s, char *result, unsigned name_length)
{
    // S0 is the address of the first column of the card.
    char *s0 = s;

#if 0
loop:
    if (compare(&s, "function")) {
        goto bot;
    } else if (compare(&s, "module")) {
        goto bot;
    } else if (compare(&s, "procedure")) {
        goto bot;
    } else if (compare(&s, "program")) {
        goto bot;
    } else if (compare(&s, "subroutine")) {
        goto bot;
    } else if (compare(&s, "real")) {
        goto loop;
    } else if (compare(&s, "integer")) {
        goto loop;
    } else if (compare(&s, "logical")) {
        goto loop;
    } else if (compare(&s, "double")) {
        goto loop;
    } else if (compare(&s, "precision")) {
        goto loop;
    } else if (compare(&s, "complex")) {
        goto loop;
    } else if (compare(&s, "character")) {
        goto loop;
    }

    // Handle size complications like "complex *16" or "character *12".
    else if (compare(&s, "*")) {
        for (++s; isdigit(*s) || isspace(*s); ++s)
            ;
        goto loop;
    } else if (compare(&s, "blockdata")) {
        while (isspace(*s))
            ++s;

        // No block data name given.  Use "BLOCKDATA".
        if (*s == '\0') {
            static int blkdatano = 0;
            sprintf(result, "blockdata%d.f", ++blkdatano);
            return;
        }
        goto bot;
    }
#else
    // For BESM-6 libraries.
    if (compare(&s, "program")) {
        goto bot;
    }
#endif
    else {
        s = "";
    }

bot:
    while (isspace(*s)) {
        ++s;
    }

    // Extract the module name.
    int i;
    for (i = 0; isalpha(*s) || isdigit(*s) || *s == '_'; i++) {
        if (i >= name_length || (s - s0) >= 72) {
            break;
        }
        result[i] = tolower(*s++);
        while (isspace(*s)) {
            ++s;
        }
    }
    if (i == 0) {
        printf("No file name from '%s'\n", s0);
        exit(EXIT_FAILURE);
    }
    result[i] = 0;

    // Tack on the period and suffix to form the filename.
    strcat(result, ".f");
}

//
// SPLIT_FILE splits up the modules contained in a single file.
//
void split_file(FILE *file_input)
{
    const char file_temp_name[] = "fsplit.tmp";
    char file_out_name[80] = "";

    // Open the temporary file for write access.
    FILE *file_temp = fopen(file_temp_name, "w");
    if (!file_temp) {
        perror(file_temp_name);
        exit(EXIT_FAILURE);
    }

    // Read a line from the input unit.
    char buf[4096];
    int line_num   = 0;
    int module_num = 0;
    while (fgets(buf, sizeof(buf), file_input) != NULL) {
        line_num = line_num + 1;

        // If the line is a comment line, output it, and get the next line.
        // We're really hoping to see a module name.
        if (*buf == 'c' || *buf == 'C' || *buf == '*' || *buf == '!') {
            fputs(buf, file_temp);
            continue;
        }

        // Write all subsequent lines to this file until a PROGRAM statement is encountered.
        if (!is_program(buf)) {
            fputs(buf, file_temp);
            continue;
        }

        // Finish current file and open the next temporary file.
        if (file_out_name[0] != 0) {
            fclose(file_temp);
            if (access(file_out_name, R_OK) >= 0) {
                fprintf(stderr, "%s: file already exists\n", file_out_name);
                exit(EXIT_FAILURE);
            }
            if (link(file_temp_name, file_out_name) < 0) {
                perror(file_out_name);
                exit(EXIT_FAILURE);
            }
            if (unlink(file_temp_name) < 0) {
                perror(file_temp_name);
                exit(EXIT_FAILURE);
            }
            file_temp = fopen(file_temp_name, "w");
            if (!file_temp) {
                perror(file_temp_name);
                exit(EXIT_FAILURE);
            }
        }

        // Get name from the PROGRAM statement.
        get_name(buf, file_out_name, sizeof(file_out_name));
        module_num = module_num + 1;

        //printf("%s from '%s'\n", file_out_name, buf);
        printf("%s\n", file_out_name);

        // Write PROGRAM statement to the new file.
        fputs(buf, file_temp);
    }

    // Finish the last file.
    if (file_out_name[0] == 0) {
        fprintf(stderr, "No Fortram program found\n");
        return;
    }

    fclose(file_temp);
    if (access(file_out_name, R_OK) >= 0) {
        fprintf(stderr, "%s: file already exists\n", file_out_name);
        exit(EXIT_FAILURE);
    }
    if (link(file_temp_name, file_out_name) < 0) {
        perror(file_out_name);
        exit(EXIT_FAILURE);
    }
    if (unlink(file_temp_name) < 0) {
        perror(file_temp_name);
        exit(EXIT_FAILURE);
    }

    printf("  Number of lines of text = %d\n", line_num);
    printf("  Number of modules = %d\n", module_num);
}

//
// MAIN is the main program for F77SPLIT.
//
// Usage:
//      f77split [-s] files
//
// Procedure X is put in file X.f.
//
// Comments preceding a procedure, and not associated with a
// preceding procedure, are included with that procedure.
//
// You cannot use the more elaborate END statements that include the
// procedure definition, such as "END FUNCTION ALPHA".
//
int main(int argc, char **argv)
{
    // Examine the argument list for switches.
    while (argc > 1 && argv[1][0] == '-') {
        for (char *s = argv[1] + 1; *s; ++s) {
            switch (*s) {
            default:
                printf("Bad option %c\n", *s);
                return 1;
            }
        }
        --argc;
        ++argv;
    }
    if (argc <= 1) {
        printf("Usage:\n");
        printf("    f77split [-s] files\n");
        return 1;
    }

    // Open each target file, split it and close it.
    for (int i = 1; i < argc; ++i) {
        printf("Splitting %s\n", argv[i]);

        FILE *file_input = fopen(argv[i], "r");
        if (!file_input) {
            perror(argv[i]);
        } else {
            split_file(file_input);
            fclose(file_input);
        }
    }
    return 0;
}
