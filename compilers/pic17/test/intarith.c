void putchar ();
void print ();
void printd ();

void div (int a, int b)
{
	printd (a);
	print (" / ");
	printd (b);
	print (" -> ");
	printd (a / b);
	print ("\r\n");
}

void mod (int a, int b)
{
	printd (a);
	print (" % ");
	printd (b);
	print (" -> ");
	printd (a % b);
	print ("\r\n");
}

void divp (int a, int b)
{
	int *p;

	printd (a);
	print (" /= ");
	printd (b);
	print (" -> ");
	p = &a;
	*p /= b;
	printd (*p);
	print ("\r\n");
}

void modp (int a, int b)
{
	int *p;

	printd (a);
	print (" %= ");
	printd (b);
	print (" -> ");
	p = &a;
	*p %= b;
	printd (*p);
	print ("\r\n");
}

void main ()
{
	div (2, 1);
	div (23, 4);
	div (5, 10);
	div (237, 7);

	mod (2, 1);
	mod (23, 4);
	mod (5, 10);
	mod (237, 7);

	divp (2, 1);
	divp (23, 4);
	divp (5, 10);
	divp (237, 7);

	modp (2, 1);
	modp (23, 4);
	modp (5, 10);
	modp (237, 7);
}
