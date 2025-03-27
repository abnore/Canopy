#----------------------------------------
# Simple Makefile for Canopy project
#----------------------------------------

test_dir = test
bin_dir  = bin
src_dir  = src
lib_dir  = lib
logger_dir = logger

cc        = clang
cc_flags  = -framework Cocoa -I. -Ilib -I$(logger_dir)

src_files =  $(test_dir)/test.c
src_files += $(src_dir)/canopy.m
src_files += $(src_dir)/canopy_event.c
src_files += $(src_dir)/picasso.c
src_files += $(src_dir)/canopy_time.c
src_files += $(src_dir)/common.c
src_files += $(logger_dir)/logger.c

output = $(bin_dir)/Test

all: $(output)

$(output): $(src_files)
	@mkdir -p $(bin_dir)
	$(cc) $(src_files) $(cc_flags) -o $(output)

clean:
	rm -f $(output)

