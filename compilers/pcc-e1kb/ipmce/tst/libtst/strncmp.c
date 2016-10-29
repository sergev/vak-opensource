/*
 * Сравнение тестовых строк по значениям байт:
 *      s1>s2: >0  s1==s2: 0  s1<s2: <0
 * Сравнивается n первых символов в строке.
 */

strncmp(s1, s2, n)
register char *s1, *s2;
register n;
{

	while (--n >= 0 && *s1 == *s2++)
		if (*s1++ == '\0')
			return(0);
	return(n < 0 ? 0 : (unsigned) (*s1) - (unsigned) (*--s2));
}
