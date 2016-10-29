void putchar ();
void print ();
void printd ();

void printb (int a)
{
	if (a & 0x80) putchar ('1'); else putchar ('0');
	if (a & 0x40) putchar ('1'); else putchar ('0');
	if (a & 0x20) putchar ('1'); else putchar ('0');
	if (a & 0x10) putchar ('1'); else putchar ('0');
	if (a & 0x08) putchar ('1'); else putchar ('0');
	if (a & 0x04) putchar ('1'); else putchar ('0');
	if (a & 0x02) putchar ('1'); else putchar ('0');
	if (a & 0x01) putchar ('1'); else putchar ('0');
}

void left (int a, int b)
{
	printb (a);
	print (" << ");
	printd (b);
	print (" -> ");
	printb (a << b);
	print ("\r\n");
}

void right (int a, int b)
{
	printb (a);
	print (" >> ");
	printd (b);
	print (" -> ");
	printb (a >> b);
	print ("\r\n");
}

void leftp (int a, int b)
{
	int *p;

	printb (a);
	print (" <<= ");
	printd (b);
	print (" -> ");
	p = &a;
	*p <<= b;
	printb (*p);
	print ("\r\n");
}

void rightp (int a, int b)
{
	int *p;

	printb (a);
	print (" >>= ");
	printd (b);
	print (" -> ");
	p = &a;
	*p >>= b;
	printb (*p);
	print ("\r\n");
}

void main ()
{
	left (2, 1);
	left (23, 4);
	left (5, 10);
	left (237, 7);

	right (2, 1);
	right (23, 4);
	right (5, 10);
	right (237, 7);

	leftp (2, 1);
	leftp (23, 4);
	leftp (5, 10);
	leftp (237, 7);

	rightp (2, 1);
	rightp (23, 4);
	rightp (5, 10);
	rightp (237, 7);
}
