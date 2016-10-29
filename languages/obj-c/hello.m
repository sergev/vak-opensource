#import <objc/objc.h>
#import <objc/Object.h>

@interface MyClass : Object
- (void) sayHello;
@end

@implementation MyClass
- (void) sayHello
{
    printf("Hello world!\n");
}
@end

int main()
{
    MyClass *anObject = [MyClass alloc];

    [anObject sayHello];
    return 0;
}
