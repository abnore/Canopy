#import <Cocoa/Cocoa.h>
#import <blackbox.h>
#import "canopy.h"

//------------------------------------------------------------------------------
// Struct to hold macOS window internals
//------------------------------------------------------------------------------
struct canopy_window {
    id window;
    id view;
    id delegate;

    framebuffer fb;
    double pixel_ratio; // support of high spi/retina screen
    uint32_t width_points, height_points; // Docs say points not pixels
    double mouse_x, mouse_y;
    bool should_close;
    bool is_opaque;
    void *user_data; // support for passing data for callbacks

    void (*callback_key)(Window *, canopy_event_key*);
    void (*callback_text)(Window *, canopy_event_text*);
    void (*callback_mouse)(Window *, canopy_event_mouse*);
};

//------------------------------------------------------------------------------
// Window Delegate
//------------------------------------------------------------------------------
@interface canopy_delegate : NSObject <NSWindowDelegate>
{
    Window* window;
}

- (instancetype)init_canopy_window:(Window*)init_window;

@end
//------------------------------------------------------------------------------
@implementation canopy_delegate

- (instancetype)init_canopy_window:(Window*)init_window
{
    TRACE("Creating Canopy Delegate");
    self = [super init];
    if (self) {
        window = init_window;
        DEBUG("Window pointer assigned to delegate: %p", window);
    }
    return self;
}

- (BOOL)windowShouldClose:(id)sender
{
    INFO("Window close requested");
    window->should_close = true;
    return NO;
}

- (void)showCustomAboutPanel:(id)sender
{
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
//------------------------------------------------------------------------------
// View
//------------------------------------------------------------------------------
@interface canopy_view : NSView <NSTextInputClient>
{
    Window* window;
    NSMutableAttributedString* markedText;
}

- (instancetype)init_with_frame:(NSRect)frame
                         window:(Window*)win;

@end
//------------------------------------------------------------------------------
@implementation canopy_view

- (instancetype)init_with_frame:(NSRect)frame
                         window:(Window*)win
{
    TRACE("Initializing Canopy View with frame (%d, %d)",
         (int)frame.size.width, (int)frame.size.height);
    self = [super initWithFrame:frame];
    if (self) {
        window = win;
    }
    return self;
}
// AppKit overrides
- (BOOL)isFlipped { return YES; }
- (BOOL)acceptsFirstResponder { return YES; }
- (BOOL)isOpaque { return window->is_opaque;}
- (void)updateTrackingAreas
{ // To receive mouse entered and exit we setup a tracking area
    NSTrackingAreaOptions opts =  NSTrackingMouseEnteredAndExited |
                                  NSTrackingActiveInKeyWindow |
                                  NSTrackingEnabledDuringMouseDrag |
                                  NSTrackingInVisibleRect;


    NSTrackingArea* area = [[NSTrackingArea alloc]
                            initWithRect:[self bounds]
                                 options:opts
                                   owner:self
                                userInfo:nil];

    [self addTrackingArea:area];
    [super updateTrackingAreas];
}
//------------------------------------------------------------------------------
//Cookie-cutter automatic code inclusion. To get insertText to fire, we need to
//at least pretend implement these. Cocoa expects your view to pretend to
//support IME by implementing a handful of NSTextInputClient methods. Dont
//implement, just stub them out to “fake it.”
//------------------------------------------------------------------------------
- (nullable NSAttributedString *)attributedSubstringForProposedRange:(NSRange)ra
                actualRange:(nullable NSRangePointer)actualRange {return nil;}
- (NSUInteger)characterIndexForPoint:(NSPoint)point {return 0;}
- (void)doCommandBySelector:(nonnull SEL)selector{}
- (NSRect)firstRectForCharacterRange:(NSRange)ra
    actualRange:(nullable NSRangePointer)actualRange{ NSRect r = {}; return r;}
- (BOOL)hasMarkedText {return markedText.length > 0;}
- (NSRange)markedRange {return NSMakeRange(0, markedText.length);}
- (NSRange)selectedRange {NSRange r = {}; return r;}
- (void)setMarkedText:(nonnull id)string selectedRange:(NSRange)selectedRange
     replacementRange:(NSRange)replacementRange {}
- (void)unmarkText {}
- (nonnull NSArray<NSAttributedStringKey> *)validAttributesForMarkedText
    {return [NSArray array];}

// All the code above enables this code to fire. It sends a character per event
- (void)insertText:(nonnull id)string
  replacementRange:(NSRange)replacementRange
{
    NSString *character;

    // If attributed string, convert to plain string
    if ([string isKindOfClass:[NSAttributedString class]])
        character = [string string];
    else
        character = (NSString*)string;

    // Now `characters` holds the actual user-typed characters
    NSData *utf8data = [character dataUsingEncoding:NSUTF8StringEncoding];
    if (utf8data.length == 0 || utf8data.length > 4) return;

    canopy_event e = {0};
    e.type = CANOPY_EVENT_TEXT;
    memcpy(e.text.utf8, utf8data.bytes, utf8data.length);
    e.text.utf8[utf8data.length] = '\0'; // Null-terminate

    push_event(e);
}

//------------------------------------------------------------------------------
// Standard mouse event handler
// so that i can add as many as i want later
//------------------------------------------------------------------------------
- (void)push_mouse_event_with_action:(canopy_action_mouse)action
                               event:(NSEvent *)event
                             scrollX:(float)sx
                             scrollY:(float)sy
{
    NSPoint pos = [self convertPoint:[event locationInWindow]
                            fromView:nil];
    window->mouse_x = pos.x;
    window->mouse_y = pos.y;

    canopy_event e = {
        .type = CANOPY_EVENT_MOUSE,
        .mouse.action = action,
        .mouse.x = (int)pos.x,
        .mouse.y = (int)pos.y,
        .mouse.button = (int)[event buttonNumber],
        .mouse.modifiers = (int)[event modifierFlags],
        .mouse.scroll_x = sx,
        .mouse.scroll_y = sy,
    };

    // Safe call only for click-related events otherwise we trap
    switch(action){
        default: e.mouse.click_count = 0;
        break;
        case CANOPY_MOUSE_PRESS:
        case CANOPY_MOUSE_RELEASE:
        case CANOPY_MOUSE_DRAG:
                 e.mouse.click_count = (int)[event clickCount];
        break;
    }

    push_event(e);
}

/* ------------  Press events ------------*/
- (void)mouseDown:(NSEvent *)event {
    [self push_mouse_event_with_action:CANOPY_MOUSE_PRESS
                                 event:event
                               scrollX:0
                               scrollY:0];
}
- (void)rightMouseDown:(NSEvent *)event {
    [self push_mouse_event_with_action:CANOPY_MOUSE_PRESS
                                 event:event
                               scrollX:0
                               scrollY:0];
}
- (void)otherMouseDown:(NSEvent *)event {
    [self push_mouse_event_with_action:CANOPY_MOUSE_PRESS
                                 event:event
                               scrollX:0
                               scrollY:0];
}
/* ------------ Release events ------------*/
- (void)mouseUp:(NSEvent *)event {
    [self push_mouse_event_with_action:CANOPY_MOUSE_RELEASE
                                 event:event
                               scrollX:0
                               scrollY:0];
}
- (void)rightMouseUp:(NSEvent *)event {
    [self push_mouse_event_with_action:CANOPY_MOUSE_RELEASE
                                 event:event
                               scrollX:0
                               scrollY:0];
}
- (void)otherMouseUp:(NSEvent *)event {
    [self push_mouse_event_with_action:CANOPY_MOUSE_RELEASE
                                 event:event
                               scrollX:0
                               scrollY:0];
}
/* ----------- Drag/move events -----------*/
- (void)mouseDragged:(NSEvent *)event {
    [self push_mouse_event_with_action:CANOPY_MOUSE_DRAG
                                 event:event
                               scrollX:0
                               scrollY:0];
}
- (void)rightMouseDragged:(NSEvent *)event {
    [self push_mouse_event_with_action:CANOPY_MOUSE_DRAG
                                 event:event
                               scrollX:0
                               scrollY:0];
}
- (void)otherMouseDragged:(NSEvent *)event {
    [self push_mouse_event_with_action:CANOPY_MOUSE_DRAG
                                 event:event
                               scrollX:0
                               scrollY:0];
}
- (void)mouseMoved:(NSEvent *)event {
    [self push_mouse_event_with_action:CANOPY_MOUSE_MOVE
                                 event:event
                               scrollX:0
                               scrollY:0];
}
/* ------------ Scroll events ------------ */
- (void)scrollWheel:(NSEvent *)event {
    [self push_mouse_event_with_action:CANOPY_MOUSE_SCROLL
                            event:event
                         scrollX:[event scrollingDeltaX]
                         scrollY:[event scrollingDeltaY]];
}
/* -------- Enter and exit events -------- */
- (void)mouseEntered:(NSEvent *)event {
    [self push_mouse_event_with_action:CANOPY_MOUSE_ENTER
                                 event:event
                               scrollX:0
                               scrollY:0];
}

- (void)mouseExited:(NSEvent *)event {
    [self push_mouse_event_with_action:CANOPY_MOUSE_EXIT
                                 event:event
                               scrollX:0
                               scrollY:0];
}
/* ------------- Key events -------------- */
- (void)push_key_event_with_action:(canopy_action_key)action
                             event:(NSEvent *)event {
    canopy_event e = {
        .type = CANOPY_EVENT_KEY,
        .key.action = action,
        .key.keycode = (int)[event keyCode],
        .key.modifiers = (int)[event modifierFlags],
        .key.is_repeat = [event isARepeat] ? 1 : 0
    };

    push_event(e);
}

//- (void)keyDown:(NSEvent *)event {
//    [self push_key_event_with_action:CANOPY_KEY_PRESS event:event];
//}
// - (void)keyDown:(NSEvent *)event {
//     [self interpretKeyEvents:@[event]];
// }
// Combining them to get the text AND the key
- (void)keyDown:(NSEvent *)event {
    [self push_key_event_with_action:CANOPY_KEY_PRESS
                               event:event];
    [self interpretKeyEvents:@[event]];
}
- (void)keyUp:(NSEvent *)event {
    [self push_key_event_with_action:CANOPY_KEY_RELEASE
                               event:event];
}

- (void)flagsChanged:(NSEvent *)event {
    static NSEventModifierFlags prev_flags = 0;
    NSEventModifierFlags new_flags = [event modifierFlags];
    NSEventModifierFlags changed = prev_flags ^ new_flags;

    int keycode = (int)[event keyCode];
    canopy_action_key action = (new_flags & changed) ?
                CANOPY_KEY_PRESS : CANOPY_KEY_RELEASE;

    // Filter out bogus values
    if (changed == 0) return;

    canopy_event e = {
        .type = CANOPY_EVENT_KEY,
        .key.action = action,
        .key.keycode = keycode,
        .key.modifiers = (int)new_flags,
        .key.is_repeat = 0
    };

    push_event(e);
    prev_flags = new_flags;
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
    NSMenuItem* appMenuItem = [menubar addItemWithTitle:@""
                                                 action:NULL
                                          keyEquivalent:@""];

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
    [[appMenu addItemWithTitle:@"Services"
                        action:NULL
                 keyEquivalent:@""] setSubmenu:servicesMenu];
    [appMenu addItem:[NSMenuItem separatorItem]];
    // Hide, Hide Others, Show All
    [appMenu addItemWithTitle:[NSString stringWithFormat:@"Hide %@", appName]
                       action:@selector(hide:)
                keyEquivalent:@"h"];
    [[appMenu addItemWithTitle:@"Hide Others"
                        action:@selector(hideOtherApplications:)
                 keyEquivalent:@"h"] setKeyEquivalentModifierMask:
                 (NSEventModifierFlagOption |
                  NSEventModifierFlagCommand)];
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
    NSMenuItem* windowMenuItem = [menubar addItemWithTitle:@""
                                                    action:NULL
                                             keyEquivalent:@""];
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
                    keyEquivalent:@"f"] setKeyEquivalentModifierMask:
                    NSEventModifierFlagControl |
                    NSEventModifierFlagCommand];
}

//--------------------------------------------------------------------------------
// Public API Implementation - C Wrappers
//--------------------------------------------------------------------------------
static bool init_framebuffer(Window *window);
static void update_backing_metrics(Window *window);

/* Window functions */
Window* create_window(char* title, int width, int height, window_style flags)
{
    @autoreleasepool {
        [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        [NSApp activateIgnoringOtherApps:YES];

        TRACE("Creating window: %dx%d \"%s\"", width, height, title);

        Window* window = canopy_malloc(sizeof(Window));

        if(!window) {
            FATAL("Failed to allocate Window");
            return NULL;
        }

        window->width_points = width;
        window->height_points = height;
        window->pixel_ratio = 1.0;
        window->mouse_x = 0;
        window->mouse_y = 0;
        window->user_data = NULL;
        window->fb.pixels = NULL;
        window->fb.width = 0;
        window->fb.height = 0;
        window->fb.pitch = 0;
        window->delegate = [[canopy_delegate alloc] init_canopy_window:window];
        window->view = [[canopy_view alloc]
            init_with_frame: NSMakeRect(0, 0, width, height)
                     window: window];
        window->window = [[NSWindow alloc]
            initWithContentRect: NSMakeRect(0, 0, width, height)
                      styleMask: (NSWindowStyleMask)flags
                        backing: NSBackingStoreBuffered
                          defer: NO];

        create_menubar(window->delegate);

        [(NSWindow*)window->window center];
        [window->window setTitle: [NSString stringWithUTF8String:title]];
        [window->window setDelegate: window->delegate];
        [window->window setContentView: window->view];
        [window->window makeKeyAndOrderFront:nil];
        [window->window setAcceptsMouseMovedEvents: YES];


        [window->view setWantsLayer: YES];
        // default opaque, can be set manually
        //[window->view setOpaque: YES];
        window->is_opaque = true;
        // ensures the layer also is opaque - not needed
        //[[window->view layer] setOpaque:YES];

        if (!init_framebuffer(window)) {
            free_window(window);
            return NULL;
        }

        [window->window makeFirstResponder: window->view];
        window->should_close = false;

        INFO("Created window: \"%s\" (%dx%d)", title, width, height);

        post_empty_event();

        return window;
    }
}

void set_icon(const char* filepath)
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
void free_window(Window* window)
{
    if( !window ) {
        WARN("Tried to free a NULL window");
        return;
    }

    @autoreleasepool {
        TRACE("Freeing canopy window");
        [window->window orderOut:nil];
        [window->window setDelegate:nil];

        [window->delegate release];
        window->delegate = nil;
        [window->view release];
        window->view = nil;

        [window->window close];
        window->window = nil;

        if (window->fb.pixels) {
            canopy_free(window->fb.pixels);
            window->fb.pixels= NULL;
        }
        // (Optional) Let Cocoa flush pending events
        pump_messages();
        DEBUG("Window closed and resources cleaned up");
    }
    canopy_free(window);
}

void pump_messages(void)
{
    @autoreleasepool {
        NSEvent* event;

        while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                           untilDate:[NSDate distantPast]
                                              inMode:NSDefaultRunLoopMode
                                             dequeue:YES]))
        {
            [NSApp sendEvent:event];
        }
    } // autoreleasepool
}

double get_window_scale(Window *window)                     // 1.0, 2.0, etc
{
    return window->pixel_ratio;
}
void get_window_size(Window *window, int *w, int *h)       // points
{
    if (!window) return;
    *w = window->width_points;
    *h = window->height_points;
}

/*   <https://www.glfw.org/docs/3.3/group__window.html>
 * GLFW call it setWindowUserPointer, and it is a nice trick to funnel data
 * through to callbacks, without bloating the main window object.
 * The value you set is retained until its either overwritten, or the window
 * is destroyed. I call it data, as pointer is self evident, but its used for
 * data, through the window by the user
 * */
void set_window_user_data(Window *window, void *user_data)
{
    window->user_data = user_data;
}
void *get_window_user_data(Window *window)
{
    return window->user_data;
}
bool window_should_close(Window *window)
{
    // This is being done in poll_events now
//    pump_events();  // Keep the UI alive
    return window->should_close;
}
void set_window_should_close(Window *window)
{
    window->should_close = true;
}
bool is_window_opaque(Window *window)
{
    return [window->view isOpaque];
}
void set_window_transparent(Window *window, bool enable)
{
    if( enable ) {
        window->is_opaque = false;
        [window->window setOpaque:NO];
        [window->window setHasShadow:NO];
        [window->window setBackgroundColor:[NSColor clearColor]];
        TRACE("Window transparent");
    } else {
        window->is_opaque = true;
        [window->window setOpaque:YES];
        [window->window setHasShadow:YES];
        TRACE("Window opaque");
    }
}
framebuffer *get_framebuffer(Window *window)
{
    return &window->fb;
}
void get_framebuffer_size(Window *window, int *width, int *height) // pixels
{
    if (!window) return;
    *width = window->fb.width;
    *height = window->fb.height;
}
void present_buffer(Window *window)
{
    @autoreleasepool {
        if( !window->fb.pixels ) {
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
           initWithSize:NSMakeSize(window->width_points, window->height_points)]
           autorelease];

        [image addRepresentation: rep];
        [(NSView*)window->view layer].contents = image;
    }
}
void swap_backbuffer(Window *window, framebuffer *backbuffer)
{
    if( !backbuffer || !backbuffer->pixels ) {
        ERROR("Backbuffer is NULL");
        return;
    }

    if( !window->fb.pixels ) {
        ERROR("Framebuffer in window is NULL");
        return;
    }

    // Old way of copying byte for byte is too expensive when i scale
    // It is much more effecient to just swap pointers since the buffers
    // are equal
    //memcpy(w->fb.pixels, backbuffer->pixels, w->fb.height*w->fb.pitch);
    uint32_t *temp = window->fb.pixels;
    window->fb.pixels = backbuffer->pixels;
    backbuffer->pixels = temp;
}

/* Event system */

void dispatch_events(Window *w)
{
    canopy_event e;

    while (poll_event(&e))
    {
        switch (e.type) {
        case CANOPY_EVENT_NONE: break;

        case CANOPY_EVENT_KEY:
            if (w->callback_key) w->callback_key(w, &e.key);
            break;

        case CANOPY_EVENT_TEXT:
            if (w->callback_text) w->callback_text(w, &e.text);
            break;

        case CANOPY_EVENT_MOUSE:
            if (w->callback_mouse) w->callback_mouse(w, &e.mouse);
            break;
        }
    }
}

void set_callback_key(Window *w, callback_key cb) { w->callback_key = cb; }
void set_callback_text(Window *w, callback_text cb) { w->callback_text = cb; }
void set_callback_mouse(Window *w, callback_mouse cb) { w->callback_mouse = cb; }

void get_mouse_pos(Window *window, double *x, double *y)
{
    if( !window || !x || !y ) return;
    *x = window->mouse_x;
    *y = window->mouse_y;
}

void post_empty_event(void)
{
    @autoreleasepool {
        NSEvent* event = [NSEvent otherEventWithType:NSEventTypeApplicationDefined
                                            location:NSMakePoint(0, 0)
                                       modifierFlags:0
                                           timestamp:0
                                        windowNumber:0
                                             context:nil
                                             subtype:0
                                               data1:0
                                               data2:0];
        [NSApp postEvent:event atStart:YES];
    } // autoreleasepool
}

void wait_events(void)
{
    @autoreleasepool {
        NSEvent* event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                            untilDate:[NSDate distantFuture]
                                               inMode:NSDefaultRunLoopMode
                                              dequeue:YES];
        if (event) {
            [NSApp sendEvent:event];
            [NSApp updateWindows];
        }
    }
}
void wait_events_timeout(double timeout_seconds)
{
    @autoreleasepool {
        NSDate* timeout_date = [NSDate dateWithTimeIntervalSinceNow:timeout_seconds];

        NSEvent* event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                            untilDate:timeout_date
                                               inMode:NSDefaultRunLoopMode
                                              dequeue:YES];
        if (event) {
            [NSApp sendEvent:event];
            [NSApp updateWindows];
        }
    }
}

/* static helper function */

/* To support retina we need a conversion function from points to pixels.
 * The backing buffer needs to be in pixels, and if every points is 2*2 pixels,
 * we need to convert it. This will then check the scale factor and update the
 * ratio correctly */
static void update_backing_metrics(Window *window)
{
    NSView *view = (NSView *)window->view;
    NSRect bounds = [view bounds];
    NSRect backing = [view convertRectToBacking:bounds];

    window->pixel_ratio = [[view window] backingScaleFactor];
    window->fb.width = (uint32_t)backing.size.width;
    window->fb.height = (uint32_t)backing.size.height;

    if ([view layer]) {
        [[view layer] setContentsScale:window->pixel_ratio];
    }
}
/* Properly handle content scaling for fidelity display (i.e. retina display)
 *  <https://developer.apple.com/library/archive/documentation/
 *   GraphicsAnimation/Conceptual/HighResolutionOSX/Explained/Explained.html>
 *
 * We need to store and convert the difference in points and pixels, and alloc
 * the correct size */
static bool init_framebuffer(Window *window)
{
    if (!window)
        return false;

    if (window->fb.pixels == NULL) {
        update_backing_metrics(window);
        INFO("Content scale is: %.2f", window->pixel_ratio);

        window->fb.pitch = window->fb.width * CANOPY_BYTES_PER_PIXEL;

        if (window->fb.width == 0 || window->fb.height == 0) {
            ERROR("Invalid framebuffer size: %ux%u",
                  window->fb.width, window->fb.height);
            return false;
        }

        window->fb.pixels = canopy_malloc(window->fb.pitch * window->fb.height);
        if (!window->fb.pixels) {
            FATAL("Failed to allocate framebuffer");
            return false;
        }

        TRACE("Initialized framebuffer: %ux%u (pitch %u, scale %.2f)",
              window->fb.width, window->fb.height,
              window->fb.pitch, window->pixel_ratio);
    }

    return true;
}
