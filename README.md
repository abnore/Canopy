<p align="center">
  <img src="https://github.com/user-attachments/assets/61dd51e4-00ce-4f4f-9220-0c98c9e80d8f" width="400" alt="Canopy Logo" />
</p>

<h1 align="center">Canopy</h1>

<p align="center">
  <strong>Lightweight windowing and event handling for macOS, built in C with a Cocoa backend.</strong>
</p>

---

## About

Canopy is a simple windowing and input library for macOS.  
It is written in C with a native Objective-C backend and uses Cocoa directly — no third-party dependencies.

This project is **educational and experimental** in nature. It’s not intended for production or general distribution.  
The goal is to better understand macOS internals, event loops, and rendering pipelines by building something from scratch.

---

## Purpose & Design

- 🎓 Built for learning and exploration
- 🧱 Provides a simple window and event loop system
- 🧵 Separates the window/event layer from the rendering layer
- 🖼️ Uses a CPU-driven framebuffer model (with plans for optional GPU acceleration)
- 🔍 Keeps full control of the stack — no SDL, no GLFW, no external runtime deps

---

## Project Status

Canopy is **under active development** and is evolving quickly.

- The API is **not stable**.
- Things may break between versions.
- It is intentionally minimal and will only support macOS.

---

## Example Output

> (Examples and demos to come)

---

## Goals

- [x] Create and manage native macOS windows
- [x] Handle mouse, keyboard, and scroll input
- [x] Provide a backbuffer and rendering pipeline
- [x] Add a logging system
- [ ] Enable hardware acceleration (future)
- [ ] Add sprite support and simple 2D rendering helpers

---

## Disclaimer

This is a side project, built for fun and learning.  
It’s focused on **clarity**, **control**, and **simplicity**, not on portability or production features.

If you’re interested in how to work directly with Cocoa and C — or building games and apps from the metal up — Canopy might be a good reference.

---



## 🧱 Features

- Pure C API with Objective-C backend (Cocoa)
- NSWindow + NSView handling under the hood
- Custom backbuffer rendering
- Input system (keyboard, mouse, scroll, modifiers)
- Logging system with multiple levels
- Minimal dependencies — just Cocoa
