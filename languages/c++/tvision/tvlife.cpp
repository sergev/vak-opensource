/*
 * TVision example: the classic life example
 *
 * Written by Sergio Sigala <sergio@sigala.it>
 *
 * The mouse may be used to edit the patterns within the life window.
 * Clicking with the left button inside the life window will cause a dot to
 * appear at the cursor location.  Clicking with the right mouse button will
 * remove the dot.  This allows the user to create his/her own patterns.
 */

#define Uses_TApplication
#define Uses_TButton
#define Uses_TDeskTop
#define Uses_TDialog
#define Uses_TKeys
#define Uses_TMenuBar
#define Uses_TMenuItem
#define Uses_TParamText
#define Uses_TStatusDef
#define Uses_TStatusItem
#define Uses_TStatusLine
#define Uses_TSubMenu
#define Uses_TWindow

#include <stdlib.h>
#include <tvision/tv.h>

enum {
    cmAbout = 101, // about box
    cmCreate,      // creates a new life window
    cmOneStep,     // advance one step
    cmUpdate,      // issued when idle
    cmStartStop,   // starts or stops a life window
    cmClearBoard,  // clears the life board
    cmRandom,      // randomly fills the life board
    cmPat01,
    cmPat02,
    cmPat03,
    cmPat04,
    cmPat05,
    cmPat06,
    cmPat07,
    cmPat08,
    cmPat09,
    cmPat10,
    cmPat11,
    cmPat12,
    cmPat13,
    cmPat14,
    cmPat15,
    cmPat16,
    cmPat17,
    cmPat18,
    cmPat19,
    cmPat20,
    cmPat21,
    cmPat22,
    cmPat23,
    cmPat24,
    cmPat25,
    cmPat26,
    cmPat27,
    cmPat28,
    cmPat29
};

class TLifeInterior : public TView {
    int running;
    uchar *board;

public:
    TLifeInterior(TRect &bounds);
    ~TLifeInterior();
    void changeBounds(const TRect &bounds);
    void clearBoard();
    void draw();
    void getPattern(int pat);
    void handleEvent(TEvent &event);
    void handleMouse(TEvent &event);
    void iterateBoard();
    uchar &map(int x, int y);
    uchar &map(int x, int y, uchar *work);
    int present(int x, int y);
    void randomizeBoard();
    void setState(ushort aState, Boolean enable);
};

class TLifeWindow : public TWindow {
    TLifeInterior *life;

public:
    static const int minW = 28;
    static const int minH = 11;
    TLifeWindow(TRect &bounds, char *str, int num);
    void sizeLimits(TPoint &min, TPoint &max);
};

class TMyApp : public TApplication {
    TCommandSet windowCommands;

public:
    TMyApp();
    static TMenuBar *initMenuBar(TRect r);
    static TStatusLine *initStatusLine(TRect r);
    void aboutBox();
    void createLifeWindow();
    TCommandSet getCommands();
    void handleEvent(TEvent &event);
    void idle();
};

/****************************************************************************
 *
 * Patterns for tvlife. Adapted from xlock; original copyright notice below.
 *
 * Copyright (c) 1988-91 by Patrick J. Naughton.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation.
 *
 * This file is provided AS IS with no warranties of any kind.	The author
 * shall have no liability with respect to the infringement of copyrights,
 * trade secrets or any patents by this file or any part thereof.  In no
 * event will the author be liable for any lost revenue or profits or
 * other special, indirect and consequential damages.
 *****************************************************************************/

#define NUMPTS 63

/* Patterns have < NUMPTS pts (and should have a size of <= 32x32,
   the Gun is an exception) */

static int patterns[][2 * NUMPTS + 1] = {
    // clang-format off
    { /* GLIDER GUN */
        6, -4,
        5, -3, 6, -3,
        -6, -2, -5, -2, 8, -2, 9, -2, 16, -2,
        -7, -1, 8, -1, 9, -1, 10, -1, 16, -1, 17, -1,
        -18, 0, -17, 0, -8, 0, 8, 0, 9, 1,
        -17, 1, -8, 1, 5, 1, 6, 1,
        -8, 2, 6, 2,
        -7, 3,
        -6, 4, -5, 4,
        127
    },
    { /* FIGURE EIGHT */
        -3, -3, -2, -3, -1, -3,
        -3, -2, -2, -2, -1, -2,
        -3, -1, -2, -1, -1, -1,
        0, 0, 1, 0, 2, 0,
        0, 1, 1, 1, 2, 1,
        0, 2, 1, 2, 2, 2,
        127
    },
    { /* PULSAR */
        -2, -1, -1, -1, 0, -1, 1, -1, 2, -1,
        -2, 0, 2, 0,
        127
    },
    { /* BARBER POLE P2 */
        -6, -6, -5, -6,
        -6, -5, -4, -5,
        -4, -3, -2, -3,
        -2, -1, 0, -1,
        0, 1, 2, 1,
        2, 3, 4, 3,
        5, 4,
        4, 5, 5, 5,
        127
    },
    { /* ACHIM P5 */
        -6, -6, -5, -6,
        -6, -5,
        -4, -4,
        -4, -3, -2, -3,
        -2, -1, 0, -1,
        0, 1, 2, 1,
        2, 3, 3, 3,
        5, 4,
        4, 5, 5, 5,
        127
    },
    { /* HERTZ P4 */
        -2, -5, -1, -5,
        -2, -4, -1, -4,
        -7, -2, -6, -2, -2, -2, -1, -2, 0, -2, 1, -2, 5, -2, 6, -2,
        -7, -1, -5, -1, -3, -1, 2, -1, 4, -1, 6, -1,
        -5, 0, -3, 0, -2, 0, 2, 0, 4, 0,
        -7, 1, -5, 1, -3, 1, 2, 1, 4, 1, 6, 1,
        -7, 2, -6, 2, -2, 2, -1, 2, 0, 2, 1, 2, 5, 2, 6, 2,
        -2, 4, -1, 4,
        -2, 5, -1, 5,
        127
    },
    { /* TUMBLER */
        -2, -3, -1, -3, 1, -3, 2, -3,
        -2, -2, -1, -2, 1, -2, 2, -2,
        -1, -1, 1, -1,
        -3, 0, -1, 0, 1, 0, 3, 0,
        -3, 1, -1, 1, 1, 1, 3, 1,
        -3, 2, -2, 2, 2, 2, 3, 2,
        127
    },
    { /* PULSE1 P4 */
        0, -3, 1, -3,
        -2, -2, 0, -2,
        -3, -1, 3, -1,
        -2, 0, 2, 0, 3, 0,
        0, 2, 2, 2,
        1, 3,
        127
    },
    { /* SHINING FLOWER P5 */
        -1, -4, 0, -4,
        -2, -3, 1, -3,
        -3, -2, 2, -2,
        -4, -1, 3, -1,
        -4, 0, 3, 0,
        -3, 1, 2, 1,
        -2, 2, 1, 2,
        -1, 3, 0, 3,
        127
    },
    { /* PULSE2 P6 */
        0, -4, 1, -4,
        -4, -3, -3, -3, -1, -3,
        -4, -2, -3, -2, 0, -2, 3, -2,
        1, -1, 3, -1,
        2, 0,
        1, 2, 2, 2,
        1, 3, 2, 3,
        127
    },
    { /* PINWHEEL, CLOCK P4 */
        -2, -6, -1, -6,
        -2, -5, -1, -5,
        -2, -3, -1, -3, 0, -3, 1, -3,
        -3, -2, -1, -2, 2, -2, 4, -2, 5, -2,
        -3, -1, 1, -1, 2, -1, 4, -1, 5, -1,
        -6, 0, -5, 0, -3, 0, 0, 0, 2, 0,
        -6, 1, -5, 1, -3, 1, 2, 1,
        -2, 2, -1, 2, 0, 2, 1, 2,
        0, 4, 1, 4,
        0, 5, 1, 5,
        127
    },
    { /* PENTADECATHOLON */
        -5, 0, -4, 0, -3, 0, -2, 0, -1, 0, 0, 0, 1, 0, 2, 0, 3, 0, 4, 0,
        127
    },
    { /* PISTON */
        1, -3, 2, -3,
        0, -2,
        -10, -1, -1, -1,
        -11, 0, -10, 0, -1, 0,        9, 0, 10, 0,
        -1, 1, 9, 1,
        0, 2,
        1, 3, 2, 3,
        127
    },
    { /* PISTON2 */
       -3, -5,
       -14, -4, -13, -4, -4, -4, -3, -4, 13, -4, 14, -4,
       -14, -3, -13, -3, -5, -3, -4, -3, 13, -3, 14, -3,
       -4, -2, -3, -2, 0, -2, 1, -2,
       -4,  2, -3, 2, 0, 2, 1, 2,
       -14, 3, -13, 3, -5, 3, -4, 3, 13, 3, 14, 3,
       -14, 4, -13, 4, -4, 4, -3, 4, 13, 4, 14, 4,
       -3, 5,
        127
    },
    { /* SWITCH ENGINE */
        -12, -3, -10, -3,
        -13, -2,
        -12, -1, -9, -1,
        -10, 0, -9,  0, -8,  0,
        13, 2, 14,  2,
        13, 3,
        127
    },
    { /* GEARS (gear, flywheel, blinker) */
        -1, -4,
        -1, -3, 1, -3,
        -3, -2,
        2, -1, 3, -1,
        -4, 0, -3, 0,
        2, 1,
        -2, 2, 0, 2,
        0, 3,

        5, 3,
        3, 4, 4, 4,
        5, 5, 6, 5,
        4, 6,

        8, 0,
        8, 1,
        8, 2,
        127
    },
    { /* TURBINE8 */
        -4, -4, -3, -4, -2, -4, -1, -4, 0, -4, 1, -4, 3, -4, 4, -4,
        -4, -3, -3, -3, -2, -3, -1, -3, 0, -3, 1, -3, 3, -3, 4, -3,
        3, -2, 4, -2,
        -4, -1, -3, -1, 3, -1, 4, -1,
        -4, 0, -3, 0, 3, 0, 4, 0,
        -4, 1, -3, 1, 3, 1, 4, 1,
        -4, 2, -3, 2,
        -4, 3, -3, 3, -1, 3, 0, 3, 1, 3, 2, 3, 3, 3, 4, 3,
        -4, 4, -3, 4, -1, 4, 0, 4, 1, 4, 2, 4, 3, 4, 4, 4,
        127
    },
    { /* P16 */
        -3, -6, 1, -6, 2, -6,
        -3, -5, 0, -5, 3, -5,
        3, -4,
        -5, -3, -4, -3, 1, -3, 2, -3, 5, -3, 6, -3,
        -6, -2, -3, -2,
        -6, -1, -3, -1,
        -5, 0, 5, 0,
        3, 1, 6, 1,
        3, 2, 6, 2,
        -6, 3, -5, 3, -2, 3, -1, 3, 4, 3, 5, 3,
        -3, 4,
        -3, 5, 0, 5, 3, 5,
        -2, 6, -1, 6, 3, 6,
        127
    },
    { /* PUFFER */
        1, -9,
        2, -8,
        -2, -7, 2, -7,
        -1, -6, 0, -6, 1, -6, 2, -6,
        -2, -2,
        -1, -1, 0, -1,
        0, 0,
        0, 1,
        -1, 2,
        1, 5,
        2, 6,
        -2, 7, 2, 7,
        -1, 8, 0, 8, 1, 8, 2, 8,
        127
    },
    { /* ESCORT */
        3, -8,
        4, -7,
        -2, -6, 4, -6,
        -1, -5, 0, -5, 1, -5, 2, -5, 3, -5, 4, -5,
        -5, -1, -4, -1, -3, -1, -2, -1, -1, -1, 0, -1,
        1, -1, 2, -1, 3, -1, 4, -1, 5, -1, 6, -1,
        -6, 0, 6, 0,
        6, 1,
        5, 2,
        3, 4,
        4, 5,
        -2, 6, 4, 6,
        -1, 7, 0, 7, 1, 7, 2, 7, 3, 7, 4, 7,
        127
    },
    { /* DART SPEED 1/3 */
        3, -7,
        2, -6, 4, -6,
        1, -5, 2, -5,
        4, -4,
        0, -3, 4, -3,
        -3, -2, 0, -2,
        -4, -1, -2, -1, 1, -1, 2, -1, 3, -1, 4, -1,
        -5, 0, -2, 0,
        -4, 1, -2, 1, 1, 1, 2, 1, 3, 1, 4, 1,
        -3, 2, 0, 2,
        0, 3, 4, 3,
        4, 4,
        1, 5, 2, 5,
        2, 6, 4, 6,
        3, 7,
        127
    },
    { /* PERIOD 4 SPEED 1/2 */
        -3, -5,
        -4, -4, -3, -4, -2, -4, -1, -4, 0, -4,
        -5, -3, -4, -3, 0, -3, 1, -3, 3, -3,
        -4, -2, 4, -2,
        -3, -1, -2, -1, 1, -1, 3, -1,
        -3, 1, -2, 1, 1, 1, 3, 1,
        -4, 2, 4, 2,
        -5, 3, -4, 3, 0, 3, 1, 3, 3, 3,
        -4, 4, -3, 4, -2, 4, -1, 4, 0, 4,
        -3, 5,
        127
    },
    { /* ANOTHER PERIOD 4 SPEED 1/2 */
        -4, -7, -3, -7, -1, -7, 0, -7, 1, -7, 2, -7, 3, -7, 4, -7,
        -5, -6, -4, -6, -3, -6, -2, -6, 5, -6,
        -6, -5, -5, -5,
        -5, -4, 5, -4,
        -4, -3, -3, -3, -2, -3, 0, -3,
        -2, -2,
        -2, -1,
        -1, 0,
        -2, 1,
        -2, 2,
        -4, 3, -3, 3, -2, 3, 0, 3,
        -5, 4, 5, 4,
        -6, 5, -5, 5,
        -5, 6, -4, 6, -3, 6, -2, 6, 5, 6,
        -4, 7, -3, 7, -1, 7, 0, 7, 1, 7, 2, 7, 3, 7, 4, 7,
        127
    },
    { /* SMALLEST KNOWN PERIOD 3 SPACESHIP SPEED 1/3 */
        0, -8,
        -1, -7, 1, -7,
        -1, -6, 1, -6,
        -1, -5,
        -2, -3, -1, -3,
        -1, -2, 1, -2,
        -2, -1, 0, -1,
        -2, 0, -1, 0, 0, 0,
        -1, 2, 1, 2,
        -1, 3, 0, 3,
        0, 4,
        0, 5, 2, 5,
        0, 6, 2, 6,
        1, 7,
        127
    },
    { /* TURTLE SPEED 1/3 */
        -4, -5, -3, -5, -2, -5, 6, -5,
        -4, -4, -3, -4, 0, -4, 2, -4, 3, -4, 5, -4, 6, -4,
        -2, -3, -1, -3, 0, -3, 5, -3,
        -4, -2, -1, -2, 1, -2, 5, -2,
        -5, -1, 0, -1, 5, -1,
        -5, 0, 0, 0, 5, 0,
        -4, 1, -1, 1, 1, 1, 5, 1,
        -2, 2, -1, 2, 0, 2, 5, 2,
        -4, 3, -3, 3, 0, 3, 2, 3, 3, 3, 5, 3, 6, 3,
        -4, 4, -3, 4, -2, 4, 6, 4,
        127
    },
    { /* SMALLEST KNOWN PERIOD 5 SPEED 2/5 */
        1, -7, 3, -7,
        -2, -6, 3, -6,
        -3, -5, -2, -5, -1, -5, 4, -5,
        -4, -4, -2, -4,
        -5, -3, -4, -3, -1, -3, 0, -3, 5, -3,
        -4, -2, -3, -2, 0, -2, 1, -2, 2, -2, 3, -2, 4, -2,
        -4, 2, -3, 2, 0, 2, 1, 2, 2, 2, 3, 2, 4, 2,
        -5, 3, -4, 3, -1, 3, 0, 3, 5, 3,
        -4, 4, -2, 4,
        -3, 5, -2, 5, -1, 5, 4, 5,
        -2, 6, 3, 6,
        1, 7, 3, 7,
        127
    },
    { /* SYM PUFFER */
        1, -4, 2, -4, 3, -4, 4, -4,
        0, -3, 4, -3,
        4, -2,
        -4, -1, -3, -1, 0, -1, 3, -1,
        -4, 0, -3, 0, -2, 0,
        -4, 1, -3, 1, 0, 1, 3, 1,
        4, 2,
        0, 3, 4, 3,
        1, 4, 2, 4, 3, 4, 4, 4,
        127
    },
    { /* ], NEAR SHIP, PI HEPTOMINO */
        -2, -1, -1, -1, 0, -1,
        1, 0,
        -2, 1, -1, 1, 0, 1,
        127
    },
    { /* R PENTOMINO */
        0, -1, 1, -1,
        -1, 0, 0, 0,
        0, 1,
        127
    }
    // clang-format on
};

#define NPATS (sizeof patterns / sizeof patterns[0])

TLifeInterior::TLifeInterior(TRect &bounds) : TView(bounds), running(0), board(0)
{
    eventMask = evMouseDown | evKeyDown | evCommand | evBroadcast;
    growMode  = gfGrowHiX | gfGrowHiY;
    options |= ofSelectable;

    board = new uchar[size.x * size.y]; // get memory
    getPattern(0);
}

TLifeInterior::~TLifeInterior()
{
    if (board != 0)
        delete[] board; // release memory
}

void TLifeInterior::changeBounds(const TRect &bounds)
{
    if (board == 0)
        return; // exit if no board

    TPoint oldSize = size; // calculate sizes
    TPoint newSize = bounds.b - bounds.a;

    uchar *work = new uchar[newSize.x * newSize.y]; // get memory
    if (work == 0)
        return;

    // clear board

    for (int i = 0; i < newSize.x * newSize.y; i++)
        work[i] = 0;

    // copy

    for (int y = 0; y < min(oldSize.y, newSize.y); y++)
        for (int x = 0; x < min(oldSize.x, newSize.x); x++) {
            uchar *from = &board[oldSize.x * y + x];
            uchar *to   = &work[newSize.x * y + x];

            *to = *from; // copy a cell
        }
    uchar *old = board; // swap buffers
    board      = work;
    delete[] old;

    TView::changeBounds(bounds); // update size and redraw
}

void TLifeInterior::clearBoard()
{
    if (board == 0)
        return; // exit if no board

    // clear board

    for (int i = 0; i < size.x * size.y; i++)
        board[i] = 0;
}

void TLifeInterior::draw()
{
    uchar color = running ? 0x4f : 0x1f;
    uchar *from = board;

#if 1
    int i = size.x * size.y;
    ushort buf[size.x * size.y];
    ushort *to = buf;

    while (i-- > 0) {
        if (*from++ != 0)
            *to = (color << 8) | '*';
        else
            *to = (color << 8) | ' ';
        to++;
    }
    writeBuf(0, 0, size.x, size.y, buf);
#else
    TDrawBuffer b;
    b.moveChar(0, ' ', color, size.x);

    for (int i = 0; i < size.y; i++) {
        TDrawBuffer d = b;

        if (from != 0) {
            for (int j = 0; j < size.x; j++) {
                if (*from != 0)
                    d.putChar(j, '*');
                from++;
            }
        }
        writeLine(0, i, size.x, 1, d);
    }
#endif
}

void TLifeInterior::getPattern(int pat)
{
    if (board == 0)
        return; // exit if no board
    clearBoard();

    int i       = pat % NPATS;
    int *patptr = &patterns[i][0];
    int col;

    while ((col = *patptr++) != 127) {
        int row = *patptr++;

        col += size.x / 2;
        row += size.y / 2;
        map(col, row) = 1;
    }
    drawView();
}

void TLifeInterior::handleEvent(TEvent &event)
{
    TView::handleEvent(event);
    switch (event.what) {
    case evBroadcast:
        if (event.message.command == cmUpdate && running) {
            iterateBoard();
            drawView();
        }
        break;
    case evCommand:
        if (event.message.command >= cmPat01 && event.message.command < cmPat01 + NPATS) {
            getPattern(event.message.command - cmPat01);
        } else
            switch (event.message.command) {
            case cmOneStep:
                iterateBoard();
                drawView();
                break;
            case cmClearBoard:
                clearBoard();
                drawView();
                break;
            case cmRandom:
                randomizeBoard();
                drawView();
                break;
            case cmStartStop:
                running = 1 - running;
                drawView();
                break;
            default:
                return;
            }
        clearEvent(event);
        break;
    case evMouseDown:
        handleMouse(event);
    }
}

void TLifeInterior::handleMouse(TEvent &event)
{
    TRect clickRect = getExtent();
    do {
        TPoint mouse = makeLocal(event.mouse.where);
        if (clickRect.contains(mouse)) {
            if ((event.mouse.buttons & mbLeftButton) != 0)
                map(mouse.x, mouse.y) = 1;
            if ((event.mouse.buttons & mbRightButton) != 0)
                map(mouse.x, mouse.y) = 0;
            drawView();
        }
    } while (mouseEvent(event, evMouseMove));
    clearEvent(event);
}

void TLifeInterior::iterateBoard()
{
    if (board == 0)
        return; // exit if no board

    uchar *work = new uchar[size.x * size.y]; // get memory
    if (work == 0)
        return;

    // clear board

    for (int i = 0; i < size.x * size.y; i++)
        work[i] = 0;

    int differences = 0;

    // iterate

    for (int y = 0; y < size.y; y++)
        for (int x = 0; x < size.x; x++) {
            // find number of neighbors

            int n = present(x - 1, y - 1) + present(x, y - 1) + present(x + 1, y - 1) +
                    present(x - 1, y) + present(x + 1, y) + present(x - 1, y + 1) +
                    present(x, y + 1) + present(x + 1, y + 1);

            if (map(x, y) == 1) {
                if (n == 2 || n == 3)
                    map(x, y, work) = 1;
            }
            if (map(x, y) == 0) {
                if (n == 3)
                    map(x, y, work) = 1;
            }
            if (map(x, y) != map(x, y, work))
                differences++;
        }
    uchar *old = board; // swap buffers
    board      = work;
    delete[] old;

    if (differences == 0)
        running = 0;
}

uchar &TLifeInterior::map(int x, int y)
{
    return board[size.x * y + x];
}

uchar &TLifeInterior::map(int x, int y, uchar *work)
{
    return work[size.x * y + x];
}

int TLifeInterior::present(int x, int y)
{
    // exit if the position is outside the rectangle

    if (x < 0 || x >= size.x || y < 0 || y >= size.y)
        return 0;
    if (board[size.x * y + x] == 0)
        return 0;
    return 1;
}

inline int range(int test, int min, int max)
{
    return test < min ? min : test > max ? max : test;
}

void TLifeInterior::randomizeBoard()
{
    if (board == 0)
        return; // exit if no board
    clearBoard();

    // insert random cells

    int howMany = size.x * size.y / 5;
    for (int i = 0; i < howMany; i++) {
        int x, y;

        do {
            x = (int)(((double)rand() * size.x) / RAND_MAX);
            y = (int)(((double)rand() * size.y) / RAND_MAX);

            x = range(0, x, size.x - 1);
            y = range(0, y, size.y - 1);
        } while (map(x, y) != 0);
        map(x, y) = 1;
    }
}

void TLifeInterior::setState(ushort aState, Boolean enable)
{
    TView::setState(aState, enable);
    /*	TCommandSet windowCommands;
            if ((aState & sfFocused) != 0)
            {
                    if (running) windowCommands += cmStop;
                    else windowCommands += cmStart;
                    if (enable != False) enableCommands(windowCommands);
                    else disableCommands(windowCommands);
            }*/
}

TLifeWindow::TLifeWindow(TRect &bounds, char *str, int num)
    : TWindowInit(&TLifeWindow::initFrame), TWindow(bounds, str, num)
{
    options |= ofFirstClick | ofTileable;

    TRect r = getClipRect();
    r.grow(-1, -1);
    insert(life = new TLifeInterior(r));
}

void TLifeWindow::sizeLimits(TPoint &min, TPoint &max)
{
    TView::sizeLimits(min, max);
    min.x = minW;
    min.y = minH;
}

TMyApp::TMyApp()
    : TProgInit(&TMyApp::initStatusLine, &TMyApp::initMenuBar, &TMyApp::initDeskTop),
      windowCommands(getCommands())
{
}

void TMyApp::aboutBox()
{
    TDialog *box = new TDialog(TRect(0, 0, 32, 10), "About");
    box->insert(new TStaticText(TRect(1, 2, 1 + 30, 2 + 5),
                                "\003TVLIFE\n"
                                "\003The classic life example\n\n"
                                "\003Written by Sergio Sigala"));
    box->insert(new TButton(TRect(11, 7, 11 + 10, 9), "O~K~", cmOK, bfDefault));
    box->options |= ofCentered;
    executeDialog(box);
}

void TMyApp::createLifeWindow()
{
    //	TRect r(0, 0, TLifeWindow::minW, TLifeWindow::minH);
    //	r.move(random() % 53, random() % 16); //randomly move around screen
    TRect r;
    r = deskTop->getExtent();
    r.grow(-1, -1);

    char title[] = "Life";
    TView *w     = validView(new TLifeWindow(r, title, 0));
    if (w != 0)
        deskTop->insert(w);
}

TCommandSet TMyApp::getCommands()
{
    TCommandSet wc;

    wc += cmCascade; // add window commands
    wc += cmClearBoard;
    wc += cmOneStep;
    wc += cmRandom;
    wc += cmStartStop;
    wc += cmTile;
    for (int i = 0; i < NPATS; i++) // scan pattern commands
    {
        wc += cmPat01 + i; // add pattern commands
    }
    return wc;
}

void TMyApp::handleEvent(TEvent &event)
{
    TApplication::handleEvent(event);
    if (event.what == evCommand) {
        switch (event.message.command) {
        case cmAbout:
            aboutBox();
            break;
        case cmCascade:
            deskTop->cascade(deskTop->getExtent());
            break;
        case cmCreate:
            createLifeWindow();
            break;
        case cmTile:
            deskTop->tile(deskTop->getExtent());
            break;
        default:
            return;
        }
        clearEvent(event);
    }
}

static Boolean isTileable(TView *p, void *)
{
    if ((p->options & ofTileable) != 0)
        return True;
    else
        return False;
}

void TMyApp::idle()
{
    TApplication::idle();
    if (deskTop->firstThat(isTileable, 0) != 0)
        enableCommands(windowCommands);
    else
        disableCommands(windowCommands);
    message(deskTop, evBroadcast, cmUpdate, 0);
}

#define ITEM(name, comm) *new TMenuItem(name, comm, kbNoKey, hcNoContext)

TMenuBar *TMyApp::initMenuBar(TRect r)
{
    TSubMenu &sub1 = *new TSubMenu("~\360~", 0, hcNoContext) +
                     *new TMenuItem("~A~bout...", cmAbout, kbNoKey, hcNoContext);

    TSubMenu &sub2 = *new TSubMenu("~F~ile", 0) +
                     *new TMenuItem("~L~ife window", cmCreate, kbF9, hcNoContext, "F9") +
                     newLine() + *new TMenuItem("E~x~it", cmQuit, kbAltX, hcNoContext, "Alt-X");

    TSubMenu &sub3 = *new TSubMenu("~A~ction", 0) +
                     *new TMenuItem("~C~lear", cmClearBoard, kbNoKey, hcNoContext) +
                     *new TMenuItem("~O~ne step", cmOneStep, kbNoKey, hcNoContext) +
                     *new TMenuItem("~R~andomize", cmRandom, kbNoKey, hcNoContext) +
                     *new TMenuItem("~S~tart/Stop", cmStartStop, kbNoKey, hcNoContext);

    TSubMenu &sub4 = *new TSubMenu("~W~indow", 0) +
                     *new TMenuItem("~S~ize/move", cmResize, kbCtrlF5, hcNoContext, "Ctrl-F5") +
                     *new TMenuItem("~Z~oom", cmZoom, kbF5, hcNoContext, "F5") +
                     *new TMenuItem("~N~ext", cmNext, kbF6, hcNoContext, "F6") +
                     *new TMenuItem("~P~revious", cmPrev, kbShiftF6, hcNoContext, "Shift-F6") +
                     *new TMenuItem("~C~lose", cmClose, kbAltF3, hcNoContext, "Alt-F3") +
                     newLine() + *new TMenuItem("~T~ile", cmTile, kbNoKey, hcNoContext) +
                     *new TMenuItem("C~a~scade", cmCascade, kbNoKey, hcNoContext);

    TSubMenu &sub5 = *new TSubMenu("Patterns ~1~", 0) + ITEM("Glider Gun", cmPat01) +
                     ITEM("Figure Eight", cmPat02) + ITEM("Pulsar", cmPat03) +
                     ITEM("Barber Pole P2", cmPat04) + ITEM("Achim P5", cmPat05) +
                     ITEM("Hertz P4", cmPat06) + ITEM("Tumbler", cmPat07) +
                     ITEM("Pulse1 P4", cmPat08) + ITEM("Shining Flower P5", cmPat09) +
                     ITEM("Pulse2 P6", cmPat10) + ITEM("Pinwheel, Clock P4", cmPat11) +
                     ITEM("Pentadecatholon", cmPat12) + ITEM("Piston", cmPat13) +
                     ITEM("Piston2", cmPat14) + ITEM("Switch Engine", cmPat15);

    TSubMenu &sub6 =
        *new TSubMenu("Patterns ~2~", 0) + ITEM("Gears (Gear, Flywheel, Blinker)", cmPat16) +
        ITEM("Turbine8", cmPat17) + ITEM("P16", cmPat18) + ITEM("Puffer", cmPat19) +
        ITEM("Escort", cmPat20) + ITEM("Dart Speed 1/3", cmPat21) +
        ITEM("Period 4 Speed 1/2", cmPat22) + ITEM("Another Period 4 Speed 1/2", cmPat23) +
        ITEM("Smallest Known Period 3 Spaceship Speed 1/3", cmPat24) +
        ITEM("Turtle Speed 1/3", cmPat25) + ITEM("Smallest Known Period 5 Speed 2/5", cmPat26) +
        ITEM("Sym Puffer", cmPat27) + ITEM("], Near Ship, Pi Heptomino", cmPat28) +
        ITEM("R Pentomino", cmPat29);

    r.b.y = r.a.y + 1;
    return new TMenuBar(r, sub1 + sub2 + sub3 + sub4 + sub5 + sub6);
}

TStatusLine *TMyApp::initStatusLine(TRect r)
{
    r.a.y = r.b.y - 1;
    return new TStatusLine(
        r, *new TStatusDef(0, 50) + *new TStatusItem("~Alt-X~ Exit", kbAltX, cmQuit) +
               *new TStatusItem("~F9~ Life Window", kbF9, cmCreate) +
               *new TStatusItem("~Alt-F3~ Close", kbAltF3, cmClose) +
               *new TStatusItem("One step", kbNoKey, cmOneStep) +
               *new TStatusItem("Randomize", kbNoKey, cmRandom) +
               *new TStatusItem("Start/Stop", kbNoKey, cmStartStop) +
               *new TStatusItem(0, kbF10, cmMenu) + *new TStatusItem(0, kbF5, cmZoom) +
               *new TStatusItem(0, kbCtrlF5, cmResize));
}

int main()
{
    TMyApp a;

    a.run();
    return 0;
}
