#----------------------------------------
# Simple Makefile for Canopy project
#----------------------------------------

test_dir = test
bin_dir  = bin
src_dir  = src

cc        = clang
cc_flags  = -framework Cocoa -I.
src_files = $(test_dir)/test.c $(src_dir)/canopy.m picasso.c
output    = $(bin_dir)/Test

all: $(output)

$(output): $(src_files)
	@mkdir -p $(bin_dir)
	$(cc) $(src_files) $(cc_flags) -o $(output)

clean:
	rm -f $(output)

