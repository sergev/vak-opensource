#
# make
# make all      -- build everything
#
# make clean    -- remove build files
#

all:    build
	ninja -C build

clean:
	rm -rf build

build:
	cmake -B $@ -G Ninja -DMLIR_DIR=/opt/homebrew/opt/llvm/lib/cmake/mlir
