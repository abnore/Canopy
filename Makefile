#----------------------------------------
# Makefile for Canopy + Picasso Tests
#----------------------------------------

test_dir    = test
bin_dir     = bin
src_dir     = src
lib_dir     = lib
inc_dir     = include

cc          = clang
cc_flags    = -framework Cocoa -I. -I$(lib_dir) -I$(src_dir) -I$(inc_dir) -Wall -Wextra
ld_flags    = -lblackbox

# Common sources used by ALL tests
src_common  = $(src_dir)/canopy.m \
              $(src_dir)/canopy_event.c \
              $(src_dir)/canopy_time.c \
              $(src_dir)/common.c \
              $(src_dir)/bmp.c \
              $(src_dir)/picasso.c \
              $(src_dir)/picasso_icc_profiles.c

# Extract test names automatically (test/test_xxx.c -> test_xxx)
tests = $(patsubst $(test_dir)/%.c,%,$(wildcard $(test_dir)/*.c))

# Final executable paths
outputs = $(addprefix $(bin_dir)/, $(tests))

all: $(outputs)

# Rule: build bin/test_basic from test/test_basic.c and common sources
$(bin_dir)/%: $(test_dir)/%.c $(src_common)
	@mkdir -p $(bin_dir)
	$(cc) $^ $(cc_flags) $(ld_flags) -o $@

clean:
	rm -rf $(bin_dir)
