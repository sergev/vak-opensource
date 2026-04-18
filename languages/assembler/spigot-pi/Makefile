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

# OpenBSD sparc64 (build only on that host; see pi-sparc64-openbsd.S).
SPARC_ASM  := pi-sparc64-openbsd.S
SPARC_OBJ  := pi-sparc64-openbsd.o
SPARC_BIN  := pi-sparc64-openbsd

.PHONY: all clean run sparc64-openbsd

all: $(BIN)

$(OBJ): $(ASM)
	$(NASM) $(NASMFLAGS) -o $(OBJ) $(ASM)

$(BIN): $(OBJ)
	$(CLANG) $(LDFLAGS) -o $(BIN) $(OBJ)

clean:
	rm -f $(OBJ) $(BIN) $(SPARC_OBJ) $(SPARC_BIN)

run: $(BIN)
	time ./$(BIN)

sparc64-openbsd: $(SPARC_BIN)

$(SPARC_OBJ): $(SPARC_ASM)
	cc -E $(SPARC_ASM) | as -o $(SPARC_OBJ) -

$(SPARC_BIN): $(SPARC_OBJ)
	ld -static -nopie $(SPARC_OBJ) -o $(SPARC_BIN)
