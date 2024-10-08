/*
**  CALL THE SHELL
*/

void shell()
{
	int             i;
	register int    pid;
	register int    sav2, sav3;
#ifdef M_XENIX
	if (!(pid = fork()))
	{
		setuid(getuid());
		nice(0);
		execl("/bin/csh", "-", 0);
		tsyserr("не могу запустить /bin/csh");
	}
	sav2 = signal(2, 1);
	sav3 = signal(3, 1);
	while (wait(&i) != pid) ;
	signal(2, sav2);
	signal(3, sav3);
#else
system("");
#endif
}
