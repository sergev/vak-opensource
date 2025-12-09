//
// Menu subpackage declarations.
//
// Copyright (C) 1994-2001 Cronyx Engineering Ltd.
// Author: Serge Vakulenko, <vak@zebub.msk.su>
//
// This software is distributed with NO WARRANTIES, not even the implied
// warranties for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// Authors grant any other persons or organisations permission to use
// or modify this software as long as this message is kept with the software,
// all derivative works or modified versions.
//
// $Id: menu.h,v 1.3 2006/04/19 17:26:54 ly Exp $
//

typedef void (*MenuFunction)();
class ScreenImp;

class MenuItem {
private:
    char *name;            // name of submenu
    int width;             // length of name
    int hotkey;            // return key
    int isactive;          // is it active for now
    int istagged;          // tag of submenu
    MenuFunction callback; // function to execute
public:
    MenuItem(const char *name = 0, MenuFunction callback = 0);

    void Display(ScreenImp *scr, int lim, int cur, int c, int i, int d, int l,
                 int li);
    void Activate(int yes = 1) { isactive = yes; }
    void Deactivate() { Activate(0); }
    void Tag(int yes = 1) { istagged = yes; }
    void Untag() { Tag(0); }
    int HotKey() { return hotkey; }
    int Width() { return width; }
    int IsActive() { return isactive; }
    void CallBack()
    {
        if (callback)
            callback();
    }
};

class SubMenu {
private:
    char *name;      // name of menu
    int namelen;     // length of name
    int hotkey;      // return key
    int height;      // height of submenu window
    int width;       // width of submenu window
    int current;     // current submenu
    MenuItem **item; // array of submenus
    int nitems;
    int itemsz;

public:
    SubMenu(char *name);
    ~SubMenu();
    MenuItem *Add(const char *name = 0, MenuFunction callback = 0);
    void Init();
    void SetKey(int key);
    void Draw(ScreenImp *scr, int row, int col, int c, int i, int d, int l,
              int li);
    void Run(ScreenImp *scr, int row, int col, int c, int i, int d, int l,
             int li);
    char *Name() { return name; }
    int NameLength() { return namelen; }
    int HotKey() { return hotkey; }
    int Current() { return current; }
};

class Menu {
private:
    int nmenus;     // number of submenus
    int menusz;     // allocated size of submenu array
    int current;    // current submenu
    SubMenu **menu; // array of submenus
    int *column;    // base columns of submenus
    int columnsz;   // allocated size of column array
    int color;
    int light;
    int inverse;
    int lightinverse;
    int dim;

public:
    Menu();
    Menu(int c, int l, int d, int i, int li);
    ~Menu();
    void Palette(int c, int l, int i, int li, int di);
    SubMenu *Add(const char *name);
    void Display(ScreenImp *scr, int row);
    void Run(ScreenImp *scr, int row, int key = -1, int subkey = -1);
    int Current(int *j = 0)
    {
        if (j)
            *j = menu[current]->Current();
        return (current);
    }
};
