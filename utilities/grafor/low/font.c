# define F(x)   ((x)&0100 ? 0100-(x) : (x))

struct chr {
	char *code;
	short length;
	short minx, miny;
	short maxx, maxy;
};

struct font {
	short minx, miny;
	short maxx, maxy;
	struct chr chr ['~'-' '+1];
};

static struct font font;
static char fontname [] = "shr00.dat";

extern char *malloc ();

main (argc, argv)
char **argv;
{
	register n;

	if (argc > 1)
		n = atoi (argv [1]);
	else
		n = 0;
	ldfont (n, &font);
	for (n=' '; n<='~'; ++n) {
		GOpen (1, 70, 70);
		draw (& font.chr [n-' ']);
		GClose ();
	}
	return (0);
}

ldfont (fontnum, fnt)
struct font *fnt;
{
	int fontfile;
	register char *p;
	register struct chr *t;
	register i;
	char buf [4];

	/* make font file name: shrXX.dat */
	p = fontname + strlen (fontname) - 6;
	*p++ += fontnum / 10;
	*p += fontnum % 10;

	if ((fontfile = open (fontname, 0)) < 0) {
		printf ("Cannot open font file %s\n", fontname);
		exit (1);
	}
	for (i=' '; i<='~'; ++i) {
		t = & fnt->chr [i-' '];
		if (t->length)
			free (t->code);
		read (fontfile, buf, 2);
		t->length = buf [0] & 0377 | buf [1] << 8;
		t->code = malloc (t->length);
		if (! t->code) {
			printf ("Out of memory\n");
			exit (1);
		}
		read (fontfile, t->code, t->length);
	}
	close (fontfile);
	scanfont (fnt);
}

draw (t)
struct chr *t;
{
	register char *p;
	int penup = 0;
	int x, y;

	GColor (2);
	GMove (10, 0);
	GPlot (10, 65);
	GMove (0, 20);
	GPlot (65, 20);
	GColor (3);
	GMove (t->minx+10, t->miny+20);
	GPlot (t->minx+10, t->maxy+20);
	GPlot (t->maxx+10, t->maxy+20);
	GPlot (t->maxx+10, t->miny+20);
	GPlot (t->minx+10, t->miny+20);
	GColor (1);
	GMove (x = 10, y = 20);
	for (p=t->code; p<t->code+t->length; p+=2) {
		if (p [0] == 0100)
			switch (p [1]) {
			case 0:
				penup = 1;
				continue;
			case 1:
				penup = 0;
				continue;
			case 2:
			default:
				return;
			}
		x = F (p [0]);
		y = F (p [1]);
		if (penup)
			GMove (x+10, y+20);
		else
			GPlot (x+10, y+20);
	}
}

scanfont (fnt)
struct font *fnt;
{
	register struct chr *t;
	register char *p;
	register short x, y;

	fnt->minx = 1000;
	fnt->miny = 1000;
	fnt->maxx = -1000;
	fnt->maxy = -1000;
	for (t=fnt->chr; t<fnt->chr+'~'-' '+1; ++t) {
		t->minx = 1000;
		t->miny = 1000;
		t->maxx = -1000;
		t->maxy = -1000;
		for (p=t->code; p<t->code+t->length; p+=2) {
			if (p [0] == 0100)
				switch (p [1]) {
				case 0:
				case 1:
					continue;
				case 2:
				default:
					goto breakloop;
				}
			x = F (p [0]);
			y = F (p [1]);
			if (x < t->minx)
				t->minx = x;
			if (y < t->miny)
				t->miny = y;
			if (x > t->maxx)
				t->maxx = x;
			if (y > t->maxy)
				t->maxy = y;
		}
breakloop:
		if (t->minx < fnt->minx)
			fnt->minx = t->minx;
		if (t->miny < fnt->miny)
			fnt->miny = t->miny;
		if (t->maxx > fnt->maxx)
			fnt->maxx = t->maxx;
		if (t->maxy > fnt->maxy)
			fnt->maxy = t->maxy;
	}
}
