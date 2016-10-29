void putchar ();
void print ();

long b;

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

void cmp (long a)
{
	printl (a);
	putchar (' ');
	printl (b);
	if (a == b)  print (" ==");
	if (a != b)  print (" !=");
	if (a < b)   print (" < ");
	if (a > b)   print (" > ");
	if (a <= b)  print (" <=");
	if (a >= b)  print (" >=");
	print ("\r\n");
}

void main ()
{
	b = 20235678; cmp (15781234);
	b = 30343254; cmp (42674567);
	b = 57899876; cmp (57899876);
	b = 10; cmp ((long)1);
	b = 1; cmp ((long)10);
	b = 12; cmp ((long)12);
}
