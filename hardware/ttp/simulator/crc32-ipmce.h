/*
 * Вычисление 32-битной контрольной суммы для TTP.
 * Полином x32+x31+x30+x29+x28+x26+x23+x21+x19+x18+x15+x14+x13+x12+x11+x9+x8+x4+x+1.
 */
unsigned long crc32_ipmce_byte (unsigned long sum, unsigned char byte);
unsigned long crc32_ipmce (unsigned long sum, unsigned const char *buf,
	unsigned int len);
