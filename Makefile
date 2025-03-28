#----------------------------------------
# Makefile for Canopy + Picasso Tests
#----------------------------------------

test_dir    = test
bin_dir     = bin
src_dir     = src
lib_dir     = lib
logger_dir  = logger

cc          = clang
cc_flags    = -framework Cocoa -I. -I$(lib_dir) -I$(logger_dir) -I$(src_dir)
src_common  = $(src_dir)/canopy.m \
              $(src_dir)/canopy_event.c \
              $(src_dir)/canopy_time.c \
              $(src_dir)/common.c \
              $(src_dir)/picasso.c \
              $(logger_dir)/logger.c

tests       = test_basic test_blit test_complex
outputs     = $(addprefix $(bin_dir)/, $(tests))

all: $(outputs)

$(bin_dir)/%: $(test_dir)/%.c $(src_common)
	@mkdir -p $(bin_dir)
	$(cc) $^ $(cc_flags) -o $@

clean:
	rm -rf $(bin_dir)

