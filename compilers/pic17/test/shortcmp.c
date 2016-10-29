void putchar ();
void print ();
void printd ();
void prints ();

void cmp (short a, short b)
{
	prints (a);
	putchar (' ');
	prints (b);
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
	cmp (1578, 2023);
	cmp (4267, 3034);
	cmp (392, 507);
	cmp (321, (short) 21);
	cmp ((short) 32, 432);
	cmp ((short) 54, (short) 54);
}
