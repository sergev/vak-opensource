// Pure C++ implementation using Objective-C++ for AppKit
#import <AppKit/AppKit.h>
#import <CoreFoundation/CoreFoundation.h>
#import <QuartzCore/QuartzCore.h>

#include <iostream>
#include <memory>

#include "metal_renderer.h"

// C++ wrapper for MetalView using Objective-C runtime
class MetalView {
private:
    void *objcView;      // NSView* wrapped
    void *cppMetalLayer; // CA::MetalLayer*
    void *renderer;      // MetalRenderer*
    void *renderTimer;   // NSTimer*

    static void renderCallback(CFRunLoopTimerRef timer, void *info)
    {
        MetalView *self = static_cast<MetalView *>(info);
        if (self->renderer) {
            renderMetalRenderer(self->renderer);
        }
    }

public:
    MetalView(double x, double y, double width, double height)
    {
        @autoreleasepool {
            // Create CA::MetalLayer - the same pointer works for both C++ and Objective-C
            void *layerPtr           = createMetalLayer();
            cppMetalLayer            = layerPtr;                          // CA::MetalLayer*
            CAMetalLayer *metalLayer = (__bridge CAMetalLayer *)layerPtr; // CAMetalLayer*

            // Create NSView
            NSRect frame    = NSMakeRect(x, y, width, height);
            NSView *view    = [[NSView alloc] initWithFrame:frame];
            view.wantsLayer = YES;
            view.layer      = metalLayer;

            CGSize size = { (CGFloat)width, (CGFloat)height };
            setMetalLayerDrawableSize(cppMetalLayer, width, height);

            // Create renderer with CA::MetalLayer
            renderer = createMetalRenderer(cppMetalLayer);

            // Create timer using CFRunLoopTimer (C API) instead of NSTimer
            CFRunLoopTimerContext context = { 0, this, nullptr, nullptr, nullptr };
            CFRunLoopTimerRef timer =
                CFRunLoopTimerCreate(kCFAllocatorDefault, CFAbsoluteTimeGetCurrent() + 1.0 / 60.0,
                                     1.0 / 60.0, 0, 0, renderCallback, &context);
            CFRunLoopAddTimer(CFRunLoopGetCurrent(), timer, kCFRunLoopCommonModes);
            renderTimer = timer;

            [view retain]; // Manual retain since we're not using ARC
            objcView = (__bridge void *)view;
        }
    }

    ~MetalView()
    {
        @autoreleasepool {
            if (renderTimer) {
                CFRunLoopTimerInvalidate((CFRunLoopTimerRef)renderTimer);
                CFRelease((CFRunLoopTimerRef)renderTimer);
            }
            if (renderer) {
                destroyMetalRenderer(renderer);
            }
            if (cppMetalLayer) {
                releaseMetalLayer(cppMetalLayer);
            }
            if (objcView) {
                NSView *view = (__bridge NSView *)objcView;
                [view release];
            }
        }
    }

    void *getNSView() const { return objcView; }

    void viewDidMoveToWindow(double width, double height)
    {
        if (cppMetalLayer) {
            setMetalLayerDrawableSize(cppMetalLayer, width, height);
        }
    }
};

// C++ Application class
class MetalApplication {
private:
    NSApplication *app;
    NSWindow *window;
    std::unique_ptr<MetalView> metalView;

public:
    MetalApplication() : app(nullptr), window(nullptr)
    {
        @autoreleasepool {
            app = [NSApplication sharedApplication];
            [app setActivationPolicy:NSApplicationActivationPolicyRegular];
        }
    }

    void createWindow(double x, double y, double width, double height, const char *title)
    {
        @autoreleasepool {
            NSRect frame = NSMakeRect(x, y, width, height);
            window       = [[NSWindow alloc]
                initWithContentRect:frame
                          styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                                    NSWindowStyleMaskMiniaturizable
                            backing:NSBackingStoreBuffered
                              defer:NO];
            [window setTitle:[NSString stringWithUTF8String:title]];
            [window center];

            metalView = std::make_unique<MetalView>(0, 0, width, height);
            [window setContentView:(__bridge NSView *)metalView->getNSView()];
        }
    }

    void run()
    {
        @autoreleasepool {
            [window makeKeyAndOrderFront:nil];
            [app activateIgnoringOtherApps:YES];
            [app run];
        }
    }

    ~MetalApplication()
    {
        @autoreleasepool {
            if (window) {
                [window release];
            }
        }
    }
};

int main(int argc, const char *argv[])
{
    @autoreleasepool {
        MetalApplication app;
        app.createWindow(0, 0, 800, 600, "Metal-cpp Triangle Demo");
        app.run();
    }
    return 0;
}
