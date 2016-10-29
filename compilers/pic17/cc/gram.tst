asm ("clrwdt");
asm ("
	xta     fsr
	a+c     2
	atx     fsr
");

char c @ 0x10, far *cp, far *cf ();
short *s, *sf ();
long l [10], lf ();
void *vp;

long f (short a) @ 0x20
{
	char b;

	asm ("clrwdt");
again:
	goto again;
	return;
	return 1;

	if (c)
		return;
	if (! c)
		goto again;
	else
		return a;
	while (b)
		continue;
	do break;
	while (b);
	;;;;
	for (a=1; a<5; ++a) {
		++b;
	}
	switch (b) {
	case 1:
		break;
	default:
		break;
	}
}

char *g (char a, char b)
{
	a = a < 10;
	a = a > 10;
	a = a <= 10;
	a = a >= 10;
	a = a, a, a;
	a = a / 10;
	a = a % 10;
	a = a + 10;
	a = a - 10;
	a = a << 10;
	a = a >> 10;
	a = a * 10;
	a = (a == 10);
	a = (a != 10);
	a = a & 10;
	a = a | 10;
	a = a ^ 10;
	a = (a && 10);
	a = (a || 10);
	a *= 10;
	a /= 10;
	a %= 10;
	a += 10;
	a <<= 10;
	a >>= 10;
	a &= 10;
	a |= 10;
	a ^= 10;
	a = 10;
	a = a ? 5 : 10;

	a++;
	a--;
	++a;
	--a;
	a = *a;
	a = &a;
	a = -a;
	a = !a;
	a = ~a;
	a = sizeof (a);
	a = (char) a;
	a = (char*) a;
	a = (char far*) a;
	a = (short) a;
	a = (short*) a;
	a = (long) a;
	a = (long*) a;
	a = (void*) a;
	a = sizeof (char);
	a = sizeof (char*);
	a = sizeof (char far*);
	a = sizeof (short);
	a = sizeof (short*);
	a = sizeof (long);
	a = sizeof (long*);
	a = sizeof (void*);
	a = a [10];
	a = f (a);
	a.5 = 1;
	a = "string";
}
