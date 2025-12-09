//
// Popup windows declarations.
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
// $Id: popup.h,v 1.5 2006/04/19 17:26:54 ly Exp $
//

class ScreenImp;
class Box;

class Flash {
private:
    Box *box;
    ScreenImp *scr;
    void InitFlash(ScreenImp *s, const char *head, const char *mesg,
                   const char *msg2, int color);

public:
    // Flash (ScreenImp *s, const char *head, const char *mesg, int color, const
    // char *mesg2 = 0);
    Flash(ScreenImp *s, const char *head, const char *mesg, const char *mesg2,
          int color)
    {
        InitFlash(s, head, mesg, mesg2, color);
    }
    Flash(ScreenImp *s, const char *head, const char *mesg, int color)
    {
        InitFlash(s, head, mesg, 0, color);
    }
    ~Flash();
};
