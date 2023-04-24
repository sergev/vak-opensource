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
        errc(-1, ENOEXEC, "Bad ELF header size");
    }
    if (elf_header.e_type != ET_DYN) {
        errc(-1, ENOEXEC, "Not dynamic");
    }
    if (elf_header.e_machine != elf_machine) {
        errc(-1, ENOEXEC, "Bad ELF machine %u, expect %u", elf_header.e_machine, elf_machine);
    }
    if (elf_header.e_version != EV_CURRENT) {
        errc(-1, ENOEXEC, "Bad ELF version");
    }
    if (elf_header.e_phentsize != sizeof(Elf64_Phdr)) {
        errc(-1, ENOEXEC, "Bad ELF Program Header Entry size");
    }
    if (elf_header.e_phoff == 0) {
        errc(-1, ENOEXEC, "Bad ELF Program Header offset");
    }
    if (elf_header.e_phnum == 0) {
        errc(-1, ENOEXEC, "Empty Program header");
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
                // Executable code.
                printf("Code 0x%jx-0x%jx size %ju bytes\n",
                       (uintmax_t)vaddr, (uintmax_t)(vaddr + memsz - 1), (uintmax_t)memsz);
            } else if (filesz > 0) {
                // Initialized data.
                printf("Data 0x%jx-0x%jx size %ju bytes\n",
                       (uintmax_t)vaddr, (uintmax_t)(vaddr + filesz - 1), (uintmax_t)filesz);
                if (memsz > filesz) {
                    // Zeroed data.
                    printf("BSS  0x%jx-0x%jx size %ju bytes\n",
                           (uintmax_t)(vaddr + filesz), (uintmax_t)(vaddr + memsz - 1), (uintmax_t)(memsz - filesz));
                }
            }

            // Read data from file.
            if (filesz > 0) {
                if (lseek(elf_file, segment[i].p_offset, SEEK_SET) != segment[i].p_offset) {
                    err(-1, "Cannot seek segment #%u", i);
                }

                char buf[filesz];
                if (read(elf_file, buf, filesz) != filesz) {
                    err(-1, "%s: Cannot read segment #%u", filename, i);
                }
                //TODO: write_output((uint8_t *)buf, vaddr, filesz);
            }

            // Clear BSS.
            if (memsz > filesz) {
                //unsigned nbytes = memsz - filesz;
                //unsigned addr = vaddr + filesz;

                //printf("Clear 0x%x-0x%x size %u bytes\n", addr, addr + nbytes - 1, nbytes);
                //TODO: memset(addr, 0, nbytes);
            }
            break;

        case PT_DYNAMIC:
            //TODO: parse dynamic section
            printf("Dynamic 0x%jx-0x%jx size %ju bytes\n",
                   (uintmax_t)vaddr, (uintmax_t)(vaddr + memsz - 1), (uintmax_t)memsz);
            break;

        case PT_LOPROC + 1:
            // .ARM.exidx section - can be safely ignored.
            break;

        default:
            errc(-1, ENOEXEC, "Unknown Program Header type 0x%x", segment[i].p_type);
            break;
        }
    }

    //
    // Set PC to the entry point.
    //
    unsigned entry_address = elf_header.e_entry;
    printf("Entry address: 0x%jx\n", (uintmax_t)entry_address);
    //TODO: cpu.set_pc(entry_address);

    //
    // Scan the symbol table to find signature labels.
    //
    if (elf_header.e_shnum > 0) {
        if (elf_header.e_shentsize != sizeof(Elf64_Shdr)) {
            errc(-1, ENOEXEC, "Bad Section header size");
        }

        // Load section headers.
        Elf64_Shdr section_header[elf_header.e_shnum];
        if (lseek(elf_file, elf_header.e_shoff, SEEK_SET) != elf_header.e_shoff) {
            err(-1, "Cannot seek Section header");
        }
        if (read(elf_file, (char *)&section_header[0], elf_header.e_shnum * sizeof(Elf64_Shdr)) != elf_header.e_shnum * sizeof(Elf64_Shdr)) {
            err(-1, "Cannot read Section header");
        }

        // Load the string table.
        char *string_table = 0;
        unsigned string_table_size = 0;
        for (unsigned i = 0; i < elf_header.e_shnum; i++) {
            if (section_header[i].sh_type == SHT_STRTAB) {
                unsigned num_bytes = section_header[i].sh_size;
                if (num_bytes > 0) {
                    // Load the string table.
                    string_table = malloc(num_bytes);
                    string_table_size = num_bytes;

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

        // Load the symbol table.
        for (unsigned i = 0; i < elf_header.e_shnum; i++) {
            if (section_header[i].sh_type == SHT_SYMTAB) {
                if (section_header[i].sh_entsize != sizeof(Elf64_Sym)) {
                    errc(-1, ENOEXEC, "Bad Symbol Entry size");
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
                            symbol_table[s].st_info == ELF64_ST_INFO(STB_GLOBAL, STT_NOTYPE) &&
                            symbol_table[s].st_name < string_table_size) {
                            // Found a global untyped symbol with valid name.
                            const char *name = &string_table[symbol_table[s].st_name];

                            printf("Symbol %s = 0x%jx\n", name, (uintmax_t) symbol_table[s].st_value);
                        }
                    }
                }
            }
        }
    }
}
