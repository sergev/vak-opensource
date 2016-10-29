/*
 * Support routines for gcc -finstrument-functions
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern const char *__progname;
#define fp_trace stdout
static unsigned lineno = 0;
static unsigned depth = 0;

void __attribute__ ((constructor))
trace_begin (void)
{
    //fp_trace = fopen("trace.out", "w");
}

void __attribute__ ((destructor))
trace_end (void)
{
    //if (fp_trace) {
        //fclose(fp_trace);
        //fp_trace = 0;
    //}
}

static char *funcname (void *addr)
{
    static char buf[256];
#if 1
    sprintf (buf, "%p", addr);
#else
#include <execinfo.h>
    char *p, *q;
    char **str;

    str = backtrace_symbols (&addr, 1);
    if (! str) {
        perror("backtrace_symbols");
        exit(1);
    }
    //fprintf(stderr, "%p -> %s\n", addr, str[0]);
    p = strchr (str[0], '(');
    if (! p) {
failed: strcpy (buf, "??");
        return buf;
    }
    q = strchr (p++, ')');
    if (! q || p == q)
        goto failed;
    if (q[-1] == '0' && q[-2] == '+')
        q -= 2;
    *q = 0;
    strcpy (buf, p);
    free (str);
#endif
    return buf;
}

void __cyg_profile_func_enter (void *func, void *caller)
{
    int i;

    if (! fp_trace)
        return;
    fprintf (fp_trace, "%u ", ++lineno);
    for (i=0; i<depth; i++)
        fprintf (fp_trace, "    ");
    fprintf (fp_trace, "call %s", funcname(func));
    fprintf (fp_trace, " from %s\n", funcname(caller));
    fflush (fp_trace);
    ++depth;
}

void __cyg_profile_func_exit (void *func, void *caller)
{
    int i;

    if (! fp_trace)
        return;
    fprintf (fp_trace, "%u ", ++lineno);
    for (i=0; i<depth; i++)
        fprintf (fp_trace, "    ");
    fprintf (fp_trace, "--- %s", funcname(func));
    fprintf (fp_trace, " return to %s\n", funcname(caller));
    fflush (fp_trace);
    --depth;
}
