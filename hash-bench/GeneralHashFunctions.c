/*
 * General Purpose Hash Function Algorithms Library
 *
 * Author: Arash Partow - 2002
 * URL: http://www.partow.net
 * URL: http://www.partow.net/programming/hashfunctions/index.html
 *
 * Copyright notice:
 * Free use of the General Purpose Hash Function Algorithms Library is
 * permitted under the guidelines and in accordance with the most current
 * version of the Common Public License.
 * http://www.opensource.org/licenses/cpl.php
 */
#include "GeneralHashFunctions.h"

/*
 * RS Hash Function
 */
unsigned int RSHash (char *str, unsigned int len)
{
	unsigned int b = 378551;
	unsigned int a = 63689;
	unsigned int hash = 0;
	unsigned int i = 0;

	for (i = 0; i < len; str++, i++) {
		hash = hash * a + (unsigned char)(*str);
		a = a * b;
	}
	return hash;
}

/*
 * JS Hash Function
 */
unsigned int JSHash (char *str, unsigned int len)
{
	unsigned int hash = 1315423911;
	unsigned int i = 0;

	for (i = 0; i < len; str++, i++) {
		hash ^= ((hash << 5) + (unsigned char)(*str) + (hash >> 2));
	}
	return hash;
}

/*
 * P. J. Weinberger Hash Function
 */
unsigned int PJWHash (char *str, unsigned int len)
{
	unsigned int BitsInUnsignedInt = (unsigned int) (sizeof (unsigned int) * 8);
	unsigned int ThreeQuarters = (unsigned int) ((BitsInUnsignedInt * 3) / 4);
	unsigned int OneEighth = (unsigned int) (BitsInUnsignedInt / 8);
	unsigned int HighBits = (unsigned int) (0xFFFFFFFF) << (BitsInUnsignedInt - OneEighth);
	unsigned int hash = 0;
	unsigned int test = 0;
	unsigned int i = 0;

	for (i = 0; i < len; str++, i++) {
		hash = (hash << OneEighth) + (unsigned char)(*str);

		if ((test = hash & HighBits) != 0) {
			hash = ((hash ^ (test >> ThreeQuarters)) & (~HighBits));
		}
	}
	return hash;
}

/*
 * ELF Hash Function
 */
unsigned int ELFHash (char *str, unsigned int len)
{
	unsigned int hash = 0;
	unsigned int x = 0;
	unsigned int i = 0;

	for (i = 0; i < len; str++, i++) {
		hash = (hash << 4) + (unsigned char)(*str);
		if ((x = hash & 0xF0000000L) != 0) {
			hash ^= (x >> 24);
			hash &= ~x;
		}
	}
	return hash;
}

/*
 * BKDR Hash Function
 */
unsigned int BKDRHash (char *str, unsigned int len)
{
	unsigned int seed = 131; /* 31 131 1313 13131 131313 etc.. */
	unsigned int hash = 0;
	unsigned int i = 0;

	for (i = 0; i < len; str++, i++) {
		hash = (hash * seed) + (unsigned char)(*str);
	}
	return hash;
}

/*
 * SDBM Hash Function
 */
unsigned int SDBMHash (char *str, unsigned int len)
{
	unsigned int hash = 0;
	unsigned int i = 0;

	for (i = 0; i < len; str++, i++) {
		hash = (unsigned char)(*str) + (hash << 6) + (hash << 16) - hash;
	}
	return hash;
}

/*
 * DJB Hash Function
 */
unsigned int DJBHash (char *str, unsigned int len)
{
	unsigned int hash = 5381;
	unsigned int i = 0;

	for (i = 0; i < len; str++, i++) {
		hash = ((hash << 5) + hash) + (unsigned char)(*str);
	}
	return hash;
}

/*
 * DEK Hash Function
 */
unsigned int DEKHash (char *str, unsigned int len)
{
	unsigned int hash = len;
	unsigned int i = 0;

	for (i = 0; i < len; str++, i++) {
		hash = ((hash << 5) ^ (hash >> 27)) ^ (unsigned char)(*str);
	}
	return hash;
}

/*
 * AP Hash Function
 */
unsigned int APHash (char *str, unsigned int len)
{
	unsigned int hash = 0;
	unsigned int i = 0;

	for (i = 0; i < len; str++, i++) {
		hash ^= ((i & 1) == 0) ?
		    ((hash << 7) ^ (unsigned char)(*str) ^ (hash >> 3)) :
		    (~((hash << 11) ^ (unsigned char)(*str) ^ (hash >> 5)));
	}
	return hash;
}
