/* Segment */

#import <Cocoa/Cocoa.h>

@interface Segment : NSObject
{
	NSPoint from;
	NSPoint to;
	int color;
}
- initWithColor: (int) color 
	   from: (NSPoint) from 
	     to: (NSPoint) to;
- (void) drawPath: (NSBezierPath*) path;
@end
