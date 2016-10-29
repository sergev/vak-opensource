struct word {
	long r, l;
};

struct ioregs {
	long r [16];		/* н0-нF index registers */
	long s [8];		/* йу0-йу7 special registers */
	struct word a;		/* ун accumulator */
	struct word y;		/* тнт young register */
	long pc;		/* уюбу address of next command */
	long cr;		/* тл command register */
	long crp;		/* тлр */
};

struct iobwr {			/* втъ */
	struct word w [8];	/* втъ */
	char wt [8];            /* ФЕЗ */
	long a [8];		/* вбъ */
};

struct iobcr {			/* вту */
	struct word w [8];	/* вту */
	char wt [8];            /* ФЕЗ */
	long a [8];		/* вбу */
};

struct iolcm {			/* нрл */
	struct word w [32];
	char wt [32];		/* ФЕЗ */
};

struct iommu {
	unsigned pa [64];	/* тбн */
	long ma [64];           /* тбж */
	char t [64];		/* лт */
	struct word en;		/* тур */
	struct word us;         /* тжп */
	struct word ro;         /* тъъ */
	int pm;			/* тну */
};

struct ioiru {
	unsigned long iir;	/* трт */
	unsigned eir;		/* зтчр */
	unsigned eim;		/* зтн */
	unsigned bci;		/* рпв */
	char scx;		/* йуеу */
	char scr;		/* лпеу */
	char scz;		/* х0еу */
	long ucr [4];		/* ЙЪ хл */
	long ucs [4];		/* Ч хл */
	long tm;		/* фн */
	struct word clock;	/* юу */
};

struct bchan {
	short rd;		/* дбор */
	char ris;		/* ууй */
	char rbc;		/* лвруй */
	long rs;		/* УПУФПСОЙЕ р */
	short td;		/* дбой */
	char tis;		/* уур */
	char tbc;		/* лврур */
	long ts;		/* УПУФПСОЙЕ й */
	struct word lcm [4];	/* нрл */
	char lcmt [4];		/* ФЕЗЙ нрл */
};