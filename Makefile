all:
	clang test.c canopy.m -framework Cocoa -o Test
clean:
	rm Test
