//
// Menu subpackage implementation.
//
// Copyright (C) 1994-2001 Cronyx Ltd.
// Author: Serge Vakulenko, <vak@zebub.msk.su>
//
// This software is distributed with NO WARRANTIES, not even the implied
// warranties for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// Authors grant any other persons or organisations permission to use
// or modify this software as long as this message is kept with the software,
// all derivative works or modified versions.
//
// $Id: menu.cpp,v 1.3 2006/04/19 17:26:54 ly Exp $
//

#include "extern.h"
#include "menu.h"
#include "screen.h"

static void displayName(ScreenImp *scr, char *name, int normal, int bold)
{
    for (char *p = name; *p; ++p)
        if (*p == '~') {
            scr->Put(*++p, bold);
        } else
            scr->Put(*p, normal);
}

Menu::Menu(int c, int l, int d, int i, int li)
{
    menusz = nmenus = columnsz = 0;
    color                      = c;
    light                      = l;
    dim                        = d;
    inverse                    = i;
    lightinverse               = li;
    current                    = 0;
}

Menu::Menu()
{
    menusz = nmenus = columnsz = 0;
    color                      = 0x70;
    light                      = 0xf0;
    dim                        = 0x30;
    inverse                    = 0x07;
    lightinverse               = 0x0f;
    current                    = 0;
}

void Menu::Palette(int c, int l, int d, int i, int li)
{
    color        = c;
    light        = l;
    dim          = d;
    inverse      = i;
    lightinverse = li;
}

Menu::~Menu()
{
    if (menusz)
        delete menu;

    if (columnsz)
        delete column;
}

SubMenu *Menu::Add(const char *name)
{
    if (!menusz) {
        menusz = 8;
        menu   = new SubMenu *[menusz];
        if (!menu)
            return (0);
    }
    if (nmenus >= menusz) {
        menusz += 8;
        menu = (SubMenu **)realloc(menu, menusz * sizeof(menu[0]));
        if (!menu) {
            menusz = nmenus = 0;
            return (0);
        }
    }
    menu[nmenus] = new SubMenu((char *)name);
    if (!menu[nmenus])
        return (0);
    return (menu[nmenus++]);
}

void Menu::Display(ScreenImp *scr, int row)
{
    int i;

    if (!nmenus)
        return;
    for (i = 0; i < nmenus; ++i)
        menu[i]->Init();
    if (!columnsz)
        column = new int[columnsz = menusz];

    else if (columnsz < menusz)
        column = (int *)realloc(column, (columnsz = menusz) * sizeof(int));
    if (!column) {
        columnsz = 0;
        return;
    }
    column[0] = 1;
    for (i = 1; i < nmenus; ++i)
        column[i] = column[i - 1] + menu[i - 1]->NameLength();
    scr->ClearLine(row, 0, color);
    for (i = 0; i < nmenus; ++i) {
        scr->Put(row, column[i], ' ', color);
        displayName(scr, menu[i]->Name(), color, light);
        scr->Put(' ', color);
    }
}

void Menu::Run(ScreenImp *scr, int row, int key, int subkey)
{
    int i;

    if (!nmenus)
        return;
    Display(scr, row);
    for (i = 0; i < nmenus; ++i)
        if (menu[i]->HotKey() == key) {
            current = i;
            menu[i]->SetKey(subkey);
        }
    for (;;) {
        scr->Put(row, column[current], ' ', inverse);
        displayName(scr, menu[current]->Name(), inverse, lightinverse);
        scr->Put(' ', inverse);
        menu[current]->Run(scr, row + 1, column[current], color, inverse, dim,
                           light, lightinverse);
        int k = scr->GetKey();

        scr->Put(row, column[current], ' ', color);
        displayName(scr, menu[current]->Name(), color, light);
        scr->Put(' ', color);
        switch (k) {
        case cntrl('M'): // Enter
        case cntrl('C'): // ^C
        case cntrl('['): // Esc
        case meta('J'):  // F10 - exit from menu
            return;
        case cntrl('J'): // Line feed - stay in menu
            break;
        default:
            for (i = 0; i < nmenus; ++i)
                if (alt(menu[i]->HotKey()) == k) {
                    current = i;
                    break;
                }
            if (i >= nmenus)
                scr->Beep();
            break;
        case meta('r'): // right
            if (++current >= nmenus)
                current = 0;
            break;
        case meta('l'): // left
            if (--current < 0)
                current = nmenus - 1;
            break;
        }
    }
}

SubMenu::SubMenu(char *n)
{
    name    = n;
    namelen = strlen(name) + 2;
    hotkey  = 0;
    char *p = strchr(name, '~');

    if (p) {
        hotkey = lowercase(p[1]);
        --namelen;
    }
    height  = 2;
    width   = 2;
    current = 0;
    nitems = itemsz = 0;
}

SubMenu::~SubMenu()
{
    if (itemsz)
        delete item;
}

MenuItem *SubMenu::Add(const char *name, MenuFunction callback)
{
    if (!itemsz) {
        itemsz = 8;
        item   = new MenuItem *[itemsz];
        if (!item)
            return (0);
    }
    if (nitems >= itemsz) {
        itemsz += 8;
        item = (MenuItem **)realloc(item, itemsz * sizeof(item[0]));
        if (!item) {
            itemsz = nitems = 0;
            return (0);
        }
    }
    item[nitems] = new MenuItem(name, callback);
    if (!item[nitems])
        return (0);
    return (item[nitems++]);
}

void SubMenu::SetKey(int key)
{
    for (int i = 0; i < nitems; ++i)
        if (item[i]->HotKey() == key)
            current = i;
}

void SubMenu::Init()
{
    height = nitems + 2;
    width  = 0;
    for (int i = 0; i < nitems; ++i)
        if (item[i]->Width() > width)
            width = item[i]->Width();
    width += 2;
}

MenuItem::MenuItem(const char *n, MenuFunction cb)
{
    name     = (char *)n;
    callback = cb;
    isactive = (cb != 0);
    istagged = 0;
    hotkey   = 0;
    width    = 4;
    if (name) {
        width += strlen(name);
        char *p = strchr(name, '~');

        if (p) {
            hotkey = lowercase(p[1]);
            --width;
        }
    }
}

void SubMenu::Run(ScreenImp *scr, int row, int col, int color, int inverse,
                  int dim, int light, int lightinverse)
{
    int i;

    while (current < nitems && !item[current]->IsActive())
        ++current;
    if (current >= nitems) {
        current = 0;
        while (current < nitems && !item[current]->IsActive())
            ++current;
        if (current >= nitems)
            current = 0;
    }
    if (col + width + 1 > scr->Columns - 1)
        col = scr->Columns - width - 2;
    Box box(*scr, row, col, height + 1, width + 1);

    for (;;) {
        Draw(scr, row, col, color, inverse, dim, light, lightinverse);
        scr->HideCursor();
        scr->Sync();
        int k = scr->GetKey();

        switch (k) {
        default:
            if (k > 0xff)
                goto goup;
            for (i = 0; i < nitems; ++i)
                if (item[i]->IsActive() && item[i]->HotKey() == k) {
                    current = i;
                    break;
                }
            if (i >= nitems) {
                scr->Beep();
                continue;
            }
            k = cntrl('M');
            // fall through
        case cntrl('M'): // ^M - execute, exit
        case cntrl('J'): // ^J - execute, stay here
            scr->Put(box);
            item[current]->CallBack();
            scr->UngetKey(k);
            return;
        case cntrl('C'): // ^C - exit
        case cntrl('['): // Esc - exit
        case meta('J'):  // F10 - exit
        case meta('r'):  // right - move
        case meta('l'):  // left - move
        goup:
            scr->Put(box);
            scr->UngetKey(k);
            return;
        case meta('u'): // up
            --current;
            while (current >= 0 && !item[current]->IsActive())
                --current;
            if (current < 0) {
                current = nitems - 1;
                while (current > 0 && !item[current]->IsActive())
                    --current;
            }
            continue;
        case meta('d'): // down
            ++current;
            while (current < nitems && !item[current]->IsActive())
                ++current;
            if (current >= nitems) {
                current = 0;
                while (current < nitems && !item[current]->IsActive())
                    ++current;
                if (current >= nitems)
                    current = 0;
            }
            continue;
        case meta('h'): // home
            current = 0;
            while (current < nitems && !item[current]->IsActive())
                ++current;
            if (current >= nitems)
                current = 0;
            continue;
        case meta('e'): // end
            current = nitems - 1;
            while (current > 0 && !item[current]->IsActive())
                --current;
            continue;
        }
    }
}

void SubMenu::Draw(ScreenImp *scr, int row, int col, int color, int inverse,
                   int dim, int light, int lightinverse)
{
    int i;

    scr->Clear(row, col, height, width, color, ' ');
    scr->DrawFrame(row, col, height, width, color);
    for (i = 0; i < nitems; ++i) {
        scr->Move(row + 1 + i, col + 1);
        item[i]->Display(scr, col + width - 1, i == current, color, inverse,
                         dim, light, lightinverse);
    }

    // Draw shadow.
    for (i = 0; i < width; ++i)
        scr->AttrLow(row + height, col + i + 1);
    for (i = 0; i < height - 1; ++i)
        scr->AttrLow(row + i + 1, col + width);
}

void MenuItem::Display(ScreenImp *scr, int lim, int cur, int color, int inverse,
                       int dim, int light, int lightinverse)
{
    if (cur) {
        scr->Put(!scr->HasInverse() ? '[' : ' ', inverse);
        scr->Put(istagged ? "* " : "  ", inverse);
        displayName(scr, name, inverse, lightinverse);
        while (scr->Col() < lim - 1)
            scr->Put(' ', inverse);
        scr->Put(!scr->HasInverse() ? ']' : ' ', inverse);
    } else if (isactive) {
        scr->Put(istagged ? " * " : "   ", color);
        displayName(scr, name, color, light);
        while (scr->Col() < lim)
            scr->Put(' ', color);
    } else if (name) {
        scr->Put(istagged ? " * " : "   ", dim);
        displayName(scr, name, dim, dim);
        while (scr->Col() < lim)
            scr->Put(' ', dim);
    } else {
        int c = scr->Col();

        scr->HorLine(scr->Row(), c, lim - 1 - c, color);
        scr->Put(' ', color);
    }
}
