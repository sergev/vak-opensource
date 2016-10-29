/*
 *      MemCheckIndex (type& array, type, int& bound, int quant, int index)
 *              Check index of dynamic array.
 *              Enlarge array if needed.
 */

# define MemCheckIndex(array, type, bound, quant, index)\
	if ((index) >= (bound)) (array) = (type) MemRealloc ((mem *) (array),\
		(int) ((bound) += (quant)) * (int) sizeof (*(array)))

# ifdef BCOPY
#    define MemCopy(t,f,n)      bcopy(f,t,n)
#    define MemCompare(a,b,n)   bcmp(a,b,n)
     extern bcopy ();
     extern bcmp ();
# else
#    define MemCopy(t,f,n)      memcpy(t,f,n)
#    define MemCompare(a,b,n)   memcmp(a,b,n)
     extern char *memcpy ();
     extern memcmp ();
# endif

typedef char mem;

extern mem *MemAlloc ();
extern mem *MemRealloc ();
