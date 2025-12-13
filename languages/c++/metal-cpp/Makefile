#
# Top level makefile.
# Targets:
#       make
#       make all        -- build everything
#       make clean      -- remove build files
#
all:    build
	$(MAKE) -C build $@

clean:
	rm -rf build

.PHONY: all clean install test

build:
	cmake -B $@
