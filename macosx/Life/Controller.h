/* Controller */

#import <Cocoa/Cocoa.h>
#import "LifeView.h"

@interface Controller : NSObject
{
	IBOutlet NSTextField *age;
	IBOutlet LifeView *screen;
	int age_counter;
}
- (IBAction)step:(id)sender;
@end
