/*
 * Descriptor of file segment.  Contains from 1 to 127 lines of file,
 * written sequentially.  An elementary component of descriptor chain.
 */
#define FSDMAXL 127 /* Max. number of lines in descriptor */

typedef struct segment {
    struct segment *prev; /* Previous descriptor in chain */
    struct segment *next; /* Next descriptor in chain */
    int nlines;           /* Count of lines in descriptor,
                           * or 0 when end of chain */
    int fdesc;            /* File descriptor, or 0 for end of chain */
    off_t seek;           /* Byte offset in the file */
    char data;            /* Varying amount of data, needed
                           * to store the next nlines-1 lines. */
    /*
     * Interpretation of next byte:
     * 1-127   offset of this line from a previous line
     * 0       empty line
     * -n      line starts at offset n*128+next bytes
     *         from beginning of previous line.
     * There are at least nlines-1 bytes allocated or more,
     * in case of very long lines.
     */
} segment_t;

#define sizeof_segment_t offsetof(segment_t, data)

typedef struct {
    segment_t *chain; /* Chain of segments */
    char *name;       /* File name */
    char writable;    /* Do we have a write permission */
#define EDITED 2      /* Value when file was modified */

    char backup_done; /* Already copied to .bak */
    int nlines;       /* Number of non-empty lines in file */
} file_t;

extern file_t file[MAXFILES]; /* Table of files */
extern int curfile;

/*
 * Workspace describes a generic file under edit.
 */
typedef struct {
    segment_t *cursegm; /* Current segment */
    int topline;        /* Top left row on a display */
    int offset;         /* Column offset of a displayed text */
    int line;           /* Current line number */
    int segmline;       /* Number of first line in the current segment */
    int wfile;          /* File number, or 0 when not attached */
    int cursorcol;      /* Saved cursorcol, when not active */
    int cursorrow;      /* Saved cursorline, when not active */
} workspace_t;

extern workspace_t *curwksp, *pickwksp;

/*
 * Write a segment chain into the file.
 * When maxlines>0 - put only maxlines lines of text.
 * When maxlines<0 - put only -maxlines paragraphs.
 * Return a count of written lines, or -1 on error.
 */
int segmwrite(segment_t *ff, int maxlines, int newf)
{
    segment_t *f;
    char *c;
    int i;
    int j, k, bflag, tlines;

    if (cline_max < LBUFFER)
        cline_expand(LBUFFER);
    f = ff;
    bflag = 1;
    tlines = 0;
    while (f->fdesc && maxlines) {
        if (f->fdesc > 0) {
            i = 0;
            c = &f->data;
            for (j = f->nlines; j; j--) {
                if (maxlines < 0) {
                    /* Check the count of empty lines. */
                    if (bflag && *c != 1)
                        bflag = 0;
                    else if (bflag == 0 && *c == 1) {
                        bflag = 1;
                        if (++maxlines == 0)
                            break;
                    }
                }
                if (*c & 0200)
                    i += 128 * (*c++ & 0177);
                i += *c++;
                ++tlines;
                /* Check the line count. */
                if (maxlines > 0 && --maxlines == 0)
                    break;
            }
            lseek(f->fdesc, f->seek, 0);
            while (i) {
                j = (i < LBUFFER) ? i : LBUFFER;
                if (read(f->fdesc, cline, j) < 0)
                    /* ignore errors */;
                if (write(newf, cline, j) < 0) {
                    error("DANGER -- WRITE ERROR");
                    close(newf);
                    return (-1);
                }
                i -= j;
            }
        } else {
            j = f->nlines;
            if (maxlines < 0) {
                if (bflag == 0 && ++maxlines == 0)
                    j = 0;
                bflag = 1;
            } else {
                if (j > maxlines)
                    j = maxlines;
                maxlines -= j;
            }
            k = j;
            while (k)
                cline[--k] = '\n';
            if (j && write(newf, cline, j) < 0) {
                error("DANGER -- WRITE ERROR");
                close(newf);
                return (-1);
            }
            tlines += j;
        }
        f = f->next;
    }
    close(newf);
    return tlines;
}

/*
 * Scan a file and create a segment chain.
 * Works from a current offset in the file.
 */
static segment_t *fdesc2segm(int chan)
{
    segment_t *thissegm = 0, *lastsegm = 0;
    segment_t *firstsegm = 0;
    int nby = 0;
    char *bpt;
    int c;
    char fby[NBYMAX + 1];
    int i, lct, nl = 0, sl, kl;

    c = -2;
    sl = 0;
    for (;;) {
        /* Here c is a next symbol: -1 designates EOF, -2 - enter a loop
         */
        if ((c < 0) || nby >= NBYMAX || nl == FSDMAXL) {
            if (c != -2) {
                lastsegm = thissegm;
                thissegm = (segment_t *)salloc(nby + sizeof_segment_t);
                if (firstsegm == 0)
                    firstsegm = thissegm;
                else
                    lastsegm->next = thissegm;
                thissegm->prev = lastsegm;
                thissegm->next = 0;
                thissegm->nlines = nl;
                file[chan].nlines += nl;
                thissegm->fdesc = chan;
                thissegm->seek = sl;
                bpt = &(thissegm->data);
                for (i = 0; i < nby; ++i)
                    *bpt++ = fby[i];
            }
            if (c == -1) {
                /* Append a tail block and done. */
                thissegm->next = lastsegm = (segment_t *)salloc(sizeof_segment_t);
                lastsegm->prev = thissegm;
                return (firstsegm);
            }
            sl = cline_read_offset;
            nl = nby = lct = 0;
        }
        kl = cline_read_offset;
        c = cline_read(0);
        lct = cline_read_offset - kl;
        if (c != -1 || lct) {
            if (lct > 127) {
                fby[nby++] = (lct / 128) | 0200;
                lct = lct % 128;
            }
            fby[nby++] = lct;
            ++nl;
        }
    }
}

/*
 * Create a segment with n empty lines.
 */
static segment_t *blanklines(int n)
{
    int i;
    segment_t *f, *g;
    char *c;

    f = (segment_t *)salloc(sizeof_segment_t);
    while (n) {
        i = n > FSDMAXL ? FSDMAXL : n;
        g = (segment_t *)salloc(sizeof_segment_t + i);
        g->next = f;
        f->prev = g;
        g->nlines = i;
        g->fdesc = -1;
        c = &g->data;
        n -= i;
        while (i--)
            *c++ = 1;
        f = g;
    }
    return (f);
}

/*
 * Split a segment by line n in workspace w.
 * On return, w.line == segmline and cursegm points to the first line
 * after the break (which can be a line from the tail block).
 * Initial segm can keep some unused space.
 * When applied to the end of file, the current position will be
 * at the final segment.
 * When the needed line is beyond the end of file, additional segments
 * are appended (with fdesc == -1).
 * When realloc_flag==1, the block is reallocated to free some space.
 * WARNING: breaksegm can disrupt the validity of pointers in workspace.
 * Recommended to call wksp_redraw().
 */
static int breaksegm(workspace_t *w, int n, int realloc_flag)
{
    int nby, i, j, jj, k, lfb0;
    segment_t *f, *ff;
    segment_t *fn;
    char *c;
    char *cc;
    off_t offs;

    DEBUGCHECK;
    if (wksp_position(w, n)) {
        f = w->cursegm;
        ff = f->prev;
        free((char *)f);
        fn = blanklines(n - w->line);
        w->cursegm = fn;
        fn->prev = ff;
        if (ff)
            ff->next = fn;
        else
            file[w->wfile].chain = fn;
        wksp_position(w, n);
        return (1);
    }
    f = w->cursegm;
    cc = c = &f->data;
    offs = 0;
    ff = f;
    nby = n - w->segmline;
    if (nby != 0) {
        /* get down to the nth line */
        for (i = 0; i < nby; i++) {
            j = *c++;
            if (j & 0200) {
                offs += 128 * (j & 0177);
                j = *c++;
            }
            offs += j;
        }
        /* now make a new segm from the remainder of f */
        i = j = jj = f->nlines - nby; /* number of lines in new segm */
        lfb0 = c - cc;
        cc = c;
        while (--i >= 0) {
            if (*cc++ < 0) {
                j++;
                cc++;
            }
        }
        ff = (segment_t *)salloc(sizeof_segment_t + j);
        ff->nlines = jj;
        ff->fdesc = f->fdesc;
        offs += f->seek;
        ff->seek = offs;
        cc = &ff->data;
        for (k = 0; k < jj; k++)
            if ((*cc++ = *c++) < 0)
                *cc++ = *c++;
        if ((ff->next = f->next))
            ff->next->prev = ff;
        ff->prev = f;
        f->next = ff;
        f->nlines = nby;
        if (realloc_flag && jj > 4 && f->prev) {
            ff = (segment_t *)salloc(sizeof_segment_t + lfb0);
            *ff = *f;
            c = &(ff->data);
            cc = &(f->data);
            while (lfb0--) {
                *c++ = *cc++;
            }
            ff->prev->next = ff->next->prev = ff;
            free((char *)f);
            f = ff;
            ff = f->next;
        }
    }
    w->cursegm = ff;
    w->segmline = n;
    DEBUGCHECK;
    return (0);
}

/*
 * Try to merge several segments into one to save some space.
 * Join w->cursegm->prev and w->cursegm, in case they are adjacent.
 */
static int catsegm(workspace_t *w)
{
    segment_t *f0, *f;
    segment_t *f2;
    char *c;
    char *cc;
    int i, j, l0 = 0, l1 = 0, lb0 = 0, lb1, dl, nl0, nl1, fd0, kod = 0;
    /* l0,  l1:  byte count in a file area, described by f0, f;
     * lb0, lb1: length of data in segm;
     * nl0, nl1: number of lines in segm */

    f = w->cursegm;
    if ((f0 = f->prev) == 0) {
        file[w->wfile].chain = f;
        return (0);
    }
    f0->next = f;
    fd0 = f0->fdesc;
    nl0 = f0->nlines;
    while (fd0 > 0 && fd0 == f->fdesc && (nl0 + (nl1 = f->nlines) < FSDMAXL)) {
        dl = f->seek - f0->seek;
        /* Compute the block length, if unknown */
        if (l0 == 0) {
            i = nl0;
            cc = c = &f0->data;
            while (i--) {
                if ((j = *c++) & 0200)
                    j = (j & 0177) * 128 + *c++;
                l0 += j;
            }
            lb0 = c - cc;
        }
        if (dl != l0)
            return (kod);
        /* Merge two segments and try to repeat */
        i = nl1;
        cc = c = &(f0->data);
        while (i--) {
            if ((j = *c++) & 0200)
                j = (j & 0177) * 128 + *c++;
            l1 += j;
        }
        lb1 = c - cc;
        f2 = f;
        f = (segment_t *)salloc(sizeof_segment_t + lb0 + lb1);
        *f = *f0;
        f->next = f2->next;
        w->cursegm = f;
        w->segmline -= nl0;
        nl0 = f->nlines = nl0 + nl1;
        c = &(f->data);
        i = lb0;
        cc = &(f0->data);
        while (i--)
            *c++ = *cc++;
        i = lb1;
        cc = &(f2->data);
        while (i--)
            *c++ = *cc++;
        lb0 += lb1;
        l0 += l1;
        kod = 1;
        free((char *)f2);
        free((char *)f0);
        f->next->prev = f;
        f0 = f->prev;
        if (f0)
            f0->next = f;
        else
            file[w->wfile].chain = f;
        f0 = f;
        f = f0->next;
    }
    return (kod);
}

/*
 * Insert a segment f into file, described by wksp, before the line at.
 * The calling routine should call wksp_redraw() with needed args.
 */
static void insert(workspace_t *wksp, segment_t *f, int at)
{
    segment_t *w0, *wf, *ff;

    DEBUGCHECK;
    ff = f;
    while (ff->next->fdesc) {
        ff = ff->next;
    }
    breaksegm(wksp, at, 1);
    wf = wksp->cursegm;
    w0 = wf->prev;
    free((char *)ff->next);
    ff->next = wf;
    wf->prev = ff;
    f->prev = w0;
    wksp->cursegm = f;
    wksp->line = wksp->segmline = at;
    if (file[wksp->wfile].writable)
        file[wksp->wfile].writable = EDITED;
    catsegm(wksp);
    DEBUGCHECK;
}

/*
 * Append a line buf of length n to the temporary file.
 * Return a segment for this line.
 */
static segment_t *writemp(char *buf, int nbytes)
{
    segment_t *f1, *f2;
    char *p;

    if (file_desc == tempfile)
        file_desc = 0;
    lseek(tempfile, tempseek, 0);

    nbytes = int_to_ext(buf, nbytes - 1);
    if (write(tempfile, buf, nbytes) != nbytes)
        return 0;

    /* now make segm */
    f1 = (segment_t *)salloc(2 + sizeof_segment_t);
    f2 = (segment_t *)salloc(sizeof_segment_t);
    f2->prev = f1;
    f1->next = f2;
    f1->nlines = 1;
    f1->fdesc = tempfile;
    f1->seek = tempseek;
    if (nbytes <= 127)
        f1->data = nbytes;
    else {
        p = &f1->data;
        *p++ = (nbytes / 128) | 0200;
        *p = nbytes % 128;
    }
    tempseek += nbytes;
    return (f1);
}

/*
 * Delete specified lines from the workspace.
 * Returns a segment chain of the deleted lines, with tail segment appended.
 * Needs wksp_redraw().
 */
static segment_t *delete(workspace_t *wksp, int from, int to)
{
    segment_t *w0;
    segment_t *wf, *f0, *ff;
    breaksegm(wksp, to + 1, 1);
    DEBUGCHECK;
    wf = wksp->cursegm;
    breaksegm(wksp, from, 1);
    f0 = wksp->cursegm;
    ff = wf->prev;
    w0 = f0->prev;
    wksp->cursegm = wf;
    wf->prev = w0;
    f0->prev = 0;
    ff->next = (segment_t *)salloc(sizeof_segment_t);
    ff->next->prev = ff;
    catsegm(wksp);
    file[wksp->wfile].writable = EDITED;
    DEBUGCHECK;
    return (f0);
}

/*
 * Returns a copy of segment subchain, from f to end, not including end.
 * When end = NULL - up to the end of file.
 */
static segment_t *copysegm(segment_t *f, segment_t *end)
{
    segment_t *res, *ff, *rend = 0;
    int i;
    char *c1, *c2;

    res = 0;
    while (f->fdesc && f != end) {
        c1 = &f->data;
        for (i = f->nlines; i; i--)
            if (*c1++ & 0200)
                c1++;
        c2 = (char *)f; /* !!! Count the size !!! */
        i = c1 - c2;
        c2 = salloc(i);
        ff = (segment_t *)c2;
        c2 += i;
        while (i--)
            *--c2 = *--c1;
        if (res) {
            rend->next = ff;
            ff->prev = rend;
            rend = ff;
        } else
            res = rend = ff;
        f = f->next;
    }
    if (res) {
        rend->next = (segment_t *)salloc(sizeof_segment_t);
        rend->next->prev = rend;
        rend = rend->next;
    } else
        res = rend = (segment_t *)salloc(sizeof_segment_t);

    if (f->fdesc == 0)
        rend->seek = f->seek;
    return res;
}

/*
 * Returns a segment chain for the given lines, tail segment appended.
 * Needs wksp_redraw.
 */
static segment_t *pick(workspace_t *wksp, int from, int to)
{
    segment_t *wf;

    breaksegm(wksp, to + 1, 1);
    wf = wksp->cursegm;
    breaksegm(wksp, from, 1);
    return copysegm(wksp->cursegm, wf);
}

/*
 * Free the segment chain.
 */
void freesegm(segment_t *f)
{
    segment_t *g;

    while (f) {
        g = f;
        f = f->next;
        free((char *)g);
    }
}

/*
 * Debug output of segm chains.
 */
void printsegm(segment_t *f)
{
    int i;
    char *c;

    printf("\n**********");
    while (f) {
        printf("\nsegmnl=%d chan=%d seek=%lu at %p", f->nlines, f->fdesc, (long)f->seek, f);
        if (f->next && f != f->next->prev)
            printf("\n*** next block bad backpointer ***");
        c = &(f->data);
        for (i = 0; i < f->nlines; i++) {
            if ((i % 20) == 0)
                putchar('\n');
            printf(" %d", *c++);
        }
        f = f->next;
    }
}
