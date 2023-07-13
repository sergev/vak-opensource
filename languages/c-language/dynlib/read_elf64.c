static void parse_elf64_dynamic(char *exec_buf, unsigned dynamic_offset, unsigned nbytes)
{
    unsigned dynstr_nbytes = 0;
    const char *dynstr = 0;
    const char *dynsym = 0;
    const char *got_plt = 0;
    const char *rela_plt = 0;

    for (char *addr = exec_buf + dynamic_offset;
         nbytes >= sizeof(Elf64_Dyn);
         addr += sizeof(Elf64_Dyn),
         nbytes -= sizeof(Elf64_Dyn)) {

        Elf64_Dyn *item = (Elf64_Dyn*) addr;
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
            dynstr = exec_buf + item->d_un.d_val;
            break;

        case DT_SYMTAB:
            // Address of section .dynsym - a symbol table.
            // For example:
            // Num:    Value  Size Type    Bind   Vis      Ndx Name
            //   0: 00000000     0 NOTYPE  LOCAL  DEFAULT  UND
            //   1: 00000000     0 FUNC    GLOBAL DEFAULT  UND say
            //   2: 000001a0    23 FUNC    GLOBAL DEFAULT    6 main
            dynsym = exec_buf + item->d_un.d_val;
            break;

        case DT_STRSZ:
            // Size of the string table in bytes.
            dynstr_nbytes = item->d_un.d_val;
            break;

        case DT_SYMENT:
            // Size of entry in the symbol table in bytes.
	    if (item->d_un.d_val != sizeof(Elf64_Sym)) {
                errexit(-1, ENOEXEC, "Bad size of symbol table");
            }
            break;

        case DT_PLTGOT:
            // Address of .got.plt table.
            got_plt = exec_buf + item->d_un.d_val;
            break;

        case DT_PLTRELSZ:
            // Size of the relocation table entry.
	    if (item->d_un.d_val != sizeof(Elf64_Rela)) {
                errexit(-1, ENOEXEC, "Bad size of relocation table entry");
            }
            break;

        case DT_PLTREL:
            // Type of the PLT relocation: either Elf_Rel or Elf_Rela.
	    if (item->d_un.d_val != DT_RELA) {
                errexit(-1, ENOEXEC, "Bad type of the PLT relocation");
            }
            break;

        case DT_JMPREL:
            // Address of section .rela.plt - PLT relocation table.
            // For example:
            // r_offset r_info       r_type            st_value st_name + r_addend
            // 00001350 000100000007 R_X86_64_JMP_SLOT 00000000 say + 0
            rela_plt = exec_buf + item->d_un.d_val;
            break;
        }
    }

    printf("dynstr = %p, size %u bytes\n", dynstr, dynstr_nbytes);
    printf("dynsym = %p\n", dynsym);     // array of Elf64_Sym
    printf("got_plt = %p\n", got_plt);   // array of Elf64_Addr (uint64_t)
    printf("rela_plt = %p\n", rela_plt); // array of Elf64_Rela
}

//
// Find total size of the ELF executable
//
static uint64_t find_elf64_size(const Elf64_Ehdr *hdr, const Elf64_Phdr segment[])
{
    uint64_t vaddr_min = 0, vaddr_max = 0;

    for (int i = 0; i < hdr->e_phnum; i++) {
        // Chooze loadable non-empty segments.
        if (segment[i].p_type == PT_LOAD && segment[i].p_memsz > 0) {
            uint64_t first = segment[i].p_vaddr;
            uint64_t last  = first + segment[i].p_memsz;

            if (vaddr_min > first)
                vaddr_min = first;
            if (vaddr_max < last)
                vaddr_max = last;
        }
    }
    return vaddr_max - vaddr_min;
}

void read_elf64_file(int elf_file, const char *filename, unsigned elf_machine)
{
    //
    // Read ELF header, which is at the beginning of the file.
    //
    Elf64_Ehdr elf_header;
    if (lseek(elf_file, 0, SEEK_SET) != 0) {
        err(-1, "Cannot seek ELF header");
    }
    if (read(elf_file, (char *)&elf_header, sizeof elf_header) != sizeof elf_header) {
        err(-1, "%s: Cannot read ELF header", filename);
    }

    //
    // Make sure ELF header is correct.
    //
    if (elf_header.e_ehsize != sizeof(Elf64_Ehdr)) {
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
    if (elf_header.e_phentsize != sizeof(Elf64_Phdr)) {
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
    if (elf_header.e_shentsize != sizeof(Elf64_Shdr)) {
        errexit(-1, ENOEXEC, "Bad Section header size");
    }

    //
    // Read program header.
    //
    Elf64_Phdr segment[elf_header.e_phnum];
    if (lseek(elf_file, elf_header.e_phoff, SEEK_SET) != elf_header.e_phoff) {
        err(-1, "Cannot seek Program header");
    }
    if (read(elf_file, (char *)&segment[0], elf_header.e_phnum * sizeof(Elf64_Phdr)) != elf_header.e_phnum * sizeof(Elf64_Phdr)) {
        err(-1, "Cannot read Program header");
    }

    //
    // Allocate virtual memory for the executable via mmap().
    //
    uint64_t exec_nbytes = find_elf64_size(&elf_header, segment);
    void *exec_buf = mmap(NULL, exec_nbytes, PROT_READ | PROT_WRITE,
                          MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (!exec_buf) {
        err(-1, "Cannot map anonymous memory");
    }
    //printf("exec_buf = %p, size %ju bytes\n", exec_buf, (uintmax_t)exec_nbytes);

    //
    // Read loadable segments.
    //
    for (int i = 0; i < elf_header.e_phnum; i++) {
        // Chooze loadable non-empty segments.
        uint64_t vaddr  = segment[i].p_vaddr;
        uint64_t memsz  = segment[i].p_memsz;
        uint64_t filesz = segment[i].p_filesz;

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
                printf("Code 0x%jx-0x%jx size %ju bytes\n",
                       (uintmax_t)vaddr, (uintmax_t)(vaddr + memsz - 1), (uintmax_t)memsz);

                if (vaddr != 0) {
                    errexit(-1, ENOEXEC, "Virtual address of code segment must be 0");
                }
                if (segment[i].p_offset != 0) {
                    errexit(-1, ENOEXEC, "File offset of code segment must be 0");
                }

                // Map executable code to the same address.
                void *code_addr = mmap(exec_buf, filesz, PROT_READ | PROT_EXEC,
                                       MAP_PRIVATE | MAP_FIXED, elf_file, 0);
                //printf("exec_buf = %p, code_addr = %p\n", exec_buf, code_addr);
                if (code_addr != exec_buf) {
                    err(-1, "Cannot map executable segment");
                }

            } else if (filesz > 0) {
                //
                // Initialized data.
                //
                printf("Data 0x%jx-0x%jx size %ju bytes\n",
                       (uintmax_t)vaddr, (uintmax_t)(vaddr + filesz - 1), (uintmax_t)filesz);

                // Read data from file.
                if (lseek(elf_file, segment[i].p_offset, SEEK_SET) != segment[i].p_offset) {
                    err(-1, "Cannot seek segment #%u", i);
                }

                char *data_addr = (char*)exec_buf + vaddr;
                //printf("read %u bytes from offset %u to vaddr 0x%x (%p)\n", (unsigned)filesz, (unsigned)segment[i].p_offset, (unsigned)vaddr, data_addr);
                if (read(elf_file, data_addr, filesz) != filesz) {
                    err(-1, "%s: Cannot read segment #%u", filename, i);
                }

                if (memsz > filesz) {
                    // Clear BSS.
                    printf("BSS  0x%jx-0x%jx size %ju bytes\n",
                           (uintmax_t)(vaddr + filesz), (uintmax_t)(vaddr + memsz - 1), (uintmax_t)(memsz - filesz));
                    memset(data_addr + filesz, 0, memsz - filesz);
                }
            }
            break;

        case PT_DYNAMIC:
            // Parse dynamic section.
            printf("Dynamic 0x%jx-0x%jx size %ju bytes\n",
                   (uintmax_t)vaddr, (uintmax_t)(vaddr + memsz - 1), (uintmax_t)memsz);
            parse_elf64_dynamic(exec_buf, vaddr, memsz);
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
    Elf64_Shdr section_header[elf_header.e_shnum];
    if (lseek(elf_file, elf_header.e_shoff, SEEK_SET) != elf_header.e_shoff) {
        err(-1, "Cannot seek Section header");
    }
    if (read(elf_file, (char *)&section_header[0], elf_header.e_shnum * sizeof(Elf64_Shdr)) != elf_header.e_shnum * sizeof(Elf64_Shdr)) {
        err(-1, "Cannot read Section header");
    }

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
            if (section_header[i].sh_entsize != sizeof(Elf64_Sym)) {
                errexit(-1, ENOEXEC, "Bad Symbol Entry size");
            }

            unsigned num_symbols = section_header[i].sh_size / section_header[i].sh_entsize;
            if (num_symbols > 0) {
                // Load the symbol table.
                Elf64_Sym symbol_table[num_symbols];

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

                        printf("Symbol %s = 0x%jx\n", name, (uintmax_t) symbol_table[s].st_value);
                    }
                }
            }
        }
    }
}
