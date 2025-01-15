#
# make
# make all      -- build everything
#
# make clean    -- remove build files
#

all:    build
	$(MAKE) -C build $@

clean:
	rm -rf build

build:
	cmake -B $@ .

upload: build/gpio_usb_pico.uf2
	$(MAKE) -C build $@
