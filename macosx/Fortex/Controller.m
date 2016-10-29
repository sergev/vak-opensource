#import "Controller.h"

@implementation Controller

static Controller *cocoa_controller;

void cocoa_putc (int c)
{
	unichar sym = c;

//	NSAlert *alert = [NSAlert alertWithMessageText:@"putc."
//		defaultButton: @"OK" alternateButton: nil
//		otherButton: nil informativeTextWithFormat:
//		@"'%c'",  c];
//	[alert runModal];
//	[alert release];
	[[[cocoa_controller->output_panel textStorage]
		mutableString] appendString:
		[NSString stringWithCharacters: &sym length: 1]];
}

int cocoa_getc (void)
{
	return [cocoa_controller getChar];
}

int main (int argc, char *argv[])
{
	return NSApplicationMain (argc, (const char **) argv);
}

void cocoa_grinit (int *maxx, int *maxy)
{
	NSRect rect = [cocoa_controller->graph_panel frame];

	*maxx = (int) (rect.size.width - 0.5);
	*maxy = (int) (rect.size.height - 0.5);
	[cocoa_controller->graph_panel clear];
}

void cocoa_grclear (void)
{
	[cocoa_controller->graph_panel clear];
}

void cocoa_grline (int xfrom, int yfrom, int xto, int yto, int c)
{
	[cocoa_controller->graph_panel addLineWithColor: c
		from: NSMakePoint (xfrom, yfrom)
		to: NSMakePoint (xto, yto)];
}

- (int)getChar
{
	while (input_index >= input_length) {
		[NSThread sleepUntilDate:
			[NSDate dateWithTimeIntervalSinceNow: 0.05]];
	}
//	NSAlert *alert = [NSAlert alertWithMessageText:@"getc."
//		defaultButton: @"OK" alternateButton: nil
//		otherButton: nil informativeTextWithFormat:
//		@"Return '%c'.",  [input_text characterAtIndex: input_index]];
//	[alert runModal];
//	[alert release];

	return [input_text characterAtIndex: input_index++];
}

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification
{
	cocoa_controller = self;
	input_text = [[NSMutableString alloc] init];
	[graph_panel clear];
	[NSThread detachNewThreadSelector: @selector (runForth:)
		toTarget: self withObject: nil];
}

- (void)runForth:(id)arg
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
//	NSAlert *alert = [NSAlert alertWithMessageText:@"Running in new thread."
//		defaultButton: @"OK" alternateButton: nil
//		otherButton: nil informativeTextWithFormat:
//		@"Starting forth."];
//	[alert runModal];
//	[alert release];

// printf ("pwd = '%s'\n", getcwd (0, 0));
	forth_main (1, 0);

	[pool release];
	[NSThread exit];
}

- (IBAction)action_input:(id)sender
{
	while (input_index < input_length) {
		[NSThread sleepUntilDate:
			[NSDate dateWithTimeIntervalSinceNow: 0.05]];
	}
	[input_text setString: [input_panel stringValue]];
	if ([input_text compare: @""] != 0) {
		[input_panel setStringValue: @""];
		[input_text appendString: @"\n"];
		[[[cocoa_controller->output_panel textStorage]
			mutableString] appendString: input_text];
	}
	input_length = [input_text length];
	input_index = 0;
}
@end
