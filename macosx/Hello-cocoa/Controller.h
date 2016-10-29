/* Controller */

#import <Cocoa/Cocoa.h>

@interface Controller : NSObject
{
    IBOutlet NSTextField *text_field;
}
- (IBAction)kick:(id)sender;
@end
