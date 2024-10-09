#ifndef FLOAT
#define FLOAT double
#endif

struct cvntab /* used for getcodpar() paramater list */
{
    char *abrev;
    char *full;
    int value;
    int value2;
    void (*fun)(int);
};

int getintpar(char *s);
FLOAT getfltpar(char *);
int getynpar(char *s);
struct cvntab *getcodpar(char *s, struct cvntab tab[]);
void getstrpar(char *s, char *r, int l, char *t);
int testnl();
void skiptonl(char c);
int testterm();
int readdelim(char d);
