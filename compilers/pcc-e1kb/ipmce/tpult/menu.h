# define CHOICE(m,sm)    ((m)<<8|(sm))

struct submenu {
	char *name;                     /* name of submenu */
	int key;                        /* return key */
	void (*exec) ();                /* function to execute */
	int active;                     /* is it active for now */
	int tag;                        /* tag of submenu */
	int row;                        /* row of submenu */
};

struct menu {
	char *name;                     /* name of menu */
	int key;                        /* return key */
	struct submenu *submenu;        /* array of submenus */
	int col;                        /* base column of submenu window */
	int len;                        /* length of head name */
	int hgh;                        /* height of submenu window */
	int wid;                        /* width of submenu window */
	int nsm;                        /* current submenu */
	BOX *box;                       /* box under menu */
	int box_on;                     /* box on screen */
};

extern struct menu menu [];             /* main menu array */

extern nmenu;                           /* current menu */
extern runmenu(), getmenu();
extern int  small_menu ();
