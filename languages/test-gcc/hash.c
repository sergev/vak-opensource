/*
 * Rotation hash function by Serge Vakulenko.
 */
unsigned long rot13_hash (unsigned char *str, unsigned int len)
{
	unsigned long hash = 0;

	while (len-- > 0) {
		hash += *str++;
		hash -= (hash << 13) | (hash >> 19);
	}
	return hash;
}
