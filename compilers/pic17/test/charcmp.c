void putchar ();
void print ();
void printd ();

void cmp (char a, char b)
{
	printd (a);
	putchar (' ');
	printd (b);
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
	cmp (15, 20);
	cmp (42, 30);
	cmp (57, 57);
}
