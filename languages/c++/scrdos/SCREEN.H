//
// General declarations for screen management package.
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
// $Id: screen.h,v 1.6 2008/04/18 12:57:55 ly Exp $
//
// #define CYRILLIC                // enable internal cyrillics (CY, Cs, Ce, Ct)

extern "C" {
#include <signal.h>
#include <stdarg.h>
#include <string.h>
};

#define cntrl(c) ((c) & 0x1f)
#define meta(c)  ((c) | 0x100)
#define alt(c)   ((c) | 0x200)

class ScreenImp;
class Box;
class Cursor;

class Box {
    friend class ScreenImp;

private:
    short y, x;
    short ny, nx;
    short *mem;

public:
    Box(ScreenImp &scr, int by, int bx, int bny, int bnx);
    ~Box()
    {
        if (mem)
            delete mem;
    }
    short *Memory() { return mem; }
};

class Cursor {
    friend class ScreenImp;

private:
    short y, x;

public:
    Cursor(ScreenImp &scr);
};

//
// Defines for MSDOS. ---------------------------------------------------------
//
enum {
#if 0
    ULC = '\17', UT, URC, CLT, CX, CRT, LLC, LT, LRC, VERT, HOR, BLK,
#else
    ULC  = 0xda,
    UT   = 0xc2,
    URC  = 0xbf,
    CLT  = 0xc3,
    CX   = 0xc5,
    CRT  = 0xb4,
    LLC  = 0xc0,
    LT   = 0xc1,
    LRC  = 0xd9,
    VERT = 0xb3,
    HOR  = 0xc4,
    BLK  = 0xc5, /* TODO */
#endif
};

class ScreenImp {
    friend class Cursor;
    friend class Box;

public:
    int Lines;
    int Columns;

    //      static int ULC, UT, URC, CLT, CX, CRT, LLC, LT, LRC, VERT, HOR, BLK;

    void Open(int colormode, int graphmode);
    void Close();

    void Restore() {}
    void ReopenTerm() {}

    int Col() { return curx; }
    int Row() { return cury; }

    int HasDim() { return 1; }
    int HasBold() { return 1; }
    int HasInverse() { return 1; }
    int HasColors() { return ColorMode; }
    int HasGraph() { return 1; }

    void Clear(int attr = 0);
    void Clear(int y, int x, int ny, int nx, int attr = 0, int sym = ' ');
    void ClearLine(int y, int x, int attr = 0);
    void ClearLine(int attr) { ClearLine(cury, curx, attr); }

    void ClearOk(int ok) {}

    // It is possible to set current column
    // to the left from the left margin
    // or to the right of the right margin.
    // Printed text will be clipped properly.
    void Move(int y, int x)
    {
        curx   = x;
        cury   = y;
        hidden = 0;
    }

    void Put(int c, int attr = 0);
    void Put(Box &box, int ca = 0);
    void Put(const char *str, int attr = 0)
    {
        while (*str)
            Put(*str++, attr);
    }

    void Put(int y, int x, int ch, int attr = 0)
    {
        Move(y, x);
        Put(ch, attr);
    }
    void Put(int y, int x, const char *str, int attr = 0)
    {
        Move(y, x);
        Put(str, attr);
    }
    void Put(int y, int x, Box &box, int ca = 0)
    {
        Move(y, x);
        Put(box, ca);
    }
    void PutLimited(const char *str, int lim, int attr = 0)
    {
        while (--lim >= 0 && *str)
            Put(*str++, attr);
    }
    void PutLimited(int y, int x, const char *str, int lim, int attr = 0)
    {
        Move(y, x);
        while (--lim >= 0 && *str)
            Put(*str++, attr);
    }

    void Print(int attr, const char *fmt, ...);
    void Print(int y, int x, int attr, const char *fmt, ...);
    void PrintVect(int attr, const char *fmt, va_list vect);
    void PrintVect(int y, int x, int attr, const char *fmt, va_list vect)
    {
        Move(y, x);
        PrintVect(attr, fmt, vect);
    }

    int GetChar() { return GetChar(cury, curx); }
    int GetChar(int y, int x) { return scr[y][x]; }

    int GetKey(void);
    void UngetKey(int key);
    void AddHotKey(int key, void (*f)(int));
    void DelHotKey(int key);
    int Kbhit(void);

    void SetCursor(Cursor c) { Move(c.y, c.x); }
    void HideCursor() { hidden = 1; }

    void Sync(int y) { Sync(); }
    void Sync();
    void Redraw() {}
    void Beep();

    void DelLine(int line, int attr = 0);
    void InsLine(int line, int attr = 0);

    void Error(int c, int i, const char *head, const char *reply, const char *s,
               ...);
    int Popup(const char *head, const char *mesg, const char *mesg2,
              const char *c1, const char *c2, const char *c3, int c, int i);
    char *GetString(int w, char *str, const char *head, char *mesg, int c,
                    int i);
    void PopupString(const char *title, const char *str, const char *reply,
                     int color, int inverse);
    int SelectFromList(int color, int inverse, const char *head,
                       const char *mesg, int cnt, ...);
    int SelectFromTable(int color, int inverse, const char *head,
                        const char *mesg, int cnt, char *tab[]);

    void HorLine(int y, int x, int nx, int attr = 0);
    void VertLine(int x, int y, int ny, int attr = 0);
    void DrawFrame(int y, int x, int ny, int nx, int attr = 0);

    void AttrSet(int y, int x, int ny, int nx, int attr = 0);
    void AttrLow(int y, int x);
    void Dump(const char *filename);

private:
    struct callBack {
        int key;
        void (*func)(int);
    };

    short far **scr;
    int curx, cury;
    int flgs;
    int ColorMode;
    unsigned NormalAttr;
    int hidden;
    int ungetkey, ungetflag;
    struct callBack callBackTable[20];
    int ncallb;

    void initChoice(int row, int col, const char *c1, const char *c2,
                    const char *c3);
    int menuChoice(int c, int i);
    char *editString(int r, int c, int w, char *str, int cp, int color);

    void pokeChar(int y, int x, int v) { scr[y][x] = v; }
};

// End Defines for MSDOS. -----------------------------------------------------

class Screen : public ScreenImp {
public:
    Screen(int colormode = 2, int graphmode = 1)
    {
        memset(this, 0, sizeof(Screen));
        Open(colormode, graphmode);
    }
    ~Screen()
    {
        Close();
        memset(this, 0, sizeof(Screen));
    }
    void Reopen()
    {
        Close();
        Open(-1, -1);
    }
};

inline Cursor::Cursor(ScreenImp &scr)
{
    y = scr.cury;
    x = scr.curx;
}

const char *GetNumerateSuffix(int Value, const char *Suffixes);
const char *GetSubstring(const char *strings, int index);
