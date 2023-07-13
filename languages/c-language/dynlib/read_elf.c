#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alloca.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <err.h>
#include <sys/mman.h>
#include "elf.h"

#define errexit(eval, code, ...) { \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, ": %s\n", strerror(code)); \
        exit(eval); \
    }

#include "read_elf32.c"

#include "read_elf64.c"

void read_elf_file(const char *filename, unsigned elf_machine)
{
    // Open input file.
    int elf_file = open(filename, O_RDONLY);
    if (elf_file < 0) {
        err(-1, "%s: Cannot open file", filename);
    }
    printf("\nFile %s\n", filename);

    // Identify ELF class: 32-bit or 64-bit.
    unsigned char ident[EI_NIDENT];
    if (read(elf_file, (char *)&ident[0], sizeof ident) != sizeof ident) {
        err(-1, "%s: Cannot read ELF magic", filename);
    }
    if (ident[EI_MAG0] != ELFMAG0 || ident[EI_MAG1] != ELFMAG1 ||
        ident[EI_MAG2] != ELFMAG2 || ident[EI_MAG3] != ELFMAG3) {
        errexit(-1, ENOEXEC, "Bad ELF magic");
    }
    if (ident[EI_DATA] != ELFDATA2LSB) {
        errexit(-1, ENOEXEC, "Wrong ELF data format, little-endian expected");
    }
    if (ident[EI_VERSION] != EV_CURRENT) {
        errexit(-1, ENOEXEC, "Unknown ELF version");
    }
    if (ident[EI_OSABI] != ELFOSABI_SYSV) {
        errexit(-1, ENOEXEC, "Unknown ABI identification");
    }

    switch (ident[EI_CLASS]) {
    case ELFCLASS32:
        read_elf32_file(elf_file, filename, elf_machine);
        break;
    case ELFCLASS64:
        read_elf64_file(elf_file, filename, elf_machine);
        break;
    default:
        errexit(-1, ENOEXEC, "Unknown ELF class");
    }

    close(elf_file);
}

int main()
{
#if __aarch64__
    read_elf_file("aarch64/hello.elf", EM_AARCH64);
#endif
#if __x86_64__
    read_elf_file("x86_64/hello.elf", EM_X86_64);
#endif
    read_elf_file("arm32/hello.elf", EM_ARM);
}
