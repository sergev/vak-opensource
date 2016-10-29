
unsigned char *tpb = (unsigned char *)0177566;
unsigned char *tps = (unsigned char *)0177564;

void tpchr(ch)
char ch;
{
	while (*tps > 0);
	*tpb = ch;
}

char msg[] = "\r\nHello world!\r\n";

main()
{
	char *msg;
	char ch;

	while (ch = *msg++) {
		tpchr(ch);
	}
}
