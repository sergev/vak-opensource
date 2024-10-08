#ifndef FLOAT
# define FLOAT float
#endif

struct cvntab           /* used for getcodpar() paramater list */
{
	char    *abrev;
	char    *full;
	int     value;
	int     value2;
	void    (*fun)(int);
};

extern FLOAT            getfltpar();
extern struct cvntab    *getcodpar();
