//
// Sample program based on screen management package:
// hexadecimal file view utility.
//
// Copyright (C) 1994-2004 Cronyx Engineering.
// Author: Serge Vakulenko, <vak@cronyx.ru>
//
// This software is distributed with NO WARRANTIES, not even the implied
// warranties for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// Authors grant any other persons or organisations permission to use
// or modify this software as long as this message is kept with the software,
// all derivative works or modified versions.
//
// $Id: hv.cpp,v 1.4 2006/04/19 17:26:54 ly Exp $
//

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef unix
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
extern char **environ;
#endif
#if !defined(_WINDOWS) && !defined(_WIN32)
int open(char *, int);
int close(int);
int read(int, void *, unsigned);
//      long lseek (int, long, int);
void exit(int);
//      int sscanf (char *, const char *, ...);
//      int sprintf (char *, const char *, ...);
#endif
};

#include "menu.h"
#include "screen.h"

class HexView {
public:
    HexView(char *file, long off = 0);
    ~HexView();
    void Run();

private:
    enum ViewMode {
        byteMode,
        shortMode,
        longMode,
        charMode,
    };
    char *filename;     // File name
    FILE *fd;           // File read descriptor
    unsigned long flen; // File length (for regular files) or -1
    long bn;            // Current block number
    long bsz;           // Block size
    long minbsz;        // Minimal block size
    long nb;            // File length in blocks
    int rawmode;
    ViewMode mode;
    char filbuf[512];

    void ViewLine(int b, int line);
    void ViewChar(int ch);
};

Screen V;

Menu M(0x1b, 0x1f, 0x37, 0x3b, 0x13);
SubMenu *menuFiles;
SubMenu *menuOptions;

int NormalColor     = 0x70;
int DimColor        = 0x7c;
int InverseColor    = 0x1b;
int DimInverseColor = 0x13;
int PopupColor      = 0x1f;
int PopupInverse    = 0x70;
int ErrorColor      = 0x4f;
int ErrorInverse    = 0x70;

void Quit()
{
    V.Move(V.Lines - 1, 0);
    V.ClearLine(NormalColor);
    V.Sync();
    exit(0);
}

void PromptQuit()
{
    int choice = V.Popup(" Quit ", "Do you really want to quit ?", 0, " Yes ",
                         " No ", 0, PopupColor, PopupInverse);

    if (choice == 0)
        Quit();
}

void ColorSetup()
{
}

void RunShell()
{
    Box box(V, 0, 0, V.Lines, V.Columns);

    V.Clear(NormalColor);
    V.Sync();
    V.Restore();
#ifdef unix
    char *shell = getenv("SHELL");

    if (!shell || *shell != '/')
        shell = "/bin/sh";
    int t = fork();

    // Потомок.
    if (t == 0) {
        for (int i = 3; i < 20; ++i)
            close(i);
        execle(shell, 1 + strchr(shell, '/'), "-i", (char *)0, environ);
        _exit(-1); // file not found
    }
    // Предок.
    if (t > 0) {
#ifdef SIGTSTP
        // Игнорируем suspend, пока ждем завершения шелла.
        void (*oldtstp)(int) = signal(SIGTSTP, SIG_IGN);
#endif

        // Ждем, пока он не отпадет.
        int status;

        while (t != wait(&status))
            ;

#ifdef SIGTSTP
        // Восстановим реакцию на suspend.
        signal(SIGTSTP, oldtstp);
        // Вернем терминал себе, если шелл его отобрал.
        // Иначе получим SIGTTOU.
        tcsetpgrp(2, getpid());
#endif
    }
#else
#endif
    V.Reopen();
    V.Clear(NormalColor);
    V.Put(box);
}

inline int GETHEX(int x)
{
    return (x >= 'A' ? (x & 15) + 9 : x & 15);
}

HexView::HexView(char *file, long off)
{
    struct stat st;

    filename = file;
    minbsz   = 16;
    rawmode  = 0;
    mode     = byteMode;
    flen     = (unsigned long)-1L;
    bsz      = minbsz;
    nb       = flen >> 4;
    fd       = fopen(filename, "rb");
    // if (fd)
    //      setbuffer (fd, filbuf, sizeof (filbuf));
    if (stat(filename, &st) >= 0) {
        switch (st.st_mode & S_IFMT) {
        case S_IFREG:
        case S_IFDIR:
            flen = st.st_size;
            nb   = (flen + bsz - 1) / bsz;
            break;
#ifdef S_IFBLK
        case S_IFBLK:
            minbsz = 512;
#endif
        case S_IFCHR:
            break;
        }
    }
    if (off < 0)
        off += flen;
    bn = (unsigned long)off / bsz;
    if (bn >= nb && nb > 0)
        bn = nb - 1;
}

HexView::~HexView()
{
    if (fd)
        fclose(fd);
}

void HexView::Run()
{
    if (!fd) {
        V.Error(ErrorColor, ErrorInverse, "Error", "Ok", "Cannot read %s",
                filename);
        return;
    }
    int stepud = V.Lines - 3; // step up-down
    int sline  = -1;
    int soff   = -1;

    V.Move(V.Lines - 1, 0);
    V.Put("1", DimColor);
    V.Put("      ", DimInverseColor);
    V.Put(" 2", DimColor);
    V.Put("Goto  ", DimInverseColor);
    V.Put(" 3", DimColor);
    V.Put("Raw   ", DimInverseColor);
    V.Put(" 4", DimColor);
    V.Put("Mode  ", DimInverseColor);
    V.Put(" 5", DimColor);
    V.Put("Block ", DimInverseColor);
    V.Put(" 6", DimColor);
    V.Put("      ", DimInverseColor);
    V.Put(" 7", DimColor);
    V.Put("      ", DimInverseColor);
    V.Put(" 8", DimColor);
    V.Put("      ", DimInverseColor);
    V.Put(" 9", DimColor);
    V.Put("      ", DimInverseColor);
    V.Put("10", DimColor);
    V.Put("Quit ", DimInverseColor);
    for (;;) {
        for (int i = 0; i < V.Lines - 2; ++i)
            ViewLine(bn + i, i + 1);
        for (;;) {
            char *p;

            V.Clear(0, 0, 1, 80, DimInverseColor, ' ');
            V.Print(0, 1, DimInverseColor, "File %s", filename);
            if (flen != (unsigned long)-1L)
                V.Print(DimInverseColor, "    %ld bytes", flen);
            if (sline >= bn && sline < bn + V.Lines - 2)
                V.Move(sline - bn + 1, soff * 3);
            else {
                V.HideCursor();
                sline = -1;
            }
            V.Sync();
            switch (V.GetKey()) {
            case meta('A'): // f1 - help
                            //                              Help ();
                continue;
            case meta('I'): // f9 - menu
                M.Run(&V, 0);
                continue;
            case meta('B'): // f2 - goto
                char gotobuf[16];

                sprintf(gotobuf, "%08lx", bn * bsz);
                p = V.GetString(8, gotobuf, " Goto ", "Go to offset",
                                PopupColor, PopupInverse);
                if (!p)
                    continue;
                unsigned long off;

                sscanf(p, "%lx", &off);
                if (off > flen) {
                    V.Error(ErrorColor, ErrorInverse, "Error", "Ok",
                            "Too big offset %lx", off);
                    continue;
                }
                bn = (off + bsz - 1) / bsz;
                break;
            default:
                V.Beep();
                continue;
            case cntrl('L'): // ^L - redraw screen
            case cntrl('R'): // ^R - redraw screen
                V.Redraw();
                continue;
            case meta('C'): // f3 - raw
                rawmode ^= 1;
                break;
            case cntrl('I'): // Tab - mode
            case meta('D'):  // f4 - mode
                switch (mode) {
                case byteMode:
                    mode = shortMode;
                    break;
                case shortMode:
                    mode = longMode;
                    break;
                case longMode:
                    mode = charMode;
                    break;
                case charMode:
                    mode = byteMode;
                    break;
                }
                break;
            case meta('E'): // f5 - block size
                char sizebuf[16];

                sprintf(sizebuf, "%07lx", bsz);
                p = V.GetString(7, sizebuf, " Block ", "Set block size",
                                PopupColor, PopupInverse);
                if (!p)
                    continue;
                off = bn * bsz;
                sscanf(p, "%lx", &bsz);
                if (flen == (unsigned long)-1L)
                    bsz = (bsz + minbsz - 1) / minbsz * minbsz;
                if (bsz < minbsz)
                    bsz = minbsz;
                else if (bsz >= 0x100000)
                    bsz = 0x100000;
                nb = (flen + bsz - 1) / bsz;
                bn = off / bsz;
                if (bn >= nb && nb > 0)
                    bn = nb - 1;
                break;
            case cntrl('C'):
            case cntrl('['):
            case meta('J'): // f10 - quit
                PromptQuit();
                continue;
            case meta('u'): // up/
                if (bn <= 0)
                    continue;
                --bn;
                V.DelLine(V.Lines - 2, NormalColor);
                V.InsLine(1, NormalColor);
                ViewLine(bn, 1);
                continue;
            case cntrl('M'): // down
            case cntrl('J'): // down
            case meta('d'):  // down
                if (bn >= nb - (V.Lines - 2))
                    continue;
                V.DelLine(1, NormalColor);
                V.InsLine(V.Lines - 2, NormalColor);
                ViewLine(++bn + V.Lines - 3, V.Lines - 2);
                continue;
            case meta('n'): // next page
                if (bn >= nb - (V.Lines - 2))
                    continue;
                bn += stepud;
                if (bn > nb - (V.Lines - 2))
                    bn = nb - (V.Lines - 2);
                break;
            case meta('p'): // prev page
                if (bn <= 0)
                    continue;
                bn -= stepud;
                if (bn < 0)
                    bn = 0;
                break;
            case meta('h'): // home
                if (bn == 0)
                    continue;
                bn = 0;
                break;
            case meta('e'): // end
                if (bn >= nb - (V.Lines - 2))
                    continue;
                bn = nb - (V.Lines - 2);
                break;
            case meta('G'): // f7 - search
                //                              char *p = getstring (SEARCHSZ-1,
                //                              searchstr,
                //                                      " Search ", "Search for
                //                                      the string");
                //                              if (! p)
                //                                      continue;
                //                              strcpy (searchstr, p);
                //                              searchsz = cvtsrch (viewsrch,
                //                              viewsbuf); if (sline < 0) {
                //                                      sline = bn;
                //                                      soff = -1;
                //                              }
                //                              if (hsearch (sline, soff+1,
                //                              &sline, &soff)) {
                //                                      if (bn>sline ||
                //                                      bn+V.Lines-2<=sline)
                //                                              bn = sline;
                //                                      if (bn > nb -
                //                                      (V.Lines-2))
                //                                              bn = nb -
                //                                              (V.Lines-2);
                //                                      if (bn < 0)
                //                                              bn = 0;
                //                                      break;
                //                              }
                //                              sline = -1;
                continue;
            }
            break;
        }
    }
}

void HexView::ViewLine(int b, int i)
{
    V.Move(i, 0);
    if (b >= nb) {
        V.ClearLine(NormalColor);
        return;
    }

    V.Print(NormalColor, " %08lx  ", b * bsz);

    int bufsize;

    if (flen == (unsigned long)-1L)
        bufsize = minbsz;
    else if (bsz > V.Columns)
        bufsize = V.Columns;
    else
        bufsize = bsz;
    char buf[2048];
    int n;

    if (fseek(fd, b * bsz, 0) < 0)
        V.Put("<seek error>", DimInverseColor);
    else if ((n = fread(buf, 1, bufsize, fd)) < 0)
        V.Put("<read error>", DimInverseColor);
    else if (n == 0)
        V.Put("<eof>", DimInverseColor);
    else {
        for (int i = 0; i < n && i < V.Columns; ++i) {
            int c;

            switch (mode) {
            case byteMode:
                if (V.Col() >= V.Columns - 4)
                    break;
                c = buf[i];
                V.Put("0123456789abcdef"[c >> 4 & 0xf], NormalColor);
                V.Put("0123456789abcdef"[c & 0xf], NormalColor);
                V.Put(' ', NormalColor);
                continue;
            case shortMode:
                if (V.Col() >= V.Columns - 6)
                    break;
                V.Print(NormalColor, "%04x ", *(unsigned short *)(buf + i));
                i += sizeof(short) - 1;
                continue;
            case longMode:
                if (V.Col() >= V.Columns - 10)
                    break;
                V.Print(NormalColor, "%08lx ", *(unsigned long *)(buf + i));
                i += sizeof(long) - 1;
                continue;
            case charMode:
                if (V.Col() >= V.Columns - 2)
                    break;
                ViewChar(buf[i]);
                continue;
            }
            V.Put("\3>\2", InverseColor);
            break;
        }
    }
    V.ClearLine(NormalColor);
}

void HexView::ViewChar(int c)
{
    if (c == ' ')
        V.Put('.', DimColor);
    else if (rawmode) {
        c &= 0377;
        if (c >= ' ')
            V.Put(c, NormalColor);
        else
            V.Put(c + 0100, DimColor);
    } else if (c & 0200) {
        c &= 0177;
        if (c >= ' ' && c <= '~')
            V.Put(c, InverseColor);
        else
            V.Put((c + 0100) & 0177, DimInverseColor);
    } else if (c >= ' ' && c <= '~')
        V.Put(c, NormalColor);
    else
        V.Put((c + 0100) & 0177, DimColor);
}

#if 0
static hsearch (l, c, pline, pcol)
     int *pline, *pcol;
{
    char buf[2 * SEARCHSZ], *p;
    register char *s, *e;
    int n;

    for (;;) {
        lseek (fd, l * 16L, 0);
        n = read (fd, buf, sizeof (buf));
        if (n <= 0)
            break;
        p = buf;
        e = buf + SEARCHSZ;
        for (s = p + c; s < e; ++s)
            if (!MemCompare
                (s, viewsbuf, n < searchsz ? n : searchsz)) {
                *pline = l + (s - p) / 16;
                *pcol = (s - p) % 16;
                return (1);
            }
        if (n < sizeof (buf))
            break;
        l += SEARCHSZ / 16;
        c = 0;
    }
    error ("String not found");
    return (0);
}

cvtsrch (from, to)
     register char *from, *to;
{
    register c, x, count;

    count = 0;
    for (count = 0; c = *from++; *to++ = c, ++count)
        if (c == '\\' && (x = *from)) {
            ++from;
            if (x != '\\') {
                c = GETHEX (x);
                if ((x = *from) && x != '\\') {
                    ++from;
                    c = c << 4 | GETHEX (x);
                }
            }
        }
    return (count);
}
#endif

int main(int argc, char **argv)
{
    V.Clear(NormalColor);
    for (--argc, ++argv; *argv && **argv == '-'; --argc, ++argv) {
        for (char *p = 1 + *argv; *p; ++p)
            switch (*p) {
            default:
                goto usage;
            }
    }
    if (argc < 1) {
    usage:
        V.Error(ErrorColor, ErrorInverse, "Usage", "Ok",
                "Usage: hv file [offset]");
        V.Move(V.Lines - 1, 0);
        V.Sync();
        exit(-1);
    }
    char *file = *argv++;

    --argc;

    long off = 0;

    if (argc > 0) {
        if (**argv != '0')
            sscanf(*argv, "%ld", &off);
        else if ((*argv)[1] != 'x')
            sscanf(*argv + 1, "%lo", &off);
        else
            sscanf(*argv + 2, "%lx", &off);
    }

    if (!V.HasColors()) {
        NormalColor     = 0x07;
        DimColor        = 0x07;
        InverseColor    = 0x1f;
        DimInverseColor = 0x10;
        PopupColor      = 0x1f;
        PopupInverse    = 0x10;
        ErrorColor      = 0x1f;
        ErrorInverse    = 0x10;
    }

    menuFiles   = M.Add("~Files");
    menuOptions = M.Add("~Options");
    menuFiles->Add("Escape to ~Shell", RunShell);
    menuFiles->Add("E~xit", Quit);
    menuOptions->Add("~Colors...", ColorSetup);

    HexView hv(file, off);

    hv.Run();
    return (0);
}
