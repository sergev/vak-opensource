//
//  main.c
//  Hello-carbon
//
//  Created by Serge Vakulenko on 11/02/2006.
//  Copyright __MyCompanyName__ 2006. All rights reserved.
//

#include <Carbon/Carbon.h>

static OSStatus PrintHello (EventHandlerCallRef handler,
	EventRef event, void *data)
{
    WindowRef window = (WindowRef)data;
    Rect bounds;

    GetWindowPortBounds (window, &bounds);
    EraseRect (&bounds);
    InsetRect (&bounds, 12, 12);
    TextFont (FMGetFontFamilyFromName ("\pTimes"));
    TextSize (48);
    TextFace (italic);
    TXNDrawCFStringTextBox (CFSTR("Hello, World!"), &bounds, NULL, NULL);
    return noErr;
}

int main (int argc, char* argv[])
{
    IBNibRef 		nibRef;
    WindowRef 		window;
    
    OSStatus		err;

    // Create a Nib reference passing the name of the nib file (without the .nib extension)
    // CreateNibReference only searches into the application bundle.
    err = CreateNibReference(CFSTR("main"), &nibRef);
    require_noerr( err, CantGetNibRef );
    
    // Once the nib reference is created, set the menu bar. "MainMenu" is the name of the menu bar
    // object. This name is set in InterfaceBuilder when the nib is created.
    err = SetMenuBarFromNib(nibRef, CFSTR("MenuBar"));
    require_noerr( err, CantSetMenuBar );
    
    // Then create a window. "MainWindow" is the name of the window object. This name is set in 
    // InterfaceBuilder when the nib is created.
    err = CreateWindowFromNib(nibRef, CFSTR("MainWindow"), &window);
    require_noerr( err, CantCreateWindow );

    // We don't need the nib reference anymore.
    DisposeNibReference(nibRef);
    
	EventTypeSpec eventSpec = 
		{ kEventClassWindow, kEventWindowDrawContent };

	InstallWindowEventHandler (window, NewEventHandlerUPP (PrintHello), 
		1, &eventSpec, (void*) window, NULL);
	
    // The window was created hidden so show it.
    ShowWindow( window );
    
    // Call the event loop
    RunApplicationEventLoop();

CantCreateWindow:
CantSetMenuBar:
CantGetNibRef:
	return err;
}

