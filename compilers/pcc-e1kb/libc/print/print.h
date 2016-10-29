/*	@(#)print.h	1.5	*/
/* Maximum number of digits in any integer representation */
#define MAXDIGS 11

/* Maximum total number of digits in E format */
#if u3b || u3b5
#define MAXECVT 17
#else
#define MAXECVT 18
#endif

/* Maximum number of digits after decimal point in F format */
#define MAXFCVT 60

/* Maximum significant figures in a floating-point number */
#define MAXFSIG MAXECVT

/* Maximum number of characters in an exponent */
#if u3b || u3b5
#define MAXESIZ 5
#else
#define MAXESIZ 4
#endif

/* Maximum (positive) exponent */
#if u3b || u3b5
#define MAXEXP 310
#else
#define MAXEXP 40
#endif

/* Data type for flags */
typedef char bool;

/* Convert a digit character to the corresponding number */
#define tonumber(x) ((x)-'0')

/* Convert a number between 0 and 9 to the corresponding digit */
#define todigit(x) ((x)+'0')

/* Max and Min macros */
#define max(a,b) ((a) > (b)? (a): (b))
#define min(a,b) ((a) < (b)? (a): (b))
