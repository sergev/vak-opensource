# define DOSADDR 0x3fff2L

# define DOPEN          1
# define DCLOSE         2
# define DREAD          3
# define DWRITE         4
# define DSEEK          5
# define DOPENDIR       6
# define DREADDIR       7
# define DSTAT          8

struct dosio {
	int op;
	int fd;
	unsigned length;
	unsigned long addr;
};

struct dosreply {
	int err;
	int type;
	unsigned length;
	unsigned long addr;
};
