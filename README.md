<p align="center">
  <img src="https://github.com/user-attachments/assets/671c5596-605c-4842-856a-12175a9375d8" width="400" alt="Canopy Logo" />
</p>

<p align="center">
  <strong>Lightweight windowing and event handling for macOS, built in C with a Cocoa backend.</strong>
</p>

---

## About

Canopy is a simple windowing and input library for macOS.  
It is written in C with a native Objective-C backend and uses Cocoa directly — no third-party dependencies.

This project is **educational and experimental** in nature. It’s not intended for production or general distribution.  
The goal is to better understand macOS internals, event loops, and rendering pipelines by building something from scratch.

>[!WARNING]
>This project is **under active development** and is evolving quickly.
>Features, function names, and structures may change at any time. Breaking changes are likely.
---

## Purpose & Design

-  Built for learning and exploration
-  Provides a simple window and event loop system
-  Separates the window/event layer from the rendering layer
-  Uses a CPU-driven framebuffer model (with plans for optional GPU acceleration)
-  Keeps full control of the stack — no SDL, no GLFW, no external runtime deps

>[!NOTE]
>Canopy is intentionally minimal and only targets macOS.
>It does not aim for cross-platform support — it’s a learning tool and experiment in Cocoa + C interop.

---


## Goals

- [x] Create and manage native macOS windows
- [x] Handle mouse, keyboard, and scroll input
- [x] Provide a backbuffer and rendering pipeline
- [x] Add a logging system
- [ ] Enable hardware acceleration (future)
- [ ] Add sprite support and simple 2D rendering helpers


##  Features

- Pure C API with Objective-C backend (Cocoa)
- NSWindow + NSView handling under the hood
- Custom backbuffer rendering
- Input system (keyboard, mouse, scroll, modifiers)
- Logging system with multiple levels
- Minimal dependencies — just Cocoa




---

## Getting Started

### 🔧 Requirements

- macOS (tested on Monterey and later)
- Clang (or Xcode CLI tools)
- C99-compatible compiler

###  Minimal Example
## Example Output

> (Examples and demos to come)
>
> <p align="center">
  <img src="https://github.com/user-attachments/assets/75867839-a59f-422f-8e22-e20968fc2215" width="300" alt="Example output with colored background" />
</p>

---
```c
/*******************************************************************************************
*
*   CANOPY [Example] - Custom Framebuffer Rendering
*
*   Description:
*       Renders a solid-colored rectangle using a manually created framebuffer.
*       Demonstrates how to allocate, fill, and display a custom framebuffer using Canopy.
*
*   Controls:
*       [Close Window] - Exit application
*
********************************************************************************************/

#include "canopy.h"

#define WIDTH   400
#define HEIGHT  400
#define PIXELS  WIDTH*HEIGHT

// Phthalo Blue https://colors.artyclick.com/color-names-dictionary/color-names/phthalo-blue-color
// little endian, abgr (rgba backwards)
#define PHTALO_BLUE 0xff890f00
#define CANOPY_BLUE 0xffff0000

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    if (!init_log(NULL, true)) {
        WARN("Failed to initialize logger\n");
        return 1;
    }

    canopy_window* win = canopy_create_window("Canopy - Custom Framebuffer",
                                                WIDTH, HEIGHT,
                                                CANOPY_WINDOW_STYLE_TITLED |
                                                CANOPY_WINDOW_STYLE_CLOSABLE);
    canopy_init_timer();
    //canopy_set_fps(60); // default is 60

    framebuffer fb;
    fb.width = 400;
    fb.height = 400;
    fb.pitch = fb.width * CANOPY_BYTES_PER_PIXEL;

    size_t buffer_size = fb.pitch * fb.height;
    fb.pixels = canopy_malloc(buffer_size);

    if (!fb.pixels) {
        FATAL("Failed to allocate framebuffer");
        return 1;
    }

    //--------------------------------------------------------------------------------------

    // Main Game Loop
    while (!canopy_window_should_close(win))
    {
        // Update
        //----------------------------------------------------------------------------------
        canopy_event event;
        while (canopy_poll_event(&event))
        {
            // Handle events (mouse, keyboard, etc.)
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        if (canopy_should_render_frame())
        {
            // Fill the framebuffer with its clear color
            for (int i = 0; i < PIXELS; ++i) {
                if(i >= PIXELS/2){
                    fb.pixels[i] = CANOPY_BLUE;
                }
                else{
                    fb.pixels[i] = PHTALO_BLUE;
                }
            }

            // Do other stuff graphicly here

            canopy_swap_backbuffer(win,&fb);    // Switch pointers of custom framebuffer
            canopy_present_buffer(win);         // Present on screen
        }
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    canopy_free(fb.pixels);
    canopy_free_window(win);
    shutdown_log();
    //--------------------------------------------------------------------------------------

    return 0;
}
```


### 🛠️ Building

Use `clang` to build:

```bash
clang main.c src/canopy.m src/canopy_event.c src/canopy_time.c \
     src/common.c logger/logger.c \
     -framework Cocoa -I. -Ilib -Ilogger -o bin/Test
```
Or integrate it into your own CMake or Makefile setup.


## License & Disclaimer

This library is **not intended for production use**.
This is a side project, built for fun and learning.  
It’s focused on **clarity**, **control**, and **simplicity**, not on portability or production features.

If you’re interested in how to work directly with Cocoa and C — or building games and apps from the metal up — Canopy might be a good reference.
It was created as a personal side project for learning and experimentation.

-  Designed to be **dependency-free**
-  Built for **educational purposes**
-  Meant to help explore **event-driven architecture** and **graphics**
-  Aims to eventually support **hardware acceleration**, while keeping the CPU path simple and accessible

Use it, break it, learn from it.  
Feel free to fork and experiment — just don’t expect it to replace SDL, Raylib or GLFW (yet).

