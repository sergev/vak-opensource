
# define LOOP(k) for (k=1; k<=6; k++)

main ()
{
	int s[2];
	int x[500];
	register i;

	LOOP (s[0])
		LOOP (s[1])
			x [cubes (s)] ++;

	for (i=0; i<500; i++)
		if (x[i])
			printf ("%d\t%d\n", i, x[i]);
	exit (0);
}

int weight [7] = { 0, 10, 2, 3, 4, 5, 6, };

cubes (top)
int top [];
{
	register k, s=0, n1=0, n5=0;

	for (k=0; k<2; k++) {
		switch (top[k]) {
		case 1:
			n1++;
			break;
		case 5:
			n5++;
			break;
		}
	}

	s += 10*n1 + 5*n5;

	return (s);
}
