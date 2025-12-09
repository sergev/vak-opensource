//
// Popup windows implementation.
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
// $Id: popup.cpp,v 1.7 2009/09/23 10:48:41 ly Exp $
//

#include <stdarg.h>

#include "extern.h"
#include "popup.h"
#include "screen.h"

struct choice {
    char *name;
    int namlen;
    int r, c;
};

static struct choice tab[3];
static int cnum;

void ScreenImp::Error(int c, int i, const char *head, const char *reply,
                      const char *s, ...)
{
    char buf[100];
    char *ln;
    va_list args;

    va_start(args, s);
    vsprintf(buf, s, args);
    va_end(args);

    ln = strchr(buf, '\n');
    if (ln)
        *ln++ = 0;
    Popup(head, buf, 0, reply, 0, 0, c, i);
}

int ScreenImp::Popup(const char *head, const char *mesg, const char *mesg2,
                     const char *c1, const char *c2, const char *c3, int color,
                     int inverse)
{
    int len;
    int w = strlen(mesg);

    if (mesg2) {
        len = strlen(mesg2);
        if (len > w)
            w = len;
    }
    len = strlen(head);
    if (len > w)
        w = len;
    len = 0;
    if (c1)
        len += strlen(c1);
    if (c2)
        len += strlen(c2);
    if (c3)
        len += strlen(c3);
    if (len > w)
        w = len;
    int h = 6;

    w += 10;
    int r = Lines / 2 - h / 2;
    int c = Columns / 2 - w / 2;

    Box box(*this, r, c, h + 1, w + 1); // save box

    Clear(r, c, h, w, color, ' ');
    DrawFrame(r, c, h, w, color);
    Put(r, c + (w - strlen(head)) / 2, head, color); // head
    if (mesg2) {
        Put(r + 1, c + (w - strlen(mesg)) / 2, mesg, color);
        Put(r + 2, c + (w - strlen(mesg2)) / 2, mesg2, color);
    } else
        Put(r + 2, c + (w - strlen(mesg)) / 2, mesg, color);

    // Draw shadow.
    int i;

    for (i = 0; i < w; ++i)
        AttrLow(r + h, c + i + 1);
    for (i = 0; i < h - 1; ++i)
        AttrLow(r + i + 1, c + w);

    initChoice(r + 4, c + w / 2, c1, c2, c3);

    int ch = menuChoice(color, inverse);

    Put(box);
    return (ch);
}

void ScreenImp::initChoice(int row, int col, const char *c1, const char *c2,
                           const char *c3)
{
    cnum        = c2 ? (c3 ? 3 : 2) : 1;
    tab[0].name = (char *)c1;
    tab[1].name = (char *)c2;
    tab[2].name = (char *)c3;
    int w       = cnum - 1;

    for (int i = 0; i < cnum; ++i) {
        w += tab[i].namlen = strlen(tab[i].name);
        tab[i].r           = row;
    }
    tab[0].c = col - w / 2;
    tab[1].c = tab[0].c + tab[0].namlen + 1;
    tab[2].c = tab[1].c + tab[1].namlen + 1;
}

int ScreenImp::menuChoice(int color, int inverse)
{
    int ch = 0;

    for (;;) {
        for (int i = 0; i < cnum; ++i) {
            if (i == ch) {
                Put(tab[i].r, tab[i].c, tab[i].name, inverse);
                if (!HasInverse()) {
                    Put(tab[i].r, tab[i].c, '[', inverse);
                    Put(tab[i].r, tab[i].c + strlen(tab[i].name) - 1, ']',
                        inverse);
                }
            } else
                Put(tab[i].r, tab[i].c, tab[i].name, color);
        }
        HideCursor();
        Sync();
        switch (GetKey()) {
        default:
            Beep();
            continue;
        case cntrl('['):
        case cntrl('C'):
        case meta('J'): // f0
            return (-1);
        case cntrl('M'):
        case cntrl('J'):
            return (ch);
        case ' ':
        case cntrl('I'):
        case meta('r'): // right
            if (++ch >= cnum)
                ch = 0;
            continue;
        case cntrl('H'):
        case meta('l'): // left
            if (--ch < 0)
                ch = cnum - 1;
            continue;
        }
    }
}

char *ScreenImp::editString(int r, int c, int w, char *str, int cp, int color)
{
    int k;
    int firstkey = 1;

    if (cp) {
        for (cp = 0; str[cp]; ++cp)
            continue;
        firstkey = 0;
    }
    for (;; firstkey = 0) {
        Clear(r, c, 1, w, color, ' ');
        Put(r, c, str, color);
        Move(r, c + cp);
        Sync();
        int key = GetKey();

        switch (key) {
        default:
            if (key < ' ' || key == 0177 || key > 0377) {
                Beep();
                continue;
            }
            if (firstkey) {
                str[0] = key;
                str[1] = 0;
                cp     = 1;
                continue;
            }
            for (k = cp; str[k]; ++k) {
                int t = key;

                key    = str[k];
                str[k] = t;
            }
            str[k] = key;
            str[w] = str[k + 1] = 0;
            // fall through
        case meta('r'): // right
            if (str[cp]) {
                ++cp;
                if (cp >= w)
                    cp = w - 1;
            }
            continue;
        case meta('l'): // left
            if (--cp < 0)
                cp = 0;
            continue;
        case cntrl('C'):
        case cntrl('['):
        case meta('J'): // f0
            return (0);
        case cntrl('M'):
        case cntrl('J'):
            return (str);
        case cntrl('I'):
            if (str[cp])
                while (str[++cp])
                    continue;
            else
                cp = 0;
            continue;
        case meta('h'): // home
            cp = 0;
            continue;
        case meta('e'): // end
            while (str[cp])
                ++cp;
            continue;
        case cntrl('H'): // back space
            if (cp) {
                for (k = cp--; str[k]; ++k)
                    str[k - 1] = str[k];
                str[k - 1] = 0;
            }
            continue;
        case meta('x'): // delete
            if (!str[cp])
                continue;
            for (k = cp + 1; str[k]; ++k)
                str[k - 1] = str[k];
            str[k - 1] = 0;
            continue;
        case cntrl('Y'): // clear line
            str[cp = 0] = 0;
            continue;
        }
    }
}

char *ScreenImp::GetString(int w, char *str, const char *head, char *mesg,
                           int color, int inverse)
{
    int len = strlen(mesg);

    if (len > w)
        w = len;
    len = strlen(head);
    if (len > w)
        w = len;
    int h = 4;

    w += 4;
    int r = (Lines - h) / 2;
    int c = (Columns - w) / 2;

    Box box(*this, r, c, h + 1, w + 1); // save box

    Clear(r, c, h, w, color, ' ');
    DrawFrame(r, c, h, w, color);
    Put(r, c + (w - strlen(head)) / 2, head, color); // head
    Put(r + 1, c + 2, mesg, color);                  // message

    // Draw shadow.
    int i;

    for (i = 0; i < w; ++i)
        AttrLow(r + h, c + i + 1);
    for (i = 0; i < h - 1; ++i)
        AttrLow(r + i + 1, c + w);

    static char buf[129];

    strncpy(buf, str ? str : "", 80);

    str = editString(r + 2, c + 2, w - 4, buf, 0, inverse);
    Put(box);
    return (str);
}

// Отрисовка мультистроки в popup-окне с задержкой ввода.

void ScreenImp::PopupString(const char *title, const char *str,
                            const char *reply, int color, int inverse)
{
    int w, h, r, c;

    // Мультистрока str содержит несколько строк,
    // разделенных символом \n.  Просмотрим мультистроку и вычислим
    // ее ширину и высоту.
    w = h = r     = 0;
    const char *p = str;

    for (;;) {
        const char *q = strchr(p, '\n');

        if (!q)
            break;
        if (q - p > w)
            w = q - p;
        ++h;
        p = q + 1;
    }

    // Проверим, что заголовок и кнопка помещаются.
    r = strlen(title);
    if (r > w)
        w = r;
    r = strlen(reply);
    if (r > w)
        w = r;

    // Отводим место место под рамки, кнопку и отступ.
    h += 4;
    w += 4;

    // Вычисляем левый верхний угол (окно в центре экрана).
    r = Lines / 2 - h / 2;
    c = Columns / 2 - w / 2;

    // Сохраняем изображение под окном, рисуем окно.
    Box box(*this, r, c, h + 1, w + 1);

    Clear(r, c, h, w, color, ' '); // заполняем поле
    DrawFrame(r, c, h, w, color);  // рамка
    int i;

    for (i = 0; i < w; ++i)
        AttrLow(r + h, c + i + 1); // тень снизу
    for (i = 0; i < h - 1; ++i)
        AttrLow(r + i + 1, c + w); // тень справа

    // Рисуем заголовок.
    Put(r, c + (w - strlen(title)) / 2, title, color);

    // Рисуем мультистроку.
    p = str;
    for (i = r + 1;; ++i) {
        const char *q = strchr(p, '\n');

        if (!q)
            break;
        PutLimited(i, c + 2, p, q - p, color);
        p = q + 1;
    }

    // Рисуем кнопку.
    Put(r + h - 2, c + (w - strlen(reply)) / 2, reply, inverse);

    // Задерживаем ввод.
    HideCursor();
    Sync();
    for (;;) {
        switch (GetKey()) {
        default:
            Beep();
            continue;
        case cntrl('['): // Esc
        case cntrl('C'): // ^C
        case meta('J'):  // F0
        case cntrl('M'): // Return
        case cntrl('J'): // LineFeed
            Put(box);
            return;
        }
    }
}

void Flash::InitFlash(ScreenImp *s, const char *head, const char *mesg,
                      const char *mesg2, int color)
{
    scr     = s;
    int h   = 5;
    int w   = strlen(mesg);
    int len = strlen(head);

    if (len > w)
        w = len;
    if (mesg2) {
        len = strlen(mesg2);
        if (len > w)
            w = len;
        h += 2;
    }
    w += 4;
    int r = s->Lines / 2 - h / 2;
    int c = s->Columns / 2 - w / 2;

    box = new Box(*s, r, c, h + 1, w + 1); // save box
    s->Clear(r, c, h, w, color, ' ');
    s->DrawFrame(r, c, h, w, color);
    s->Put(r, c + (w - strlen(head)) / 2, head, color); // head
    s->Put(r + 2, c + (w - strlen(mesg)) / 2, mesg, color);
    if (mesg2)
        s->Put(r + 4, c + (w - strlen(mesg2)) / 2, mesg2, color);

    // Draw shadow.
    int i;

    for (i = 0; i < w; ++i)
        s->AttrLow(r + h, c + i + 1);
    for (i = 0; i < h - 1; ++i)
        s->AttrLow(r + i + 1, c + w);
    s->HideCursor();
    s->Sync();
}

Flash::~Flash()
{
    scr->Put(*box);
    delete box;
}

int ScreenImp::SelectFromList(int color, int inverse, const char *head,
                              const char *mesg, int cnt, ...)
{
    int w   = strlen(mesg);
    int len = strlen(head);

    if (len > w)
        w = len;

    // Compute the maximum length of strings from the list.
    va_list ap;

    va_start(ap, cnt);
    int i, maxlen = 0;

    for (i = 0; i < cnt; ++i) {
        char *elem = va_arg(ap, char *);

        len = strlen(elem);
        if (len > maxlen)
            maxlen = len;
    }
    va_end(ap);
    if (maxlen > w)
        w = maxlen;

    int h = cnt + 4;

    w += 4;
    int r   = Lines / 2 - h / 2;
    int c   = Columns / 2 - w / 2;
    int off = (w - maxlen) / 2;

    Box box(*this, r, c, h + 1, w + 1); // save box

    Clear(r, c, h, w, color, ' ');
    DrawFrame(r, c, h, w, color);
    Put(r, c + (w - strlen(head)) / 2, head, color); // head
    Put(r + 1, c + (w - strlen(mesg)) / 2, mesg, color);

    // Draw shadow.
    for (i = 0; i < w; ++i)
        AttrLow(r + h, c + i + 1);
    for (i = 0; i < h - 1; ++i)
        AttrLow(r + i + 1, c + w);

    // Рисуем мультистроку.
    va_start(ap, cnt);
    for (i = 0; i < cnt; ++i) {
        char *elem = va_arg(ap, char *);

        Put(r + 3 + i, c + off, elem, color);
    }
    va_end(ap);

    i = 0;
    for (;;) {
        Box *curbox = new Box(*this, r + 3 + i, c + off, 1, maxlen);

        Put(*curbox, inverse);
        HideCursor();
        Sync();
        int n = GetKey();

        Put(*curbox);
        delete (curbox);
        switch (n) {
        default:
            Beep();
            continue;
        case cntrl(']'): /* redraw screen */
            Redraw();
            continue;
        case cntrl('J'):
        case cntrl('M'):
            break;
        case cntrl('C'):
        case cntrl('['):
        case meta('J'): /* f0 */
            i = -1;
            break;
        case meta('u'): /* up */
            if (--i < 0)
                i = cnt - 1;
            continue;
        case meta('d'): /* down */
            if (++i >= cnt)
                i = 0;
            continue;
        case meta('h'): /* home */
            i = 0;
            continue;
        case meta('e'): /* end */
            i = cnt - 1;
            continue;
        }
        break;
    }
    Put(box);
    return (i);
}

int ScreenImp::SelectFromTable(int color, int inverse, const char *head,
                               const char *mesg, int cnt, char *elem[])
{
    int w   = strlen(mesg);
    int len = strlen(head);

    if (len > w)
        w = len;

    // Compute the number of columns.
    int ncol = (cnt + Lines - 7) / (Lines - 6);
    int nrow = (cnt + ncol - 1) / ncol;

    // Compute the maximum length of strings from the list.
    int i, maxlen = 0;

    for (i = 0; i < cnt; ++i) {
        len = strlen(elem[i]);
        if (len > maxlen)
            maxlen = len;
    }
    if (maxlen * ncol + ncol >= w)
        w = maxlen * ncol + ncol;

    int h = nrow + 4;

    w += 4;
    int r = Lines / 2 - h / 2;
    int c = Columns / 2 - w / 2;

    // Compute column offsets.
    int off[80];
    int o = (w - 4 - ncol * maxlen) / (ncol + 1);

    off[0] = 2 + o;
    for (i = 1; i < ncol; ++i)
        off[i] = off[i - 1] + maxlen + o;

    Box box(*this, r, c, h + 1, w + 1); // save box

    Clear(r, c, h, w, color, ' ');
    DrawFrame(r, c, h, w, color);
    Put(r, c + (w - strlen(head)) / 2, head, color); // head
    Put(r + 1, c + (w - strlen(mesg)) / 2, mesg, color);

    // Draw shadow.
    for (i = 0; i < w; ++i)
        AttrLow(r + h, c + i + 1);
    for (i = 0; i < h - 1; ++i)
        AttrLow(r + i + 1, c + w);

    // Рисуем мультистроки.
    int n;

    for (n = 0; n < ncol; ++n)
        for (i = 0; i < nrow; ++i)
            Put(r + 3 + i, c + off[n], elem[i + nrow * n], color);

    n = i = 0;
    for (;;) {
        Box *curbox = new Box(*this, r + 3 + i, c + off[n], 1, maxlen);

        Put(*curbox, inverse);
        HideCursor();
        Sync();
        int k = GetKey();

        Put(*curbox);
        delete (curbox);
        switch (k) {
        default:
            Beep();
            continue;
        case cntrl(']'): /* redraw screen */
            Redraw();
            continue;
        case cntrl('J'):
        case cntrl('M'):
            Put(box);
            return (i + n * nrow);
        case cntrl('C'):
        case cntrl('['):
        case meta('J'): /* f0 */
            Put(box);
            return (-1);
        case meta('u'): /* up */
            if (--i < 0)
                i = nrow - 1;
            continue;
        case meta('d'): /* down */
            if (++i >= nrow)
                i = 0;
            continue;
        case meta('l'): /* left */
            if (--n < 0)
                n = ncol - 1;
            continue;
        case meta('r'): /* right */
            if (++n >= ncol)
                n = 0;
            continue;
        case meta('h'): /* home */
            n = i = 0;
            continue;
        case meta('e'): /* end */
            i = nrow - 1;
            n = ncol - 1;
            continue;
        }
    }
}
