/* Graphics */

#import <Cocoa/Cocoa.h>

@interface Graphics : NSView
{
	NSMutableArray *lines;
}
- (void) clear;
- (void) addLineWithColor: (int) color 
		     from: (NSPoint) from 
		       to: (NSPoint) to;
- (void) drawRect: (NSRect) rect;
@end
