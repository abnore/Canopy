#import <Cocoa/Cocoa.h>
#import "canopy.h"

//----------------------------------------
// Struct to hold macOS window internals
//----------------------------------------
struct canopy_window {
    id window;
    id view;
    id delegate;
    framebuffer fb;
    bool should_close;
};

//----------------------------------------
// Window Delegate
//----------------------------------------
@interface CanopyDelegate : NSObject <NSWindowDelegate> {
    canopy_window* window;
}
- (instancetype)initWithCanopyWindow:(canopy_window*)initWindow;
@end

@implementation CanopyDelegate

- (instancetype)initWithCanopyWindow:(canopy_window*)initWindow {
    TRACE("Creating CanopyDelegate");
    self = [super init];
    if (self) {
        window = initWindow;
        DEBUG("Window pointer assigned to delegate: %p", window);
    }
    return self;
}

- (BOOL)windowShouldClose:(id)sender {
    INFO("Window close requested");
    window->should_close = true;
    return NO;
}

- (void)showCustomAboutPanel:(id)sender {
    INFO("Displaying About panel");

    NSImage* icon = [[NSImage alloc] initWithContentsOfFile:@"assets/icon.svg"];
    if (!icon) {
        WARN("Could not load custom icon, falling back to default");
    }

    NSDictionary* options = @{
        @"ApplicationName": @"Canopy",
        @"ApplicationVersion": @"0.1.0",
        @"ApplicationIcon": icon ?: [NSImage imageNamed:NSImageNameApplicationIcon],
        @"Copyright": @"© 2025 Canopy",
        @"Credits": [[NSAttributedString alloc]
                    initWithString:@"Built with Maria using Cocoa and C"]
    };

    [NSApp orderFrontStandardAboutPanelWithOptions:options];
    TRACE("About panel shown");
}
@end

//----------------------------------------
// View
//----------------------------------------
@interface CanopyView : NSView {
    canopy_window* window;
}
- (instancetype)initWithFrame:(NSRect)frame window:(canopy_window*)win;
- (void)setRenderColor:(color)color;
- (color)getRenderColor;
@end

@implementation CanopyView

- (instancetype)initWithFrame:(NSRect)frame window:(canopy_window*)win {
    TRACE("Initializing CanopyView with frame (%d, %d)", (int)frame.size.width, (int)frame.size.height);
    self = [super initWithFrame:frame];
    if (self) {
        window = win;
        window->fb.clear_color = CANOPY_DARK_GRAY;
        INFO("Default background color set: %s", color_to_string(CANOPY_DARK_GRAY));
    }
    return self;
}

- (BOOL)isFlipped { return YES; }
- (BOOL)acceptsFirstResponder { return YES; }

- (void)setRenderColor:(color)color {
    TRACE("Setting render color: %s", color_to_string(color));
    window->fb.clear_color = color;
    [self setNeedsDisplay:YES];
}

- (color)getRenderColor {
    return window->fb.clear_color;
}


//----------------------------------------
// Standard mouse event handler
// so that i can add as many as i want later
//----------------------------------------
- (void)pushMouseEventWithAction:(canopy_mouse_action)action
                           event:(NSEvent *)event
                         scrollX:(float)sx
                         scrollY:(float)sy
{
    NSPoint pos = [self convertPoint:[event locationInWindow] fromView:nil];

    canopy_event e;
    e.type = CANOPY_EVENT_MOUSE;
    e.mouse.action = action;
    e.mouse.x = (int)pos.x;
    e.mouse.y = (int)pos.y;
    e.mouse.button = (int)[event buttonNumber];
    e.mouse.modifiers = (int)[event modifierFlags];
    e.mouse.scroll_x = sx;
    e.mouse.scroll_y = sy;

    // Safe call only for click-related events
    if (action == CANOPY_MOUSE_PRESS ||
        action == CANOPY_MOUSE_RELEASE ||
        action == CANOPY_MOUSE_DRAG) {
        e.mouse.click_count = (int)[event clickCount];
    } else {
        e.mouse.click_count = 0;
    }

    canopy_push_event(e);
}

/* ------------  Press events ------------*/
- (void)mouseDown:(NSEvent *)event {
    [self pushMouseEventWithAction:CANOPY_MOUSE_PRESS event:event scrollX:0 scrollY:0];
}
- (void)rightMouseDown:(NSEvent *)event {
    [self pushMouseEventWithAction:CANOPY_MOUSE_PRESS event:event scrollX:0 scrollY:0];
}
- (void)otherMouseDown:(NSEvent *)event {
    [self pushMouseEventWithAction:CANOPY_MOUSE_PRESS event:event scrollX:0 scrollY:0];
}
/* ------------ Release events ------------*/
- (void)mouseUp:(NSEvent *)event {
    [self pushMouseEventWithAction:CANOPY_MOUSE_RELEASE event:event scrollX:0 scrollY:0];
}
- (void)rightMouseUp:(NSEvent *)event {
    [self pushMouseEventWithAction:CANOPY_MOUSE_RELEASE event:event scrollX:0 scrollY:0];
}
- (void)otherMouseUp:(NSEvent *)event {
    [self pushMouseEventWithAction:CANOPY_MOUSE_RELEASE event:event scrollX:0 scrollY:0];
}
/* ------------Drag/move events ------------*/
- (void)mouseDragged:(NSEvent *)event {
    [self pushMouseEventWithAction:CANOPY_MOUSE_DRAG event:event scrollX:0 scrollY:0];
}
- (void)rightMouseDragged:(NSEvent *)event {
    [self pushMouseEventWithAction:CANOPY_MOUSE_DRAG event:event scrollX:0 scrollY:0];
}
- (void)otherMouseDragged:(NSEvent *)event {
    [self pushMouseEventWithAction:CANOPY_MOUSE_DRAG event:event scrollX:0 scrollY:0];
}
- (void)mouseMoved:(NSEvent *)event {
    [self pushMouseEventWithAction:CANOPY_MOUSE_MOVE event:event scrollX:0 scrollY:0];
}
/* ------------Scroll events ------------ */
- (void)scrollWheel:(NSEvent *)event {
    [self pushMouseEventWithAction:CANOPY_MOUSE_SCROLL
                            event:event
                         scrollX:[event scrollingDeltaX]
                         scrollY:[event scrollingDeltaY]];
}

// To receive mouse entered and exit we setup a tracking area
- (void)updateTrackingAreas {
    [super updateTrackingAreas];
    NSTrackingAreaOptions opts =
        NSTrackingMouseEnteredAndExited | NSTrackingActiveInKeyWindow;

    NSTrackingArea* area = [[NSTrackingArea alloc]
                            initWithRect:[self bounds]
                                 options:opts
                                   owner:self
                                userInfo:nil];

    [self addTrackingArea:area];
}

- (void)mouseEntered:(NSEvent *)event {
    [self pushMouseEventWithAction:CANOPY_MOUSE_ENTER event:event scrollX:0 scrollY:0];
}

- (void)mouseExited:(NSEvent *)event {
    [self pushMouseEventWithAction:CANOPY_MOUSE_EXIT event:event scrollX:0 scrollY:0];
}

/* ------------ Key events ------------ */
- (void)pushKeyEventWithAction:(canopy_key_action)action event:(NSEvent *)event {
    canopy_event e;
    e.type = CANOPY_EVENT_KEY;
    e.key.action = action;
    e.key.keycode = (int)[event keyCode];
    e.key.modifiers = (int)[event modifierFlags];
    e.key.is_repeat = [event isARepeat] ? 1 : 0;
    canopy_push_event(e);
}

- (void)keyDown:(NSEvent *)event {
    [self pushKeyEventWithAction:CANOPY_KEY_PRESS event:event];
}

- (void)keyUp:(NSEvent *)event {
    [self pushKeyEventWithAction:CANOPY_KEY_RELEASE event:event];
}
@end

//----------------------------------------
// Menubar Setup - internal
// 	Taken from GLFW as this
// 	is nasty stuff
//----------------------------------------
static void create_menubar(id delegate)
{
    NSString* appName = [[NSProcessInfo processInfo] processName];

    // Top-level menubar
    NSMenu* menubar = [[NSMenu alloc] init];
    [NSApp setMainMenu:menubar];

    // App menu item (blank title)
    NSMenuItem* appMenuItem = [menubar addItemWithTitle:@"" action:NULL keyEquivalent:@""];

    // Application menu
    NSMenu* appMenu = [[NSMenu alloc] init];

    // About
    NSMenuItem* aboutItem = [[NSMenuItem alloc]
	    initWithTitle:[NSString stringWithFormat:@"About %@", appName]
		   action:@selector(showCustomAboutPanel:)
	    keyEquivalent:@""];
    [aboutItem setTarget:delegate];
    [appMenu addItem:aboutItem];

    [appMenu addItem:[NSMenuItem separatorItem]];

    // Services
    NSMenu* servicesMenu = [[NSMenu alloc] init];
    [NSApp setServicesMenu:servicesMenu];
    [[appMenu addItemWithTitle:@"Services" action:NULL keyEquivalent:@""] setSubmenu:servicesMenu];

    [appMenu addItem:[NSMenuItem separatorItem]];

    // Hide, Hide Others, Show All
    [appMenu addItemWithTitle:[NSString stringWithFormat:@"Hide %@", appName]
                       action:@selector(hide:)
                keyEquivalent:@"h"];
    [[appMenu addItemWithTitle:@"Hide Others"
                        action:@selector(hideOtherApplications:)
                 keyEquivalent:@"h"]
        setKeyEquivalentModifierMask:(NSEventModifierFlagOption | NSEventModifierFlagCommand)];

    [appMenu addItemWithTitle:@"Show All"
                       action:@selector(unhideAllApplications:)
                keyEquivalent:@""];

    [appMenu addItem:[NSMenuItem separatorItem]];

    // Quit
    [appMenu addItemWithTitle:[NSString stringWithFormat:@"Quit %@", appName]
                       action:@selector(terminate:)
                keyEquivalent:@"q"];

    [appMenuItem setSubmenu:appMenu];

    // Cocoa semi-private fix to set this as the real app menu
    SEL setAppleMenuSelector = NSSelectorFromString(@"setAppleMenu:");
    if ([NSApp respondsToSelector:setAppleMenuSelector]) {
        [NSApp performSelector:setAppleMenuSelector withObject:appMenu];
    }

    // Add "Window" menu for things like Minimize/Zoom/Fullscreen
    NSMenuItem* windowMenuItem = [menubar addItemWithTitle:@"" action:NULL keyEquivalent:@""];
    NSMenu* windowMenu = [[NSMenu alloc] initWithTitle:@"Window"];
    [windowMenuItem setSubmenu:windowMenu];
    [NSApp setWindowsMenu:windowMenu];

    [windowMenu addItemWithTitle:@"Minimize"
                          action:@selector(performMiniaturize:)
                   keyEquivalent:@"m"];
    [windowMenu addItemWithTitle:@"Zoom"
                          action:@selector(performZoom:)
                   keyEquivalent:@""];
    [windowMenu addItem:[NSMenuItem separatorItem]];
    [windowMenu addItemWithTitle:@"Bring All to Front"
                          action:@selector(arrangeInFront:)
                   keyEquivalent:@""];
    [windowMenu addItem:[NSMenuItem separatorItem]];
    [[windowMenu addItemWithTitle:@"Enter Full Screen"
                           action:@selector(toggleFullScreen:)
                    keyEquivalent:@"f"]
     setKeyEquivalentModifierMask:NSEventModifierFlagControl | NSEventModifierFlagCommand];
}

//--------------------------------------------------------------------------------
// Public API Implementation - C Wrappers
//--------------------------------------------------------------------------------

/* Window functions */
canopy_window* canopy_create_window(int width, int height, const char* title)
{
    @autoreleasepool {
        [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        [NSApp activateIgnoringOtherApps:YES];

        TRACE("Creating window: %dx%d \"%s\"", width, height, title);

        canopy_window* win = canopy_malloc(sizeof(canopy_window));

        if(!win) {
            FATAL("Failed to allocate canopy_window");
            return NULL;

        }

        win->delegate = [[CanopyDelegate alloc]
                            initWithCanopyWindow:win];

        win->view = [[CanopyView alloc]
                            initWithFrame:NSMakeRect(0, 0, width, height)
                                   window:win];

        win->window = [[NSWindow alloc]
                         initWithContentRect:NSMakeRect(0, 0, width, height)
                                   styleMask:CANOPY_WINDOW_STYLE_TITLED | CANOPY_WINDOW_STYLE_CLOSABLE
                                     backing:NSBackingStoreBuffered
                                       defer:NO];

        create_menubar(win->delegate);

        [(NSWindow*)win->window center];
        [win->window setTitle:[NSString stringWithUTF8String:title]];
        [win->window setDelegate:win->delegate];
        [win->window setContentView:win->view];
        [win->window makeKeyAndOrderFront:nil];
        [win->window setAcceptsMouseMovedEvents:YES];
        [win->window makeFirstResponder:win->view];

        [win->view setWantsLayer:YES];
        [win->view setOpaque:YES];

        CALayer* layer = [win->view layer];
        [layer setOpaque:YES];

        canopy_init_framebuffer(win);

        win->should_close = false;

        INFO("Created window: \"%s\" (%dx%d)", title, width, height);

        return win;
    }
}

void canopy_set_icon(const char* filepath)
{
    if (!filepath) {
        WARN("No icon filepath provided");
        return;
    };


    NSString* path = [NSString stringWithUTF8String:filepath];
    NSImage* icon = [[NSImage alloc] initWithContentsOfFile:path];

    if (icon) {
        [NSApp setApplicationIconImage:icon];
        INFO("Set application icon from path: %s", filepath);
    } else {
        ERROR("Failed to load icon from path: %s", filepath);
    }
}

void canopy_free_window(canopy_window* win)
{
    if (!win) {
        WARN("Tried to free a NULL window");
        return;
    }

    @autoreleasepool {
        TRACE("Freeing canopy window");
        // Hide the window
        [win->window orderOut:nil];

        // Disconnect delegate and release
        [win->window setDelegate:nil];
        [win->delegate release];
        win->delegate = nil;

        // Release the view
        [win->view release];
        win->view = nil;

        // Close and clear the window object
        [win->window close];
        win->window = nil;

        // Free framebuffer
        if (win->fb.pixels) {
            canopy_free(win->fb.pixels);
            win->fb.pixels= NULL;
        }

        // (Optional) Let Cocoa flush pending events
        canopy_pump_events();

        DEBUG("Window closed and resources cleaned up");
    }

    canopy_free(win);
}


bool canopy_window_should_close(canopy_window *window)
{
    canopy_pump_events();  // Keep the UI alive
    return window->should_close;
}

bool canopy_init_framebuffer(canopy_window *win)
{
    if(win->fb.pixels == NULL)
    {
        NSView* view = (NSView*)win->view;
        NSRect bounds = [view bounds];

        win->fb.width = (int)bounds.size.width;
        win->fb.height = (int)bounds.size.height;
        win->fb.pitch = win->fb.width * CANOPY_BYTES_PER_PIXEL;

        if (win->fb.width <= 0 || win->fb.height <= 0)
        {
            ERROR("Invalid framebuffer size: %dx%d\n",
                        win->fb.width, win->fb.height);
            return false;
        }
        // Allocate buffer to match window size
        win->fb.pixels = canopy_malloc(win->fb.pitch * win->fb.height);
        if (!win->fb.pixels) {
            FATAL("Failed to allocate framebuffer");
            return false;
        }
        TRACE("Initialized framebuffer: %dx%d (pitch %d)",
              win->fb.width, win->fb.height, win->fb.pitch);

    }

    return true;
}

void canopy_clear_buffer(canopy_window* win)
{

    if(win->fb.pixels == NULL)
    {
        WARN("Framebuffer was uninitialized, reinitializing...");
        canopy_init_framebuffer(win);
    }

    // Get the render color (background color) from the view
    color c = [win->view getRenderColor];

    // Clear the framebuffer by filling it with the render color
    for (int i = 0; i < win->fb.width * win->fb.height; ++i) {
        win->fb.pixels[i] = color_to_u32(c);
    }
}

void canopy_present_buffer(canopy_window *window)
{
    @autoreleasepool {
        if (!window->fb.pixels) {
            ERROR("Tried to present a NULL framebuffer");
            return;
        }

        NSBitmapImageRep *rep = [[[NSBitmapImageRep alloc]
                initWithBitmapDataPlanes: (uint8_t**)&window->fb.pixels
                              pixelsWide: window->fb.width
                              pixelsHigh: window->fb.height
                           bitsPerSample: 8
                         samplesPerPixel: 4
                                hasAlpha: YES
                                isPlanar: NO
                          colorSpaceName: NSDeviceRGBColorSpace
                             bytesPerRow: window->fb.pitch
                            bitsPerPixel: 32]
                            autorelease];

        NSImage *image = [[[NSImage alloc]
                            initWithSize: NSMakeSize(window->fb.width,
                                                    window->fb.height)]
                            autorelease];

        [image addRepresentation: rep];
        [(NSView*)window->view layer].contents = image;
        //TRACE("Framebuffer presented to screen");
    }
}


framebuffer *canopy_get_framebuffer(canopy_window *window)
{
    return &window->fb;
}
void canopy_set_buffer_refresh_color(canopy_window *w, color c)
{
    DEBUG("Setting buffer clear color to: %s", color_to_string(c));
    [w->view setRenderColor:c];
}

void canopy_swap_backbuffer(canopy_window *w, framebuffer *backbuffer)
{
    if (!backbuffer || !backbuffer->pixels) {
        ERROR("Backbuffer is NULL");
        return;
    }

    if (!w->fb.pixels) {
        ERROR("Framebuffer in window is NULL");
        return;
    }

    int copy_width  = (backbuffer->width  < w->fb.width)  ? backbuffer->width  : w->fb.width;
    int copy_height = (backbuffer->height < w->fb.height) ? backbuffer->height : w->fb.height;

    for (int y = 0; y < copy_height; ++y) {
        memcpy(&w->fb.pixels[y * w->fb.width],
               &backbuffer->pixels[y * backbuffer->width],
               copy_width * sizeof(uint32_t));
    }
    TRACE("Swapped backbuffer (%dx%d)", copy_width, copy_height);
}

/* Pump messages so that the window is shown to be responsive
 * Also needed for event handling
 * */

void canopy_pump_events(void)
{
    @autoreleasepool {
        NSEvent* event;
        while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                           untilDate:nil
                                              inMode:NSDefaultRunLoopMode
                                             dequeue:YES])) {
                                    [NSApp sendEvent:event];
                                    [NSApp updateWindows];
        }
    } // autoreleasepool
}
