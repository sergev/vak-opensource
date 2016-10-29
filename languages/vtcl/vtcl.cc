//
// Tcl interface to video screen package.
//
// Copyright (C) 1994 Cronyx Ltd.
// Author: Serge Vakulenko, <vak@zebub.msk.su>
//
// This software is distributed with NO WARRANTIES, not even the implied
// warranties for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// Authors grant any other persons or organisations permission to use
// or modify this software as long as this message is kept with the software,
// all derivative works or modified versions.
//
#include <stdlib.h>
#include <string.h>
#ifdef unix
#   include <unistd.h>
#else
#   include <io.h>
#endif

#include "Screen.h"
#include "Menu.h"
#include "Popup.h"

static Screen *V;
static int color;

#include "tcl.h"

static Tcl_Interp *interp;
static Tcl_HashTable htab;
static int nextid = 1;

#define HASH_KEY(key)     ((char*) (0x80000000 | (unsigned short) key))
#define HASH_BOX(id)      ((char*) (0x40000000 | (unsigned short) id))
#define HASH_MENU(id)     ((char*) (0x20000000 | (unsigned short) id))
#define HASH_SUBMENU(id)  ((char*) (0x10000000 | (unsigned short) id))
#define HASH_MENUITEM(id) ((char*) (0x08000000 | (unsigned short) id))
#define HASH_FLASH(id)    ((char*) (0x04000000 | (unsigned short) id))
#define HASH_CALLBACK(id) ((char*) (0x02000000 | (unsigned short) id))

struct {
	char *name;
	unsigned short code;
} keynamtab[] = {
	{ "left",       meta ('l') },   { "right",      meta ('r') },
	{ "up",         meta ('u') },   { "down",       meta ('d') },
	{ "pageup",     meta ('n') },   { "pagedown",   meta ('p') },
	{ "home",       meta ('h') },   { "end",        meta ('e') },
	{ "insert",     cntrl ('T') },  { "delete",     meta ('x') },
	{ "backspace",  cntrl ('H') },  { "escape",     cntrl ('C') },
	{ "tab",        cntrl ('I') },  { "backtab",    cntrl ('K') },
	{ "f1",         meta ('A') },   { "f2",         meta ('B') },
	{ "f3",         meta ('C') },   { "f4",         meta ('D') },
	{ "f5",         meta ('E') },   { "f6",         meta ('F') },
	{ "f7",         meta ('G') },   { "f8",         meta ('H') },
	{ "f9",         meta ('I') },   { "f0",         meta ('J') },
	{ "f10",        meta ('J') },
	{ 0, 0 },
};

struct {
	char *name;
	unsigned char code;
} symnamtab[] = {
	{ "ulc", '\20' }, { "ut",  '\21' }, { "urc", '\22' },
	{ "clt", '\23' }, { "cx",  '\24' }, { "crt", '\25' },
	{ "llc", '\26' }, { "lt",  '\27' }, { "lrc", '\30' },
	{ "vert",'\31' }, { "hor", '\32' },
	{ 0, 0 },
};

//
// Generic screen management command.
// Usage:
//      vdisplay
//              Refresh the actial contents of the terminal
//              screen, applying all the changes to it.
//              Beep if "vdisplay -beep" was requested.
//              Returns nothing.
//
//      vdisplay -redraw
//              Clear the display and redraw the current screen image
//              from scratch.  Useful if the screen contents were damaged.
//              Returns nothing.
//
//      vdisplay -beep
//              Request the terminal "bell".  The actial sound will
//              be produced at the next screen refresh.
//              Returns nothing.
//
//      vdisplay -close
//              Put the terminal into "cooked" mode.
//              Useful before running some external program.
//              Returns nothing.
//
//      vdisplay -open
//              Restore the screen mode of the terminal,
//              as opposite to "vdisplay -close".
//              Returns nothing.
//
//      vdisplay -colorful
//              Returns "yes" if the terminal has colors, else "no".
//
//      vdisplay -delay $msec
//              Pause for `msec' milliseconds.  Returns nothing.
//
//      vdisplay -cursor $yes
//              Makes the terminal cursor visible or invisible.
//              Returns nothing.
//
#define USAGE_VDISPLAY "Usage: vdisplay [-redraw | -beep | -close | -open | -colorful | -cursor $yes | -delay $msec]"

static int vtcl_display (ClientData client, Tcl_Interp *ip, int argc, char **argv)
{
	if (argc == 1)
		V->Sync ();
	else if (strcmp ("-redraw", argv[1]) == 0) {
		if (argc != 2) goto usage;
		V->Redraw ();
	} else if (strcmp ("-beep", argv[1]) == 0) {
		if (argc != 2) goto usage;
		V->Beep ();
	} else if (strcmp ("-close", argv[1]) == 0) {
		if (argc != 2) goto usage;
		V->Restore ();
	} else if (strcmp ("-open", argv[1]) == 0) {
		if (argc != 2) goto usage;
		V->Reopen ();
	} else if (strcmp ("-colorful", argv[1]) == 0) {
		if (argc != 2) goto usage;
		Tcl_SetResult (ip, V->HasColors ? "yes" : "no", TCL_STATIC);
	} else if (strcmp ("-cursor", argv[1]) == 0) {
		int yes;

		if (argc != 3) goto usage;
		if (Tcl_GetBoolean (ip, argv[2], &yes) != TCL_OK) return TCL_ERROR;
		if (yes)
			V->Move (0, 0);
		else
			V->HideCursor ();
	} else if (strcmp ("-delay", argv[1]) == 0) {
		int msec;

		if (argc != 3) goto usage;
		if (Tcl_GetInt (ip, argv[2], &msec) != TCL_OK) return TCL_ERROR;
		usleep (msec * 1000L);
	} else {
usage:          Tcl_SetResult (ip, USAGE_VDISPLAY, TCL_STATIC);
		return TCL_ERROR;
	}
	return TCL_OK;
}

//
// Cursor position management command.
// Usage:
//      vmove $y $x
//              Set the cursor position. Returns nothing.
//
//      vmove
//              Get current cursor position.  Returns the list of
//              two integer values: {y x}.
//
#define USAGE_MOVE "Usage: vmove [$y $x]"

static int vtcl_move (ClientData client, Tcl_Interp *ip, int argc, char **argv)
{
	char buf[30];
	int x, y;

	if (argc == 1) {
		sprintf (buf, "%d %d", V->Row (), V->Col ());
		Tcl_SetResult (ip, buf, TCL_VOLATILE);
		return TCL_OK;
	}
	if (argc != 3) {
		Tcl_SetResult (ip, USAGE_MOVE, TCL_STATIC);
		return TCL_ERROR;
	}
	if (Tcl_GetInt (ip, argv[1], &y) != TCL_OK) return TCL_ERROR;
	if (Tcl_GetInt (ip, argv[2], &x) != TCL_OK) return TCL_ERROR;
	V->Move (y, x);
	return TCL_OK;
}

//
// Keyboard input management command.
// Usage:
//      vkey
//              Input the key from the keyboard.
//              Returns the decimal value of the key pressed.
//
//      vkey -unget $key
//              Place the key value back to the queue of pending key events.
//              Returns nothing.
//
//      vkey -call $proc $key...
//              Call the procedure "proc" every time the key "key" is pressed.
//              Returns nothing.
//
//      vkey -uncall $key...
//              Don't call any at the given key press anymore.
//              Returns nothing.
//
//      vkey -code $name
//              Returns the decimal key value of the given key.
//              The key should be of the form:
//              1) X - the ASCII symbol
//              2) ^X - the control symbol
//              3) the key name
//
#define USAGE_KEY "Usage: vkey [-get | -unget $n | -code $name | -call $proc $n... | -uncall $proc $n...]"

static void vtcl_callback (int key)
{
	Tcl_HashEntry *hent;
	char *proc;
	char buf[80];

	hent = Tcl_FindHashEntry (&htab, HASH_KEY (key));
	if (! hent) {
		V->DelHotKey (key);
		return;
	}
	proc = (char*) Tcl_GetHashValue (hent);
	sprintf (buf, "%s %d", proc, key);
	if (Tcl_GlobalEval (interp, buf) != TCL_OK) {
		fprintf (stderr, "%s\n", interp->result);
		Tcl_GlobalEval (interp, "exit 1");
	}
}

static int vtcl_key (ClientData client, Tcl_Interp *ip, int argc, char **argv)
{
	char buf[30], *proc, **p;
	int key, newflag, i;
	Tcl_HashEntry *hent;

	if (argc == 1) {
		sprintf (buf, "%d", V->GetKey ());
		Tcl_SetResult (ip, buf, TCL_VOLATILE);
	} else if (strcmp ("-unget", argv[1]) == 0) {
		if (argc != 3) goto usage;
		if (Tcl_GetInt (ip, argv[2], &key) != TCL_OK) return TCL_ERROR;
		V->UngetKey (key);
	} else if (strcmp ("-code", argv[1]) == 0) {
		char *n;

		if (argc != 3) goto usage;
		n = argv[2];
		if (! *n)
			key = 0;
		else if (! n[1])
			key = (unsigned char) *n;
		else if (*n == '^' && ! n[2])
			key = n[1] & 037;
		else {
			for (i=0; keynamtab[i].name; ++i)
				if (strcmp (n, keynamtab[i].name) == 0)
					break;
			if (! keynamtab[i].name) {
				Tcl_AppendResult (ip, "vkey -code: no such key:",
					n, (char*)0);
				return TCL_ERROR;
			}
			key = keynamtab[i].code;
		}
		sprintf (buf, "%d", key);
		Tcl_SetResult (ip, buf, TCL_VOLATILE);
	} else if (strcmp ("-call", argv[1]) == 0) {
		if (argc < 4) goto usage;
		proc = argv[2];
		for (p=argv+3; *p; ++p) {
			if (Tcl_GetInt (ip, *p, &key) != TCL_OK)
				return TCL_ERROR;
			hent = Tcl_CreateHashEntry (&htab, HASH_KEY (key),
				&newflag);
			if (! newflag)
				free (Tcl_GetHashValue (hent));
			Tcl_SetHashValue (hent, strdup (proc));
			V->AddHotKey (key, vtcl_callback);
		}
	} else if (strcmp ("-uncall", argv[1]) == 0) {
		if (argc < 3) goto usage;
		for (p=argv+2; *p; ++p) {
			if (Tcl_GetInt (ip, *p, &key) != TCL_OK)
				return TCL_ERROR;
			hent = Tcl_FindHashEntry (&htab, HASH_KEY (key));
			if (hent) {
				free (Tcl_GetHashValue (hent));
				Tcl_DeleteHashEntry (hent);
			}
			V->DelHotKey (key);
		}
	} else {
usage:          Tcl_SetResult (ip, USAGE_KEY, TCL_STATIC);
		return TCL_ERROR;
	}
	return TCL_OK;
}

//
// Screen clear/fill command.
// Usage:
//      vclear [-color $c] [-fill $char]
//              Clear the whole screen, use the color given or gray on black
//              by default.  Clearing is made by filing by the character
//              given (space by default).
//              Returns nothing.
//
//      vclear $line [$col] [-color $c] [-fill $char]
//              Clear the line from the given column (or from the beginning).
//              To clear the current line, use `-' as the `line' argument.
//              Returns nothing.
//
//      vclear $line $col $height $width [-color $c] [-fill $char]
//              Clear the rectangular region.
//              Returns nothing.
//
#define USAGE_CLEAR "Usage: vclear [$y [$x [$ny $nx]]] [-color $n] [-fill $char]"

static int vtcl_clear (ClientData client, Tcl_Interp *ip, int argc, char **argv)
{
	int c = color;
	int f = ' ';
	int x = 0, y = -1, nx = 0, ny = 1;

	--argc, ++argv;

	if (argc < 1) goto erase;
	if (**argv != '-') {
		if (Tcl_GetInt (ip, *argv, &y) != TCL_OK) return TCL_ERROR;
	} else if (argv[0][1] == 0)
		y = V->Row ();
	else
		goto options;
	--argc, ++argv;

	if (argc < 1) goto eraseline;
	if (**argv == '-') goto options;
	if (Tcl_GetInt (ip, *argv, &x) != TCL_OK) return TCL_ERROR;
	--argc, ++argv;

	if (argc < 1) goto eraseline;
	if (**argv == '-') goto options;
	if (Tcl_GetInt (ip, *argv, &ny) != TCL_OK) return TCL_ERROR;
	--argc, ++argv;

	if (argc < 1) goto usage;
	if (**argv == '-') goto options;
	if (Tcl_GetInt (ip, *argv, &nx) != TCL_OK) return TCL_ERROR;
	--argc, ++argv;

	if (argc<1 || **argv!='-') goto usage;
options:
	for (; argc>0; --argc, ++argv)
		if (strcmp ("-color", *argv) == 0) {
			--argc, ++argv;
			if (argc < 1) goto usage;
			if (Tcl_GetInt (ip, *argv, &c) != TCL_OK) return TCL_ERROR;
		} else if (strcmp ("-fill", *argv) == 0) {
			--argc, ++argv;
			if (argc < 1) goto usage;
			f = **argv;
		} else {
usage:                  Tcl_SetResult (ip, USAGE_CLEAR, TCL_STATIC);
			return TCL_ERROR;
		}

	if (nx > 0)
		V->Clear (y, x, ny, nx, c, f);
	else if (y >= 0)
eraseline:      V->ClearLine (y, x, c, f);
	else
erase:          V->Clear (c, f);
	return TCL_OK;
}

//
// String output command.
// Usage:
//      vputs $string [$y $x] [-color $n] [-limit $n] [-center | -right]
//              Print the string on the screen, in position (x,y),
//              using the given color.  Limit the length of the string,
//              if specified.  By default the string is left aligned.
//              Options "-right" makes it right aligned: the string is printed
//              to the left of the base position.  Option "-center"
//              make the string centered relative to the base position.
//              Returns nothing.
//
#define USAGE_PUTS "Usage: vputs $string [$y $x] [-color $n] [-limit $n] [-center] [-right]"

static int vtcl_puts (ClientData client, Tcl_Interp *ip, int argc, char **argv)
{
	int y = -1, x, lim = 0, c = color, align = -1;
	char *str;

	if (argc < 2) {
usage:          Tcl_SetResult (ip, USAGE_PUTS, TCL_STATIC);
		return TCL_ERROR;
	}
	str = argv[1];
	argc -= 2; argv += 2;
	if (argc>0 && **argv != '-') {
		if (argc < 2) goto usage;
		if (Tcl_GetInt (ip, *argv++, &y) != TCL_OK) return TCL_ERROR;
		if (Tcl_GetInt (ip, *argv++, &x) != TCL_OK) return TCL_ERROR;
		argc -= 2;
	}
	for (; argc>0; --argc, ++argv)
		if (strcmp ("-color", *argv) == 0) {
			--argc, ++argv;
			if (argc < 1) goto usage;
			if (Tcl_GetInt (ip, *argv, &c) != TCL_OK) return TCL_ERROR;
		} else if (strcmp ("-limit", *argv) == 0) {
			--argc, ++argv;
			if (argc < 1) goto usage;
			if (Tcl_GetInt (ip, *argv, &lim) != TCL_OK) return TCL_ERROR;
		} else if (strcmp ("-center", *argv) == 0)
			align = 0;
		else if (strcmp ("-right", *argv) == 0)
			align = 1;
		else
			goto usage;
	if (align >= 0) {
		int len = strlen (str);
		if (lim && len > lim)
			len = lim;
		if (y < 0) {
			y = V->Row ();
			x = V->Col ();
		}
		if (align > 0)
			x -= len-1;
		else
			x -= (len+1) / 2;
		if (x < 0) {
			str -= x;
			if (lim)
				lim += x;
			x = 0;
		}
	}
	if (y >= 0)
		V->Move (y, x);
	if (lim > 0)
		V->PutLimited (str, lim, c);
	else
		V->Put (str, c);
	return TCL_OK;
}

//
// Symbol output command.
// Usage:
//      vputc $n [$y $x] [-color $n]
//              Print the symbol on the screen.  The symbol
//              is represented by the integer code.
//              Returns nothing.
//
#define USAGE_PUTC "Usage: vputc $n [$y $x] [-color $n]"

static int vtcl_putc (ClientData client, Tcl_Interp *ip, int argc, char **argv)
{
	int y = -1, x, c = color, sym;

	if (argc < 2) {
usage:          Tcl_SetResult (ip, USAGE_PUTC, TCL_STATIC);
		return TCL_ERROR;
	}
	--argc, ++argv;
	if (Tcl_GetInt (ip, *argv, &sym) != TCL_OK) return TCL_ERROR;
	--argc, ++argv;
	if (argc > 0 && **argv != '-') {
		if (argc < 2) goto usage;
		if (Tcl_GetInt (ip, *argv++, &y) != TCL_OK) return TCL_ERROR;
		if (Tcl_GetInt (ip, *argv++, &x) != TCL_OK) return TCL_ERROR;
		argc -= 2;
	}
	if (argc > 0 && strcmp ("-color", *argv) == 0) {
		--argc, ++argv;
		if (argc < 1) goto usage;
		if (Tcl_GetInt (ip, *argv, &c) != TCL_OK) return TCL_ERROR;
		--argc, ++argv;
	}
	if (argc > 0)
		goto usage;
	if (y >= 0)
		V->Move (y, x);
	V->Put (sym, c);
	return TCL_OK;
}

//
// Generic output command.
// Usage:
//      vput -vline $len [$y $x] [-color $n]
//              Draw the vertical line of length len. Returns nothing.
//
//      vput -hline $len [$y $x] [-color $n]
//              Draw the horizontal line of length len. Returns nothing.
//
//      vput -frame $ny $nx [$y $x] [-color $n]
//              Draw the rectangle of height ny and width nx.  Returns nothing.
//
//      vput -special $name [$y $x] [-color $n]
//              Draw the special character named `name'.  Returns nothing.
//
//      vput -box $box [$y $x] [-color $n]
//              Draw the contents of the box.  If the color is specified,
//              the the box is drawn with that color.  By default
//              the original color set of the box is restored.
//              Returns nothing.
//
#define USAGE_PUT "Usage: vput [-box $box | -vline $len | -hline $len | -frame $ny $nx | -special $name] [$y $x] [-color $n]"

static int vtcl_put (ClientData client, Tcl_Interp *ip, int argc, char **argv)
{
	int y = -1, x, c = 0;
	char *op, *arg, *arg2 = "";

	if (argc < 3) {
usage:          Tcl_SetResult (ip, USAGE_PUT, TCL_STATIC);
		return TCL_ERROR;
	}
	--argc, ++argv;
	op = *argv++; --argc;
	arg = *argv++; --argc;
	if (strcmp ("-frame", op) == 0) {
		if (argc < 1) goto usage;
		arg2 = *argv++; --argc;
	}
	if (argc > 0 && **argv != '-') {
		if (argc < 2) goto usage;
		if (Tcl_GetInt (ip, *argv++, &y) != TCL_OK) return TCL_ERROR;
		if (Tcl_GetInt (ip, *argv++, &x) != TCL_OK) return TCL_ERROR;
		argc -= 2;
	}
	if (argc > 0 && strcmp ("-color", *argv) == 0) {
		--argc, ++argv;
		if (argc < 1) goto usage;
		if (Tcl_GetInt (ip, *argv, &c) != TCL_OK) return TCL_ERROR;
		--argc, ++argv;
	}
	if (argc > 0)
		goto usage;
	if (y >= 0)
		V->Move (y, x);
	if (strcmp ("-vline", op) == 0) {
		int len;

		if (Tcl_GetInt (ip, arg, &len) != TCL_OK) return TCL_ERROR;
		if (y < 0) {
			y = V->Row ();
			x = V->Col ();
		}
		if (c == 0)
			c = color;
		V->VertLine (x, y, len, c);
	} else if (strcmp ("-hline", op) == 0) {
		int len;

		if (Tcl_GetInt (ip, arg, &len) != TCL_OK) return TCL_ERROR;
		if (y < 0) {
			y = V->Row ();
			x = V->Col ();
		}
		if (c == 0)
			c = color;
		V->HorLine (y, x, len, c);
	} else if (strcmp ("-frame", op) == 0) {
		int w, h;

		if (Tcl_GetInt (ip, arg, &h) != TCL_OK) return TCL_ERROR;
		if (Tcl_GetInt (ip, arg2, &w) != TCL_OK) return TCL_ERROR;
		if (y < 0) {
			y = V->Row ();
			x = V->Col ();
		}
		if (c == 0)
			c = color;
		V->DrawFrame (y, x, h, w, c);
	} else if (strcmp ("-special", op) == 0) {
		int i;

		for (i=0; symnamtab[i].name; ++i)
			if (strcmp (arg, symnamtab[i].name) == 0)
				break;
		if (! symnamtab[i].name) {
			Tcl_AppendResult (ip, "vput -special: no such symbol:",
				arg, (char*)0);
			return TCL_ERROR;
		}
		if (y >= 0)
			V->Move (y, x);
		if (c == 0)
			c = color;
		V->Put (symnamtab[i].code, c);
	} else if (strcmp ("-box", op) == 0) {
		int id;
		Box *b;
		Tcl_HashEntry *hent;

		if (*arg != 'b') {
			Tcl_SetResult (ip, "vput -box: invalid arg", TCL_STATIC);
			return TCL_ERROR;
		}
		id = atoi (arg+1);
		hent = Tcl_FindHashEntry (&htab, HASH_BOX (id));
		if (! hent) {
			Tcl_SetResult (ip, "vput -box: box not found", TCL_STATIC);
			return TCL_ERROR;
		}
		b = (Box*) Tcl_GetHashValue (hent);
		V->Put (y, x, *b, c);
	}
	return TCL_OK;
}

//
// Subwindow (box) management command.
// Usage:
//      vbox -get $ny $nx [$y $x]
//              Copy the contents of the square portion of the screen
//              to some object called the box.  Returns the descriptor,
//              which can be used for subsequent operations on the box.
//
//      vbox -delete $box
//              Delete the previosly allocated box, freeing the memory.
//              Returns nothing.
//
#define USAGE_BOX "Usage: vbox [-get $ny $nx [$y $x] | -delete $box]"

static int vtcl_box (ClientData client, Tcl_Interp *ip, int argc, char **argv)
{
	int id;
	Box *b;
	Tcl_HashEntry *hent;

	if (argc < 3) {
usage:          Tcl_SetResult (ip, USAGE_BOX, TCL_STATIC);
		return TCL_ERROR;
	}
	if (strcmp ("-get", argv[1]) == 0) {
		int ny, nx, y = V->Row(), x = V->Col(), newflag;
		char buf[30];

		if (argc != 4 && argc != 6) goto usage;
		if (Tcl_GetInt (ip, argv[2], &ny) != TCL_OK) return TCL_ERROR;
		if (Tcl_GetInt (ip, argv[3], &nx) != TCL_OK) return TCL_ERROR;
		if (argc == 6) {
			if (Tcl_GetInt (ip, argv[4], &y) != TCL_OK) return TCL_ERROR;
			if (Tcl_GetInt (ip, argv[5], &x) != TCL_OK) return TCL_ERROR;
		}
		b = new Box (*V, y, x, ny, nx);
		if (! b) {
			Tcl_SetResult (ip, "vbox -get: out of memory", TCL_STATIC);
			return TCL_ERROR;
		}

		// Make new hash table box entry.
		id = nextid++;
		hent = Tcl_CreateHashEntry (&htab, HASH_BOX (id), &newflag);
		Tcl_SetHashValue (hent, b);
		sprintf (buf, "b%d", id);
		Tcl_SetResult (ip, buf, TCL_VOLATILE);
	} else if (strcmp ("-delete", argv[1]) == 0) {
		if (argc != 3) goto usage;
		if (argv[2][0] != 'b') {
			Tcl_SetResult (ip, "vbox -delete: invalid arg", TCL_STATIC);
			return TCL_ERROR;
		}
		id = atoi (&argv[2][1]);
		hent = Tcl_FindHashEntry (&htab, HASH_BOX (id));
		if (! hent) {
			Tcl_SetResult (ip, "vbox -delete: box not found", TCL_STATIC);
			return TCL_ERROR;
		}
		b = (Box*) Tcl_GetHashValue (hent);
		delete b;
		Tcl_DeleteHashEntry (hent);
	} else
		goto usage;
	return TCL_OK;
}

//
// Menu management command.
// Usage:
//      vmenu -create
//              Create new menu object.  Returns the descriptor,
//              which can be used for subsequent operations on the menu.
//
//      vmenu -delete $menu
//              Delete the previosly allocated menu, freeing the memory.
//              Returns nothing.
//
//      vmenu -palette $menu $norm $light $dim $inv $linv
//              Set up the color palette of the menu.  Returns nothing.
//
//      vmenu -add $menu $title
//              Add new submenu object to the menu.  Returns the descriptor,
//              which can be used for subsequent operations on the submenu.
//
//      vmenu -display $menu $y
//              Display the menu bar on the screen. Returns nothing.
//
//      vmenu -run $menu $y
//              Run the menu.  Returns nothing.
//
//      vmenu -get $menu
//              Get the current state of the menu.
//
#define USAGE_MENU "Usage: vmenu [-create | -palette $menu $norm $light $dim $inv $linv | -add $menu $title | -display $menu $y | -run $menu $y | -get $menu | -delete $menu]"

static int vtcl_menu (ClientData client, Tcl_Interp *ip, int argc, char **argv)
{
	int id;
	Menu *m;
	Tcl_HashEntry *hent;

	if (argc == 2 && strcmp ("-create", argv[1]) == 0) {
		int newflag;
		char buf[30];

		// Make new hash table menu entry.
		m = new Menu ();
		if (! m) {
			Tcl_SetResult (ip, "vmenu -create: out of memory", TCL_STATIC);
			return TCL_ERROR;
		}
		id = nextid++;
		hent = Tcl_CreateHashEntry (&htab, HASH_MENU (id), &newflag);
		Tcl_SetHashValue (hent, m);
		sprintf (buf, "m%d", id);
		Tcl_SetResult (ip, buf, TCL_VOLATILE);
		return TCL_OK;
	}

	if (argc < 3) {
usage:          Tcl_SetResult (ip, USAGE_MENU, TCL_STATIC);
		return TCL_ERROR;
	}

	// Find the menu entry in the hash table.
	if (argv[2][0] != 'm') {
		Tcl_SetResult (ip, "vmenu: invalid menu descriptor", TCL_STATIC);
		return TCL_ERROR;
	}
	id = atoi (&argv[2][1]);
	hent = Tcl_FindHashEntry (&htab, HASH_MENU (id));
	if (! hent) {
		Tcl_SetResult (ip, "vmenu: menu not found", TCL_STATIC);
		return TCL_ERROR;
	}
	m = (Menu*) Tcl_GetHashValue (hent);

	if (strcmp ("-delete", argv[1]) == 0) {
		if (argc != 3) goto usage;
		delete m;
		Tcl_DeleteHashEntry (hent);
	} else if (strcmp ("-palette", argv[1]) == 0) {
		int norm, light, dim, inv, linv;

		if (argc != 8) goto usage;
		if (Tcl_GetInt (ip, argv[3], &norm)  != TCL_OK) return TCL_ERROR;
		if (Tcl_GetInt (ip, argv[4], &light) != TCL_OK) return TCL_ERROR;
		if (Tcl_GetInt (ip, argv[5], &dim)   != TCL_OK) return TCL_ERROR;
		if (Tcl_GetInt (ip, argv[6], &inv)   != TCL_OK) return TCL_ERROR;
		if (Tcl_GetInt (ip, argv[7], &linv)  != TCL_OK) return TCL_ERROR;

		m->Palette (norm, light, dim, inv, linv);
	} else if (strcmp ("-display", argv[1]) == 0) {
		int y;

		if (argc != 4) goto usage;
		if (Tcl_GetInt (ip, argv[3], &y) != TCL_OK) return TCL_ERROR;

		m->Display (V, y);
	} else if (strcmp ("-run", argv[1]) == 0) {
		int y;

		if (argc != 4) goto usage;
		if (Tcl_GetInt (ip, argv[3], &y) != TCL_OK) return TCL_ERROR;

		m->Run (V, y);
	} else if (strcmp ("-get", argv[1]) == 0) {
		int curr, subcurr;
		char buf [40];

		if (argc != 3) goto usage;
		curr = m->Current (&subcurr);
		sprintf (buf, "%d %d", curr, subcurr);
		Tcl_SetResult (ip, buf, TCL_VOLATILE);
	} else if (strcmp ("-add", argv[1]) == 0) {
		int newflag;
		char buf[30];
		SubMenu *s;

		// Make new hash table submenu entry.
		if (argc != 4) goto usage;
		s = m->Add (argv[3]);
		if (! s) {
			Tcl_SetResult (ip, "vmenu -add: out of memory", TCL_STATIC);
			return TCL_ERROR;
		}
		id = nextid++;
		hent = Tcl_CreateHashEntry (&htab, HASH_SUBMENU (id), &newflag);
		Tcl_SetHashValue (hent, s);
		sprintf (buf, "s%d", id);
		Tcl_SetResult (ip, buf, TCL_VOLATILE);
	} else
		goto usage;
	return TCL_OK;
}

//
// Submenu management command.
// Usage:
//      vsubmenu -add $submenu $title $proc
//              Add new item to the submenu.  Returns the descriptor,
//              which can be used for subsequent operations on the item.
//
//      vsubmenu -delete $submenu
//              Delete the previosly allocated submenu, freeing the memory.
//              Returns nothing.
//
#define USAGE_SUBMENU "Usage: vsubmenu [-add $submenu $title $proc | -delete $submenu]"

static void vtcl_callmenu (void *data)
{
	Tcl_HashEntry *hent;
	char *proc;
	char buf[80];
	int id = (int) data;

	hent = Tcl_FindHashEntry (&htab, HASH_CALLBACK (id));
	if (! hent) {
		fprintf (stderr, "vtcl menu: callback %d not found\n", id);
		Tcl_GlobalEval (interp, "exit 1");
		return;
	}
	proc = (char*) Tcl_GetHashValue (hent);
	if (! proc) {
		fprintf (stderr, "vtcl menu: bad callback %s\n", proc);
		Tcl_GlobalEval (interp, "exit 1");
		return;
	}
	sprintf (buf, "%s t%d", proc, id);
	if (Tcl_GlobalEval (interp, buf) != TCL_OK) {
		fprintf (stderr, "%s\n", interp->result);
		Tcl_GlobalEval (interp, "exit 1");
	}
}

static int vtcl_submenu (ClientData client, Tcl_Interp *ip, int argc, char **argv)
{
	int id;
	SubMenu *s;
	Tcl_HashEntry *hent;

	if (argc != 3 && argc != 5) {
usage:          Tcl_SetResult (ip, USAGE_SUBMENU, TCL_STATIC);
		return TCL_ERROR;
	}

	// Find the submenu entry in the hash table.
	if (argv[2][0] != 's') {
		Tcl_SetResult (ip, "vsubmenu: invalid submenu descriptor", TCL_STATIC);
		return TCL_ERROR;
	}
	id = atoi (&argv[2][1]);
	hent = Tcl_FindHashEntry (&htab, HASH_SUBMENU (id));
	if (! hent) {
		Tcl_SetResult (ip, "vsubmenu: submenu not found", TCL_STATIC);
		return TCL_ERROR;
	}
	s = (SubMenu*) Tcl_GetHashValue (hent);

	if (strcmp ("-delete", argv[1]) == 0) {
		if (argc != 3) goto usage;
		delete s;
		Tcl_DeleteHashEntry (hent);
	} else if (strcmp ("-add", argv[1]) == 0) {
		int newflag;
		char buf[30];
		MenuItem *t;

		// Make new hash table menu item entry.
		if (argc != 5) goto usage;
		id = nextid++;
		t = s->Add (argv[3], vtcl_callmenu, (void*) id);
		if (! t) {
			Tcl_SetResult (ip, "vsubmenu -add: out of memory", TCL_STATIC);
			return TCL_ERROR;
		}
		hent = Tcl_CreateHashEntry (&htab, HASH_MENUITEM (id), &newflag);
		Tcl_SetHashValue (hent, t);
		hent = Tcl_CreateHashEntry (&htab, HASH_CALLBACK (id), &newflag);
		Tcl_SetHashValue (hent, strdup (argv[4]));
		sprintf (buf, "t%d", id);
		Tcl_SetResult (ip, buf, TCL_VOLATILE);
	} else
		goto usage;
	return TCL_OK;
}

//
// Menu item management command.
// Usage:
//      vmenuitem -delete $menuitem
//              Delete the previosly allocated menu, freeing the memory.
//              Returns nothing.
//
//      vmenuitem -active $menuitem $yes
//              Activate/deactivate the menu item.  Returns nothing.
//
//      vmenuitem -tag $menuitem $yes
//              Set/unset the tag on the menu item.  Returns nothing.
//
#define USAGE_MENUITEM "Usage: vmenuitem [-active $menuitem $yes | -tag $menuitem $yes | -delete $menuitem]"

static int vtcl_menuitem (ClientData client, Tcl_Interp *ip, int argc, char **argv)
{
	int id, yes;
	MenuItem *t;
	Tcl_HashEntry *hent;

	if (argc < 3) {
usage:          Tcl_SetResult (ip, USAGE_MENUITEM, TCL_STATIC);
		return TCL_ERROR;
	}

	// Find the menu item entry in the hash table.
	if (argv[2][0] != 't') {
		Tcl_SetResult (ip, "vmenuitem: invalid item descriptor", TCL_STATIC);
		return TCL_ERROR;
	}
	id = atoi (&argv[2][1]);
	hent = Tcl_FindHashEntry (&htab, HASH_MENUITEM (id));
	if (! hent) {
		Tcl_SetResult (ip, "vmenuitem: item not found", TCL_STATIC);
		return TCL_ERROR;
	}
	t = (MenuItem*) Tcl_GetHashValue (hent);

	if (strcmp ("-delete", argv[1]) == 0) {
		if (argc != 3) goto usage;
		delete t;
		Tcl_DeleteHashEntry (hent);
	} else if (strcmp ("-active", argv[1]) == 0) {
		if (argc != 4) goto usage;
		if (Tcl_GetBoolean (ip, argv[3], &yes) != TCL_OK) return TCL_ERROR;
		if (yes)
			t->Activate ();
		else
			t->Deactivate ();
	} else if (strcmp ("-tag", argv[1]) == 0) {
		if (argc != 4) goto usage;
		if (Tcl_GetBoolean (ip, argv[3], &yes) != TCL_OK) return TCL_ERROR;
		if (yes)
			t->Tag ();
		else
			t->Untag ();
	} else
		goto usage;
	return TCL_OK;
}

//
// Message display command.
// Usage:
//      vmessage $title $msg ${reply} $norm $rev
//              Display the message and wait for reply.
//              The message can consist of several lines,
//              separated by newlines.  Returns the number of the reply
//              chosen or the empty string when aborted.
//
//      vmessage $title $msg -flash $norm $rev
//              Build and display the flash message.
//              The message can consist of several lines,
//              separated by newlines.  Returns nothing the flash descriptor.
//
//      vmessage -delete $flash
//              Delete the previosly created flash message, freeing the memory.
//              Returns nothing.
//
#define USAGE_MESSAGE "Usage: vmessage [$title $msg [${reply} | -flash] $norm $rev | -delete $flash]"

static int vtcl_message (ClientData client, Tcl_Interp *ip, int argc, char **argv)
{
	int id, norm, rev, rep, nreply;
	Flash *f;
	Tcl_HashEntry *hent;
	char **reply;

	if (argc == 3 && strcmp ("-delete", argv[1]) == 0) {
		if (argv[2][0] != 'f') {
			Tcl_SetResult (ip, "vmessage -delete: invalid arg", TCL_STATIC);
			return TCL_ERROR;
		}
		id = atoi (&argv[2][1]);
		hent = Tcl_FindHashEntry (&htab, HASH_FLASH (id));
		if (! hent) {
			Tcl_SetResult (ip, "vmessage -delete: flash not found", TCL_STATIC);
			return TCL_ERROR;
		}
		f = (Flash*) Tcl_GetHashValue (hent);
		delete f;
		Tcl_DeleteHashEntry (hent);
		return TCL_OK;
	}

	if (argc != 6) {
		Tcl_SetResult (ip, USAGE_MESSAGE, TCL_STATIC);
		return TCL_ERROR;
	}
	if (Tcl_GetInt (ip, argv[4], &norm) != TCL_OK) return TCL_ERROR;
	if (Tcl_GetInt (ip, argv[5], &rev) != TCL_OK) return TCL_ERROR;

	if (strcmp ("-flash", argv[3]) == 0) {
		int newflag;
		char buf[30];

		f = new Flash (V, argv[1], argv[2], norm, rev);
		if (! f) {
			Tcl_SetResult (ip, "vmessage -flash: out of memory", TCL_STATIC);
			return TCL_ERROR;
		}

		// Make new hash table flash entry.
		id = nextid++;
		hent = Tcl_CreateHashEntry (&htab, HASH_FLASH (id), &newflag);
		Tcl_SetHashValue (hent, f);
		sprintf (buf, "f%d", id);
		Tcl_SetResult (ip, buf, TCL_VOLATILE);
		return TCL_OK;
	}

	// Split the list of replies.
	if (Tcl_SplitList (ip, argv[3], &nreply, &reply) != TCL_OK) return TCL_ERROR;
	rep = V->Message (argv[1], argv[2], reply, nreply, norm, rev);
	if (rep >= 0)
		Tcl_SetResult (ip, reply[rep], TCL_VOLATILE);
	free ((void*) reply);
	return TCL_OK;
}

//
// Input prompt command.
// Usage:
//      vinput $title $msg $norm $rev -string $len $dflt [-verify $proc]
//              Edit the string in the prompt window.
//              The message should be the single line.
//              The verify proc is called whe the edition is done,
//              with the edited string as the argument.
//              When it returns false, the edit process will be repeated.
//              Returns the edited string or the empty string when aborted.
//
#define USAGE_INPUT "Usage: vinput $title $msg $norm $rev -string $len $dflt [-verify $proc]"

static int vtcl_input (ClientData client, Tcl_Interp *ip, int argc, char **argv)
{
	int norm, rev;
	char *title, *msg;

	if (argc < 6) {
usage:          Tcl_SetResult (ip, USAGE_INPUT, TCL_STATIC);
		return TCL_ERROR;
	}
	title = argv[1];
	msg = argv[2];
	if (Tcl_GetInt (ip, argv[3], &norm) != TCL_OK) return TCL_ERROR;
	if (Tcl_GetInt (ip, argv[4], &rev) != TCL_OK) return TCL_ERROR;

	if (strcmp ("-string", argv[5]) == 0) {
		int len, yes;
		char *dflt, *str, *proc = 0, buf[80];

		if (argc < 8) goto usage;
		if (Tcl_GetInt (ip, argv[6], &len) != TCL_OK) return TCL_ERROR;
		dflt = argv[7];
		if (argc > 8) {
			if (argc != 10) goto usage;
			if (strcmp ("-verify", argv[8]) != 0) goto usage;
			proc = argv[9];
		}
		for (;;) {
			str = V->GetString (len, dflt, title, msg, norm, rev);
			if (str) {
				if (proc) {
					sprintf (buf, "%s {%s}", proc, str);
					if (Tcl_GlobalEval (ip, buf) != TCL_OK) return TCL_ERROR;
					strcpy (buf, ip->result);
					if (Tcl_GetBoolean (ip, buf, &yes) != TCL_OK) return TCL_ERROR;
					if (! yes)
						continue;
				}
				Tcl_SetResult (ip, str, TCL_VOLATILE);
			}
			return TCL_OK;
		}
	}
	return TCL_OK;
}

//
// Edit command.
// Usage:
//      vedit $title $msg $norm $rev [-readonly | -verify $proc]
//              Edit the multiline string.
//              The verify proc is called whe the edition is done,
//              with the edited string as the argument.
//              When it returns false, the edit process will be repeated.
//              The resulting string will always have the trailing \n.
//              Returns the edited string or the empty string when aborted.
//
#define USAGE_EDIT "Usage: vedit $title $msg $norm $rev [-readonly | -verify $proc]"

static int vtcl_edit (ClientData client, Tcl_Interp *ip, int argc, char **argv)
{
	int norm, rev, readonly = 0;
	char *title, *dflt, *proc = 0;

	if (argc < 5) {
usage:          Tcl_SetResult (ip, USAGE_EDIT, TCL_STATIC);
		return TCL_ERROR;
	}
	title = argv[1];
	dflt = argv[2];
	if (Tcl_GetInt (ip, argv[3], &norm) != TCL_OK) return TCL_ERROR;
	if (Tcl_GetInt (ip, argv[4], &rev) != TCL_OK) return TCL_ERROR;

	if (argv == 6 && strcmp ("-readonly", argv[5]) == 0)
		readonly = 1;
	else if (argv == 7 && strcmp ("-verify", argv[5]) == 0)
		proc = argv[6];
	else
		goto usage;
	for (;;) {
		str = V->EditString (title, dflt, norm, rev, readonly);
		if (str) {
			if (proc) {
				sprintf (buf, "%s {%s}", proc, str);
				if (Tcl_GlobalEval (ip, buf) != TCL_OK) return TCL_ERROR;
				strcpy (buf, ip->result);
				if (Tcl_GetBoolean (ip, buf, &yes) != TCL_OK) return TCL_ERROR;
				if (! yes)
					continue;
			}
			Tcl_SetResult (ip, str, TCL_VOLATILE);
		}
		return TCL_OK;
	}
	return TCL_OK;
}

struct {
	char *name;
	Tcl_CmdProc *func;
} cmdtab [] = {
	{ "vdisplay",   vtcl_display,  },
	{ "vmove",      vtcl_move,     },
	{ "vclear",     vtcl_clear,    },
	{ "vkey",       vtcl_key,      },
	{ "vput",       vtcl_put,      },
	{ "vputs",      vtcl_puts,     },
	{ "vputc",      vtcl_putc,     },
	{ "vbox",       vtcl_box,      },
	{ "vmenu",      vtcl_menu,     },
	{ "vsubmenu",   vtcl_submenu,  },
	{ "vmenuitem",  vtcl_menuitem, },
	{ "vmessage",   vtcl_message,  },
	{ "vinput",     vtcl_input,    },
	{ "vedit",      vtcl_edit,     },
	{ 0,            0,             },
};

extern int main ();
int *vtcl_dummy_main_ptr = (int*) main; // link main() frol the Tcl library

#define NULLDATA ((ClientData) 0)
#define NULLPROC ((Tcl_CmdDeleteProc*) 0)

static void vtcl_quit (ClientData client, Tcl_Interp *ip)
{
	if (! V)
		return;
	V->Close ();
	delete V;
	V = 0;
}

int Tcl_AppInit (Tcl_Interp *ip)
{
	int i;

	// Initialize the Tcl package.
	if (Tcl_Init (ip) == TCL_ERROR)
		return TCL_ERROR;

	// Initialize the screen.
	V = new Screen ();
	if (! V) {
		Tcl_SetResult (ip, "Cannot initialize terminal", TCL_STATIC);
		return TCL_ERROR;
	}
	Tcl_CallWhenDeleted (ip, vtcl_quit, NULLDATA);
	atexit ((void(*)()) vtcl_quit);

	color = V->HasColors () ? 0x1f : 0x07;
	V->Clear (color);

	// Create screen state variables.
	Tcl_LinkVar (ip, "vlines",   (char*) &V->Lines,   TCL_LINK_INT | TCL_LINK_READ_ONLY);
	Tcl_LinkVar (ip, "vcolumns", (char*) &V->Columns, TCL_LINK_INT | TCL_LINK_READ_ONLY);
	Tcl_LinkVar (ip, "vcolor",   (char*) &color,      TCL_LINK_INT);

	// Create screen management commands.
	for (i=0; cmdtab[i].name; ++i)
		Tcl_CreateCommand (ip, cmdtab[i].name, cmdtab[i].func,
			NULLDATA, NULLPROC);

	// Allocate hash table for boxes, menus, etc.
	Tcl_InitHashTable (&htab, TCL_ONE_WORD_KEYS);

	// Specify a startup file.
	tcl_RcFileName = "~/.vtclrc";

	interp = ip;
	return TCL_OK;
}
