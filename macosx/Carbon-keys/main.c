//
//  main.c
//  Carbon-keys
//
//  Created by Serge Vakulenko on 12/02/2006.
//
#include <Carbon/Carbon.h>

static OSStatus callback_key_event (EventHandlerCallRef handler,
	EventRef event, void *data)
{
	WindowRef window = (WindowRef) data;
	EventRef keyboard_event;
	ControlRef text_ref;
	CFMutableStringRef text_string;
	unsigned long modifiers, keycode, text_size;
	unsigned short text [20];
	int i;
	const ControlID text_id = { 'TEXT', 128 };

	if (GetEventParameter (event,
		kEventParamTextInputSendKeyboardEvent,
		typeEventRef, NULL, sizeof (keyboard_event),
		NULL, &keyboard_event) != noErr) {
		printf ("cannot get keyboard event\n");
		return noErr;
	}
	if (GetEventParameter (keyboard_event, kEventParamKeyModifiers,
		typeUInt32, NULL, sizeof (modifiers),
		NULL, &modifiers) != noErr) {
		printf ("cannot get key modifiers\n");
		return noErr;
	}
	if (GetEventParameter (keyboard_event, kEventParamKeyCode,
		typeUInt32, NULL, sizeof (typeUInt32),
		NULL, &keycode) != noErr) {
		printf ("cannot get key code\n");
		return noErr;
	}
	if (GetEventParameter (event, kEventParamTextInputSendText,
		typeUnicodeText, NULL, 0, &text_size, NULL) != noErr) {
		printf ("cannot get input text size\n");
		return noErr;
	}
	if (GetEventParameter (event, kEventParamTextInputSendText,
		typeUnicodeText, NULL, text_size, NULL, text) != noErr) {
		printf ("cannot get input text\n");
		return noErr;
	}
	if (GetControlByID (window, &text_id, &text_ref) != noErr) {
		printf ("cannot get control id\n");
		return noErr;
	}

	// Build output string. 
	text_string = CFStringCreateMutable (0, 0);
	CFStringAppendFormat (text_string, 0, CFSTR("(%x"), keycode);
	if (modifiers) {
		CFStringAppend (text_string, CFSTR(" +"));
		if (modifiers & 0x10000) 
			CFStringAppend (text_string, CFSTR(" Num"));
		if (modifiers & 0x00400) 
			CFStringAppend (text_string, CFSTR(" Caps"));
		if (modifiers & 0x00800) 
			CFStringAppend (text_string, CFSTR(" Alt"));
		if (modifiers & 0x01000) 
			CFStringAppend (text_string, CFSTR(" Ctrl"));
		if (modifiers & 0x00200) 
			CFStringAppend (text_string, CFSTR(" Shift"));
		if (modifiers & 0x00100) 
			CFStringAppend (text_string, CFSTR(" Cmd"));
	}
	CFStringAppend (text_string, CFSTR(") ->"));
	for (i=0; i<text_size; i+=2)
		CFStringAppendFormat (text_string, 0, CFSTR(" %02x"), text[i]);
	CFStringAppend (text_string, CFSTR("\n"));

	// Draw the text. 
	SetControlData (text_ref, 0, kControlStaticTextCFStringTag, 
		sizeof (text_string), &text_string);
    CFRelease (text_string);
    DrawOneControl (text_ref);
	return noErr;
}

int main(int argc, char* argv[])
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

	EventTypeSpec event_spec =
		{ kEventClassTextInput, kEventTextInputUnicodeForKeyEvent };

	InstallEventHandler (GetWindowEventTarget (window),
		NewEventHandlerUPP (callback_key_event),
		1, &event_spec, (void*) window, NULL);

	 // The window was created hidden so show it.
	ShowWindow( window );

	// Call the event loop
	RunApplicationEventLoop();

CantCreateWindow:
CantSetMenuBar:
CantGetNibRef:
	return err;
}
