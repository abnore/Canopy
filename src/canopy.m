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
    uint8_t 	*framebuffer;
    int		bitmap_width, bitmap_height, pitch;
    bool	should_close;
};

struct canopy_cursor {
    id		cursor;
};

#define CANOPY_MAX_EVENTS 64
static canopy_event event_queue[CANOPY_MAX_EVENTS];
static int event_head = 0;
static int event_tail = 0;
static void canopy_push_event(canopy_event ev);

//----------------------------------------
// Window Delegate
//----------------------------------------
@interface CanopyDelegate : NSObject <NSWindowDelegate>
@end

@implementation CanopyDelegate

- (BOOL)windowShouldClose:(id)sender
{
    [NSApp terminate:nil];
    return YES;
}
- (void)showCustomAboutPanel:(id)sender
{

    NSImage* icon = [[NSImage alloc] initWithContentsOfFile:@"newcanopy/Asset_2.png"];

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
    uint8_t* buffer;
    int width;
    int height;
    int pitch;
}
- (void)setBuffer:(uint8_t*)buf width:(int)w height:(int)h pitch:(int)pitch;
@end
@implementation CanopyView

- (BOOL)isFlipped {
    return YES; // Makes (0,0) the top-left instead of bottom-left
}

- (void)setBuffer:(uint8_t*)buf width:(int)w height:(int)h pitch:(int)p
{
    buffer = buf;
    width = w;
    height = h;
    pitch = p;
    [self setNeedsDisplay:YES]; // Triggers redraw
}

- (void)mouseDown:(NSEvent *)event {
    NSPoint pos = [self convertPoint:[event locationInWindow] fromView:nil];
    canopy_event e = {
        .type = CANOPY_EVENT_MOUSE_DOWN,
        .mouse = {
            .x = (int)pos.x,
            .y = (int)pos.y,
            .button = 0,
            .modifiers = (int)[event modifierFlags]
        }
    };
    canopy_push_event(e);
}
- (void)mouseUp:(NSEvent *)event {
    NSPoint pos = [self convertPoint:[event locationInWindow] fromView:nil];
    canopy_event e = {
        .type = CANOPY_EVENT_MOUSE_UP,
        .mouse = {
            .x = (int)pos.x,
            .y = (int)pos.y,
            .button = 0,
            .modifiers = (int)[event modifierFlags]
        }
    };
    canopy_push_event(e);
}
- (void)mouseMoved:(NSEvent *)event {
    NSPoint pos = [self convertPoint:[event locationInWindow] fromView:nil];
    canopy_event e = {
        .type = CANOPY_EVENT_MOUSE_MOVE,
        .mouse = {
            .x = (int)pos.x,
            .y = (int)pos.y,
            .button = 0,
            .modifiers = (int)[event modifierFlags]
        }
    };
    canopy_push_event(e);
}


- (void)drawRect:(NSRect)dirtyRect
{
    if (!buffer) return;

    NSBitmapImageRep* rep = [[NSBitmapImageRep alloc]
        initWithBitmapDataPlanes:&buffer
                      pixelsWide:width
                      pixelsHigh:height
                   bitsPerSample:8
                 samplesPerPixel:4
                        hasAlpha:YES
                        isPlanar:NO
                  colorSpaceName:NSDeviceRGBColorSpace
                     bytesPerRow:pitch
                    bitsPerPixel:32];

    NSImage* image = [[NSImage alloc] initWithSize:NSMakeSize(width, height)];
    [image addRepresentation:rep];
    [image drawInRect:dirtyRect];
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
// Public API Implementation
//--------------------------------------------------------------------------------
canopy_window* canopy_create_window(int width, int height, const char* title)
{
    @autoreleasepool {
        [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

        canopy_window* win = malloc(sizeof(canopy_window));
        win->delegate = [[CanopyDelegate alloc] init];

        create_menubar(win->delegate);
        NSRect frame = NSMakeRect(0, 0, width, height);
      	CanopyView* view = [[CanopyView alloc] initWithFrame:frame];
        NSUInteger style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable;

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
	[win->view  setWantsLayer:YES];

        [NSApp activateIgnoringOtherApps:YES];

	canopy_refresh_buffer(win);
	win->should_close = false;

        return win;
    }
}

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
    }
}

void canopy_destroy_window(canopy_window* win)
{
    if (!win) return;
    [win->window close];
    [win->delegate release];
    free(win);
    win->should_close = true;
}

bool canopy_window_should_close(canopy_window *window)
{
	return window->should_close;
}

bool canopy_poll_event(canopy_event* out_event)
{
    canopy_pump_events();

    if (event_head == event_tail) return false;
    *out_event = event_queue[event_head];
    event_head = (event_head + 1) % CANOPY_MAX_EVENTS;
    return true;
}


static void canopy_push_event(canopy_event ev) {
    int next = (event_tail + 1) % CANOPY_MAX_EVENTS;
    if (next != event_head) { // only add if queue not full
        event_queue[event_tail] = ev;
        event_tail = next;
    }
}

void canopy_refresh_buffer(canopy_window* win)
{
    NSView* view = (NSView*)win->view;
    NSRect bounds = [view bounds];

    win->bitmap_width = (int)bounds.size.width;
    win->bitmap_height = (int)bounds.size.height;
    win->pitch = win->bitmap_width * 4;

    // Free old buffer if it exists
    if (win->framebuffer) {
        free(win->framebuffer);
    }

    // Allocate buffer to match window size
    win->framebuffer = malloc(win->pitch * win->bitmap_height);
    if (!win->framebuffer) {
        fprintf(stderr, "Failed to allocate framebuffer\n");
    }
}


void canopy_redraw_buffer(canopy_window *window)
{
    @autoreleasepool {
	if (window->framebuffer == NULL) {
		NSLog(@"Buffer is NULL in platform_redraw_buffer");
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
			    bitsPerPixel: 32] autorelease];

	NSImage *image = [[[NSImage alloc] initWithSize:
		NSMakeSize(window->bitmap_width, window->bitmap_height)] autorelease];

	[image addRepresentation: rep];
	[(NSView*)window->view layer].contents = image;
    }
}

void canopy_render_bitmap(canopy_window* win, void* small_buf, int w, int h, int x, int y) {
    int big_w = win->bitmap_width;
    int big_h = win->bitmap_height;
    uint8_t* dst = win->framebuffer;

    if (x + w > big_w || y + h > big_h) {
        printf("Bitmap out of bounds!\n");
        return;
    }

    for (int row = 0; row < h; ++row) {
        uint8_t* dst_row = dst + ((y + row) * big_w + x) * 4;
        uint8_t* src_row = ((uint8_t*)small_buf) + row * w * 4;
        memcpy(dst_row, src_row, w * 4);
    }
}

