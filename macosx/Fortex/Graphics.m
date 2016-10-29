#import "Graphics.h"
#import "Segment.h"

@implementation Graphics

- (void) clear
{
	if (! lines) 
		lines = [[[NSMutableArray alloc] init] retain];
	[lines removeAllObjects];
//	[self setNeedsDisplay: YES];
	[self display];
}

- (void) addLineWithColor: (int) color 
		     from: (NSPoint) from 
		       to: (NSPoint) to
{
	Segment *segm = [[Segment alloc] initWithColor: color 
		from: from to: to];
	
	[lines addObject: segm];
	[segm release];
//	[self setNeedsDisplay: YES];
	[self display];
}

- (void) drawRect: (NSRect) rect
{
//	[[NSColor blackColor] set];
//	NSRectFill (rect);	
	NSEraseRect (rect);
	
	NSEnumerator *enumerator = [lines objectEnumerator];
	Segment *segm;
	
	while (segm = [enumerator nextObject]) {
		NSBezierPath *path = [NSBezierPath bezierPath];
		[segm drawPath: path];
		[path stroke];
	}	
}
@end
