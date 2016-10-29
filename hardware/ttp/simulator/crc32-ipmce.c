/*
 * Вычисление 32-битной контрольной суммы для TTP.
 * Полином x32+x31+x30+x29+x28+x26+x23+x21+x19+x18+x15+x14+x13+x12+x11+x9+x8+x4+x+1.
 * Из статьи Philipp Koopman "32-Bit Cyclic Redundancy Codes for Internet Applications".
 */
#include "crc32-ipmce.h"

#if 1
#include <stdio.h>
static unsigned long crc32_ipmce_table [256] = {
	0x00000000, 0x30850ff5, 0x610a1fea, 0x518f101f,
	0xc2143fd4, 0xf2913021, 0xa31e203e, 0x939b2fcb,
	0x159615f7, 0x25131a02, 0x749c0a1d, 0x441905e8,
	0xd7822a23, 0xe70725d6, 0xb68835c9, 0x860d3a3c,
	0x2b2c2bee, 0x1ba9241b, 0x4a263404, 0x7aa33bf1,
	0xe938143a, 0xd9bd1bcf, 0x88320bd0, 0xb8b70425,
	0x3eba3e19, 0x0e3f31ec, 0x5fb021f3, 0x6f352e06,
	0xfcae01cd, 0xcc2b0e38, 0x9da41e27, 0xad2111d2,
	0x565857dc, 0x66dd5829, 0x37524836, 0x07d747c3,
	0x944c6808, 0xa4c967fd, 0xf54677e2, 0xc5c37817,
	0x43ce422b, 0x734b4dde, 0x22c45dc1, 0x12415234,
	0x81da7dff, 0xb15f720a, 0xe0d06215, 0xd0556de0,
	0x7d747c32, 0x4df173c7, 0x1c7e63d8, 0x2cfb6c2d,
	0xbf6043e6, 0x8fe54c13, 0xde6a5c0c, 0xeeef53f9,
	0x68e269c5, 0x58676630, 0x09e8762f, 0x396d79da,
	0xaaf65611, 0x9a7359e4, 0xcbfc49fb, 0xfb79460e,
	0xacb0afb8, 0x9c35a04d, 0xcdbab052, 0xfd3fbfa7,
	0x6ea4906c, 0x5e219f99, 0x0fae8f86, 0x3f2b8073,
	0xb926ba4f, 0x89a3b5ba, 0xd82ca5a5, 0xe8a9aa50,
	0x7b32859b, 0x4bb78a6e, 0x1a389a71, 0x2abd9584,
	0x879c8456, 0xb7198ba3, 0xe6969bbc, 0xd6139449,
	0x4588bb82, 0x750db477, 0x2482a468, 0x1407ab9d,
	0x920a91a1, 0xa28f9e54, 0xf3008e4b, 0xc38581be,
	0x501eae75, 0x609ba180, 0x3114b19f, 0x0191be6a,
	0xfae8f864, 0xca6df791, 0x9be2e78e, 0xab67e87b,
	0x38fcc7b0, 0x0879c845, 0x59f6d85a, 0x6973d7af,
	0xef7eed93, 0xdffbe266, 0x8e74f279, 0xbef1fd8c,
	0x2d6ad247, 0x1defddb2, 0x4c60cdad, 0x7ce5c258,
	0xd1c4d38a, 0xe141dc7f, 0xb0cecc60, 0x804bc395,
	0x13d0ec5e, 0x2355e3ab, 0x72daf3b4, 0x425ffc41,
	0xc452c67d, 0xf4d7c988, 0xa558d997, 0x95ddd662,
	0x0646f9a9, 0x36c3f65c, 0x674ce643, 0x57c9e9b6,
	0xc8df352f, 0xf85a3ada, 0xa9d52ac5, 0x99502530,
	0x0acb0afb, 0x3a4e050e, 0x6bc11511, 0x5b441ae4,
	0xdd4920d8, 0xedcc2f2d, 0xbc433f32, 0x8cc630c7,
	0x1f5d1f0c, 0x2fd810f9, 0x7e5700e6, 0x4ed20f13,
	0xe3f31ec1, 0xd3761134, 0x82f9012b, 0xb27c0ede,
	0x21e72115, 0x11622ee0, 0x40ed3eff, 0x7068310a,
	0xf6650b36, 0xc6e004c3, 0x976f14dc, 0xa7ea1b29,
	0x347134e2, 0x04f43b17, 0x557b2b08, 0x65fe24fd,
	0x9e8762f3, 0xae026d06, 0xff8d7d19, 0xcf0872ec,
	0x5c935d27, 0x6c1652d2, 0x3d9942cd, 0x0d1c4d38,
	0x8b117704, 0xbb9478f1, 0xea1b68ee, 0xda9e671b,
	0x490548d0, 0x79804725, 0x280f573a, 0x188a58cf,
	0xb5ab491d, 0x852e46e8, 0xd4a156f7, 0xe4245902,
	0x77bf76c9, 0x473a793c, 0x16b56923, 0x263066d6,
	0xa03d5cea, 0x90b8531f, 0xc1374300, 0xf1b24cf5,
	0x6229633e, 0x52ac6ccb, 0x03237cd4, 0x33a67321,
	0x646f9a97, 0x54ea9562, 0x0565857d, 0x35e08a88,
	0xa67ba543, 0x96feaab6, 0xc771baa9, 0xf7f4b55c,
	0x71f98f60, 0x417c8095, 0x10f3908a, 0x20769f7f,
	0xb3edb0b4, 0x8368bf41, 0xd2e7af5e, 0xe262a0ab,
	0x4f43b179, 0x7fc6be8c, 0x2e49ae93, 0x1ecca166,
	0x8d578ead, 0xbdd28158, 0xec5d9147, 0xdcd89eb2,
	0x5ad5a48e, 0x6a50ab7b, 0x3bdfbb64, 0x0b5ab491,
	0x98c19b5a, 0xa84494af, 0xf9cb84b0, 0xc94e8b45,
	0x3237cd4b, 0x02b2c2be, 0x533dd2a1, 0x63b8dd54,
	0xf023f29f, 0xc0a6fd6a, 0x9129ed75, 0xa1ace280,
	0x27a1d8bc, 0x1724d749, 0x46abc756, 0x762ec8a3,
	0xe5b5e768, 0xd530e89d, 0x84bff882, 0xb43af777,
	0x191be6a5, 0x299ee950, 0x7811f94f, 0x4894f6ba,
	0xdb0fd971, 0xeb8ad684, 0xba05c69b, 0x8a80c96e,
	0x0c8df352, 0x3c08fca7, 0x6d87ecb8, 0x5d02e34d,
	0xce99cc86, 0xfe1cc373, 0xaf93d36c, 0x9f16dc99,
};
#else
#define X(n)	(1UL << (31 - n))

static const unsigned long poly = /* X(32) - подразумевается */
	X(31) + X(30) + X(29) + X(28) +	        X(26) +			/* F4 */
	X(23) +         X(21) +         X(19) + X(18) +			/* AC */
	X(15) + X(14) + X(13) + X(12) + X(11) +          X(9) + X(8) +	/* FB */
				X(4)  +                  X(1) + X(0);	/* 13 */

static unsigned long crc32_ipmce_table [256];

/*
 * Вычисляем таблицу остатков от деления полиномов вида 0xNN0000
 * на порождающий полином, для значений NN от 0 до 0xFF.
 * Передаём младшим битом вперёд.
 */
static void crc32_ipmce_build_table ()
{
	int i, n;
	unsigned long val;

	for (i=0; i<256; i++) {
		val = i;
		for (n=0; n<8; ++n)
			if (val & 1)
				val = (val >> 1) ^ poly;
			else
				val >>= 1;

		crc32_ipmce_table [i] = val;
	}
}

int main ()
{
	int i;

	printf ("x^32");
	for (i=31; i>1; --i)
		if (poly << i & 0x80000000)
			printf ("+x^%d", i);
	if (poly & 0x40000000)
		printf ("+x");
	printf ("+1\n");

	crc32_ipmce_build_table ();
	printf ("unsigned long crc32_ipmce_table [256] = {\n\t");
	for (i=0; i<256; ++i) {
		printf ("0x%08x, ", crc32_ipmce_table [i]);
		if ((i & 3) == 3)
			printf ("\n\t");
	}
	printf ("};\n");

	return 0;
}
#endif

/*
 * Вычисляем контрольную сумму для очередного байта данных.
 */
unsigned long crc32_ipmce_byte (unsigned long sum, unsigned char byte)
{
	byte ^= sum;
	sum = crc32_ipmce_table [byte] ^ (sum >> 8);
	return sum;
}

/*
 * Вычисляем контрольную сумму для массива данных.
 */
unsigned long crc32_ipmce (unsigned long sum, unsigned const char *buf,
	unsigned int len)
{
	if (len) {
		do {
			sum = crc32_ipmce_byte (sum, *buf++);
		} while (--len);
	}
	return sum;
}

#if 0
/*
 * Тестируем вычисление контрольной суммы.
 */
#include <stdio.h>

void test (unsigned const char *data, unsigned int len)
{
	unsigned char buf [512];
	int i;
	unsigned long sum;

	/* Подсчитываем контрольную сумму данных. */
	for (i=0; i<len; ++i) {
		buf [i] = data [i];
		printf ("%02x ", buf [i]);
	}
	sum = crc32_ipmce (0, buf, len);

	/* Дописываем контрольную сумму в конец пакета. */
	buf [len] = sum;
	buf [len+1] = sum >> 8;
	buf [len+2] = sum >> 16;
	buf [len+3] = sum >> 24;
	printf ("(%02x %02x %02x %02x)",
		buf [len], buf [len+1], buf [len+2], buf [len+3]);

	/* Проверяем, что в сумме получается 0. */
	sum = crc32_ipmce (0, buf, len+4);
	printf (" - %02x %02x %02x %02x\n", (unsigned char) sum,
		(unsigned char) (sum >> 8), (unsigned char) (sum >> 16),
		(unsigned char) (sum >> 24));
	if (sum != 0)
		printf (" - ERROR!\n");
}

int main ()
{
	test ("\x00", 1);
	test ("\x01", 1);
	test ("\x01\x02", 2);
	test ("\x01\x02\x03", 3);
	test ("\x01\x02\x03\x04", 4);
	return 0;
}
#endif
