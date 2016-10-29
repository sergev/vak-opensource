#import "LifeView.h"

@implementation LifeView

static int func (int x, int a, int b, int c, int d, int f, int g, int h, int i)
{
	int n;
	
	// New cell gets older.
	if (x == 1)
		return 2;
		
	n = (a!=0) + (b!=0) + (c!=0) + (d!=0) + (f!=0) + (g!=0) + (h!=0) + (i!=0);
	if (n == 3)
		return x ? 0 : 1;
	if (n == 2)
		return x;
	return 0;
}

- (id)initWithFrame:(NSRect)frameRect
{
	if ((self = [super initWithFrame:frameRect]) != nil) {
		// Add initialization code here
		cell [25] [15] = 2;
		cell [25] [16] = 2;
		cell [25] [14] = 2;
		cell [26] [15] = 1;
		cell [24] [16] = 1;
		cell [24] [14] = 1;
	}
	return self;
}

- (void) compute
{
	int x, y;
	int x1, x2, y1, y2;
	unsigned char new [NX] [NY];
	
	for (x=0; x<NX; ++x) {
		x1 = x - 1;
		if (x1 < 0)
			x1 += NX;
			
		x2 = x + 1;
		if (x2 >= NX)
			x2 -= NX;
			
		for (y=0; y<NY; ++y) {
			y1 = y - 1;
			if (y1 < 0)
				y1 += NY;
			
			y2 = y + 1;
			if (y2 >= NY)
				y2 -= NY;
			
			new [x] [y] = func (cell [x] [y], 
				cell [x1] [y1], cell [x] [y1], cell [x2] [y1], 
				cell [x1] [y], cell [x2] [y], 
				cell [x1] [y2], cell [x] [y2], cell [x2] [y2]);
				
			changed [x] [y] = (new [x] [y] != cell [x] [y]);
		}
	}	
	memcpy (cell, new, NX * NY);
}

- (void) redrawCellsAll: (BOOL) all
{
	NSRect r;
	int x, y;
    
	// Draw all cells.
	[[NSColor yellowColor] set];
	r.size.width = CW - 1;
	r.size.height = CH - 1;
	for (x=0; x<NX; ++x) {
		r.origin.x = 1 + x * CW;
		for (y=0; y<NY; ++y) {
			if (! all && ! changed [x] [y])
				continue;
				
			switch (cell [x] [y]) {
			case 0:
				[[NSColor blackColor] set];
				break;
			case 1:
			default:
				[[NSColor greenColor] set];
				break;
			case 2:
				[[NSColor yellowColor] set];
				break;
			}
			r.origin.y = 1 + y * CH;
			NSRectFill (r);
			changed [x] [y] = 0;
		}
	}
} 

- (void) drawGrid
{
	NSPoint p1, p2;
	float dash [2] = { 1, 2 };
	NSBezierPath *path;
	int x, y;
    
	// Draw grid.
	[[NSColor grayColor] set];
	path = [NSBezierPath bezierPath];
	[path setLineWidth: 1];
	[path setLineDash: dash count: 2 phase: 0];
	p1.y = 0;
	p2.y = NY * CH;
	for (x=0; x<NX; ++x) {
		p1.x = p2.x = 0.5 + x * CW;
		[path moveToPoint: p1];
		[path lineToPoint: p2];
	}
	p1.x = 0;
	p2.x = NX * CW;
	for (y=0; y<NY; ++y) {
		p1.y = p2.y = 0.5 + y * CH;
		[path moveToPoint: p1];
		[path lineToPoint: p2];
	}
	[path stroke];
}

- (void)drawRect:(NSRect)rect
{
	// Clear all field.
	[[NSColor blackColor] set];
	NSRectFill (rect);
	
	[self drawGrid];
	[self redrawCellsAll: YES];
} 

- (void) step 
{
	[self compute];
	[self setNeedsDisplay: YES];
	[self redrawCellsAll: NO];
}

@end
