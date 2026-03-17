# ============================================================
# Canopy — Minimal macOS Makefile
# ============================================================

LIBNAME     = canopy
TARGET      = lib$(LIBNAME).dylib

PREFIX      = /usr/local
INCLUDEDIR  = $(PREFIX)/include
LIBDIR      = $(PREFIX)/lib

CC          = clang

SRC         = src/canopy.m \
              src/canopy_event.c \
              src/canopy_input.c \
              src/canopy_memory.c \
              src/canopy_time.c

HDR         = canopy.h

CFLAGS      = -Wall -Wextra -O2 -fPIC -I.
LDFLAGS     = -dynamiclib \
              -install_name $(LIBDIR)/$(TARGET) \
              -framework Cocoa \
			  -lblackbox

all: $(TARGET)

$(TARGET): $(SRC) $(HDR)
	@echo "==> Building Canopy shared library"
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SRC)
	@echo "    - Built $(TARGET)"

install: $(TARGET)
	@echo "==> Installing Canopy..."
	@mkdir -p $(INCLUDEDIR)
	@mkdir -p $(LIBDIR)
	@echo "    - Installing header -> $(INCLUDEDIR)/$(HDR)"
	@sudo cp $(HDR) $(INCLUDEDIR)/
	@echo "    - Installing library -> $(LIBDIR)/$(TARGET)"
	@sudo cp $(TARGET) $(LIBDIR)/
	@echo "==> Install complete."

uninstall:
	@echo "==> Uninstalling Canopy..."
	@sudo rm -f $(INCLUDEDIR)/$(HDR)
	@sudo rm -f $(LIBDIR)/$(TARGET)
	@echo "==> Uninstall complete."

clean:
	@echo "==> Cleaning build artifacts"
	@rm -f $(TARGET)
	@echo "    ✓ Clean"

.PHONY: all install uninstall clean
