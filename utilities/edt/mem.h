/*
 *      MemCheckIndex (type& array, type, int& bound, int quant, int index)
 *              Check index of dynamic array.
 *              Enlarge array if needed.
 */

# define MemCheckIndex(array, type, bound, quant, index)\
	if ((index) >= (bound)) (array) = (type) MemRealloc ((char *) (array),\
		(int) ((bound) += (quant)) * (int) sizeof (*(array)))

# ifdef BCOPY
#include <strings.h>
#    define MemCopy(t,f,n)      bcopy(f,t,n)
#    define MemCompare(a,b,n)   bcmp(a,b,n)
# else
#include <string.h>
#    define MemCopy(t,f,n)      memcpy(t,f,n)
#    define MemCompare(a,b,n)   memcmp(a,b,n)
# endif

extern char *MemAlloc ();
extern char *MemRealloc ();
