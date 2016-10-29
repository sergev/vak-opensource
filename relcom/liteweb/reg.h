#if USE_REGEXP
#   ifdef sun
#      define REGEXP int
#      define REGCOMP(exp) (! re_comp(exp))
#      define REGEXEC(e,s) (re_exec(s))
       extern char *re_comp (char *exp);
       extern int re_exec (char *str);
#   else
#   ifdef __FreeBSD__
#      include <sys/types.h>
#      include <regex.h>
#      define REGEXP int
#      define REGCOMP(exp) (! regcomp(&reg_buf,exp,REG_NOSUB))
#      define REGEXEC(e,s) (! regexec(&reg_buf,s,0,0,0))
       static regex_t reg_buf;
#   else
#      include <regexp.h>
#      define REGEXP regexp
#      define REGCOMP(exp) regcomp(exp)
#      define REGEXEC(e,s) regexec(e,s)
#   endif
#   endif
#else
#   define REGEXP char*
#   define REGCOMP(exp) (exp)
#   define REGEXEC(e,s) match(s,e)
    extern int match (unsigned char *name, unsigned char *pat);
#endif
