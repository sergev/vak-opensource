#
# make
# make all      -- build everything
#
# make install  -- install picosim binaries to /usr/local
#
# make clean    -- remove build files
#

all:    build
	$(MAKE) -C build $@

install: build
	$(MAKE) -C build $@

clean:
	rm -rf build

build:
	mkdir $@
	cmake -B $@ .

.PHONY: clean all install
