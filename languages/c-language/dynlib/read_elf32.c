//
// Information about dynamic object.
//
typedef struct {
    const char *dynstr;     // Bytes of .dynstr section - a string table for dynamic symbols
    unsigned dynstr_nbytes; // Size of .dynstr string table

    Elf32_Sym *dynsym;      // Table of dynamic symbols

    const char *got_plt;    // .got.plt table
    const char *rel_plt;    // .rel.plt table
} Dynamic32;

//
// Parse the PT_DYNAMIC section.
// Obtain addresses of .dynstr and .dynsym tables.
//
static void parse_elf32_dynamic(Dynamic32 *dyninfo, char *exec_buf, unsigned dynamic_offset, unsigned nbytes)
{
    for (char *addr = exec_buf + dynamic_offset;
         nbytes >= sizeof(Elf32_Dyn);
         addr += sizeof(Elf32_Dyn),
         nbytes -= sizeof(Elf32_Dyn)) {

        Elf32_Dyn *item = (Elf32_Dyn*) addr;
        printf("d_tag = %ju, d_val = 0x%jx\n", (intmax_t)item->d_tag, (uintmax_t)item->d_un.d_val);

        switch (item->d_tag) {
        case DT_NULL:
            // Empty record - ignore.
            break;

        case DT_NEEDED:
            // Name of the shared object this object depends on, like "mylib-x86_64.so".
            // Value is offset in .dynstr table.
            // Ignore.
            break;

        case DT_AUXILIARY:
            // String "pie" in in .dynstr table.
            // Ignore.
            break;

        case DT_HASH:
            // Address of .hash section.
            // Ignore.
            break;

        case DT_STRTAB:
            // Address of section .dynstr - a string table.
            // For example:
            //  [ 1]  main
            //  [ 6]  say
            //  [ a]  mylib-x86_64.so
            //  [1a]  pie
            dyninfo->dynstr = exec_buf + item->d_un.d_val;
            break;

        case DT_SYMTAB:
            // Address of section .dynsym - a symbol table.
            // For example:
            // Num:    Value  Size Type    Bind   Vis      Ndx Name
            //   0: 00000000     0 NOTYPE  LOCAL  DEFAULT  UND
            //   1: 00000000     0 FUNC    GLOBAL DEFAULT  UND say
            //   2: 000001a0    23 FUNC    GLOBAL DEFAULT    6 main
            dyninfo->dynsym = (Elf32_Sym*) (exec_buf + item->d_un.d_val);
            break;

        case DT_STRSZ:
            // Size of the string table in bytes.
            dyninfo->dynstr_nbytes = item->d_un.d_val;
            break;

        case DT_SYMENT:
            // Size of entry in the symbol table in bytes.
	    if (item->d_un.d_val != sizeof(Elf32_Sym)) {
                errexit(-1, ENOEXEC, "Bad size of symbol table");
            }
            break;

        case DT_PLTGOT:
            // Address of .got.plt table.
            dyninfo->got_plt = exec_buf + item->d_un.d_val;
            break;

        case DT_PLTRELSZ:
            // Size of the relocation table entry.
	    if (item->d_un.d_val != sizeof(Elf32_Rel)) {
                errexit(-1, ENOEXEC, "Bad size of relocation table entry");
            }
            break;

        case DT_PLTREL:
            // Type of the PLT relocation: either Elf_Rel or Elf_Rela.
	    if (item->d_un.d_val != DT_REL) {
                errexit(-1, ENOEXEC, "Bad type of the PLT relocation");
            }
            break;

        case DT_JMPREL:
            // Address of section .rel.plt - PLT relocation table.
            // For example:
            //  Offset     Info    Type            Sym.Value  Sym. Name
            // 00001224  00000216 R_ARM_JUMP_SLOT   00000000   say
            dyninfo->rel_plt = exec_buf + item->d_un.d_val;
            break;
        }
    }

    printf("dynstr = %p, size %u bytes\n", dyninfo->dynstr, dyninfo->dynstr_nbytes);
    printf("dynsym = %p\n", dyninfo->dynsym);   // array of Elf32_Sym
    printf("got_plt = %p\n", dyninfo->got_plt); // array of Elf32_Addr (uint32_t)
    printf("rel_plt = %p\n", dyninfo->rel_plt); // array of Elf32_Rel
}

//
// Find total size of the ELF executable
//
static unsigned find_elf32_size(const Elf32_Ehdr *hdr, const Elf32_Phdr segment[])
{
    unsigned vaddr_min = 0, vaddr_max = 0;

    for (int i = 0; i < hdr->e_phnum; i++) {
        // Chooze loadable non-empty segments.
        if (segment[i].p_type == PT_LOAD && segment[i].p_memsz > 0) {
            unsigned first = segment[i].p_vaddr;
            unsigned last  = first + segment[i].p_memsz;

            if (vaddr_min > first)
                vaddr_min = first;
            if (vaddr_max < last)
                vaddr_max = last;
        }
    }
    return vaddr_max - vaddr_min;
}

void read_elf32_file(int elf_file, const char *filename, unsigned elf_machine)
{
    //
    // Read ELF header, which is at the beginning of the file.
    //
    Elf32_Ehdr elf_header;
    if (lseek(elf_file, 0, SEEK_SET) != 0) {
        err(-1, "Cannot seek ELF header");
    }
    if (read(elf_file, (char *)&elf_header, sizeof elf_header) != sizeof elf_header) {
        err(-1, "%s: Cannot read ELF header", filename);
    }

    //
    // Make sure ELF header is correct.
    //
    if (elf_header.e_ehsize != sizeof(Elf32_Ehdr)) {
        errexit(-1, ENOEXEC, "Bad ELF header size");
    }
    if (elf_header.e_type != ET_DYN) {
        errexit(-1, ENOEXEC, "Not dynamic");
    }
    if (elf_header.e_machine != elf_machine) {
        errexit(-1, ENOEXEC, "Bad ELF machine %u, expect %u", elf_header.e_machine, elf_machine);
    }
    if (elf_header.e_version != EV_CURRENT) {
        errexit(-1, ENOEXEC, "Bad ELF version");
    }
    if (elf_header.e_phentsize != sizeof(Elf32_Phdr)) {
        errexit(-1, ENOEXEC, "Bad ELF Program Header Entry size");
    }
    if (elf_header.e_phoff == 0) {
        errexit(-1, ENOEXEC, "Bad ELF Program Header offset");
    }
    if (elf_header.e_phnum == 0) {
        errexit(-1, ENOEXEC, "Empty Program headers");
    }
    if (elf_header.e_shnum == 0) {
        errexit(-1, ENOEXEC, "Empty Section headers");
    }
    if (elf_header.e_shentsize != sizeof(Elf32_Shdr)) {
        errexit(-1, ENOEXEC, "Bad Section header size");
    }

    //
    // Read program header.
    //
    Elf32_Phdr segment[elf_header.e_phnum];
    if (lseek(elf_file, elf_header.e_phoff, SEEK_SET) != elf_header.e_phoff) {
        err(-1, "Cannot seek Program header");
    }
    if (read(elf_file, (char *)&segment[0], elf_header.e_phnum * sizeof(Elf32_Phdr)) != elf_header.e_phnum * sizeof(Elf32_Phdr)) {
        err(-1, "Cannot read Program header");
    }

    //
    // Allocate memory for the executable via calloc().
    //
    unsigned exec_nbytes = find_elf32_size(&elf_header, segment);
    void *exec_buf = calloc(exec_nbytes, 1);
    if (!exec_buf) {
        err(-1, "Cannot allocale memory");
    }
    printf("exec_buf = %p, size %u bytes\n", exec_buf, exec_nbytes);

    //
    // Read loadable segments.
    //
    Dynamic32 dyninfo = {0};
    for (int i = 0; i < elf_header.e_phnum; i++) {
        // Chooze loadable non-empty segments.
        unsigned vaddr  = segment[i].p_vaddr;
        unsigned memsz  = segment[i].p_memsz;
        unsigned filesz = segment[i].p_filesz;

        switch (segment[i].p_type) {
        case PT_LOAD:
            if (memsz <= 0) {
                // Skip empty sections.
                continue;
            }

            // Verbose logging.
            if (segment[i].p_flags & PF_X) {
                //
                // Executable code.
                //
                printf("Code 0x%x-0x%x size %u bytes\n",
                       vaddr, vaddr + memsz - 1, memsz);

                if (vaddr != 0) {
                    errexit(-1, ENOEXEC, "Virtual address of code segment must be 0");
                }
                if (segment[i].p_offset != 0) {
                    errexit(-1, ENOEXEC, "File offset of code segment must be 0");
                }

                // Read data from file.
                if (lseek(elf_file, 0, SEEK_SET) != 0) {
                    err(-1, "Cannot seek executable segment #%u", i);
                }
                if (read(elf_file, exec_buf, filesz) != filesz) {
                    err(-1, "%s: Cannot read executable segment #%u", filename, i);
                }

            } else if (filesz > 0) {
                //
                // Initialized data.
                //
                printf("Data 0x%x-0x%x size %u bytes\n",
                       vaddr, vaddr + filesz - 1, filesz);

                // Read data from file.
                if (lseek(elf_file, segment[i].p_offset, SEEK_SET) != segment[i].p_offset) {
                    err(-1, "Cannot seek data segment #%u", i);
                }

                char *data_addr = (char*)exec_buf + vaddr;
                //printf("read %u bytes from offset %u to vaddr 0x%x (%p)\n", (unsigned)filesz, (unsigned)segment[i].p_offset, (unsigned)vaddr, data_addr);
                if (read(elf_file, data_addr, filesz) != filesz) {
                    err(-1, "%s: Cannot read data segment #%u", filename, i);
                }

                if (memsz > filesz) {
                    // Clear BSS.
                    printf("BSS  0x%x-0x%x size %u bytes\n",
                           vaddr + filesz, vaddr + memsz - 1, memsz - filesz);
                    memset(data_addr + filesz, 0, memsz - filesz);
                }
            }
            break;

        case PT_DYNAMIC:
            // Parse dynamic section
            printf("Dynamic 0x%x-0x%x size %u bytes\n",
                   vaddr, vaddr + memsz - 1, memsz);
            parse_elf32_dynamic(&dyninfo, exec_buf, vaddr, memsz);
            break;

        case PT_LOPROC + 1:
            // .ARM.exidx section - can be safely ignored.
            break;

        default:
            errexit(-1, ENOEXEC, "Unknown Program Header type 0x%x", segment[i].p_type);
            break;
        }
    }

    //
    // Set PC to the entry point.
    //
    unsigned entry_address = elf_header.e_entry;
    printf("Entry address 0x%jx\n", (uintmax_t)entry_address);
    //TODO: cpu.set_pc(entry_address);

    //
    // Load section headers.
    //
    Elf32_Shdr section_header[elf_header.e_shnum];
    if (lseek(elf_file, elf_header.e_shoff, SEEK_SET) != elf_header.e_shoff) {
        err(-1, "Cannot seek Section header");
    }
    if (read(elf_file, (char *)&section_header[0], elf_header.e_shnum * sizeof(Elf32_Shdr)) != elf_header.e_shnum * sizeof(Elf32_Shdr)) {
        err(-1, "Cannot read Section header");
    }

#if 0
    //
    // Load Section Header String table.
    //
    unsigned shstr_table_size = section_header[elf_header.e_shstrndx].sh_size;
    char shstr_table[shstr_table_size];
    if (shstr_table_size > 0) {
        // Load the string table.
        if (lseek(elf_file, section_header[elf_header.e_shstrndx].sh_offset, SEEK_SET) != section_header[elf_header.e_shstrndx].sh_offset) {
            err(-1, "Cannot seek Section Header String table");
        }
        if (read(elf_file, shstr_table, shstr_table_size) != shstr_table_size) {
            err(-1, "Cannot read Section Header String table");
        }
    }

    //
    // Load the string table.
    //
    char *string_table = 0;
    unsigned string_table_size = 0;
    for (unsigned i = 0; i < elf_header.e_shnum; i++) {
        if (section_header[i].sh_type == SHT_STRTAB &&
            section_header[i].sh_name < shstr_table_size &&
            strcmp(".strtab", section_header[i].sh_name + shstr_table) == 0) {

            unsigned num_bytes = section_header[i].sh_size;
            if (num_bytes > 0) {
                // Load the string table.
                string_table_size = num_bytes;
                string_table = alloca(num_bytes);
                if (string_table == 0) {
                    errexit(-1, ENOMEM, "Cannot allocate String table");
                }
                if (lseek(elf_file, section_header[i].sh_offset, SEEK_SET) != section_header[i].sh_offset) {
                    err(-1, "Cannot seek String table");
                }
                if (read(elf_file, string_table, num_bytes) != num_bytes) {
                    err(-1, "Cannot read String table");
                }
            }
            break;
        }
    }
    if (string_table == 0) {
        errexit(-1, ENOEXEC, "No String table");
    }

    //
    // Load the symbol table.
    //
    for (unsigned i = 0; i < elf_header.e_shnum; i++) {
        if (section_header[i].sh_type == SHT_SYMTAB) {
            if (section_header[i].sh_entsize != sizeof(Elf32_Sym)) {
                errexit(-1, ENOEXEC, "Bad Symbol Entry size");
            }

            unsigned num_symbols = section_header[i].sh_size / section_header[i].sh_entsize;
            if (num_symbols > 0) {
                // Load the symbol table.
                Elf32_Sym symbol_table[num_symbols];

                if (lseek(elf_file, section_header[i].sh_offset, SEEK_SET) != section_header[i].sh_offset) {
                    err(-1, "Cannot seek Symbol Table");
                }
                if (read(elf_file, (char *)&symbol_table[0], section_header[i].sh_size) != section_header[i].sh_size) {
                    err(-1, "Cannot read Symbol Table");
                }

                for (unsigned s = 0; s < num_symbols; s++) {
                    if (symbol_table[s].st_name != STN_UNDEF &&
                        symbol_table[s].st_name < string_table_size) {
                        // Found a global untyped symbol with valid name.
                        unsigned name_offset = symbol_table[s].st_name;
                        const char *name = string_table + name_offset;

                        printf("Symbol %s = 0x%x\n", name, symbol_table[s].st_value);
                    }
                }
            }
        }
    }
#endif

    //
    // Parse the .rel.plt section.
    //
    // Name       Type  Addr     Off    Size   ES Flg Lk Inf Al
    // .rel.plt   REL   00000134 000134 000008 08  AI  2  10  4
    //
    for (unsigned i = 0; i < elf_header.e_shnum; i++) {
        if (section_header[i].sh_type == SHT_REL) {
            if (section_header[i].sh_entsize != sizeof(Elf32_Rel)) {
                errexit(-1, ENOEXEC, "Bad size of relocation table entry");
            }

            unsigned num_relocations = section_header[i].sh_size / section_header[i].sh_entsize;
            if (num_relocations > 0) {
                // Load the relocation table.
                Elf32_Rel relocation_table[num_relocations];

                if (lseek(elf_file, section_header[i].sh_offset, SEEK_SET) != section_header[i].sh_offset) {
                    err(-1, "Cannot seek relocation table");
                }
                if (read(elf_file, (char *)&relocation_table[0], section_header[i].sh_size) != section_header[i].sh_size) {
                    err(-1, "Cannot read relocation table");
                }

                for (unsigned r = 0; r < num_relocations; r++) {
                    Elf32_Addr offset = relocation_table[r].r_offset; // Location (file byte offset, or program virtual addr)
                    Elf32_Addr info   = relocation_table[r].r_info;   // Symbol table index and type of relocation to apply

                    unsigned sym  = ELF32_R_SYM(info);  // Index in the .dynsym table
                    unsigned type = ELF32_R_TYPE(info); // Type like 22=R_ARM_JUMP_SLOT or 23=R_ARM_RELATIVE

                    const char *name = &dyninfo.dynstr[dyninfo.dynsym[sym].st_name];

                    printf("Relocation offset = 0x%jx, sym = #%u '%s', type = %u\n",
                           (uintmax_t)offset, sym, name, type);
                }
            }
        }
    }

    // Free the memory after execution.
    free(exec_buf);
}
