# ============================================================
# Canopy — Minimal macOS Makefile
# ============================================================

LIBNAME     = canopy
SRC         = src/canopy.m src/canopy_event.c src/canopy_time.c src/common.c
HDR         = canopy.h

PREFIX      = /usr/local
INCLUDEDIR  = $(PREFIX)/include
LIBDIR      = $(PREFIX)/lib

TARGET      = lib$(LIBNAME).dylib

CC          = clang
CFLAGS      = -Wall -Wextra -O2 -fpic -I.
LDFLAGS     = -dynamiclib -install_name $(LIBDIR)/$(TARGET)

# ============================================================
# Build
# ============================================================

all: $(TARGET)

$(TARGET): $(SRC) $(HDR)
	@echo "==> Building Canopy shared library"
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SRC)
	@echo "    - Built $(TARGET)"

# ============================================================
# Install
# ============================================================

install: $(TARGET)
	@echo "==> Installing Canopy..."
	@echo "    - Ensuring directories exist"
	@mkdir -p $(INCLUDEDIR)
	@mkdir -p $(LIBDIR)
	@echo "    - Installing header -> $(INCLUDEDIR)/$(HDR)"
	@sudo cp $(HDR) $(INCLUDEDIR)/
	@echo "    - Installing library -> $(LIBDIR)/$(TARGET)"
	@sudo cp $(TARGET) $(LIBDIR)/
	@echo "==> Install complete."

# ============================================================
# Uninstall
# ============================================================

uninstall:
	@echo "==> Uninstalling Canopy..."
	@echo "    - Removing header"
	@sudo rm -f $(INCLUDEDIR)/$(HDR)
	@echo "    - Removing library"
	@sudo rm -f $(LIBDIR)/$(TARGET)
	@echo "==> Uninstall complete."

# ============================================================
# Clean
# ============================================================

clean:
	@echo "==> Cleaning build artifacts"
	@rm -f $(TARGET)
	@echo "    ✓ Clean"

.PHONY: all install uninstall clean

