# macOS x86_64: NASM macho64 + bare-metal link (no libc).
#
# Apple Silicon: Rosetta must be installed; build the x86_64 binary with
#   make ARCH_PREFIX='arch -x86_64'
# so both nasm and clang emit/link amd64. On an Intel Mac, omit ARCH_PREFIX.

ASM      := pi-macos64.asm
OBJ      := pi-macos64.o
BIN      := pi-macos64
ARCH_PREFIX ?=

NASM     := $(ARCH_PREFIX) nasm
CLANG    := $(ARCH_PREFIX) clang

NASMFLAGS := -f macho64
LDFLAGS   := -nostdlib -static -Wl,-e,_start

.PHONY: all clean run

all: $(BIN)

$(OBJ): $(ASM)
	$(NASM) $(NASMFLAGS) -o $(OBJ) $(ASM)

$(BIN): $(OBJ)
	$(CLANG) $(LDFLAGS) -o $(BIN) $(OBJ)

clean:
	rm -f $(OBJ) $(BIN)

run: $(BIN)
	time ./$(BIN)
