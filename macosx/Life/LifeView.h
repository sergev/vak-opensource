/* LifeView */

#import <Cocoa/Cocoa.h>

#define NX 50
#define NY 30
#define CW 10
#define CH 10

@interface LifeView : NSView
{
	unsigned char cell [NX] [NY];
	unsigned char changed [NX] [NY];
}

/* Game control */
- (void) step;
@end
