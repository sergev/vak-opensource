How to load and dynamically link ELF binary into your program
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

It is possible to load external binaries into your program and run them.
The binaries in ELF format must be compiled as position-independent code,
and linked as shared objects.

For examplem for x86_64 (Intel/AMD) architecture, C program must be
compiled with "-fpie" option, and linked with "-shared -fpie" options.

Such ELF binaries have three program segments:

    Program Headers:
      Type           Offset     VirtAddr   PhysAddr   FileSiz    MemSiz      Flg  Align
      LOAD           0x00000000 0x00000000 0x00000000 0x00000224 0x00000224  R E  0x1000
      LOAD           0x00000228 0x00001228 0x00001228 0x00000130 0x00000130  RW   0x1000
      DYNAMIC        0x00000228 0x00001228 0x00001228 0x00000110 0x00000110  RW   0x8

The first LOAD segment contains executable code and read-only data.
It can be mmaped with Read+Execute permissions.

The second LOAD segment contains read-write data.
It should be copied from ELF file on a memory page(s) allocated
right after the first LOAD segment.

The third segment points to the area inside the second LOAD segment.
It provides data needed to connect the loaded binary with your program.
It helps to set up adresses in the Global Offset Table for symbols,
exported by your program and consumed by the loaded binary, and vice versa.

Dynamic section typically contains the following entries:

    Tag        Type          Name/Value
    0x00000001 DT_NEEDED     Shared library: [mylib-x86_64.so]

        Name of the shared library to be linked with.

    0x00000004 DT_HASH       0xe8

        Address of section .hash which contains a hash table for dynamic symbols.

    0x00000005 DT_STRTAB     0x148

        Address of section .dynstr - a string table.

    0x00000006 DT_SYMTAB     0x100

        Address of section .dynsym - a symbol table.

    0x0000000a DT_STRSZ      30 (bytes)

        Size of the string table in bytes.

    0x0000000b DT_SYMENT     24 (bytes)

        Number of items in the symbol table.

    0x00000003 DT_PLTGOT     0x1338

        Address of .got.plt table.

    0x00000002 DT_PLTRELSZ   24 (bytes)

        Size of the relocation table entry in bytes.

    0x00000014 DT_PLTREL     RELA

        Type of the PLT relocation table: either Elf_Rel or Elf_Rela.

    0x00000017 DT_JMPREL     0x168

        Address of section .rela.plt - PLT relocation table.

    0x7ffffffd DT_AUXILIARY  0x1a = "pie"

        Name offset of the binary type.

    0x00000011 DT_REL        0x12c

        Address of section .rel.dyn - relocation table.

    0x00000012 DT_RELSZ      8 (bytes)

        Size of the relocation table in bytes.

    0x00000013 DT_RELENT     8 (bytes)

        Size of structure Elf_Rel in bytes.

    0x6ffffffa DT_RELCOUNT   1

Contents of PLT relocation table .rela.plt:

    Relocation section with addend (.rela.plt):
    r_offset     r_info       r_type              st_value         st_name + r_addend
    000000001350 000100000007 R_X86_64_JMP_SLOT   0000000000000000 say + 0

    Relocation section (.rel.plt):
    r_offset r_info   r_type              st_value st_name
    00001224 00000216 R_ARM_JUMP_SLOT     00000000 say

    Relocation section (.rel.dyn):
    r_offset r_info   r_type              st_value st_name
    00001228 00000017 R_ARM_RELATIVE      00000000
