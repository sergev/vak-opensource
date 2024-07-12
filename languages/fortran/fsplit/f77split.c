#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BIG 4096

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/*
  Symbolic constants for the language choice:
*/
#define FORTRAN77 0
#define FORTRAN90 1

#define NAMELENGTH 80

int blkdatano = 0;
int language  = FORTRAN77;
int strip     = FALSE;
char suffix   = 'f';

int main(int argc, char **argv);
int compare(char **s0, char *t);
int endcard(char *s);
void get_name(char *s, char *f);
void shorten_line(char *s0);
void split_file(FILE *file_input);

/******************************************************************************/

int main(int argc, char **argv)

/******************************************************************************/
/*
  Purpose:

    MAIN is the main program for F77SPLIT.

  Usage:

    f77split -[language] -s files

  Discussion:

    Procedure X is put in file X.f or X.f90, depending on
    the language option (-f) or the file extension.

    The -s option causes FORTRAN77 procedures to be stripped to 72
    or fewer characters, with trailing blanks removed.

    Comments preceding a procedure, and not associated with a
    preceding procedure, are included with that procedure.

    You cannot use the more elaborate END statements that include the
    procedure definition, such as "END FUNCTION ALPHA".

    If the source file has an extension of F90, I would want the
    output files to have the same extension.

    I can live without the STRIP option.

  Modified:

    04 March 2006
*/
{
    FILE *file_input;
    int i;
    char *s;
    /*
      Examine the argument list for switches.
    */
    while (argc > 1 && argv[1][0] == '-') {
        for (s = argv[1] + 1; *s; ++s) {
            switch (*s) {
            case 'f':
                language = FORTRAN77;
                suffix   = 'f';
                break;

            case 's':
                strip = TRUE;
                break;

            default:
                fprintf(stderr, "\n");
                fprintf(stderr, "F77SPLIT: Error!\n");
                fprintf(stderr, "  Bad option %c\n", *s);
                fprintf(stderr, "  Allowable options are -f, -s.\n");
                return 1;
            }
        }

        --argc;
        ++argv;
    }
    /*
      Only strip FORTRAN77 programs.
    */
    if (strip && language != FORTRAN77) {
        fprintf(stderr, "\n");
        fprintf(stderr, "F77SPLIT: Error!\n");
        fprintf(stderr, "  Only FORTRAN77 programs can be stripped.\n");
    }

    /*
      If no filename was specified, then split up standard input.
    */
    if (argc <= 1) {
        split_file(stdin);
    }
    /*
      Otherwise, open each target file, split it and close it.
    */
    else {
        for (i = 1; i < argc; ++i) {
            printf("Splitting %s.\n", argv[i]);

            file_input = fopen(argv[i], "r");

            if (file_input == NULL) {
                fprintf(stderr, "\n");
                fprintf(stderr, "F77SPLIT: Error!\n");
                fprintf(stderr, "  Cannot open \"%s\"\n", argv[i]);
            } else {
                split_file(file_input);
                fclose(file_input);
            }
        }
    }
    /*
      Terminate.
    */
    printf("\n");
    printf("F77SPLIT:\n");
    printf("  Normal end of execution.\n");

    return 0;
}
/******************************************************************************/

int compare(char **s0, char *t)

/******************************************************************************/
/*
  Purpose:

    COMPARE compares two strings for equality.

  Discussion;

    Assume that t is all lower case.
    Ignore blanks and decase s during comparison.
    s0 points to next character after successful comparison.

  Modified:

    04 March 2006
*/
{
    char *s;
    int s1;

    s = *s0;

    while (*t) {
        while (isspace(*s)) {
            ++s;
        }

        s1 = *s++;

        if (isupper(s1)) {
            s1 = tolower(s1);
        }

        if (s1 != *t++) {
            return (FALSE);
        }
    }
    *s0 = s;
    return (TRUE);
}
/******************************************************************************/

int endcard(char *s)

/******************************************************************************/
/*
  Purpose:

    ENDCARD checks to see whether the current record is an END statement.

  Modified:

    04 March 2006
*/
{
    /*
      column 1 of card image
    */
    char *s0 = s;

    if (*s == 0) {
        return (TRUE);
    }
    /*
      Search for "END" statement somewhere in the card image
    */
    while (isspace(*s)) {
        ++s;
    }

    if (*s != 'e' && *s != 'E') {
        return (FALSE);
    }

    s++;

    while (isspace(*s)) {
        ++s;
    }

    if (*s != 'n' && *s != 'N') {
        return (FALSE);
    }

    s++;
    while (isspace(*s)) {
        ++s;
    }

    if (*s != 'd' && *s != 'D') {
        return (FALSE);
    }

    s++;
    while (isspace(*s)) {
        ++s;
    }

    /*
      Legitimate ending to "END" card?
      This must be modified to handle lines like "END FUNCTION".
    */
    if (*s == '\0' || *s == '!' || *s == '\n' || (s - s0) >= 72) {
        return (TRUE);
    } else {
        return (FALSE);
    }
}
/******************************************************************************/

void get_name(char *s, char *f)

/******************************************************************************/
/*
  Purpose:

    GET_NAME creates a file name based on the name of the current module.

  Modified:

    04 March 2006
*/
{
    int i;
    /*
     *S0 is the address of the first column of the card.
     */
    char *s0 = s;

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
    /*
      Handle size complications like "complex *16" or "character *12".
    */
    else if (compare(&s, "*")) {
        for (++s; isdigit(*s) || isspace(*s); ++s)
            ;
        goto loop;
    } else if (compare(&s, "blockdata")) {
        while (isspace(*s))
            ++s;
        /*
          No block data name given.  Use "BLOCKDATA".
        */
        if (*s == '\0') {
            sprintf(f, "BLOCKDATA%d.%c", ++blkdatano, suffix);
            return;
        }
        goto bot;
    } else {
        s = "";
    }

bot:

    while (isspace(*s)) {
        ++s;
    }
    /*
      Extract the module name.
    */
    for (i = 0; isalpha(*s) || isdigit(*s) || *s == '_'; i++) {
        if (i >= NAMELENGTH || (s - s0) >= 72) {
            break;
        }

        f[i] = tolower(*s++);

        while (isspace(*s)) {
            ++s;
        }
    }

    /*
      Tack on the period and suffix to form the filename.
    */
    if (i > 0) {
        f[i++] = '.';
        f[i++] = suffix;
        f[i++] = '\0';
    } else {
        sprintf(f, "main.%c", suffix);
    }

    return;
}
/******************************************************************************/

void shorten_line(char *s0)

/******************************************************************************/
/*
  Purpose:

    SHORTEN_LINE removes characters beyond column 72, and trailing blanks.

  Modified:

    04 March 2006
*/
{
    char *s;
    char *s72;

    s72 = s0 + 72;

    for (s = s0; s < s72; ++s) {
        if (*s == '\n' || *s == '\0') {
            break;
        }
    }

    while (s > s0 && s[-1] == ' ') {
        --s;
    }

    s[0] = '\n';
    s[1] = '\0';

    return;
}
/******************************************************************************/

void split_file(FILE *file_input)

/******************************************************************************/
/*
  Purpose:

    SPLIT_FILE splits up the modules contained in a single file.

  Modified:

    30 March 2011
*/
{
    char file_out_name[NAMELENGTH];
    char file_temp_name[NAMELENGTH];
    FILE *file_temp;
    int i;
    char in[BIG];
    int line_num;
    int module_num;
    /*
      Set FILE_TEMP_NAME to a template name.
    */
    strcpy(file_temp_name, "fsplit.XXXXX");
    /*
      MKSTEMP replaces the 'X' characters in FILE_TEMP_NAME by characters that
      result in a unique file name.
    */

    /*
      mkstemp ( file_temp_name );
    */

    /*
      Open the temporary file for write access.
    */
    file_temp = fopen(file_temp_name, "w");

    if (file_temp == NULL) {
        fprintf(stderr, "\n");
        fprintf(stderr, "F77SPLIT: Error!\n");
        fprintf(stderr, "  Can't open temporary file \"%s\".\n", file_temp_name);
        exit(EXIT_FAILURE);
    }
    /*
      Read a line from the input unit.
    */
    line_num   = 0;
    module_num = 0;

    while (fgets(in, BIG, file_input) != NULL) {
        line_num = line_num + 1;
        /*
          If the line is a comment line, output it, and get the next line.
          We're really hoping to see a module name.
        */
        if (*in == 'c' || *in == 'C' || *in == '*' || *in == '!') {
            fputs(in, file_temp);
            continue;
        }

        for (i = 0; i < 72; i++) {
            if (in[i] == '\0' || in[i] == '\n') {
                i = 72;
                break;
            }

            if (in[i] != ' ' && in[i] != '\t') {
                break;
            }
        }

        if (i == 72) {
            fputs(in, file_temp);
            continue;
        }

        if (strip) {
            shorten_line(in);
        }

        get_name(in, file_out_name);

        module_num = module_num + 1;

        if (unlink(file_out_name),
            link(file_temp_name, file_out_name) == -1 || unlink(file_temp_name) == -1) {
            fprintf(stderr, "\n");
            fprintf(stderr, "F77SPLIT: Error!\n");
            fprintf(stderr, "  Cannot move \"%s\" to \"%s\".\n", file_temp_name, file_out_name);
            exit(EXIT_FAILURE);
        }

        printf("%s\n", file_out_name);

        fputs(in, file_temp);
        /*
          Write all subsequent lines to this file until an END statement is encountered.
        */
        while (!endcard(in) && fgets(in, BIG, file_input)) {
            line_num = line_num + 1;
            if (strip) {
                shorten_line(in);
            }

            fputs(in, file_temp);
        }
        /*
          Close the current file, and open the next temporary file.
        */
        fclose(file_temp);

        file_temp = fopen(file_temp_name, "w");

        if (file_temp == NULL) {
            fprintf(stderr, "\n");
            fprintf(stderr, "F77SPLIT: Error:\n");
            fprintf(stderr, "  Can't open temporary file %s.", file_temp_name);
            exit(EXIT_FAILURE);
        }
    }

    if (unlink(file_temp_name) == -1) {
        fprintf(stderr, "\n");
        fprintf(stderr, "F77SPLIT: Error:\n");
        fprintf(stderr, "  Couldn't remove the temp file %s\n", file_temp_name);
        exit(EXIT_FAILURE);
    }

    printf("\n");
    printf("F77SPLIT:\n");
    printf("  Number of lines of text = %d\n", line_num);
    printf("  Number of modules = %d\n", module_num);

    return;
}
