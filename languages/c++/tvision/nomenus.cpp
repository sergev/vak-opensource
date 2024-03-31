/*
 * TVision example: how to handle dialogs without menu bar and status line
 *
 * Written by Sergio Sigala <sergio@sigala.it>
 */

#define Uses_MsgBox
#define Uses_TApplication
#define Uses_TBackground
#define Uses_TButton
#define Uses_TCheckBoxes
#define Uses_TDeskTop
#define Uses_TDialog
#define Uses_TInputLine
#define Uses_TLabel
#define Uses_TRadioButtons
#define Uses_TSItem
#define Uses_TView
#define Uses_TWindow

#include <tvision/tv.h>

// define a custom background

class TMyBackground : public TBackground {
public:
    TMyBackground(const TRect &bounds);
    virtual void draw();
};

TMyBackground::TMyBackground(const TRect &bounds) : TBackground(bounds, '\xB0')
{
}

void TMyBackground::draw()
{
    TDrawBuffer b;
    int x                 = 0;
    static const char s[] = "Background example ";

    while (x < size.x) {
        b.moveStr(x, s, 0x17);
        x += strlen(s);
    }
    writeLine(0, 0, size.x, size.y, b);
}

// define a custom desktop, with our custom background

class TMyDeskTop : public TDeskTop {
public:
    TMyDeskTop(const TRect &bounds);
    static TBackground *initBackground(TRect);
};

TMyDeskTop::TMyDeskTop(const TRect &bounds)
    : TDeskInit(&TMyDeskTop::initBackground), TDeskTop(bounds)
{
}

TBackground *TMyDeskTop::initBackground(TRect r)
{
    return new TMyBackground(r);
}

// our application

struct DialogData {
    ushort checkBoxData;
    ushort radioButtonData;
    char inputLineData[128];
};

class TMyApp : public TApplication {
public:
    TMyApp();
    void doWork();
    ushort newDialog(DialogData &data);
    static TDeskTop *initDeskTop(TRect r);
};

// Application constructor. We don't call initStatusLine() and initMenuBar()
// because we don't want status line or menus in this application.

TMyApp::TMyApp()
    : TProgInit(0,                    //&TMyApp::initStatusLine,
                0,                    //&TMyApp::initMenuBar,
                &TMyApp::initDeskTop) // this call is required
{
}

// Creates a new desktop.  The address of this function is passed to the
// TProgInit constructor, which creates a TDeskTop object for the application
// and stores a pointer to it in the deskTop global variable.  Few applications
// need to redefine it to have a custom desktop, instead of the default empty
// TDeskTop.

TDeskTop *TMyApp::initDeskTop(TRect r)
{
    // By removing the following two lines we require a full screen desktop.
    // Since status line and menus are not used in this application, this will
    // give us two lines of otherwise unused screen space.

    // r.a.y++;
    // r.b.y--;
    return new TMyDeskTop(r);
}

// executes a dialog

ushort TMyApp::newDialog(DialogData &data)
{
    TDialog *pd = new TDialog(TRect(20, 6, 60, 19), "Cheese order");
    if (pd) {
        TView *b = new TCheckBoxes(
            TRect(3, 3, 18, 6),
            new TSItem("~H~varti", new TSItem("~T~ilset", new TSItem("~J~arlsberg", 0))));
        pd->insert(b);
        pd->insert(new TLabel(TRect(2, 2, 10, 3), "Cheeses", b));

        b = new TRadioButtons(
            TRect(22, 3, 34, 6),
            new TSItem("~S~olid", new TSItem("~R~unny", new TSItem("~M~elted", 0))));
        pd->insert(b);
        pd->insert(new TLabel(TRect(21, 2, 33, 3), "Consistency", b));

        b = new TInputLine(TRect(3, 8, 37, 9), 128);
        pd->insert(b);
        pd->insert(new TLabel(TRect(2, 7, 24, 8), "Delivery Instructions", b));

        pd->insert(new TButton(TRect(15, 10, 25, 12), "~O~K", cmOK, bfDefault));
        pd->insert(new TButton(TRect(28, 10, 38, 12), "~C~ancel", cmCancel, bfNormal));

        // we set the dialog data

        pd->setData(&data);

        ushort control = deskTop->execView(pd);

        // and read it back when the dialog box is successfully closed

        if (control != cmCancel)
            pd->getData(&data);

        destroy(pd);
        return control;
    }
    return cmCancel;
}

// open dialogs here

void TMyApp::doWork()
{
    messageBox("\003Welcome to the cheese ordering system", mfInformation + mfOKButton);

    DialogData data;

    data.checkBoxData    = 1;
    data.radioButtonData = 2;
    strcpy(data.inputLineData, "By box");

    ushort control = newDialog(data);

    if (control == cmOK)
        messageBox("Your order is accepted", mfInformation + mfOKButton);
    else
        messageBox("You canceled the order", mfError + mfOKButton);
}

int main()
{
    TMyApp myApp;
    // myApp.run();	//don't call this
    myApp.doWork();
    return 0;
}
