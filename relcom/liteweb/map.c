#include <stdio.h>
#include <string.h>

#include "map.h"

#define MAXPT           16              /* max points per polygon */
#define MAXAREA         64              /* max areas per map */
#define ISSPACE(c)	((c) == ' ' || (c) == '\t')

enum area_type {
	POINT,                          /* point,     npt==1 */
	RECT,                           /* rectangle, npt==2 */
	CIRCLE,                         /* circle,    npt==2 */
	POLY,                           /* polygon,   npt>=3 */
};

struct point {
	unsigned short x, y;            /* point coords */
};

struct area {
	enum area_type  type;           /* area type */
	char           *ref;            /* resulting url */
	int             npt;            /* number of points */
	struct point    pt[MAXPT];
} map[MAXAREA];

int narea;                              /* number of areas in the map */
char *map_default;                      /* default url */

/*
 * Read the map description from the file.
 * Return 0 in case of error.
 */
int map_read (char *filename)
{
	FILE *fd;
	char line[256], *p, *cmd, *ref, *coords;
	struct point *pt;
	int ok = 0;

	fd = fopen (filename, "r");
	if (! fd)
		return (0);

	while (narea<MAXAREA && fgets (line, sizeof(line), fd)) {
		/*
		 * Remove trailing spaces and CR-NL
		 */
		p = line + strlen(line) - 1;
		while (p >= line && strchr (" \t\r\n", *p))
			*p-- = 0;
		p = line;

		/*
		 * Skip comments and empty lines
		 */
		if (! *p || *p == '#')
			continue;
		while (ISSPACE (*p))
			++p;

		/*
		 * Get command name
		 */
		cmd = p;
		while (*p && ! ISSPACE (*p))
			++p;
		while (ISSPACE (*p))
			*p++ = 0;
		if (! *p)
			goto ret;

		/*
		 * Get reference
		 */
		ref = p;
		while (*p && ! ISSPACE (*p))
			++p;
		while (ISSPACE (*p))
			*p++ = 0;

		/*
		 * Parse command name
		 */
		if (strcmp (cmd, "default") == 0) {
			map_default = strdup (ref);
			continue;
		}
		if      (strcmp (cmd, "point")  == 0) map[narea].type = POINT;
		else if (strcmp (cmd, "rect")   == 0) map[narea].type = RECT;
		else if (strcmp (cmd, "circle") == 0) map[narea].type = CIRCLE;
		else if (strcmp (cmd, "poly")   == 0) map[narea].type = POLY;
		else
			goto ret;

		/*
		 * Get points
		 */
		pt = map[narea].pt;
		while (*p && pt < map[narea].pt + MAXPT) {
			coords = p;
			while (*p && ! ISSPACE (*p))
				++p;
			while (ISSPACE (*p))
				*p++ = 0;
			if (sscanf (coords, "%hd,%hd", &pt->x, &pt->y) != 2)
				goto ret;
			++map[narea].npt;
			++pt;
		}

		/*
		 * Check the number of points
		 */
		switch (map[narea].type) {
		case POINT:  if (map[narea].npt != 1) goto ret; break;
		case RECT:   if (map[narea].npt != 2) goto ret; break;
		case CIRCLE: if (map[narea].npt != 2) goto ret; break;
		case POLY:   if (map[narea].npt < 3)  goto ret; break;
		}
		map[narea].ref = strdup (ref);
		++narea;
	}
	ok = 1;
ret:
	fclose (fd);
	return (ok);
}

/*
 * Compute the distance between two points.
 */
static unsigned long distance (struct point *a, struct point *b)
{
	long dx = (long) a->x - (long) b->x;
	long dy = (long) a->y - (long) b->y;

	return (dx * dx + dy * dy);
}

static int side (struct point *t, struct point *a, struct point *b)
{
	long v = (long) (t->x - a->x) * (b->y - a->y) -
		 (long) (t->y - a->y) * (b->x - a->x);
	return (v<0 ? -1 : v>0 ? 1 : 0);
}

/*
 * Return 1 if the point is inside the (convex) polygon.
 * The algoritm: for every side of the polygon, test if all other N-2 vertexes
 * and the point in question lie on the same side.
 */
static int inpoly (struct point *a, struct point *pt, int npt)
{
	int i, j, k, s, sk;

	for (i=0; i<npt; ++i) {
		j = i<npt-1 ? i+1 : 0;

		s = side (a, &pt[i], &pt[j]);
		for (k=0; k<npt; ++k) {
			if (k == i || k == j)
				continue;
			sk = side (&pt[k], &pt[i], &pt[j]);
			if (s && s != sk)
				return (0);
			s = sk;
		}
	}
	return (1);
}

/*
 * Given the point in side the clickable map,
 * return the resulting URL reference.
 * Return 0 if no default URL is defined.
 */
char *map_find (char *coords)
{
	struct point pt;
	struct area *a;
	unsigned long d, mind;
	char *ref;

	if (sscanf (coords, "%hd,%hd", &pt.x, &pt.y) != 2)
		return (0);

	mind = ~0L;
	ref = map_default;
	for (a=map; a<map+narea; ++a)
		switch (a->type) {
		case RECT:
			if (pt.x >= a->pt[0].x && pt.x <= a->pt[1].x &&
			    pt.y >= a->pt[0].y && pt.y <= a->pt[1].y)
				return (a->ref);
			break;
		case CIRCLE:
			if (distance (&pt, &a->pt[0]) <=
			    distance (&a->pt[0], &a->pt[1]))
				return (a->ref);
			break;
		case POLY:
			if (inpoly (&pt, a->pt, a->npt))
				return (a->ref);
			break;
		case POINT:
			d = distance (&pt, &a->pt[0]);
			if (d < mind) {
				mind = d;
				ref = a->ref;
			}
			break;
		}
	return (ref);
}
