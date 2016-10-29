#ifndef DEBUG_CRC8
#include "sys/lib.h"
#endif

#ifndef __AVR__
#define __flash__ /* void */
#endif

/*
 * Select one of available polynoms:
 * x^8 + x^2 + x + 1   - used in ATM
 * x^8 + x^5 + x^4 + 1 - used in Dallas Semiconductor iButton devices
 */
#define ATM_MODE
/*#define DALLAS_MODE*/

#ifdef ATM_MODE
#define POLY_CONST	0x07	/* x^8 + x^2 + x + 1 */
unsigned char poly_tab [256] __flash__ = {
	  0,  7, 14,  9, 28, 27, 18, 21, 56, 63, 54, 49, 36, 35, 42, 45,
	112,119,126,121,108,107, 98,101, 72, 79, 70, 65, 84, 83, 90, 93,
	224,231,238,233,252,251,242,245,216,223,214,209,196,195,202,205,
	144,151,158,153,140,139,130,133,168,175,166,161,180,179,186,189,
	199,192,201,206,219,220,213,210,255,248,241,246,227,228,237,234,
	183,176,185,190,171,172,165,162,143,136,129,134,147,148,157,154,
	 39, 32, 41, 46, 59, 60, 53, 50, 31, 24, 17, 22,  3,  4, 13, 10,
	 87, 80, 89, 94, 75, 76, 69, 66,111,104, 97,102,115,116,125,122,
	137,142,135,128,149,146,155,156,177,182,191,184,173,170,163,164,
	249,254,247,240,229,226,235,236,193,198,207,200,221,218,211,212,
	105,110,103, 96,117,114,123,124, 81, 86, 95, 88, 77, 74, 67, 68,
	 25, 30, 23, 16,  5,  2, 11, 12, 33, 38, 47, 40, 61, 58, 51, 52,
	 78, 73, 64, 71, 82, 85, 92, 91,118,113,120,127,106,109,100, 99,
	 62, 57, 48, 55, 34, 37, 44, 43,  6,  1,  8, 15, 26, 29, 20, 19,
	174,169,160,167,178,181,188,187,150,145,152,159,138,141,132,131,
	222,217,208,215,194,197,204,203,230,225,232,239,250,253,244,243,
};
#endif

#ifdef DALLAS_MODE
#define POLY_CONST	0x31	/* x^8 + x^5 + x^4 + 1 */
unsigned char poly_tab [256] __flash__ = {
	  0, 49, 98, 83,196,245,166,151,185,136,219,234,125, 76, 31, 46,
	 67,114, 33, 16,135,182,229,212,250,203,152,169, 62, 15, 92,109,
	134,183,228,213, 66,115, 32, 17, 63, 14, 93,108,251,202,153,168,
	197,244,167,150,  1, 48, 99, 82,124, 77, 30, 47,184,137,218,235,
	 61, 12, 95,110,249,200,155,170,132,181,230,215, 64,113, 34, 19,
	126, 79, 28, 45,186,139,216,233,199,246,165,148,  3, 50, 97, 80,
	187,138,217,232,127, 78, 29, 44,  2, 51, 96, 81,198,247,164,149,
	248,201,154,171, 60, 13, 94,111, 65,112, 35, 18,133,180,231,214,
	122, 75, 24, 41,190,143,220,237,195,242,161,144,  7, 54,101, 84,
	 57,  8, 91,106,253,204,159,174,128,177,226,211, 68,117, 38, 23,
	252,205,158,175, 56,  9, 90,107, 69,116, 39, 22,129,176,227,210,
	191,142,221,236,123, 74, 25, 40,  6, 55,100, 85,194,243,160,145,
	 71,118, 37, 20,131,178,225,208,254,207,156,173, 58, 11, 88,105,
	  4, 53,102, 87,192,241,162,147,189,140,223,238,121, 72, 27, 42,
	193,240,163,146,  5, 52,103, 86,120, 73, 26, 43,188,141,222,239,
	130,179,224,209, 70,119, 36, 21, 59, 10, 89,104,255,206,157,172,
};
#endif

/*
 * Directly Compute 8-bit checksum using polynome.
 * Count length as the first byte of sequence.
 */
unsigned char crc8 (unsigned const char *buf, unsigned char len)
{
#if __AVR__
	unsigned char *ptr;
	unsigned char sum, dummy;

	asm volatile (
	"SUM = %1 \n"
	"LEN = %2 \n"
	"	mov SUM, LEN \n"
	"1:	ld __tmp_reg__, %a0+ \n"
	"	lsl __tmp_reg__ \n rol SUM \n brcc .+2 \n eor SUM, %5 \n"
	"	lsl __tmp_reg__ \n rol SUM \n brcc .+2 \n eor SUM, %5 \n"
	"	lsl __tmp_reg__ \n rol SUM \n brcc .+2 \n eor SUM, %5 \n"
	"	lsl __tmp_reg__ \n rol SUM \n brcc .+2 \n eor SUM, %5 \n"
	"	lsl __tmp_reg__ \n rol SUM \n brcc .+2 \n eor SUM, %5 \n"
	"	lsl __tmp_reg__ \n rol SUM \n brcc .+2 \n eor SUM, %5 \n"
	"	lsl __tmp_reg__ \n rol SUM \n brcc .+2 \n eor SUM, %5 \n"
	"	lsl __tmp_reg__ \n rol SUM \n brcc .+2 \n eor SUM, %5 \n"
	"	subi LEN, 1 \n"
	"	brne 1b \n"
	: "=&e" (ptr), "=r" (sum), "=&r" (dummy)
	: "0" (buf), "2" (len), "r" ((char)POLY_CONST));

	return ~sum;
#else
	unsigned char i, n;
	unsigned short sum;

	sum = len << 8;
	for (i=0; i<len; ++i) {
		sum |= *buf++;
		for (n=0; n<8; ++n) {
			if (sum & 0x8000)
				sum = (sum << 1) ^ (POLY_CONST << 8);
			else
				sum <<= 1;
		}
	}
	return ~(unsigned char) (sum >> 8);
#endif
}

/*
 * Table-based computing of 8-bit checksum.
 * Count length as the first byte of sequence.
 */
unsigned char crc8_tab (unsigned const char *buf, unsigned char len)
{
	unsigned char sum;
#if __AVR__
	unsigned char *ptr1, *ptr2;
	unsigned char dummy;

	asm volatile (
	"SUM = %2 \n"
	"LEN = %3 \n"
	"	mov r30, LEN \n"
	"	mov SUM, LEN \n"
	"	mov SUM, LEN \n"
	"1:	elpm \n"
	"	ld r30 Y+ \n"
	"	eor r30, r0 \n"
	"	subi LEN, 1 \n"
	"	brne 1b \n"
	: "=&e" (ptr1), "=&e" (ptr2), "=r" (sum), "=&r" (dummy)
	: "0" (buf), "1" (poly_tab), "3" (len));
#else
	unsigned char i;

	sum = len;
	for (i=0; i<len; ++i)
		sum = *buf++ ^ poly_tab [sum];
#endif
	return ~sum;
}

#ifdef DEBUG_CRC8
#include <stdio.h>

void test (const char *buf, unsigned char len)
{
	unsigned char i;

	for (i=0; i<len-1; ++i)
		printf ("%02x ", buf[i]);
	printf ("(%02x) - %02x - %02x\n", buf[len-1],
		crc8 ((unsigned const char*) buf, len),
		crc8_tab ((unsigned const char*) buf, len));
}

int main ()
{
	char array [] = "\x10\x20\x00";

	test ("\x00", 1);
	test ("\x07", 1);
	test ("\x01", 2);
	test ("\x02", 2);
	test ("\x04", 2);
	test ("\x08", 2);
	test ("\x10", 2);
	test ("\x20", 2);
	test ("\x40", 2);
	test ("\x80", 2);

	test ("\x00\x01", 3);
	test ("\x00\x02", 3);
	test ("\x00\x04", 3);
	test ("\x00\x08", 3);
	test ("\x00\x10", 3);
	test ("\x00\x20", 3);
	test ("\x00\x40", 3);
	test ("\x00\x80", 3);

	test ("\x01\x00", 3);
	test ("\x02\x00", 3);
	test ("\x04\x00", 3);
	test ("\x08\x00", 3);
	test ("\x10\x00", 3);
	test ("\x20\x00", 3);
	test ("\x40\x00", 3);
	test ("\x80\x00", 3);

	test (array, 3);
	array[2] = crc8 ((unsigned const char*) array, 3);
	test (array, 3);

	return 0;
}
#endif /* DEBUG_CRC8 */
