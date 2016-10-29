DEST		= "/Library/Keyboard Layouts"
SYSDEST		= "/System/Library/Keyboard Layouts/Unicode.bundle/Contents/Resources"
KBDEST		= "$(HOME)/Library/KeyBindings"
FOXDEST		= "/Applications/Firefox.app/Contents/MacOS/chrome/toolkit.jar"

all:		USExtended.pch

install:
		install USExtended-new.keylayout $(SYSDEST)/USExtended.keylayout
		install Russian-win.keylayout $(DEST)
		install Russian-win.icns $(DEST)
		install Russian-mac.keylayout $(DEST)
		install Russian-mac.icns $(DEST)
		install KeyBinding.dict $(KBDEST)/DefaultKeyBinding.dict

firefox:	platformHTMLBindings.xml
		@-rm -rf content toolkit.jar
		jar xf toolkit-orig.jar
		cp -f platformHTMLBindings.xml content/global/
		jar cfM0 toolkit.jar content
		install toolkit.jar $(FOXDEST)
		@-rm -rf content toolkit.jar

uninstall:
		install USExtended.keylayout $(SYSDEST)/USExtended.keylayout
		rm -f $(DEST)/Russian-win.keylayout
		rm -f $(DEST)/Russian-win.icns
		rm -f $(DEST)/Russian-mac.keylayout
		rm -f $(DEST)/Russian-mac.icns

USExtended.pch:	USExtended.keylayout USExtended-new.keylayout
		-diff -s -u -b USExtended.keylayout USExtended-new.keylayout > USExtended.pch

clean:
		rm -f *~ content toolkit.jar
