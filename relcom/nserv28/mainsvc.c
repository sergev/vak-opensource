# ifndef IPPROTO_TCP
# include <netinet/in.h>
# endif

# ifndef SIGTERM
# include <signal.h>
# endif

extern SVCXPRT *svctcp_create ();
extern void quit ();

main()
{
	SVCXPRT *transp;

	if ( fork() > 0 ) {
	       sleep(10);
	       exit(0);
	};
	signal (SIGTERM, quit);
	signal (SIGINT, SIG_IGN);

	/* save our process id to file */
	savepid ("rgrouppid",1);

	pmap_unset(RGROUPSPROG, RGROUPSVERS);

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		fprintf(stderr, "cannot create tcp service.\n");
		exit(1);
	}
	if (!svc_register(transp, RGROUPSPROG, RGROUPSVERS, rgroupsprog_1, IPPROTO_TCP)) {
		fprintf(stderr, "unable to register (RGROUPSPROG, RGROUPSVERS, tcp).\n");
		exit(1);
	}
	svc_run();
	fprintf(stderr, "svc_run returned\n");
	exit(1);
}
