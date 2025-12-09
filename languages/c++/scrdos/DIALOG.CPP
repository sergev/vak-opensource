//
// Dialog windows implementation.
//
// Copyright (C) 1996-2004 Cronyx Engineering.
// Author: Serge Vakulenko, <vak@cronyx.ru>
//
// This software is distributed with NO WARRANTIES, not even the implied
// warranties for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// Authors grant any other persons or organisations permission to use
// or modify this software as long as this message is kept with the software,
// all derivative works or modified versions.
//
// $Id: dialog.cpp,v 1.6 2009/04/13 12:37:38 ly Exp $
//

//
// horizontal glue              |
// horizontal space             -
// vertical space               {}
// next row                     space
// next column                  +
// Group                        (...)
// Group with centering         <...>
// Group with frame             [...]
// Label                        {label}
// Button int                   B {label}
// String char* (maxlen)        S :width :maxlen
// Password char* (maxlen)      P :width :maxlen
// Title char*                  T :width
// RadioButton int              R :val {label} :val {label}...
// CompactRadioButton int       C :val {label} :val {label}...
// Number long (min max step)   N :min :max :step
// Hex number long              X :min :max :step
// Command                      * :val {label}
// List                         L :width :maxw
// Mask                         M :width
//
#include <stdarg.h>

#include "dialog.h"
#include "extern.h"
#include "screen.h"

enum {
    ColorText                   = 0,
    ColorFrame                  = 1,
    ColorTextSelected           = 2,
    ColorField                  = 3,
    ColorFieldSelected          = 4,
    ColorCommand                = 5,
    ColorCommandDefault         = 6,
    ColorCommandSelected        = 7,
    ColorCommandDefaultSelected = 8,
};

static const int GRP_FRAMED = 1;
static const int GRP_CENTER = 2;

#if 0
extern "C" {
    void Message (const char *fmt, ...);
//      getch (void);
};
#endif

int DialogElement::baserow;
int DialogElement::basecol;
DialogElement *DialogElement::list;
unsigned char *DialogElement::palette;
Screen *DialogElement::scr;

DialogElement::DialogElement(int row, int col, int reg)
{
    r = row;
    c = col;
    w = h   = 0;
    oblig   = 0;
    enabled = 1; // true
    if (reg) {
        if (list) {
            next       = list;
            prev       = list->prev;
            list->prev = this;
            prev->next = this;
        } else
            list = next = prev = this;
    }
}

DialogLabel::DialogLabel(int row, int col, const char *&p)
    : DialogElement(row, col)
{
    label = GetString(p);
    w     = label ? strlen(label) : 0;
    h     = 1;
}

DialogButton::DialogButton(int row, int col, const char *&p, int *v)
    : DialogElement(row, col, 1)
{
    label = GetString(p);
    w     = 4 + (label ? strlen(label) : 0);
    h     = 1;
    val   = v;
}

DialogRadioButton::DialogRadioButton(int row, int col, const char *&p, int *v)
    : DialogElement(row, col, 1)
{
    setval = GetNumber(p);
    if (*p == '~') {
        ++p;
        enabled = 0; // false
    }
    label   = GetString(p);
    int len = label ? strlen(label) : 0;

    w   = len ? len + 4 : 3;
    h   = 1;
    val = v;
}

DialogCompactRadioButton::DialogCompactRadioButton(int row, int col,
                                                   const char *&p, int *v)
    : DialogElement(row, col, 1)
{
    setval = GetNumber(p);
    w      = 1;
    h      = 1;
    val    = v;
}

DialogList::DialogList(int row, int col, const char *&p, long *v, DialogListFunc f)
    : DialogElement(row, col, 1)
{
    w    = 2 + GetNumber(p);
    maxw = GetNumber(p);
    if (!maxw)
        maxw = 32;
    h    = 1;
    val  = v;
    func = f;
    (*func)(0, 0, 0, 0, v, str);
}

DialogTitle::DialogTitle(int row, int col, const char *&p, char *v)
    : DialogElement(row, col, 0)
{
    w   = GetNumber(p);
    h   = 1;
    val = v;
}

DialogString::DialogString(int row, int col, const char *&p, char *v)
    : DialogElement(row, col, 1)
{
    w      = GetNumber(p);
    maxlen = GetNumber(p);
    if (!maxlen)
        maxlen = 32;
    h   = 1;
    val = v;
}

DialogPassword::DialogPassword(int row, int col, const char *&p, char *v)
    : DialogElement(row, col, 1)
{
    w      = GetNumber(p);
    maxlen = GetNumber(p);
    if (!maxlen)
        maxlen = 32;
    h   = 1;
    val = v;
}

DialogNumber::DialogNumber(int row, int col, const char *&p, long *v)
    : DialogElement(row, col, 1)
{
    min  = GetNumber(p);
    max  = GetNumber(p);
    step = GetNumber(p);
    if (!max)
        max = 999999999;
    if (!step)
        step = 1;
    w = 1;
    for (long i = max; i; i /= 10)
        ++w;
    h   = 1;
    val = v;
}

DialogHexNumber::DialogHexNumber(int row, int col, const char *&p, long *v)
    : DialogElement(row, col, 1)
{
    min  = GetHexNumber(p);
    max  = GetHexNumber(p);
    step = GetHexNumber(p);
    if (!max)
        max = 999999999;
    if (!step)
        step = 1;
    w = 1;
    for (long i = max; i; i /= 16)
        ++w;
    h   = 1;
    val = v;
}

DialogMask::DialogMask(int row, int col, const char *&p, long *v)
    : DialogElement(row, col, 1)
{
    w   = GetNumber(p);
    h   = 1;
    val = v;
}

DialogCommand::DialogCommand(int row, int col, const char *&p)
    : DialogElement(row, col, 1)
{
    val   = GetNumber(p);
    label = GetString(p);
    w     = label ? strlen(label) : 0;
    h     = 1;
}

int DialogButton::Run()
{
    scr->Put(r + baserow, c + basecol, *val ? "[*] " : "[ ] ",
             palette[ColorTextSelected]);
    if (label)
        scr->Put(label, palette[ColorTextSelected]);
    for (;;) {
        scr->Move(r + baserow, c + basecol + 1);
        scr->Sync();
        switch (scr->GetKey()) {
        default:
            scr->Beep();
            continue;
        case cntrl('['):
            return DialogCANCEL; // Esc
        case cntrl('C'):
            return DialogCANCEL; // ^C
        case meta('J'):
            return DialogCANCEL; // F10
        case cntrl('M'):
            return DialogOK; // Return
        case cntrl('J'):
            return DialogOK; // Newline
        case cntrl('I'):
            return DialogNEXT; // Tab
        case cntrl('H'):
            return DialogPREV; // Backspace
        case meta('d'):
            return DialogDOWN;
        case meta('r'):
            return DialogRIGHT;
        case meta('u'):
            return DialogUP;
        case meta('l'):
            return DialogLEFT;
        case meta('h'):
            return DialogHOME;
        case meta('e'):
            return DialogEND;
        case ' ': // Space -- switch
            *val = !*val;
            scr->Put(*val ? '*' : ' ', palette[ColorTextSelected]);
            continue;
        }
    }
}

int DialogRadioButton::Run()
{
    scr->Move(r + baserow, c + basecol);
    scr->Put(*val == setval ? "(*) " : "( ) ", palette[ColorTextSelected]);
    if (label)
        scr->Put(label, palette[ColorTextSelected]);
    for (;;) {
        scr->Move(r + baserow, c + basecol + 1);
        scr->Sync();
        switch (scr->GetKey()) {
        default:
            scr->Beep();
            continue;
        case cntrl('['):
            return DialogCANCEL; // Esc
        case cntrl('C'):
            return DialogCANCEL; // ^C
        case meta('J'):
            return DialogCANCEL; // F10
        case cntrl('M'):
            return DialogOK; // Return
        case cntrl('J'):
            return DialogOK; // Newline
        case cntrl('I'):
            return DialogNEXT; // Tab
        case cntrl('H'):
            return DialogPREV; // Backspace
        case meta('d'):
            return DialogDOWN;
        case meta('r'):
            return DialogRIGHT;
        case meta('u'):
            return DialogUP;
        case meta('l'):
            return DialogLEFT;
        case meta('h'):
            return DialogHOME;
        case meta('e'):
            return DialogEND;
        case ' ': // Space -- switch
            *val = setval;
            scr->Put('*', palette[ColorTextSelected]);
            for (DialogElement *e = list; e; e = e->Next())
                if (e != this)
                    e->Redraw();
            continue;
        }
    }
}

int DialogCompactRadioButton::Run()
{
    scr->Move(r + baserow, c + basecol);
    scr->Put(*val == setval ? '#' : '.', palette[ColorTextSelected]);
    for (;;) {
        scr->Move(r + baserow, c + basecol);
        scr->Sync();
        switch (scr->GetKey()) {
        default:
            scr->Beep();
            continue;
        case cntrl('['):
            return DialogCANCEL; // Esc
        case cntrl('C'):
            return DialogCANCEL; // ^C
        case meta('J'):
            return DialogCANCEL; // F10
        case cntrl('M'):
            return DialogOK; // Return
        case cntrl('J'):
            return DialogOK; // Newline
        case cntrl('I'):
            return DialogNEXT; // Tab
        case cntrl('H'):
            return DialogPREV; // Backspace
        case meta('d'):
            return DialogDOWN;
        case meta('r'):
            return DialogRIGHT;
        case meta('u'):
            return DialogUP;
        case meta('l'):
            return DialogLEFT;
        case meta('h'):
            return DialogHOME;
        case meta('e'):
            return DialogEND;
        case ' ': // Space -- switch
            if (*val == setval) {
                *val = -1;
                scr->Put('.', palette[ColorTextSelected]);
            } else {
                *val = setval;
                scr->Put('#', palette[ColorTextSelected]);
            }
            for (DialogElement *e = list; e; e = e->Next())
                if (e != this)
                    e->Redraw();
            //                      continue;
            return DialogRIGHT;
        }
    }
}

int DialogCommand::Run()
{
    if (label)
        scr->Put(r + baserow, c + basecol, label,
                 val == 1 ? palette[ColorCommandDefaultSelected]
                          : palette[ColorCommandSelected]);
    for (;;) {
        scr->HideCursor();
        scr->Sync();
        switch (scr->GetKey()) {
        default:
            scr->Beep();
            continue;
        case cntrl('['):
            return DialogCANCEL; // Esc
        case cntrl('C'):
            return DialogCANCEL; // ^C
        case meta('J'):
            return DialogCANCEL; // F10
        case cntrl('I'):
            return DialogNEXT; // Tab
        case cntrl('H'):
            return DialogPREV; // Backspace
        case meta('d'):
            return DialogDOWN;
        case meta('r'):
            return DialogRIGHT;
        case meta('u'):
            return DialogUP;
        case meta('l'):
            return DialogLEFT;
        case meta('h'):
            return DialogHOME;
        case meta('e'):
            return DialogEND;
        case cntrl('M'):
            return val; // Return
        case cntrl('J'):
            return val; // Newline
        }
    }
}

int DialogList::Run()
{
again:
    int len = strlen(str);

    scr->Move(r + baserow, c + basecol);
    scr->Put('[', palette[ColorText]);
    for (int i = 0; i < w - 2; ++i)
        scr->Put(i < len ? str[i] : ' ', palette[ColorFieldSelected]);
    scr->Put(']', palette[ColorText]);
    for (;;) {
        scr->Move(r + baserow, c + basecol + 1);
        scr->Sync();
        int k = scr->GetKey();

        switch (k) {
        default:
            scr->Beep();
            continue;
        case cntrl('['):
            return DialogCANCEL; // Esc
        case cntrl('C'):
            return DialogCANCEL; // ^C
        case meta('J'):
            return DialogCANCEL; // F10
        case cntrl('M'):
            return DialogOK; // Return
        case cntrl('J'):
            return DialogOK; // Newline
        case cntrl('I'):
            return DialogNEXT; // Tab
        case meta('d'):
            return DialogDOWN;
        case meta('u'):
            return DialogUP;
        case meta('r'):
            return DialogRIGHT;
        case meta('l'):
            return DialogLEFT;
        case meta('h'):
            return DialogHOME;
        case meta('e'):
            return DialogEND;
        case ' ':
            break; // Choose
        }
        int col = c + basecol;

        if (col + maxw >= scr->Columns)
            col -= maxw - w;
        if ((*func)(scr, r + baserow, col, maxw, val, str) < 0)
            return DialogCANCEL;
        goto again;
    }
}

int DialogString::Run()
{
    int i, x = 0;
    int len = strlen(val);

    scr->PutLimited(r + baserow, c + basecol, val, w,
                    palette[ColorFieldSelected]);
    for (;;) {
        scr->Move(r + baserow, c + basecol + x);
        scr->Sync();
        int k = scr->GetKey();

        switch (k) {
        case cntrl('['):
            return DialogCANCEL; // Esc
        case cntrl('C'):
            return DialogCANCEL; // ^C
        case meta('J'):
            return DialogCANCEL; // F10
        case cntrl('M'):
            return DialogOK; // Return
        case cntrl('J'):
            return DialogOK; // Newline
        case cntrl('I'):
            return DialogNEXT; // Tab
        case meta('d'):
            return DialogDOWN;
        case meta('u'):
            return DialogUP;
        case meta('r'): // Right
            if (x < len)
                ++x;
            continue;
        case meta('l'): // Left
            if (x > 0)
                --x;
            continue;
        case meta('h'): // Home
            x = 0;
            continue;
        case meta('e'): // End
            x = len;
            continue;
        default:
            if (k < ' ' || k == 0177 || k > 0377 || len >= maxlen) {
                scr->Beep();
                continue;
            }
            for (i = len; i > x; --i)
                val[i] = val[i - 1];
            val[x] = k;
            if (len < maxlen)
                ++len;
            val[len] = 0;
            scr->PutLimited(val + x, w - x, palette[ColorFieldSelected]);
            ++x;
            break;
        case cntrl('H'): // Backspace
            if (x == 0)
                continue;
            --x;
            scr->Move(r + baserow, c + basecol + x);
            // Fall through...
        case meta('x'): // Delete
            if (x == len)
                continue;
            for (i = x; i < len; ++i)
                val[i] = val[i + 1];
            --len;
            scr->PutLimited(val + x, w - x, palette[ColorFieldSelected]);
            scr->Put(' ', palette[ColorFieldSelected]);
            break;
        }
    }
}

int DialogPassword::Run()
{
    int len = strlen(val);
    int i, x = 0;

    scr->Move(r + baserow, c + basecol);
    for (i = 0; i < w; ++i)
        scr->Put(i < len ? '*' : ' ', palette[ColorFieldSelected]);
    for (;;) {
        scr->Move(r + baserow, c + basecol + x);
        scr->Sync();
        int k = scr->GetKey();

        switch (k) {
        case cntrl('['):
            return DialogCANCEL; // Esc
        case cntrl('C'):
            return DialogCANCEL; // ^C
        case meta('J'):
            return DialogCANCEL; // F10
        case cntrl('M'):
            return DialogOK; // Return
        case cntrl('J'):
            return DialogOK; // Newline
        case cntrl('I'):
            return DialogNEXT; // Tab
        case meta('d'):
            return DialogDOWN;
        case meta('u'):
            return DialogUP;
        case meta('r'): // Right
            if (x < len)
                ++x;
            continue;
        case meta('l'): // Left
            if (x > 0)
                --x;
            continue;
        case meta('h'): // Home
            x = 0;
            continue;
        case meta('e'): // End
            x = len;
            continue;
        default:
            if (k < ' ' || k == 0177 || k > 0377 || len >= maxlen) {
                scr->Beep();
                continue;
            }
            for (i = len; i > x; --i)
                val[i] = val[i - 1];
            val[x++] = k;
            if (len < maxlen)
                ++len;
            val[len] = 0;
            scr->Put(r + baserow, c + basecol + len - 1, '*',
                     palette[ColorFieldSelected]);
            break;
        case cntrl('H'): // Backspace
            if (x == 0)
                continue;
            --x;
            // Fall through...
        case meta('x'): // Delete
            if (x == len)
                continue;
            for (i = x; i < len; ++i)
                val[i] = val[i + 1];
            --len;
            scr->Put(r + baserow, c + basecol + len, ' ',
                     palette[ColorFieldSelected]);
            break;
        }
    }
}

int DialogNumber::Run()
{
    char buf[80];
    int i, ret = 0;

    *buf = 0;
    if (*val)
        sprintf(buf, "%ld", *val);
    scr->Put(r + baserow, c + basecol, buf, palette[ColorFieldSelected]);
    int x   = 0;
    int len = strlen(buf);

    while (!ret) {
        scr->Move(r + baserow, c + basecol + x);
        scr->Sync();
        int k = scr->GetKey();

        switch (k) {
        case cntrl('['):
            return DialogCANCEL; // Esc
        case cntrl('C'):
            return DialogCANCEL; // ^C
        case meta('J'):
            return DialogCANCEL; // F10
        case cntrl('M'):
            ret = DialogOK;
            break; // Return
        case cntrl('J'):
            ret = DialogOK;
            break; // Newline
        case cntrl('I'):
            ret = DialogNEXT;
            break; // Tab
        case meta('d'):
            ret = DialogDOWN;
            break;
        case meta('u'):
            ret = DialogUP;
            break;
        case meta('r'): // Right
            if (x < len)
                ++x;
            continue;
        case meta('l'): // Left
            if (x > 0)
                --x;
            continue;
        case meta('h'): // Home
            x = 0;
            continue;
        case meta('e'): // End
            x = len;
            continue;
        default:
            if (k < '0' || k > '9' || len >= w) {
                scr->Beep();
                continue;
            }
            for (i = len; i > x; --i)
                buf[i] = buf[i - 1];
            buf[x]     = k;
            buf[++len] = 0;
            scr->PutLimited(buf + x, w - x, palette[ColorFieldSelected]);
            ++x;
            break;
        case cntrl('H'): // Backspace
            if (x <= 0)
                continue;
            --x;
            scr->Move(r + baserow, c + basecol + x);
            // Fall through...
        case meta('x'): // Delete
            if (x == len)
                continue;
            for (i = x; i < len; ++i)
                buf[i] = buf[i + 1];
            --len;
            scr->PutLimited(buf + x, w - x, palette[ColorFieldSelected]);
            scr->Put(' ', palette[ColorFieldSelected]);
            break;
        }
    }
    *val = strtol(buf, 0, 0);
    if (*val > max)
        *val = max;
    *val = (*val - min) / step * step + min;
    return ret;
}

int DialogHexNumber::Run()
{
    char buf[80];
    int i, ret = 0;

    *buf = 0;
    if (*val)
        sprintf(buf, "%lx", *val);
    scr->Put(r + baserow, c + basecol, buf, palette[ColorFieldSelected]);
    int x   = 0;
    int len = strlen(buf);

    while (!ret) {
        scr->Move(r + baserow, c + basecol + x);
        scr->Sync();
        int k = scr->GetKey();

        switch (k) {
        case cntrl('['):
            return DialogCANCEL; // Esc
        case cntrl('C'):
            return DialogCANCEL; // ^C
        case meta('J'):
            return DialogCANCEL; // F10
        case cntrl('M'):
            ret = DialogOK;
            break; // Return
        case cntrl('J'):
            ret = DialogOK;
            break; // Newline
        case cntrl('I'):
            ret = DialogNEXT;
            break; // Tab
        case meta('d'):
            ret = DialogDOWN;
            break;
        case meta('u'):
            ret = DialogUP;
            break;
        case meta('r'): // Right
            if (x < len)
                ++x;
            continue;
        case meta('l'): // Left
            if (x > 0)
                --x;
            continue;
        case meta('h'): // Home
            x = 0;
            continue;
        case meta('e'): // End
            x = len;
            continue;
        default:
            if ((!(k >= '0' && k <= '9') && !(k >= 'a' && k <= 'f') &&
                 !(k >= 'A' && k <= 'F')) ||
                len >= w) {
                scr->Beep();
                continue;
            }
            if (k >= 'A' && k <= 'F')
                k += 'a' - 'A';
            for (i = len; i > x; --i)
                buf[i] = buf[i - 1];
            buf[x]     = k;
            buf[++len] = 0;
            scr->PutLimited(buf + x, w - x, palette[ColorFieldSelected]);
            ++x;
            break;
        case cntrl('H'): // Backspace
            if (x <= 0)
                continue;
            --x;
            scr->Move(r + baserow, c + basecol + x);
            // Fall through...
        case meta('x'): // Delete
            if (x == len)
                continue;
            for (i = x; i < len; ++i)
                buf[i] = buf[i + 1];
            --len;
            scr->PutLimited(buf + x, w - x, palette[ColorFieldSelected]);
            scr->Put(' ', palette[ColorFieldSelected]);
            break;
        }
    }
    *val = strtol(buf, 0, 16);
    if (*val > max)
        *val = max;
    *val = (*val - min) / step * step + min;
    return ret;
}

int DialogMask::Run()
{
    int ret = 0;
    int x   = 0;

    while (!ret) {
        scr->Move(r + baserow, c + basecol);
        for (int i = 0; i < w; ++i)
            scr->Put(*val >> i & 1 ? '#' : '.', palette[ColorFieldSelected]);
        scr->Move(r + baserow, c + basecol + x);
        scr->Sync();
        int k = scr->GetKey();

        switch (k) {
        case cntrl('['):
            return DialogCANCEL; // Esc
        case cntrl('C'):
            return DialogCANCEL; // ^C
        case meta('J'):
            return DialogCANCEL; // F10
        case cntrl('M'):
            ret = DialogOK;
            break; // Return
        case cntrl('J'):
            ret = DialogOK;
            break; // Newline
        case cntrl('I'):
            ret = DialogNEXT;
            break; // Tab
        case meta('d'):
            ret = DialogDOWN;
            break;
        case meta('u'):
            ret = DialogUP;
            break;
        case ' ': // Space
            *val ^= 1L << x;
            // fall through...
        case meta('r'): // Right
            if (x < w - 1)
                ++x;
            else
                return DialogRIGHT;
            continue;
        case cntrl('H'): // Backspace
        case meta('l'):  // Left
            if (x > 0)
                --x;
            else
                return DialogLEFT;
            continue;
        case meta('h'): // Home
            x = 0;
            continue;
        case meta('e'): // End
            x = w - 1;
            continue;
        default:
            scr->Beep();
            continue;
        }
    }
    return ret;
}

DialogGroup::DialogGroup(int row0, int col0, const char *&fmt, va_list &args,
                         int flags, DialogGroup *parent)
    : DialogElement(row0, col0)
{
    DialogElement *v = 0;
    int *radio       = 0;

    tab       = 0;
    maxnv     = 0;
    nv        = 0;
    frameflag = (flags & GRP_FRAMED) != 0;
    if (frameflag)
        ++row0, col0 += 2;
    int currow = row0;
    int curcol = col0;

    while (*fmt) {
        int oblig = (*fmt >= 'a' && *fmt <= 'z');

        switch (*fmt++) {
        default:
            continue;
        case 0:
            --fmt;
            // fall through...
        case ']':
        case ')':
        case '>':
            if (frameflag)
                ++h, w += 2;
            if ((flags & GRP_CENTER) && !col0)
                Center(parent->w);
            return;
        case '-':
            ++curcol;
            continue;
        case '`':
            --curcol;
            continue;
        case '|':
            if (!v)
                continue;
            currow = v->r;
            curcol = v->c + v->w + 1;
            continue;
        case '+':
            currow = row0;
            col0 = curcol = c + (++w);
            continue;
        case '{':
            if (*fmt == '}') {
                ++currow;
                ++fmt;
                continue;
            }
            --fmt;
            v = new DialogLabel(currow, curcol, fmt);
            break;
        case '_':
            v = new DialogLine(currow, curcol, this);
            break;
        case '*':
            v = new DialogCommand(currow, curcol, fmt);
            break;
        case '(':
            v = new DialogGroup(currow, curcol, fmt, args);
            break;
        case '[':
            v = new DialogGroup(currow, curcol, fmt, args, GRP_FRAMED);
            break;
        case '<':
            v = new DialogGroup(currow, curcol, fmt, args, GRP_CENTER, this);
            break;
        case 'b':
        case 'B':
            v = new DialogButton(currow, curcol, fmt, va_arg(args, int *));
            v->oblig = oblig;
            break;
        case 't':
        case 'T':
            v = new DialogTitle(currow, curcol, fmt, va_arg(args, char *));
            v->oblig = oblig;
            break;
        case 's':
        case 'S':
            v = new DialogString(currow, curcol, fmt, va_arg(args, char *));
            v->oblig = oblig;
            break;
        case 'p':
        case 'P':
            v = new DialogPassword(currow, curcol, fmt, va_arg(args, char *));
            v->oblig = oblig;
            break;
        case 'l':
        case 'L':
            v        = new DialogList(currow, curcol, fmt, va_arg(args, long *),
                                      va_arg(args, DialogListFunc));
            v->oblig = oblig;
            break;
        case 'r':
        case 'R':
            v = new DialogRadioButton(currow, curcol, fmt,
                                      radio = va_arg(args, int *));
            break;
        case ':':
            --fmt;
            v = new DialogRadioButton(currow, curcol, fmt, radio);
            break;
        case 'c':
        case 'C':
            v = new DialogCompactRadioButton(currow, curcol, fmt,
                                             radio = va_arg(args, int *));
            break;
        case '.':
            --fmt;
            v = new DialogCompactRadioButton(currow, curcol, fmt, radio);
            break;
        case 'n':
        case 'N':
            v = new DialogNumber(currow, curcol, fmt, va_arg(args, long *));
            v->oblig = oblig;
            break;
        case 'x':
        case 'X':
            v = new DialogHexNumber(currow, curcol, fmt, va_arg(args, long *));
            v->oblig = oblig;
            break;
        case 'm':
        case 'M':
            v = new DialogMask(currow, curcol, fmt, va_arg(args, long *));
            v->oblig = oblig;
            break;
        }
        if (v) {
            Add(v);
            curcol = col0;
            currow = v->r + v->h;
        }
    }
}

void DialogGroup::Add(DialogElement *v)
{
    if (nv >= maxnv) {
        maxnv += 10;
        DialogElement **newtab = new DialogElement *[maxnv];

        if (!newtab)
            return;
        if (nv) {
            memcpy(newtab, tab, nv * sizeof(*tab));
            delete tab;
        }
        tab = newtab;
    }
    tab[nv++] = v;
    if (w < v->c + v->w - c)
        w = v->c + v->w - c;
    if (h < v->r + v->h - r)
        h = v->r + v->h - r;
}

DialogGroup::~DialogGroup()
{
    if (!tab)
        return;
    for (int i = 0; i < nv; ++i)
        if (tab[i])
            delete tab[i];
    delete tab;
}

void DialogGroup::Center(int wid)
{
    if (w < wid)
        w = wid;
    int i;

    if (nv > 1 && tab[0]->r == tab[1]->r) { // horizontal group
        int n, space = w;

        for (i = 0; i < nv; ++i)
            space -= tab[i]->w;
        int off = 0;

        for (i = 0, n = nv + 1; i < nv; --n, ++i) {
            off += space / n;
            tab[i]->c += off;
            space -= space / n;
        }
    } else // vertical group
        for (i = 0; i < nv; ++i)
            tab[i]->c = (w - tab[i]->w) / 2;
}

DialogElement *DialogElement::Find(int dir)
{
    DialogElement *e, *x = this;
    int d = 0, dr, dc, xd;

    switch (dir) {
    case DialogRIGHT:
        for (xd = 9999, e = list; e; e = e->Next())
            if (e->r == r && e->c > c && (d = e->c - c - w) < xd)
                x = e, xd = d;
        break;
    case DialogLEFT:
        for (xd = 9999, e = list; e; e = e->Next())
            if (e->r == r && e->c < c && (d = c - e->c - e->w) < xd)
                x = e, xd = d;
        break;
    case DialogUP:
        for (xd = 9999, e = list; e; e = e->Next())
            if (e->c == c && e->r < r && (d = r - e->r - e->h) < xd)
                x = e, xd = d;
        break;
    case DialogDOWN:
        for (xd = 9999, e = list; e; e = e->Next())
            if (e->c == c && e->r > r && (d = e->r - r - h) < xd)
                x = e, xd = d;
        break;
    }
    if (x != this)
        return x;
    for (xd = 9999, e = list; e; e = e->Next()) {
        switch (dir) {
        case DialogDOWN:
            if (e->r > r)
                break;
            continue;
        case DialogUP:
            if (e->r < r)
                break;
            continue;
        case DialogRIGHT:
            if (e->c >= c + w)
                break;
            continue;
        case DialogLEFT:
            if (e->c + e->w <= c)
                break;
            continue;
        }
        dr = e->r - r;
        if (e->c + e->w < c)
            dc = c - e->c - e->w;
        else if (e->c > c + w)
            dc = e->c - c - w;
        else
            dc = 0;
        d = 4 * dr * dr + dc * dc;
        if (d < xd) {
            x  = e;
            xd = d;
        }
    }
    return x;
}

DialogElement *DialogElement::Home()
{
    DialogElement *e, *x = 0;
    int xd = 9999;

    for (e = list; e; e = e->Next()) {
        int d = 4 * e->r * e->r + e->c * e->c;

        if (d < xd) {
            x  = e;
            xd = d;
        }
    }
    return x;
}

DialogElement *DialogElement::End()
{
    DialogElement *e, *x = 0;
    int xd = 0;

    for (e = list; e; e = e->Next()) {
        int d = 4 * e->r * e->r + e->c * e->c;

        if (d > xd) {
            x  = e;
            xd = d;
        }
    }
    return x;
}

int Dialog::Run(Screen *scr)
{
    int w = grp->w + 4;
    int h = grp->h + 2;

    r = 1 + (scr->Lines - h) / 2;
    c = 2 + (scr->Columns - w) / 2;

    // Save box.
    Box box(*scr, r - 1, c - 2, h + 1, w + 1);

    scr->Clear(r, c - 1, h - 2, 1, palette[ColorText], ' ');
    scr->Clear(r, c + w - 4, h - 2, 1, palette[ColorText], ' ');

    // Draw frame.
    scr->DrawFrame(r - 1, c - 2, h, w, palette[ColorFrame]);
    scr->Put(r - 1, c - 2 + (w - strlen(title)) / 2, title,
             palette[ColorFrame]);

    // Draw shadow.
    int i;

    for (i = 0; i < w; ++i)
        scr->AttrLow(r - 1 + h, c + i - 1);
    for (i = 0; i < h - 1; ++i)
        scr->AttrLow(r + i, c + w - 2);

    DialogElement::scr     = scr;
    DialogElement::baserow = r;
    DialogElement::basecol = c;
    DialogElement::palette = palette;
    DialogElement::list    = list;

    grp->Draw(); // draw dialog

    DialogElement *ep = list;
    DialogElement *e  = DialogElement::Home();

    while (e->enabled == 0 && e != DialogElement::End())
        e = e->next;
    if (e->enabled == 0)
        e = ep;
    DialogElement *ef;
    int n = DialogCANCEL;

    while (e) {
        n = e->Run(); // run dialog
        if (n == DialogOK)
            for (; e->next != e->list; e = e->next)
                if (e->next->oblig) {
                    n = DialogNEXT;
                    break;
                }
        if (n >= 0)
            break;
        e->Draw();
        ep = e;
        switch (n) {
        default:
            scr->Beep();
            continue;
        case DialogHOME:
            e = DialogElement::Home();
            while (e->enabled == 0 && e != DialogElement::End())
                e = e->next;
            if (e->enabled == 0)
                e = ep;
            continue;
        case DialogEND:
            e = DialogElement::End();
            while (e->enabled == 0 && e != DialogElement::Home())
                e = e->prev;
            if (e->enabled == 0)
                e = ep;
            continue;
        case DialogNEXT:
            e = e->next;
            while (e->enabled == 0 && ep != e)
                e = e->next;
            continue;
        case DialogPREV:
            e = e->prev;
            while (e->enabled == 0 && ep != e)
                e = e->prev;
            continue;
        case DialogUP:
        case DialogDOWN:
        case DialogRIGHT:
        case DialogLEFT:
            e = e->Find(n);
            while (e->enabled == 0 && ep != e) {
                ef = e;
                if ((e = e->Find(n)) != ef)
                    continue;
                switch (n) {
                case DialogUP:
                    n = DialogDOWN;
                    break;
                case DialogDOWN:
                    n = DialogUP;
                    break;
                case DialogRIGHT:
                    n = DialogLEFT;
                    break;
                case DialogLEFT:
                    n = DialogRIGHT;
                    break;
                }

                if ((e = e->Find(n)) == ef) {
                    e = ep;
                    break;
                }
            }
            continue;
        }
    }
    scr->Put(box); // restore screen
    return (n);
}

char *DialogElement::GetString(const char *&p)
{
    char buf[256], *q = buf;

    while (*p && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n'))
        ++p;
    if (*p == '{') {
        while (*++p && *p != '}')
            *q++ = *p;
        if (*p)
            p++;
    }
    *q = 0;
    q  = new char[strlen(buf) + 1];

    if (!q)
        return (0);
    strcpy(q, buf);
    return (q);
}

long DialogElement::GetNumber(const char *&p)
{
    long v = 0;

    while (*p && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n'))
        ++p;
    if (*p != ':' && *p != '.')
        return (0);
    while (*++p && *p >= '0' && *p <= '9')
        v = v * 10 + *p - '0';
    return (v);
}

long DialogElement::GetHexNumber(const char *&p)
{
    long v = 0;

    while (*p && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n'))
        ++p;
    if (*p != ':')
        return (0);
    while (*++p && ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f') ||
                    (*p >= 'A' && *p <= 'F'))) {
        v = v * 16 + *p;
        if (*p >= '0' && *p <= '9')
            v -= '0';
        else if (*p >= 'a' && *p <= 'f')
            v -= 'a' - 10;
        else
            v -= 'A' - 10;
    }
    return (v);
}

Dialog::Dialog(const char *t, unsigned char *pal, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    title               = t;
    palette             = pal;
    DialogElement::list = 0;
    grp                 = new DialogGroup(0, 0, fmt, args);
    va_end(args);
    list = DialogElement::list;
    if (grp->w < (int)strlen(title) - 2)
        grp->w = strlen(title) - 2;
}

void DialogLabel::Draw()
{
    if (label)
        scr->Put(r + baserow, c + basecol, label, palette[ColorText]);
}

void DialogLine::Draw()
{
    if (!w) {
        w = parent->w;
        if (parent->frameflag)
            w -= 4;
    }
    scr->HorLine(r + baserow, c + basecol, w, palette[ColorText]);
}

void DialogButton::Draw()
{
    scr->Put(r + baserow, c + basecol, *val ? "[*] " : "[ ] ",
             palette[ColorText]);
    if (label)
        scr->Put(label, palette[ColorText]);
}

void DialogNumber::Draw()
{
    scr->Print(r + baserow, c + basecol, palette[ColorField], "%*s", w, "");
    scr->Print(r + baserow, c + basecol, palette[ColorField], "%ld", *val);
}

void DialogHexNumber::Draw()
{
    scr->Print(r + baserow, c + basecol, palette[ColorField], "%*s", w, "");
    scr->Print(r + baserow, c + basecol, palette[ColorField], "%lx", *val);
}

void DialogMask::Draw()
{
    scr->Move(r + baserow, c + basecol);
    for (int i = 0; i < w; ++i)
        scr->Put(*val >> i & 1 ? '#' : '.', palette[ColorField]);
}

void DialogCommand::Draw()
{
    if (label)
        scr->Put(
            r + baserow, c + basecol, label,
            val == 1 ? palette[ColorCommandDefault] : palette[ColorCommand]);
}

void DialogRadioButton::Draw()
{
    scr->Move(r + baserow, c + basecol);
    scr->Put(*val == setval ? "(*) " : "( ) ",
             enabled ? palette[ColorText] : 0x17);
    if (label)
        scr->Put(label, enabled ? palette[ColorText] : 0x17);
}

void DialogRadioButton::Redraw()
{
    scr->Move(r + baserow, c + basecol + 1);
    scr->Put(*val == setval ? '*' : ' ', palette[ColorText]);
}

void DialogCompactRadioButton::Draw()
{
    scr->Move(r + baserow, c + basecol);
    scr->Put(*val == setval ? '#' : '.', palette[ColorText]);
}

void DialogCompactRadioButton::Redraw()
{
    Draw();
}

void DialogList::Draw()
{
    int len = strlen(str);

    scr->Move(r + baserow, c + basecol);
    scr->Put('[', palette[ColorText]);
    for (int i = 0; i < w - 2; ++i)
        scr->Put(i < len ? str[i] : ' ', palette[ColorField]);
    scr->Put(']', palette[ColorText]);
}

void DialogTitle::Draw()
{
    int len = strlen(val);

    scr->Move(r + baserow, c + basecol);
    for (int i = 0; i < w; ++i)
        scr->Put(i < len ? val[i] : ' ', palette[ColorField]);
}

void DialogString::Draw()
{
    int len = strlen(val);

    scr->Move(r + baserow, c + basecol);
    for (int i = 0; i < w; ++i)
        scr->Put(i < len ? val[i] : ' ', palette[ColorField]);
}

void DialogPassword::Draw()
{
    int len = strlen(val);

    scr->Move(r + baserow, c + basecol);
    for (int i = 0; i < w; ++i)
        scr->Put(i < len ? '*' : ' ', palette[ColorField]);
}

void DialogGroup::Draw()
{
    scr->Clear(r + baserow, c + basecol, h, w, palette[ColorText], ' ');
    if (frameflag)
        scr->DrawFrame(r + baserow, c + basecol, h, w, palette[ColorFrame]);
    for (int i = 0; i < nv; ++i)
        tab[i]->Draw();
}
