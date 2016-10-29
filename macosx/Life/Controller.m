#import "Controller.h"

@implementation Controller

- (IBAction)step:(id)sender
{
	++age_counter;
	[age setStringValue: [NSString stringWithFormat: @"Age: %d", 
		age_counter]];
	[screen step];
}

@end
