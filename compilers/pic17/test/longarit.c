void putchar ();
void print ();
void printd ();
void prints ();

void printl (long v)
{
	if (v > 999999999) putchar ((char) (v / 1000000000) + '0');
	if (v > 99999999)  putchar ((char) ((v / 100000000) % 10) + '0');
	if (v > 9999999)   putchar ((char) ((v / 10000000) % 10) + '0');
	if (v > 999999)    putchar ((char) ((v / 1000000) % 10) + '0');
	if (v > 99999)     putchar ((char) ((v / 100000) % 10) + '0');
	if (v > 9999)      putchar ((char) ((v / 10000) % 10) + '0');
	if (v > 999)       putchar ((char) ((v / 1000) % 10) + '0');
	if (v > 99)        putchar ((char) ((v / 100) % 10) + '0');
	if (v > 9)         putchar ((char) ((v / 10) % 10) + '0');
	putchar ((char) (v % 10) + '0');
}

void main ()
{
	long l;
	short s;
	char c;

	printl ((long) 9);      putchar ('\n');
	printl ((long) 89);     putchar ('\n');
	printl ((long) 789);    putchar ('\n');
	printl ((long) 6789);   putchar ('\n');
	printl ((long) 56789);  putchar ('\n');
	printl (456789);        putchar ('\n');
	printl (3456789);       putchar ('\n');
	printl (23456789);      putchar ('\n');
	printl (123456789);     putchar ('\n');
	printl (1234567890);    putchar ('\n');

	l = 12345678;
	printl (l++);           print (" - 12345678 expected\n");
	printl (++l);           print (" - 12345680 expected\n");
	printl (l--);           print (" - 12345680 expected\n");
	printl (--l);           print (" - 12345678 expected\n");

	c = 12;
	printl (l+c);           print (" - 12345678 expected\n");
	printl (l-c);           print (" - 12345678 expected\n");
	l += c;
	printl (l);             print (" - 12345678 expected\n");
	l -= c;
	printl (l);             print (" - 12345678 expected\n");

	s = 1234;
	printl (l+s);           print (" - 12345678 expected\n");
	printl (l-s);           print (" - 12345678 expected\n");
	l += s;
	printl (l);             print (" - 12345678 expected\n");
	l -= s;
	printl (l);             print (" - 12345678 expected\n");

	/*
	 * To do: + - += -= /= %= & | ^ << >>
	 */
}
