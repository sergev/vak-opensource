# ifndef cntrl
#    define cntrl(c)    (c & 037)
# endif
# ifndef meta
#    define meta(c)     (c | 0400)
# endif

typedef struct {
	short cury, curx;
	short flgs;
	char clear;
	short **y;
	short *firstch;
	short *lastch;
	short *lnum;
} WINDOW;

typedef struct {
	short y, x;
	short ny, nx;
	short *mem;
} BOX;

typedef long CURSOR;

extern LINES, COLS;
extern WINDOW VScreen;

extern BOX *VGetBox ();
extern CURSOR VGetCursor ();
extern short VGetAttr ();
extern void VOpen(), VReopen(), VClose(), VRestore(), VIClear(), VIPrompt(), Vredraw();
extern void VSyncLine(), DOSgetinfo(), DOSsetattr(), DOSclrscr(), VFlush(), VRedraw(), DOSputch();
extern void DOSgotoxy(), VPutChar(), VMove(), VClearLine(), VSync(), VPutString();
extern void VDrawBox(), VSetPalette(), VFillBox(), VStandEnd(), VSetNormal();
extern void VUngetBox(), VFreeBox(), VClearBox(), VBeep(), VSetCursor(), VHorLine();
extern void VSetAttr ();
extern VInit(), VStandOut(), VSetBold(), VSetDim();

# define VMPutChar(y,x,ch) (VMove (y,x), VPutChar (ch))
# define VMPutString(y,x,str) (VMove (y,x), VPutString (str))
# define VMGetChar(yy,x) (VMove (yy,x), (VScreen.y[VScreen.cury][VScreen.curx] & 0377))
