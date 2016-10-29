extern long time ();

main ()
{
	printf ("%ld\n", time ((long *) 0));
	return (0);
}
