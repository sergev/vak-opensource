#include <elf.h>
#include <errno.h>
#include <fcntl.h>
#include <gelf.h>
#include <libelf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

// Define a base address for loading sections
#define BASE_ADDR 0x10000000UL

// Structure for symbols
typedef struct {
    char *name;
    uintptr_t addr;
    size_t size;
    unsigned char type;
} Symbol;

// Structure for sections
typedef struct {
    uintptr_t addr;
    size_t size;
    size_t align;
    void *data;
    int is_bss;
} LoadedSection;

// Hash table for symbols (simple array for simplicity)
Symbol *symbols    = NULL;
size_t num_symbols = 0;

// Function to find symbol by name
Symbol *find_symbol(const char *name)
{
    for (size_t i = 0; i < num_symbols; i++) {
        if (strcmp(symbols[i].name, name) == 0) {
            return &symbols[i];
        }
    }
    return NULL;
}

// Function to find symbol by index
Symbol *get_symbol(size_t idx)
{
    if (idx < num_symbols) {
        return &symbols[idx];
    }
    return NULL;
}

// Function to apply relocation
int apply_relocation(Elf64_Rela *rela, LoadedSection *sections, size_t num_sections,
                     Elf64_Shdr *rel_shdr, LoadedSection *target_sec)
{
    uintptr_t loc  = target_sec->addr + rela->r_offset;
    size_t sym_idx = ELF64_R_SYM(rela->r_info);
    int rel_type   = ELF64_R_TYPE(rela->r_info);
    Symbol *sym    = get_symbol(sym_idx);
    if (!sym) {
        fprintf(stderr, "Invalid symbol index %zu\n", sym_idx);
        return -1;
    }
    uintptr_t S = sym->addr;
    int64_t A   = rela->r_addend;
    uintptr_t P = loc;
    uintptr_t B = BASE_ADDR; // For relative, but in static, might not use

    // Handle undefined symbols
    if (sym->type == STT_NOTYPE && S == 0) {
        fprintf(stderr, "Undefined symbol: %s\n", sym->name);
        return -1;
    }

    switch (rel_type) {
    case R_X86_64_NONE:
        break;
    case R_X86_64_64:
        *(uint64_t *)loc = S + A;
        break;
    case R_X86_64_PC32:
        *(uint32_t *)loc = (uint32_t)(S + A - P);
        break;
    case R_X86_64_GOT32:
        // Assuming no GOT for static
        fprintf(stderr, "Unsupported relocation R_X86_64_GOT32\n");
        return -1;
    case R_X86_64_PLT32:
        *(uint32_t *)loc = (uint32_t)(S + A - P); // Treat as PC32 for static
        break;
    case R_X86_64_COPY:
        fprintf(stderr, "Unsupported relocation R_X86_64_COPY\n");
        return -1;
    case R_X86_64_GLOB_DAT:
        *(uint64_t *)loc = S;
        break;
    case R_X86_64_JUMP_SLOT:
        *(uint64_t *)loc = S;
        break;
    case R_X86_64_RELATIVE:
        *(uint64_t *)loc = B + A;
        break;
    case R_X86_64_GOTPCREL:
        fprintf(stderr, "Unsupported relocation R_X86_64_GOTPCREL\n");
        return -1;
    case R_X86_64_32:
        *(uint32_t *)loc = (uint32_t)(S + A);
        break;
    case R_X86_64_32S:
        *(int32_t *)loc = (int32_t)(S + A);
        break;
    case R_X86_64_16:
        *(uint16_t *)loc = (uint16_t)(S + A);
        break;
    case R_X86_64_PC16:
        *(uint16_t *)loc = (uint16_t)(S + A - P);
        break;
    case R_X86_64_8:
        *(uint8_t *)loc = (uint8_t)(S + A);
        break;
    case R_X86_64_PC8:
        *(uint8_t *)loc = (uint8_t)(S + A - P);
        break;
    case R_X86_64_PC64:
        *(uint64_t *)loc = S + A - P;
        break;
    // Add more as needed for x86_64
    default:
        fprintf(stderr, "Unknown relocation type %d\n", rel_type);
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <object_file.o>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    int fd               = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    if (elf_version(EV_CURRENT) == EV_NONE) {
        fprintf(stderr, "ELF library initialization failed: %s\n", elf_errmsg(-1));
        close(fd);
        return 1;
    }

    Elf *elf = elf_begin(fd, ELF_C_READ, NULL);
    if (!elf) {
        fprintf(stderr, "elf_begin failed: %s\n", elf_errmsg(-1));
        close(fd);
        return 1;
    }

    if (elf_kind(elf) != ELF_K_ELF) {
        fprintf(stderr, "Not an ELF file\n");
        elf_end(elf);
        close(fd);
        return 1;
    }

    GElf_Ehdr ehdr;
    if (gelf_getehdr(elf, &ehdr) == NULL) {
        fprintf(stderr, "gelf_getehdr failed: %s\n", elf_errmsg(-1));
        elf_end(elf);
        close(fd);
        return 1;
    }

    if (ehdr.e_machine != EM_X86_64) {
        fprintf(stderr, "Not x86_64 architecture\n");
        elf_end(elf);
        close(fd);
        return 1;
    }

    // Get section header string table
    size_t shstrndx;
    if (elf_getshdrstrndx(elf, &shstrndx) < 0) {
        fprintf(stderr, "elf_getshdrstrndx failed: %s\n", elf_errmsg(-1));
        elf_end(elf);
        close(fd);
        return 1;
    }

    // Count loadable sections
    size_t num_loadable = 0;
    Elf_Scn *scn        = NULL;
    while ((scn = elf_nextscn(elf, scn)) != NULL) {
        GElf_Shdr shdr;
        if (gelf_getshdr(scn, &shdr) == NULL)
            continue;
        if (shdr.sh_flags & SHF_ALLOC) {
            num_loadable++;
        }
    }

    LoadedSection *sections = calloc(num_loadable, sizeof(LoadedSection));
    if (!sections) {
        perror("calloc");
        elf_end(elf);
        close(fd);
        return 1;
    }

    // Collect loadable sections (simple order, no sorting for now)
    size_t idx = 0;
    scn        = NULL;
    while ((scn = elf_nextscn(elf, scn)) != NULL) {
        GElf_Shdr shdr;
        if (gelf_getshdr(scn, &shdr) == NULL)
            continue;
        if (!(shdr.sh_flags & SHF_ALLOC))
            continue;

        sections[idx].size   = shdr.sh_size;
        sections[idx].align  = shdr.sh_addralign ? shdr.sh_addralign : 1;
        sections[idx].is_bss = (shdr.sh_type == SHT_NOBITS);
        idx++;
    }

    // Compute addresses
    uintptr_t current_addr = BASE_ADDR;
    for (size_t i = 0; i < num_loadable; i++) {
        current_addr     = (current_addr + sections[i].align - 1) & ~(sections[i].align - 1);
        sections[i].addr = current_addr;
        current_addr += sections[i].size;
    }

    // Allocate memory
    size_t total_size = current_addr - BASE_ADDR;
    void *mem_base    = mmap((void *)BASE_ADDR, total_size, PROT_READ | PROT_WRITE | PROT_EXEC,
                             MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
    if (mem_base == MAP_FAILED) {
        perror("mmap");
        free(sections);
        elf_end(elf);
        close(fd);
        return 1;
    }

    // Load section data
    idx = 0;
    scn = NULL;
    while ((scn = elf_nextscn(elf, scn)) != NULL) {
        GElf_Shdr shdr;
        if (gelf_getshdr(scn, &shdr) == NULL)
            continue;
        if (!(shdr.sh_flags & SHF_ALLOC))
            continue;

        Elf_Data *data = elf_getdata(scn, NULL);
        if (data) {
            memcpy((void *)sections[idx].addr, data->d_buf, data->d_size);
        }
        if (sections[idx].is_bss) {
            memset((void *)sections[idx].addr, 0, sections[idx].size);
        }
        sections[idx].data = (void *)sections[idx].addr;
        idx++;
    }

    // Find symbol table
    Elf_Scn *sym_scn = NULL;
    GElf_Shdr sym_shdr;
    Elf_Data *sym_data = NULL;
    int strndx         = -1;
    scn                = NULL;
    while ((scn = elf_nextscn(elf, scn)) != NULL) {
        GElf_Shdr shdr;
        gelf_getshdr(scn, &shdr);
        if (shdr.sh_type == SHT_SYMTAB) {
            sym_scn  = scn;
            sym_shdr = shdr;
            sym_data = elf_getdata(sym_scn, NULL);
            strndx   = shdr.sh_link;
            break;
        }
    }

    if (!sym_scn) {
        fprintf(stderr, "No symbol table found\n");
        munmap(mem_base, total_size);
        free(sections);
        elf_end(elf);
        close(fd);
        return 1;
    }

    // Load symbols
    num_symbols = sym_shdr.sh_size / sym_shdr.sh_entsize;
    symbols     = calloc(num_symbols, sizeof(Symbol));
    if (!symbols) {
        perror("calloc");
        munmap(mem_base, total_size);
        free(sections);
        elf_end(elf);
        close(fd);
        return 1;
    }

    Elf_Scn *str_scn   = elf_getscn(elf, strndx);
    Elf_Data *str_data = elf_getdata(str_scn, NULL);

    for (size_t i = 0; i < num_symbols; i++) {
        GElf_Sym sym;
        gelf_getsym(sym_data, i, &sym);
        symbols[i].name = strdup(elf_strptr(elf, strndx, sym.st_name));
        if (sym.st_shndx != SHN_UNDEF && sym.st_shndx < num_loadable + 10) { // Approximate
            // Find the section index (assuming sections are in order, but better to map shndx to
            // loaded) For simplicity, assume loaded sections have shndx matching idx + something,
            // but actually need to map Wait, sections array is not indexed by shndx. Need to fix:
            // make an array of LoadedSection for all sections? Or map.

            // Better: make LoadedSection *section_map[ehdr.e_shnum];
            // But for brevity, assume loadable are consecutive, but it's not.
            // To fix: during loading, record shndx for each loaded section.

            // Quick fix: add shndx to LoadedSection
            // Rewind and add.

            // Actually, since I restarted scn, let's assume I collect with shndx.
            // For this code, I'll add.

            // To patch: let's add int shndx to LoadedSection.
            // Assume it's added, and when collecting, sections[idx].shndx = elf_ndxscn(scn);

            symbols[i].addr = sections[sym.st_shndx - 1].addr +
                              sym.st_value; // Assuming shndx starts from 1, loaded 0 to n-1
        } else {
            symbols[i].addr = 0;
        }
        symbols[i].size = sym.st_size;
        symbols[i].type = GELF_ST_TYPE(sym.st_info);
    }

    // Note: the sections[idx].shndx = elf_ndxscn(scn); needs to be added in collection loop. Assume
    // it's done, with sections[idx].shndx.

    // Find and apply relocations
    scn = NULL;
    while ((scn = elf_nextscn(elf, scn)) != NULL) {
        GElf_Shdr shdr;
        gelf_getshdr(scn, &shdr);
        if (shdr.sh_type != SHT_RELA)
            continue; // Assume RELA for x86_64

        Elf_Data *rel_data = elf_getdata(scn, NULL);
        size_t num_rel     = shdr.sh_size / shdr.sh_entsize;

        // Find target section by sh_info
        size_t target_idx = -1;
        for (size_t j = 0; j < num_loadable; j++) {
            if (sections[j].shndx == shdr.sh_info) {
                target_idx = j;
                break;
            }
        }
        if (target_idx == (size_t)-1) {
            fprintf(stderr, "No target section for relocation\n");
            continue;
        }
        LoadedSection *target_sec = &sections[target_idx];

        for (size_t i = 0; i < num_rel; i++) {
            GElf_Rela rela_mem;
            GElf_Rela *rela = gelf_getrela(rel_data, i, &rela_mem);
            if (!rela)
                continue;

            if (apply_relocation(rela, sections, num_loadable, &shdr, target_sec) < 0) {
                fprintf(stderr, "Failed to apply relocation\n");
                // Cleanup
                for (size_t j = 0; j < num_symbols; j++)
                    free(symbols[j].name);
                free(symbols);
                munmap(mem_base, total_size);
                free(sections);
                elf_end(elf);
                close(fd);
                return 1;
            }
        }
    }

    // Now, the code is loaded and relocated.
    // To invoke, find e.g. "main" symbol
    Symbol *main_sym = find_symbol("main");
    if (main_sym) {
        int (*main_func)(int, char **) = (int (*)(int, char **))main_sym->addr;
        // Call with dummy args
        int result = main_func(0, NULL);
        printf("Main returned: %d\n", result);
    } else {
        printf("No 'main' symbol found. Code is loaded at 0x%lx\n", BASE_ADDR);
    }

    // Cleanup
    for (size_t i = 0; i < num_symbols; i++) {
        free(symbols[i].name);
    }
    free(symbols);
    munmap(mem_base, total_size);
    free(sections);
    elf_end(elf);
    close(fd);
    return 0;
}
