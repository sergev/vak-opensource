#import "Segment.h"

@implementation Segment

- initWithColor: (int) c 
	   from: (NSPoint) f
	     to: (NSPoint) t
{
	if (! (self = [super init])) 
		return nil;
	color = c;
	from = f;
	to = t;
	return self;
}

- (void) drawPath: (NSBezierPath*) path
{
	switch (color) {
	case 0: [[NSColor blackColor] set]; break;
	case 1: [[NSColor blueColor] set]; break;
	case 2: [[NSColor greenColor] set]; break;
	case 3: [[NSColor cyanColor] set]; break;
	case 4: [[NSColor redColor] set]; break;
	case 5: [[NSColor magentaColor] set]; break;
	case 6: [[NSColor brownColor] set]; break;
	case 7: [[NSColor grayColor] set]; break;
	}
	[path moveToPoint: from];
	[path lineToPoint: to];
//	printf ("%d:%d - %d:%d - %d\n", 
//		from.x, from.y, to.x, to.y, color);
}
@end
