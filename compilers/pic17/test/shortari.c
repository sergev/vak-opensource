void putchar ();
void print ();
void printd ();
void prints ();

void main ()
{
	short s;
	char c;
#if 0
	prints ((short) 9);     putchar ('\n');
	prints ((short) 89);    putchar ('\n');
	prints (789);           putchar ('\n');
	prints (6789);          putchar ('\n');
	prints (56789);         putchar ('\n');

	s = 1234;
	prints (s++);           print (" - 1234 expected\n");
	prints (++s);           print (" - 1236 expected\n");
	prints (s--);           print (" - 1236 expected\n");
	prints (--s);           print (" - 1234 expected\n");

	c = 12;
	prints (s+c);           print (" - 1234 expected\n");
	prints (s-c);           print (" - 1234 expected\n");
	s += c;
	prints (s);             print (" - 1234 expected\n");
	s -= c;
	prints (s);             print (" - 1234 expected\n");
#endif
	/*
	 * To do: + - += -= /= %=
	 */
	c = 255;
	s = (short) c * 64;
	prints ((short) 64);		print ("\r\n");
	prints ((short) c);		print ("\r\n");
	prints (s);			print ("\r\n");
	printd ((int) (s>>8));		print ("\r\n");
	printd ((int) s);		print ("\r\n");
}
