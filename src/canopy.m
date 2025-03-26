#import <Cocoa/Cocoa.h>

#import "canopy.h"

//----------------------------------------
// Struct to hold macOS window internals
//----------------------------------------
struct canopy_window {
    id 		window;
    id 		view;
    id 		delegate;

    id		bitmapRep;
    uint8_t *framebuffer;
    int		bitmap_width, bitmap_height, pitch;
    bool	should_close;
    int     buffer_color;
};

//----------------------------------------
// Window Delegate
//----------------------------------------
@interface CanopyDelegate : NSObject <NSWindowDelegate>
{
    canopy_window* window;
}
- (instancetype)initWithCanopyWindow:(canopy_window*)initWindow;
@end


@implementation CanopyDelegate

- (instancetype)initWithCanopyWindow:(canopy_window*)initWindow
{
    self = [super init];
    if (self) {
        window = initWindow;
    }
    return self;
}

- (BOOL)windowShouldClose:(id)sender
{
    window->should_close = true; // Notify the C code
    return NO; // Let your app close gracefully
}

- (void)showCustomAboutPanel:(id)sender
{
    NSImage* icon = [[NSImage alloc] initWithContentsOfFile:@"assets/icon.svg"];

    NSDictionary* options = @{
        @"ApplicationName": @"Canopy",
        @"ApplicationVersion": @"0.1.0",
        @"ApplicationIcon": icon ?:[NSImage imageNamed:NSImageNameApplicationIcon],
        @"Copyright": @"© 2025 Canopy",
        @"Credits": [[NSAttributedString alloc] initWithString:@"Built with Maria using Cocoa and C"]
    };

    // Show after one frame to avoid early draw issues
	[NSApp orderFrontStandardAboutPanelWithOptions:options];
}

@end
//----------------------------------------
// Window View
//----------------------------------------
@interface CanopyView : NSView
{
    canopy_window* window;
}
- (instancetype)initWithFrame:(NSRect)frame window:(canopy_window*)win;
- (void)setRenderColor:(int)color;
- (int)getRenderColor;
@end
//----------------------------------------
@implementation CanopyView

- (instancetype)initWithFrame:(NSRect)frame window:(canopy_window*)win
{
    self = [super initWithFrame:frame];
    if (self) {
        window = win;
        window->buffer_color = color_to_u32(CANOPY_DARK_GRAY); // or any default
    }
    return self;
}

- (BOOL)isFlipped {
    return YES; // Makes (0,0) the top-left instead of bottom-left
}
- (BOOL)acceptsFirstResponder {
    return YES;
}
- (void)setRenderColor:(int)color {
    window->buffer_color = color;  // Set the render_color
    [self setNeedsDisplay:YES];  // Trigger a redraw if needed
}
- (int)getRenderColor {
    return window->buffer_color;
}

/* ------------ Standard mouse event handler,
 *              so that i can add as many as i want later ------------ */
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

/*  ------------To receive mouse entered and exit we setup a tracking area ------------ */
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
- (void)keyDown:(NSEvent *)event {
    canopy_event e;
    e.type = CANOPY_EVENT_KEY;
    e.key.action = CANOPY_KEY_PRESS;
    e.key.keycode = (int)[event keyCode];
    e.key.modifiers = (int)[event modifierFlags];
    e.key.is_repeat = [event isARepeat] ? 1 : 0;
    canopy_push_event(e);
}

- (void)keyUp:(NSEvent *)event {
    canopy_event e;
    e.type = CANOPY_EVENT_KEY;
    e.key.action = CANOPY_KEY_RELEASE;
    e.key.keycode = (int)[event keyCode];
    e.key.modifiers = (int)[event modifierFlags];
    e.key.is_repeat = 0;
    canopy_push_event(e);
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
void *canopy_calloc(size_t count, size_t size)
{
    return calloc(count, size);
}
void canopy_free(void *ptr)
{
    free(ptr);
}
void *canopy_malloc(size_t size)
{
    return malloc(size);
}
void *canopy_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

/* Window functions */
canopy_window* canopy_create_window(int width, int height, const char* title)
{
    @autoreleasepool {
        [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

        canopy_window* win = canopy_malloc(sizeof(canopy_window));
        win->delegate = [[CanopyDelegate alloc] initWithCanopyWindow:win];

        create_menubar(win->delegate);
        NSRect frame = NSMakeRect(0, 0, width, height);
        CanopyView* view = [[CanopyView alloc] initWithFrame:frame window:win];

        //NSUInteger style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable;
        NSUInteger style = WindowStyleMaskTitled | WindowStyleMaskClosable;

        win->window = [[NSWindow alloc] initWithContentRect:frame
                                                  styleMask:style
                                                    backing:NSBackingStoreBuffered
                                                      defer:NO];
        win->view = view; // Store if needed for drawing

        [win->window setTitle:[NSString stringWithUTF8String:title]];
        [(NSWindow*)win->window center];
        [win->window setDelegate:win->delegate];
        [win->window setContentView:view];
        [win->window makeKeyAndOrderFront:nil];
        [win->window setAcceptsMouseMovedEvents:YES];
        [win->window makeFirstResponder:win->view];
        [win->view setWantsLayer:YES];
        CALayer* layer = [view layer];

        [layer setOpaque:YES];
        [win->view setOpaque:YES];

        [NSApp activateIgnoringOtherApps:YES];

        canopy_clear_buffer(win);
        win->should_close = false;
        TRACE("Window created");
        if(!win) ERROR("Window failed to be created");
        return win;
    }
}
void canopy_set_icon(const char* filepath)
{
    if (!filepath) return;

    NSString* path = [NSString stringWithUTF8String:filepath];
    NSImage* icon = [[NSImage alloc] initWithContentsOfFile:path];

    if (icon) {
        [NSApp setApplicationIconImage:icon];
    } else {
        NSLog(@"[canopy] Failed to load icon from path: %s", filepath);
    }
}
void canopy_free_window(canopy_window* win)
{
    if (!win) return;

    @autoreleasepool {
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
        if (win->framebuffer) {
            canopy_free(win->framebuffer);
            win->framebuffer = NULL;
        }

        // (Optional) Let Cocoa flush pending events
        canopy_pump_events();  // if this exists and is safe

        TRACE("Window closed and cleaned up");
    }

    canopy_free(win);
}


bool canopy_window_should_close(canopy_window *window)
{
    canopy_pump_events();  // Keep the UI alive
    return window->should_close;
}

void canopy_clear_buffer(canopy_window* win)
{
    if(win->framebuffer == NULL)
    {
        NSView* view = (NSView*)win->view;
        NSRect bounds = [view bounds];

        win->bitmap_width = (int)bounds.size.width;
        win->bitmap_height = (int)bounds.size.height;
        win->pitch = win->bitmap_width * 4;

        // Allocate buffer to match window size
        win->framebuffer = canopy_malloc(win->pitch * win->bitmap_height);
        if (!win->framebuffer) {
            fprintf(stderr, "Failed to allocate framebuffer\n");
            return;
        }
    }

    // Get the render color (background color) from the view
    uint32_t color = (uint32_t)[win->view getRenderColor];

    // Clear the framebuffer by filling it with the render color
    for (int i = 0; i < win->bitmap_width * win->bitmap_height; ++i) {
        ((uint32_t*)win->framebuffer)[i] = color;
    }
}

void canopy_present_buffer(canopy_window *window)
{
    @autoreleasepool {
        if (window->framebuffer == NULL) {
            NSLog(@"Buffer is NULL in canopy_present_buffer");
            return;
        }

        NSBitmapImageRep *rep = [[[NSBitmapImageRep alloc]
                initWithBitmapDataPlanes: &window->framebuffer
                              pixelsWide: window->bitmap_width
                              pixelsHigh: window->bitmap_height
                           bitsPerSample: 8
                         samplesPerPixel: 4
                                hasAlpha: YES
                                isPlanar: NO
                          colorSpaceName: NSDeviceRGBColorSpace
                             bytesPerRow: window->pitch
                            bitsPerPixel: 32]
                            autorelease];

        NSImage *image = [[[NSImage alloc]
                            initWithSize: NSMakeSize(window->bitmap_width,
                                                    window->bitmap_height)]
                            autorelease];

        [image addRepresentation: rep];
        [(NSView*)window->view layer].contents = image;
    }
}

static inline uint32_t blend_pixel(uint32_t dst, uint32_t src) {
    uint8_t sa = (src >> 24) & 0xFF;
    if (sa == 255) return src;
    if (sa == 0) return dst;

    uint8_t sr = src & 0xFF;
    uint8_t sg = (src >> 8) & 0xFF;
    uint8_t sb = (src >> 16) & 0xFF;

    uint8_t dr = dst & 0xFF;
    uint8_t dg = (dst >> 8) & 0xFF;
    uint8_t db = (dst >> 16) & 0xFF;

    uint8_t r = (sr * sa + dr * (255 - sa)) / 255;
    uint8_t g = (sg * sa + dg * (255 - sa)) / 255;
    uint8_t b = (sb * sa + db * (255 - sa)) / 255;

    return (0xFF << 24) | (b << 16) | (g << 8) | r;
}

void canopy_raster_bitmap_ex(canopy_window* win,
                             void* src_buf, int src_w, int src_h,
                             int dest_x, int dest_y,
                             int dest_w, int dest_h,
                             bool scale,
                             bool blend,
                             bool bilinear)
{
    if (!win || !src_buf || !win->framebuffer) return;

    int screen_w = win->bitmap_width;
    int screen_h = win->bitmap_height;
    uint32_t* dst = (uint32_t*)win->framebuffer;
    uint32_t* src = (uint32_t*)src_buf;

    if (!scale) {
        dest_w = src_w;
        dest_h = src_h;
    }

    int clipped_w = dest_w;
    int clipped_h = dest_h;
    int offset_x = 0, offset_y = 0;

    if (dest_x < 0) {
        offset_x = -dest_x;
        clipped_w -= offset_x;
        dest_x = 0;
    }
    if (dest_y < 0) {
        offset_y = -dest_y;
        clipped_h -= offset_y;
        dest_y = 0;
    }

    if (dest_x + clipped_w > screen_w) clipped_w = screen_w - dest_x;
    if (dest_y + clipped_h > screen_h) clipped_h = screen_h - dest_y;
    if (clipped_w <= 0 || clipped_h <= 0) return;

    for (int dy = 0; dy < clipped_h; ++dy) {
        int sy = scale ? ((dy + offset_y) * src_h) / dest_h : dy + offset_y;
        if (sy >= src_h) continue;

        for (int dx = 0; dx < clipped_w; ++dx) {
            int sx = scale ? ((dx + offset_x) * src_w) / dest_w : dx + offset_x;
            if (sx >= src_w) continue;

            uint32_t src_pixel = src[sy * src_w + sx];
            int dst_index = (dest_y + dy) * screen_w + (dest_x + dx);

            if (blend) {
                dst[dst_index] = blend_pixel(dst[dst_index], src_pixel);
            } else {
                dst[dst_index] = src_pixel;
            }
        }
    }
}


uint8_t *canopy_get_framebuffer(canopy_window *window)
{
    return window->framebuffer;
}
void canopy_set_buffer_refresh_color(canopy_window *w, color c)
{
    //int color_int = (int)((c.r << 24) | (c.g << 16) | (c.b << 8) | c.a);
    int color_int = color_to_u32(c);

    [w->view setRenderColor:color_int];
}


/* Event functions */

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

const char* canopy_key_to_string(keys key)
{
        switch (key) {
                case CANOPY_KEY_A:
                        return "a";
                case CANOPY_KEY_B:
                        return "b";
                case CANOPY_KEY_C:
                        return "c";
                case CANOPY_KEY_D:
                        return "d";
                case CANOPY_KEY_E:
                        return "e";
                case CANOPY_KEY_F:
                        return "f";
                case CANOPY_KEY_G:
                        return "g";
                case CANOPY_KEY_H:
                        return "h";
                case CANOPY_KEY_I:
                        return "i";
                case CANOPY_KEY_J:
                        return "j";
                case CANOPY_KEY_K:
                        return "k";
                case CANOPY_KEY_L:
                        return "l";
                case CANOPY_KEY_M:
                        return "m";
                case CANOPY_KEY_N:
                        return "n";
                case CANOPY_KEY_O:
                        return "o";
                case CANOPY_KEY_P:
                        return "p";
                case CANOPY_KEY_Q:
                        return "q";
                case CANOPY_KEY_R:
                        return "r";
                case CANOPY_KEY_S:
                        return "s";
                case CANOPY_KEY_T:
                        return "t";
                case CANOPY_KEY_U:
                        return "u";
                case CANOPY_KEY_V:
                        return "v";
                case CANOPY_KEY_W:
                        return "w";
                case CANOPY_KEY_X:
                        return "x";
                case CANOPY_KEY_Y:
                        return "y";
                case CANOPY_KEY_Z:
                        return "z";
                case CANOPY_KEY_AE:
                        return "æ";
                case CANOPY_KEY_OE:
                        return "ø";
                case CANOPY_KEY_AA:
                        return "å";

                case CANOPY_KEY_NUMPAD0:
                        return "0";
                case CANOPY_KEY_NUMPAD1:
                        return "1";
                case CANOPY_KEY_NUMPAD2:
                        return "2";
                case CANOPY_KEY_NUMPAD3:
                        return "3";
                case CANOPY_KEY_NUMPAD4:
                        return "4";
                case CANOPY_KEY_NUMPAD5:
                        return "5";
                case CANOPY_KEY_NUMPAD6:
                        return "6";
                case CANOPY_KEY_NUMPAD7:
                        return "7";
                case CANOPY_KEY_NUMPAD8:
                        return "7";
                case CANOPY_KEY_NUMPAD9:
                        return "9";

                case CANOPY_KEY_1:
                        return "1";
                case CANOPY_KEY_2:
                        return "2";
                case CANOPY_KEY_3:
                        return "3";
                case CANOPY_KEY_4:
                        return "4";
                case CANOPY_KEY_5:
                        return "5";
                case CANOPY_KEY_6:
                        return "6";
                case CANOPY_KEY_7:
                        return "7";
                case CANOPY_KEY_8:
                        return "8";
                case CANOPY_KEY_9:
                        return "9";
                case CANOPY_KEY_0:
                        return "0";

                case CANOPY_KEY_F1:
                        return "f1";
                case CANOPY_KEY_F2:
                        return "f2";
                case CANOPY_KEY_F3:
                        return "f3";
                case CANOPY_KEY_F4:
                        return "f4";
                case CANOPY_KEY_F5:
                        return "f5";
                case CANOPY_KEY_F6:
                        return "f6";
                case CANOPY_KEY_F7:
                        return "f7";
                case CANOPY_KEY_F8:
                        return "f8";
                case CANOPY_KEY_F9:
                        return "f9";
                case CANOPY_KEY_F10:
                        return "f10";
                case CANOPY_KEY_F11:
                        return "f11";
                case CANOPY_KEY_F12:
                        return "f12";
                case CANOPY_KEY_F13:
                        return "f13";
                case CANOPY_KEY_F14:
                        return "f14";
                case CANOPY_KEY_F15:
                        return "f15";
                case CANOPY_KEY_F16:
                        return "f16";
                case CANOPY_KEY_F17:
                        return "f17";
                case CANOPY_KEY_F18:
                        return "f18";
                case CANOPY_KEY_F19:
                        return "f19";
                case CANOPY_KEY_F20:
                        return "f20";

                case CANOPY_KEY_ESCAPE:
                        return "escape";
                case CANOPY_KEY_SPACE:
                        return "space";
                case CANOPY_KEY_TAB:
                        return "tab";
                case CANOPY_KEY_RETURN:
                        return "return";
                case CANOPY_KEY_ENTER:
                        return "enter";
                case CANOPY_KEY_DELETE:
                        return "delete";

                case CANOPY_KEY_UP:
                        return "up";
                case CANOPY_KEY_DOWN:
                        return "down";
                case CANOPY_KEY_LEFT:
                        return "left";
                case CANOPY_KEY_RIGHT:
                        return "right";

                case CANOPY_KEY_PAGE_UP:
                        return "page up";
                case CANOPY_KEY_PAGE_DOWN:
                        return "page down";
                case CANOPY_KEY_PAGE_HOME:
                        return "home";
                case CANOPY_KEY_PAGE_END:
                        return "end";
                case CANOPY_KEY_LSHIFT:
                        return "l_shift";
                case CANOPY_KEY_RSHIFT:
                        return "r_shift";
                case CANOPY_KEY_LCONTROL:
                        return "l_control";
                case CANOPY_KEY_RCONTROL:
                        return "r_control";
                case CANOPY_KEY_LOPTION:
                        return "l_option";
                case CANOPY_KEY_ROPTION:
                        return "r_option";
                case CANOPY_KEY_LCOMMAND:
                        return "l_command";
                case CANOPY_KEY_RCOMMAND:
                        return "r_command";
                case CANOPY_KEY_CAPS_LOCK:
                        return "caps lock";
                default:
                        return "undefined key";
        }
}
