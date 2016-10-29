# define CAPNUM 1
# define CAPFLG 2
# define CAPSTR 3

struct CapTab {
	char tname [2];
	char ttype;
	char tdef;
	char *tc;
	int *ti;
	char **ts;
};
