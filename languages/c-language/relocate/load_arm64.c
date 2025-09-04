#include <dlfcn.h>
#include <fcntl.h>
#include <gelf.h>
#include <libelf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

// ARM64 relocation types (from elf.h)
#ifndef R_AARCH64_NONE
#define R_AARCH64_NONE 0
#define R_AARCH64_ABS64 257
#define R_AARCH64_ABS32 258
#define R_AARCH64_ABS16 259
#define R_AARCH64_PREL64 260
#define R_AARCH64_PREL32 261
#define R_AARCH64_PREL16 262
#define R_AARCH64_MOVW_UABS_G0 263
#define R_AARCH64_MOVW_UABS_G0_NC 264
#define R_AARCH64_MOVW_UABS_G1 265
#define R_AARCH64_MOVW_UABS_G1_NC 266
#define R_AARCH64_MOVW_UABS_G2 267
#define R_AARCH64_MOVW_UABS_G2_NC 268
#define R_AARCH64_MOVW_UABS_G3 269
#define R_AARCH64_MOVW_SABS_G0 270
#define R_AARCH64_MOVW_SABS_G1 271
#define R_AARCH64_MOVW_SABS_G2 272
#define R_AARCH64_LD_PREL_LO19 273
#define R_AARCH64_ADR_PREL_LO21 274
#define R_AARCH64_ADR_PREL_PG_HI21 275
#define R_AARCH64_ADR_PREL_PG_HI21_NC 276
#define R_AARCH64_ADD_ABS_LO12_NC 277
#define R_AARCH64_LDST8_ABS_LO12_NC 278
#define R_AARCH64_TSTBR14 279
#define R_AARCH64_CONDBR19 280
#define R_AARCH64_JUMP26 282
#define R_AARCH64_CALL26 283
#define R_AARCH64_LDST16_ABS_LO12_NC 284
#define R_AARCH64_LDST32_ABS_LO12_NC 285
#define R_AARCH64_LDST64_ABS_LO12_NC 286
#define R_AARCH64_MOVW_PREL_G0 273
#define R_AARCH64_MOVW_PREL_G0_NC 274
#define R_AARCH64_MOVW_PREL_G1 275
#define R_AARCH64_MOVW_PREL_G1_NC 276
#define R_AARCH64_MOVW_PREL_G2 277
#define R_AARCH64_MOVW_PREL_G2_NC 278
#define R_AARCH64_MOVW_PREL_G3 279
#define R_AARCH64_LDST128_ABS_LO12_NC 299
#define R_AARCH64_MOVW_GOTOFF_G0 300
#define R_AARCH64_MOVW_GOTOFF_G0_NC 301
#define R_AARCH64_MOVW_GOTOFF_G1 302
#define R_AARCH64_MOVW_GOTOFF_G1_NC 303
#define R_AARCH64_MOVW_GOTOFF_G2 304
#define R_AARCH64_MOVW_GOTOFF_G2_NC 305
#define R_AARCH64_MOVW_GOTOFF_G3 306
#define R_AARCH64_GOTREL64 307
#define R_AARCH64_GOTREL32 308
#define R_AARCH64_ADR_GOTPAGE 311
#define R_AARCH64_LD64_GOT_LO12_NC 312
#define R_AARCH64_LD64_GOTPAGE_LO15 313
#define R_AARCH64_TLSGD_ADR_PREL21 512
#define R_AARCH64_TLSGD_ADR_PAGE21 513
#define R_AARCH64_TLSGD_ADD_LO12_NC 514
#define R_AARCH64_TLSGD_MOVW_G1 515
#define R_AARCH64_TLSGD_MOVW_G0_NC 516
// And more TLS, but for basic, this should cover many.
#endif

// Structure to hold loaded section and symbol table information
typedef struct {
    void *text_addr;     // Address of loaded .text section
    size_t text_size;    // Size of .text section
    Elf64_Sym *symtab;   // Symbol table
    size_t symtab_count; // Number of symbols
    char *strtab;        // String table for symbol names
} LoadedSection;

// Helper function to apply value to instruction for MOVW types (imm16 at bits 20:5)
static void apply_movw_imm(void *reloc_addr, uint64_t value, int check_overflow, int signed_check,
                           int shift)
{
    uint32_t *ins = (uint32_t *)reloc_addr;
    uint32_t imm = (value >> shift) & 0xffff;
    if (check_overflow) {
        uint64_t max = 0xffff;
        int64_t smax = 0x7fff;
        int64_t smin = -0x8000;
        if (signed_check) {
            if ((int64_t)value < smin || (int64_t)value > smax) {
                fprintf(stderr, "MOVW overflow\n");
                return;
            }
        } else {
            if (value > max) {
                fprintf(stderr, "MOVW overflow\n");
                return;
            }
        }
    }
    *ins = (*ins & ~(0xffff << 5)) | (imm << 5);
}

// Helper for ADRP HI21 (bits 30:5 for imm21 = (value >> 12) & 0x1fffff, split imm_lo bits 30:29,
// imm_hi bits 23:5)
static void apply_adrp_hi21(void *reloc_addr, int64_t value, int check_overflow)
{
    uint32_t *ins = (uint32_t *)reloc_addr;
    int64_t imm = value >> 12;
    if (check_overflow && (imm < -0x100000 || imm > 0xfffff)) {
        fprintf(stderr, "ADRP overflow\n");
        return;
    }
    uint32_t imm_lo = (imm & 0x3) << 29;
    uint32_t imm_hi = ((imm >> 2) & 0x7ffff) << 5;
    *ins = (*ins & ~((0x3 << 29) | (0x7ffff << 5))) | imm_lo | imm_hi;
}

// Helper for ADR LO21 (bits 30:5 for imm21 = value & 0x1fffff, same split)
static void apply_adr_lo21(void *reloc_addr, int64_t value, int check_overflow)
{
    uint32_t *ins = (uint32_t *)reloc_addr;
    if (check_overflow && (value < -0x100000 || value > 0xfffff)) {
        fprintf(stderr, "ADR overflow\n");
        return;
    }
    uint32_t imm_lo = (value & 0x3) << 29;
    uint32_t imm_hi = ((value >> 2) & 0x7ffff) << 5;
    *ins = (*ins & ~((0x3 << 29) | (0x7ffff << 5))) | imm_lo | imm_hi;
}

// Helper for branch 26 bit (bits 25:0, value >> 2)
static void apply_branch26(void *reloc_addr, int64_t value, int check_overflow)
{
    uint32_t *ins = (uint32_t *)reloc_addr;
    value >>= 2;
    if (check_overflow && (value < -0x2000000 || value > 0x1ffffff)) {
        fprintf(stderr, "Branch overflow\n");
        return;
    }
    *ins = (*ins & ~0x3ffffff) | (value & 0x3ffffff);
}

// Helper for LD/ST LO12 (bits 21:10, value >> scale, scale depending on size)
static void apply_lo12(void *reloc_addr, uint64_t value, int scale, int check_overflow)
{
    uint32_t *ins = (uint32_t *)reloc_addr;
    uint32_t imm = (value >> scale) & 0xfff;
    if (check_overflow && imm > 0xfff) {
        fprintf(stderr, "LO12 overflow\n");
        return;
    }
    *ins = (*ins & ~(0xfff << 10)) | (imm << 10);
}

// Helper for LD_PREL_LO19 (bits 23:5, value >> 3)
static void apply_lo19(void *reloc_addr, int64_t value, int check_overflow)
{
    uint32_t *ins = (uint32_t *)reloc_addr;
    value >>= 2;
    if (check_overflow && (value < -0x40000 || value > 0x3ffff)) {
        fprintf(stderr, "LO19 overflow\n");
        return;
    }
    *ins = (*ins & ~(0x7ffff << 5)) | ((value & 0x7ffff) << 5);
}

// Routine to load .text, .data, .bss sections and apply relocations for ARM64
void *load_and_relocate_arm64(const char *filename, size_t *out_size)
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

    size_t *section_sizes = calloc(num_shdr, sizeof(size_t));
    if (!section_sizes) {
        perror("Failed to allocate section_sizes");
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

    // Second pass: Load all allocatable sections (.text, .data, .bss)
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
        section_sizes[ndx] = size;

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
            int64_t A = rela[i].r_addend;
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

            int64_t value = 0;
            uint64_t uvalue = 0;
            int is_signed = 0; // Flag for signed checks

            switch (type) {
            case R_AARCH64_NONE:
                continue;
            case R_AARCH64_ABS64:
                uvalue = (uint64_t)target_addr + A;
                *(uint64_t *)reloc_addr = uvalue;
                break;
            case R_AARCH64_ABS32:
                uvalue = (uint64_t)target_addr + A;
                if (uvalue > UINT32_MAX) {
                    fprintf(stderr, "R_AARCH64_ABS32 overflow\n");
                    continue;
                }
                *(uint32_t *)reloc_addr = (uint32_t)uvalue;
                break;
            case R_AARCH64_ABS16:
                uvalue = (uint64_t)target_addr + A;
                if (uvalue > UINT16_MAX) {
                    fprintf(stderr, "R_AARCH64_ABS16 overflow\n");
                    continue;
                }
                *(uint16_t *)reloc_addr = (uint16_t)uvalue;
                break;
            case R_AARCH64_PREL64:
                value = (int64_t)target_addr + A - P;
                *(uint64_t *)reloc_addr = (uint64_t)value;
                break;
            case R_AARCH64_PREL32:
                value = (int64_t)target_addr + A - P;
                if (value < INT32_MIN || value > INT32_MAX) {
                    fprintf(stderr, "R_AARCH64_PREL32 overflow\n");
                    continue;
                }
                *(uint32_t *)reloc_addr = (uint32_t)value;
                break;
            case R_AARCH64_PREL16:
                value = (int64_t)target_addr + A - P;
                if (value < INT16_MIN || value > INT16_MAX) {
                    fprintf(stderr, "R_AARCH64_PREL16 overflow\n");
                    continue;
                }
                *(uint16_t *)reloc_addr = (uint16_t)value;
                break;
            case R_AARCH64_MOVW_UABS_G0:
                uvalue = (uint64_t)target_addr + A;
                apply_movw_imm(reloc_addr, uvalue, 1, 0, 0);
                break;
            case R_AARCH64_MOVW_UABS_G0_NC:
                uvalue = (uint64_t)target_addr + A;
                apply_movw_imm(reloc_addr, uvalue, 0, 0, 0);
                break;
            case R_AARCH64_MOVW_UABS_G1:
                uvalue = (uint64_t)target_addr + A;
                apply_movw_imm(reloc_addr, uvalue, 1, 0, 16);
                break;
            case R_AARCH64_MOVW_UABS_G1_NC:
                uvalue = (uint64_t)target_addr + A;
                apply_movw_imm(reloc_addr, uvalue, 0, 0, 16);
                break;
            case R_AARCH64_MOVW_UABS_G2:
                uvalue = (uint64_t)target_addr + A;
                apply_movw_imm(reloc_addr, uvalue, 1, 0, 32);
                break;
            case R_AARCH64_MOVW_UABS_G2_NC:
                uvalue = (uint64_t)target_addr + A;
                apply_movw_imm(reloc_addr, uvalue, 0, 0, 32);
                break;
            case R_AARCH64_MOVW_UABS_G3:
                uvalue = (uint64_t)target_addr + A;
                apply_movw_imm(reloc_addr, uvalue, 0, 0, 48);
                break;
            case R_AARCH64_MOVW_SABS_G0:
                value = (int64_t)target_addr + A;
                apply_movw_imm(reloc_addr, value, 1, 1, 0);
                break;
            case R_AARCH64_MOVW_SABS_G1:
                value = (int64_t)target_addr + A;
                apply_movw_imm(reloc_addr, value, 1, 1, 16);
                break;
            case R_AARCH64_MOVW_SABS_G2:
                value = (int64_t)target_addr + A;
                apply_movw_imm(reloc_addr, value, 1, 1, 32);
                break;
            case R_AARCH64_MOVW_PREL_G0:
                value = (int64_t)target_addr + A - P;
                apply_movw_imm(reloc_addr, value, 1, 1, 0);
                break;
            case R_AARCH64_MOVW_PREL_G0_NC:
                value = (int64_t)target_addr + A - P;
                apply_movw_imm(reloc_addr, value, 0, 1, 0);
                break;
            case R_AARCH64_MOVW_PREL_G1:
                value = (int64_t)target_addr + A - P;
                apply_movw_imm(reloc_addr, value, 1, 1, 16);
                break;
            case R_AARCH64_MOVW_PREL_G1_NC:
                value = (int64_t)target_addr + A - P;
                apply_movw_imm(reloc_addr, value, 0, 1, 16);
                break;
            case R_AARCH64_MOVW_PREL_G2:
                value = (int64_t)target_addr + A - P;
                apply_movw_imm(reloc_addr, value, 1, 1, 32);
                break;
            case R_AARCH64_MOVW_PREL_G2_NC:
                value = (int64_t)target_addr + A - P;
                apply_movw_imm(reloc_addr, value, 0, 1, 32);
                break;
            case R_AARCH64_MOVW_PREL_G3:
                value = (int64_t)target_addr + A - P;
                apply_movw_imm(reloc_addr, value, 0, 1, 48);
                break;
            case R_AARCH64_ADR_PREL_PG_HI21:
                value = (((int64_t)target_addr + A) & ~0xfffLL) - (P & ~0xfffLL);
                apply_adrp_hi21(reloc_addr, value, 1);
                break;
            case R_AARCH64_ADR_PREL_PG_HI21_NC:
                value = (((int64_t)target_addr + A) & ~0xfffLL) - (P & ~0xfffLL);
                apply_adrp_hi21(reloc_addr, value, 0);
                break;
            case R_AARCH64_ADR_PREL_LO21:
                value = (int64_t)target_addr + A - P;
                apply_adr_lo21(reloc_addr, value, 1);
                break;
            case R_AARCH64_LD_PREL_LO19:
                value = (int64_t)target_addr + A - P;
                apply_lo19(reloc_addr, value, 1);
                break;
            case R_AARCH64_CALL26:
            case R_AARCH64_JUMP26:
                value = (int64_t)target_addr + A - P;
                apply_branch26(reloc_addr, value, 1);
                break;
            case R_AARCH64_ADD_ABS_LO12_NC:
                uvalue = (uint64_t)target_addr + A;
                apply_lo12(reloc_addr, uvalue, 0, 0);
                break;
            case R_AARCH64_LDST8_ABS_LO12_NC:
                uvalue = (uint64_t)target_addr + A;
                apply_lo12(reloc_addr, uvalue, 0, 0);
                break;
            case R_AARCH64_LDST16_ABS_LO12_NC:
                uvalue = (uint64_t)target_addr + A;
                apply_lo12(reloc_addr, uvalue, 1, 0);
                break;
            case R_AARCH64_LDST32_ABS_LO12_NC:
                uvalue = (uint64_t)target_addr + A;
                apply_lo12(reloc_addr, uvalue, 2, 0);
                break;
            case R_AARCH64_LDST64_ABS_LO12_NC:
                uvalue = (uint64_t)target_addr + A;
                apply_lo12(reloc_addr, uvalue, 3, 0);
                break;
            case R_AARCH64_LDST128_ABS_LO12_NC:
                uvalue = (uint64_t)target_addr + A;
                apply_lo12(reloc_addr, uvalue, 4, 0);
                break;
            // Add more cases for other types, similar to above.
            // For GOT, TLS, etc., add if needed, but for basic, this covers many.
            default:
                fprintf(stderr, "Unsupported relocation type: %lu\n", type);
                continue;
            }
        }
    }

    // Optional: Remove write protection for .text (make read/exec)
    mprotect(result.text_addr, result.text_size, PROT_READ | PROT_EXEC);

    // Cleanup ELF resources
    free(section_sizes);
    free(section_addrs); // Note: Addresses are still mapped; caller must unmap all if needed
    free(result.symtab);
    free(result.strtab);
    elf_end(elf);
    close(fd);

    *out_size = result.text_size;
    return result.text_addr;

cleanup:
    // Unmap all loaded sections
    if (section_addrs && section_sizes) {
        for (size_t i = 0; i < num_shdr; i++) {
            if (section_addrs[i]) {
                munmap(section_addrs[i], section_sizes[i]);
            }
        }
        free(section_sizes);
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
    void *text_addr = load_and_relocate_arm64("example.o", &text_size);
    if (!text_addr) {
        fprintf(stderr, "Failed to load and relocate .text section\n");
        return 1;
    }

    // Example: Call a function at the start of .text (unsafe without knowing entry point)
    typedef void (*FuncPtr)(void);
    FuncPtr func = (FuncPtr)text_addr;
    func();

    // Cleanup: Unmap .text (in production, unmap all loaded sections)
    munmap(text_addr, text_size);
    return 0;
}