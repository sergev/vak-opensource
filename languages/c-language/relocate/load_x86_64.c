#include <dlfcn.h>
#include <fcntl.h>
#include <gelf.h>
#include <libelf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

// Structure to hold loaded section and symbol table information
typedef struct {
    void *text_addr;     // Address of loaded .text section
    size_t text_size;    // Size of .text section
    Elf64_Sym *symtab;   // Symbol table
    size_t symtab_count; // Number of symbols
    char *strtab;        // String table for symbol names
} LoadedSection;

// Routine to load .text section and apply relocations
// Now extended to load all allocatable sections for proper symbol resolution and relocate all
// sections Returns pointer to executable .text section or NULL on failure
void *load_and_relocate_text(const char *filename, size_t *out_size)
{
    int fd = -1;
    Elf *elf = NULL;
    LoadedSection result = { 0 };
    Elf_Scn *scn = NULL;
    size_t shstrndx;

    // Initialize libelf
    if (elf_version(EV_CURRENT) == EV_NONE) {
        fprintf(stderr, "ELF library initialization failed: %s\n", elf_errmsg(-1));
        return NULL;
    }

    // Open the object file
    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open object file");
        return NULL;
    }

    // Initialize ELF descriptor
    elf = elf_begin(fd, ELF_C_READ, NULL);
    if (!elf) {
        fprintf(stderr, "elf_begin failed: %s\n", elf_errmsg(-1));
        close(fd);
        return NULL;
    }

    // Get section header string table index
    if (elf_getshdrstrndx(elf, &shstrndx) != 0) {
        fprintf(stderr, "elf_getshdrstrndx failed: %s\n", elf_errmsg(-1));
        goto cleanup;
    }

    // Get number of sections
    size_t num_shdr;
    if (elf_getshdrnum(elf, &num_shdr) != 0) {
        fprintf(stderr, "elf_getshdrnum failed: %s\n", elf_errmsg(-1));
        goto cleanup;
    }

    // Array to hold loaded addresses for each section index
    void **section_addrs = calloc(num_shdr, sizeof(void *));
    if (!section_addrs) {
        perror("Failed to allocate section_addrs");
        goto cleanup;
    }

    // First pass: Find .symtab and .strtab
    Elf_Scn *symtab_scn = NULL, *strtab_scn = NULL;
    GElf_Shdr shdr;
    scn = NULL;
    while ((scn = elf_nextscn(elf, scn)) != NULL) {
        if (gelf_getshdr(scn, &shdr) == NULL)
            continue;
        char *name = elf_strptr(elf, shstrndx, shdr.sh_name);
        if (!name)
            continue;
        if (strcmp(name, ".symtab") == 0)
            symtab_scn = scn;
        else if (strcmp(name, ".strtab") == 0)
            strtab_scn = scn;
    }

    if (!symtab_scn || !strtab_scn) {
        fprintf(stderr, "Missing required section (.symtab or .strtab)\n");
        goto cleanup;
    }

    // Load symbol table
    Elf_Data *symtab_data = elf_getdata(symtab_scn, NULL);
    if (!symtab_data) {
        fprintf(stderr, "Failed to load .symtab\n");
        goto cleanup;
    }
    result.symtab_count = symtab_data->d_size / sizeof(Elf64_Sym);
    result.symtab = malloc(symtab_data->d_size);
    if (!result.symtab) {
        perror("Failed to allocate symbol table");
        goto cleanup;
    }
    memcpy(result.symtab, symtab_data->d_buf, symtab_data->d_size);

    // Load string table
    Elf_Data *strtab_data = elf_getdata(strtab_scn, NULL);
    if (!strtab_data) {
        fprintf(stderr, "Failed to load .strtab\n");
        goto cleanup;
    }
    result.strtab = malloc(strtab_data->d_size);
    if (!result.strtab) {
        perror("Failed to allocate string table");
        goto cleanup;
    }
    memcpy(result.strtab, strtab_data->d_buf, strtab_data->d_size);

    // Second pass: Load all allocatable sections
    scn = NULL;
    while ((scn = elf_nextscn(elf, scn)) != NULL) {
        if (gelf_getshdr(scn, &shdr) == NULL)
            continue;
        if ((shdr.sh_flags & SHF_ALLOC) == 0)
            continue;

        char *name = elf_strptr(elf, shstrndx, shdr.sh_name);
        size_t size = shdr.sh_size;
        int prot = PROT_READ | PROT_WRITE; // Temporary write for relocations
        if (shdr.sh_flags & SHF_EXECINSTR)
            prot |= PROT_EXEC;

        void *addr = mmap(NULL, size, prot, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (addr == MAP_FAILED) {
            perror("mmap failed");
            goto cleanup;
        }

        if (shdr.sh_type == SHT_NOBITS) {
            memset(addr, 0, size);
        } else {
            Elf_Data *data = elf_getdata(scn, NULL);
            if (!data || data->d_size != size) {
                fprintf(stderr, "Failed to load data for section %s\n", name ? name : "(unknown)");
                munmap(addr, size);
                continue;
            }
            memcpy(addr, data->d_buf, size);
        }

        size_t ndx = elf_ndxscn(scn);
        section_addrs[ndx] = addr;

        if (name && strcmp(name, ".text") == 0) {
            result.text_addr = addr;
            result.text_size = size;
        }
    }

    if (!result.text_addr) {
        fprintf(stderr, "Missing .text section\n");
        goto cleanup;
    }

    // Third pass: Process all .rela* sections
    scn = NULL;
    while ((scn = elf_nextscn(elf, scn)) != NULL) {
        if (gelf_getshdr(scn, &shdr) == NULL)
            continue;
        if (shdr.sh_type != SHT_RELA)
            continue;

        char *name = elf_strptr(elf, shstrndx, shdr.sh_name);
        if (!name || strncmp(name, ".rela", 5) != 0)
            continue;

        Elf_Data *rela_data = elf_getdata(scn, NULL);
        if (!rela_data)
            continue;

        size_t rela_count = shdr.sh_size / shdr.sh_entsize;
        Elf64_Rela *rela = (Elf64_Rela *)rela_data->d_buf;

        size_t target_ndx = shdr.sh_info;
        void *base_addr = section_addrs[target_ndx];
        if (!base_addr) {
            fprintf(stderr, "Relocations for unloaded section %lu\n", target_ndx);
            continue;
        }

        for (size_t i = 0; i < rela_count; i++) {
            Elf64_Xword type = ELF64_R_TYPE(rela[i].r_info);
            Elf64_Xword sym_idx = ELF64_R_SYM(rela[i].r_info);
            int64_t A = rela[i].r_addend; // Signed for calculations
            uint64_t P = (uint64_t)((char *)base_addr + rela[i].r_offset);
            void *reloc_addr = (char *)base_addr + rela[i].r_offset;

            void *target_addr = NULL;
            Elf64_Sym *sym = NULL;
            const char *sym_name = NULL;

            if (sym_idx != 0) {
                if (sym_idx >= result.symtab_count) {
                    fprintf(stderr, "Invalid symbol index: %lu\n", sym_idx);
                    continue;
                }
                sym = &result.symtab[sym_idx];
                sym_name = result.strtab + sym->st_name;

                if (ELF64_ST_BIND(sym->st_info) == STB_LOCAL ||
                    ELF64_ST_BIND(sym->st_info) == STB_GLOBAL) {
                    if (sym->st_shndx == SHN_UNDEF) {
                        // External symbol
                        target_addr = dlsym(RTLD_DEFAULT, sym_name);
                        if (!target_addr) {
                            fprintf(stderr, "Failed to resolve external symbol %s\n", sym_name);
                            continue;
                        }
                    } else if (sym->st_shndx == SHN_ABS) {
                        target_addr = (void *)(uintptr_t)sym->st_value;
                    } else if (sym->st_shndx < num_shdr && section_addrs[sym->st_shndx]) {
                        target_addr = (char *)section_addrs[sym->st_shndx] + sym->st_value;
                    } else {
                        fprintf(stderr, "Symbol %s in unsupported section %d\n", sym_name,
                                sym->st_shndx);
                        continue;
                    }
                } else {
                    fprintf(stderr, "Unsupported symbol binding for %s\n", sym_name);
                    continue;
                }
            }

            int64_t value = 0; // Use signed for overflow checks

            if (type == R_X86_64_NONE) {
                continue;
            } else if (type == R_X86_64_64) {
                value = (int64_t)(uintptr_t)target_addr + A;
                *(uint64_t *)reloc_addr = (uint64_t)value;
            } else if (type == R_X86_64_PC32) {
                value = (int64_t)(uintptr_t)target_addr + A - (int64_t)P;
                if (value < INT32_MIN || value > INT32_MAX) {
                    fprintf(stderr, "R_X86_64_PC32 overflow for symbol %s\n", sym_name);
                    continue;
                }
                *(uint32_t *)reloc_addr = (uint32_t)value;
            } else if (type == R_X86_64_GOT32) {
                fprintf(stderr, "Unsupported relocation type R_X86_64_GOT32\n");
                continue;
            } else if (type == R_X86_64_PLT32) {
                fprintf(stderr, "Unsupported relocation type R_X86_64_PLT32\n");
                continue;
            } else if (type == R_X86_64_COPY) {
                fprintf(stderr, "Unsupported relocation type R_X86_64_COPY\n");
                continue;
            } else if (type == R_X86_64_GLOB_DAT) {
                fprintf(stderr, "Unsupported relocation type R_X86_64_GLOB_DAT\n");
                continue;
            } else if (type == R_X86_64_JUMP_SLOT) {
                fprintf(stderr, "Unsupported relocation type R_X86_64_JUMP_SLOT\n");
                continue;
            } else if (type == R_X86_64_RELATIVE) {
                if (sym_idx != 0) {
                    fprintf(stderr, "R_X86_64_RELATIVE should have sym_idx 0\n");
                    continue;
                }
                value = A; // B = 0 for relocatable files
                *(uint64_t *)reloc_addr = (uint64_t)value;
            } else if (type == R_X86_64_GOTPCREL) {
                fprintf(stderr, "Unsupported relocation type R_X86_64_GOTPCREL\n");
                continue;
            } else if (type == R_X86_64_32) {
                value = (int64_t)(uintptr_t)target_addr + A;
                if ((uint64_t)value > UINT32_MAX) {
                    fprintf(stderr, "R_X86_64_32 overflow for symbol %s\n", sym_name);
                    continue;
                }
                *(uint32_t *)reloc_addr = (uint32_t)value;
            } else if (type == R_X86_64_32S) {
                value = (int64_t)(uintptr_t)target_addr + A;
                int32_t truncated = (int32_t)value;
                if ((int64_t)truncated != value) {
                    fprintf(stderr, "R_X86_64_32S does not sign-extend for symbol %s\n", sym_name);
                    continue;
                }
                *(uint32_t *)reloc_addr = (uint32_t)truncated;
            } else if (type == R_X86_64_16) {
                value = (int64_t)(uintptr_t)target_addr + A;
                if ((uint64_t)value > 0xFFFF) {
                    fprintf(stderr, "R_X86_64_16 overflow for symbol %s\n", sym_name);
                    continue;
                }
                *(uint16_t *)reloc_addr = (uint16_t)value;
            } else if (type == R_X86_64_PC16) {
                value = (int64_t)(uintptr_t)target_addr + A - (int64_t)P;
                if (value < -0x8000 || value > 0x7FFF) {
                    fprintf(stderr, "R_X86_64_PC16 overflow for symbol %s\n", sym_name);
                    continue;
                }
                *(uint16_t *)reloc_addr = (uint16_t)value;
            } else if (type == R_X86_64_8) {
                value = (int64_t)(uintptr_t)target_addr + A;
                if ((uint64_t)value > 0xFF) {
                    fprintf(stderr, "R_X86_64_8 overflow for symbol %s\n", sym_name);
                    continue;
                }
                *(uint8_t *)reloc_addr = (uint8_t)value;
            } else if (type == R_X86_64_PC8) {
                value = (int64_t)(uintptr_t)target_addr + A - (int64_t)P;
                if (value < -0x80 || value > 0x7F) {
                    fprintf(stderr, "R_X86_64_PC8 overflow for symbol %s\n", sym_name);
                    continue;
                }
                *(uint8_t *)reloc_addr = (uint8_t)value;
            } else if (type == R_X86_64_PC64) {
                value = (int64_t)(uintptr_t)target_addr + A - (int64_t)P;
                *(uint64_t *)reloc_addr = (uint64_t)value;
            } else if (type == R_X86_64_GOTOFF64) {
                fprintf(stderr, "Unsupported relocation type R_X86_64_GOTOFF64\n");
                continue;
            } else if (type == R_X86_64_GOTPC32) {
                fprintf(stderr, "Unsupported relocation type R_X86_64_GOTPC32\n");
                continue;
            } else if (type == R_X86_64_SIZE32) {
                value = (int64_t)sym->st_size + A;
                if ((uint64_t)value > UINT32_MAX) {
                    fprintf(stderr, "R_X86_64_SIZE32 overflow for symbol %s\n", sym_name);
                    continue;
                }
                *(uint32_t *)reloc_addr = (uint32_t)value;
            } else if (type == R_X86_64_SIZE64) {
                value = (int64_t)sym->st_size + A;
                *(uint64_t *)reloc_addr = (uint64_t)value;
            } else {
                fprintf(stderr, "Unsupported relocation type: %lu\n", type);
                continue;
            }
        }
    }

    // Optional: Remove write protection if desired (mprotect)

    // Cleanup ELF resources
    free(section_addrs); // Note: Addresses are still mapped; caller must unmap
    free(result.symtab);
    free(result.strtab);
    elf_end(elf);
    close(fd);

    *out_size = result.text_size;
    return result.text_addr;

cleanup:
    // Unmap all loaded sections
    if (section_addrs) {
        for (size_t i = 0; i < num_shdr; i++) {
            if (section_addrs[i]) {
                GElf_Shdr temp_shdr;
                Elf_Scn *temp_scn = elf_getscn(elf, i);
                if (temp_scn && gelf_getshdr(temp_scn, &temp_shdr) != NULL) {
                    munmap(section_addrs[i], temp_shdr.sh_size);
                }
            }
        }
        free(section_addrs);
    }
    if (result.symtab)
        free(result.symtab);
    if (result.strtab)
        free(result.strtab);
    if (elf)
        elf_end(elf);
    if (fd >= 0)
        close(fd);
    return NULL;
}

// Example usage
int main()
{
    size_t text_size;
    void *text_addr = load_and_relocate_text("example.o", &text_size);
    if (!text_addr) {
        fprintf(stderr, "Failed to load and relocate .text section\n");
        return 1;
    }

    // Example: Call a function at the start of .text (unsafe without knowing entry point)
    typedef void (*FuncPtr)(void);
    FuncPtr func = (FuncPtr)text_addr;
    func();

    // Cleanup: Only unmapping .text here; in production, track and unmap all sections
    munmap(text_addr, text_size);
    return 0;
}
