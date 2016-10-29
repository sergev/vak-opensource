
# define LOOP(k) for (k=1; k<=6; k++)

main ()
{
	int s[5];
	int x[500];
	register i;

	LOOP (s[0])
		LOOP (s[1])
			LOOP (s[2])
				LOOP (s[3])
					LOOP (s[4])
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

	for (k=0; k<5; k++) {
		switch (top[k]) {
		case 1:
			n1++;
			break;
		case 5:
			n5++;
			break;
		}
	}

	if (top[0]==top[1] && top[1]==top[2] && top[2]==top[3] && top[3]==top[4]) {
		s += 40 * weight [top [0]];
	} else if (top[0]==top[1] && top[1]==top[2] && top[2]==top[3]) {
		s += 30 * weight [top [0]];
	} else if (top[0]==top[1] && top[1]==top[2] && top[2]==top[4]) {
		s += 30 * weight [top [0]];
	} else if (top[0]==top[1] && top[1]==top[3] && top[3]==top[4]) {
		s += 30 * weight [top [0]];
	} else if (top[0]==top[2] && top[2]==top[3] && top[3]==top[4]) {
		s += 30 * weight [top [0]];
	} else if (top[1]==top[2] && top[2]==top[3] && top[3]==top[4]) {
		s += 30 * weight [top [1]];
	} else if (top[0]==top[1] && top[1]==top[2]) {
		s += 20 * weight [top [0]];
	} else if (top[0]==top[1] && top[1]==top[3]) {
		s += 20 * weight [top [0]];
	} else if (top[0]==top[1] && top[1]==top[4]) {
		s += 20 * weight [top [0]];
	} else if (top[0]==top[2] && top[2]==top[3]) {
		s += 20 * weight [top [0]];
	} else if (top[0]==top[2] && top[2]==top[4]) {
		s += 20 * weight [top [0]];
	} else if (top[0]==top[3] && top[3]==top[4]) {
		s += 20 * weight [top [0]];
	} else if (top[1]==top[2] && top[2]==top[3]) {
		s += 20 * weight [top [1]];
	} else if (top[1]==top[2] && top[2]==top[4]) {
		s += 20 * weight [top [1]];
	} else if (top[1]==top[3] && top[3]==top[4]) {
		s += 20 * weight [top [1]];
	} else if (top[2]==top[3] && top[3]==top[4]) {
		s += 20 * weight [top [2]];
	}

	s += 10*n1 + 5*n5;

	return (s);
}
