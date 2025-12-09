/*
 * $Id: dialog.h,v 1.5 2008/04/18 12:57:55 ly Exp $
 */

static const int DialogHOME   = -8;
static const int DialogEND    = -7;
static const int DialogRIGHT  = -6;
static const int DialogLEFT   = -5;
static const int DialogUP     = -4;
static const int DialogDOWN   = -3;
static const int DialogPREV   = -2;
static const int DialogNEXT   = -1;
static const int DialogCANCEL = 0;
static const int DialogOK     = 9999;

class Screen;

class DialogElement {
public:
    int r, c;
    int w, h;
    int oblig;
    int enabled;
    static int baserow, basecol;
    DialogElement *next, *prev;
    static DialogElement *list;
    static unsigned char *palette;
    static Screen *scr;

    DialogElement(int row, int col, int reg = 0);
    virtual ~DialogElement() {}
    virtual void Draw() = 0;
    virtual void Redraw() {}
    virtual int Run() { return DialogNEXT; }
    DialogElement *Find(int dir);
    DialogElement *Next() { return next == list ? 0 : next; }
    static DialogElement *Home();
    static DialogElement *End();
    static char *GetString(const char *&p);
    static long GetNumber(const char *&p);
    static long GetHexNumber(const char *&p);
};

class DialogGroup : public DialogElement {
    DialogElement **tab;
    int nv;
    int maxnv;

public:
    int frameflag;

    DialogGroup(int row, int col, const char *&fmt, va_list &args,
                int flags = 0, DialogGroup *prnt = 0);
    ~DialogGroup();
    void Add(DialogElement *v);
    void Center(int wid = 0);
    void Draw();
};

class DialogLabel : public DialogElement {
    char *label;

public:
    DialogLabel(int row, int col, const char *&p);
    ~DialogLabel()
    {
        if (label)
            delete label;
    }
    void Draw();
};

class DialogLine : public DialogElement {
    DialogGroup *parent;

public:
    DialogLine(int row, int col, DialogGroup *p) : DialogElement(row, col)
    {
        h      = 1;
        parent = p;
    }
    void Draw();
};

class DialogButton : public DialogElement {
    char *label;
    int *val;

public:
    DialogButton(int row, int col, const char *&p, int *v);
    ~DialogButton()
    {
        if (label)
            delete label;
    }
    void Draw();
    int Run();
};

class DialogRadioButton : public DialogElement {
    int *val;
    int setval;
    char *label;

public:
    DialogRadioButton(int row, int col, const char *&p, int *v);
    ~DialogRadioButton()
    {
        if (label)
            delete label;
    }
    void Draw();
    void Redraw();
    int Run();
};

class DialogCompactRadioButton : public DialogElement {
    int *val;
    int setval;

public:
    DialogCompactRadioButton(int row, int col, const char *&p, int *v);
    void Draw();
    void Redraw();
    int Run();
};

typedef int (*DialogListFunc)(Screen *scr, int y, int x, int w, long *v, char *str);

class DialogList : public DialogElement {
    char str[80];
    long *val;
    int maxw;
    DialogListFunc func;

public:
    DialogList(int row, int col, const char *&p, long *v, DialogListFunc);
    void Draw();
    int Run();
};

class DialogString : public DialogElement {
    char *val;
    int maxlen;

public:
    DialogString(int row, int col, const char *&p, char *v);
    void Draw();
    int Run();
};

class DialogTitle : public DialogElement {
    char *val;

public:
    DialogTitle(int row, int col, const char *&p, char *v);
    void Draw();
};

class DialogPassword : public DialogElement {
    char *val;
    int maxlen;

public:
    DialogPassword(int row, int col, const char *&p, char *v);
    void Draw();
    int Run();
};

class DialogNumber : public DialogElement {
    long *val;
    long min, max, step;

public:
    DialogNumber(int row, int col, const char *&p, long *v);
    void Draw();
    int Run();
};

class DialogHexNumber : public DialogElement {
    long *val;
    long min, max, step;

public:
    DialogHexNumber(int row, int col, const char *&p, long *v);
    void Draw();
    int Run();
};

class DialogMask : public DialogElement {
    long *val;

public:
    DialogMask(int row, int col, const char *&p, long *v);
    void Draw();
    int Run();
};

class DialogCommand : public DialogElement {
    char *label;
    int val;

public:
    DialogCommand(int row, int col, const char *&p);
    ~DialogCommand()
    {
        if (label)
            delete label;
    }
    void Draw();
    int Run();
};

class Dialog {
    const char *title;
    unsigned char *palette;
    int r, c;
    DialogGroup *grp;
    DialogElement *list;

public:
    Dialog(const char *t, unsigned char *pal, const char *fmt, ...);
    ~Dialog()
    {
        if (grp)
            delete grp;
    }
    int Run(Screen *scr);
};
