/* Controller */

#import <Cocoa/Cocoa.h>
#import "Graphics.h"

@interface Controller : NSObject
{
	IBOutlet Graphics *graph_panel;
	IBOutlet NSTextField *input_panel;
	IBOutlet NSTextView *output_panel;
	NSMutableString *input_text;
	int input_length;
	int input_index;
}
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification;
- (void)runForth:(id)arg;
- (int)getChar;
- (IBAction)action_input:(id)sender;
@end
