/*
 * Сумма по полиному x^8 + x^2 + x + 1.
 * (len . buf) % 0x107
 */
static unsigned char crc8 (unsigned const char *buf, unsigned char len)
{
	unsigned char i, n;
	unsigned short sum;

	sum = len << 8;
	for (i=0; i<len; ++i) {
		sum |= *buf++;
		for (n=0; n<8; ++n) {
			if (sum & 0x8000)
				sum -= 0x8380;
			sum <<= 1;
		}
	}
	return ~(unsigned char) (sum >> 8);
}

void test (unsigned const char *buf, unsigned char len)
{
	unsigned char i;

	for (i=0; i<len-1; ++i)
		printf ("%02x ", buf[i]);
	printf ("(%02x) - %02x\n", buf[len-1], crc8 (buf, len));
}


int main ()
{
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

	test ("\x10\x20\x00", 3);
	test ("\x10\x20\x0f", 3);
	return 0;
}
