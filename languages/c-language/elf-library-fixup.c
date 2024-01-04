/*
 * https://gitlab.com/mjg59/elf-library-fixup.git
 * Copyright 2024 Matthew Garrett <mjg59@srcf.ucam.org>
 *
 * Extract dynamic symbol data from an ELF DSO's DYNAMIC section and construct
 * the relevant .dynamic, .dynsym, and .dynstr sections if they are missing.
 *
 * Portions extracted from GNU binutils - as such:
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 */
#include <stdio.h>
#include <elf.h>
#include <fcntl.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
	int fd, i, j, dyncnt, strtab, symtab, dynstrsize, dynsymsize, numsyms;
	ssize_t len;
	off_t shdr_start, strtab_len;
	struct stat stat;

	Elf64_Ehdr ehdr;
	Elf64_Phdr *phdrs;
	Elf64_Dyn *dhdrs;
	Elf64_Shdr string_shdr;
	Elf64_Shdr dynamic_shdr;
	Elf64_Shdr dynsym_shdr;
	Elf64_Shdr dynstr_shdr;

	if (argc < 3) {
		printf("Usage: %s filename symbolcount\n", argv[0]);
		return -1;
	}

	numsyms = atoi(argv[2]);

	if (numsyms == 0) {
		printf("Invalid number of symbols\n");
		return -1;
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		perror("Failed to open file");
		return -1;
	}

	if (fstat(fd, &stat) < 0) {
		perror("Failed to stat file");
		return -1;
	}

	len = read(fd, &ehdr, sizeof(ehdr));
	if (len < sizeof(ehdr)) {
		perror("Failed to read ELF header");
		return -1;
	}

	if (ehdr.e_ident[EI_CLASS] != ELFCLASS64) {
		printf("Not a 64-bit ELF file\n");
		return -1;
	}

	if (ehdr.e_shoff != 0 || ehdr.e_shnum != 0) {
		printf("Already has a section header\n");
		return -1;
	}

	if (lseek(fd, ehdr.e_phoff, SEEK_SET) < 0) {
		perror("Failed to seek");
		return -1;
	}

	phdrs = malloc(ehdr.e_phentsize * ehdr.e_phnum);
	if (!phdrs) {
		perror("Failed to allocate program headers");
		return -1;
	}

	len = read(fd, phdrs, ehdr.e_phentsize * ehdr.e_phnum);
	if (len < (ehdr.e_phentsize * ehdr.e_phnum)) {
		perror("Failed to read program header");
		return -1;
	}

	for (i = 0; i < ehdr.e_phnum; i++) {
		if (phdrs[i].p_type == PT_DYNAMIC)
			break;
	}

	if (i == ehdr.e_phnum) {
		printf("Failed to locate DYNAMIC section\n");
		return -1;
	}


	if (lseek(fd, phdrs[i].p_offset, SEEK_SET) < 0) {
		perror("Failed to seek to DYNAMIC section:");
		return -1;
	}

	dhdrs = malloc(phdrs[i].p_filesz);
	if (dhdrs == NULL) {
		printf("Failed to allocate storage for DYNAMIC\n");
		return -1;
	}

	if (read(fd, dhdrs, phdrs[i].p_filesz) < phdrs[i].p_filesz) {
		perror("Failed to read DYNAMIC section");
		return -1;
	}

	dyncnt = phdrs[i].p_filesz / sizeof(Elf64_Dyn);

	for (j = 0; j < dyncnt; j++) {
		switch (dhdrs[j].d_tag) {
		case DT_SYMTAB:
			symtab = j;
			break;
		case DT_STRTAB:
			strtab = j;
			break;
		case DT_SYMENT:
			dynsymsize = dhdrs[j].d_un.d_val;
			break;
		case DT_STRSZ:
			dynstrsize = dhdrs[j].d_un.d_val;
			break;
		}
	}

	if (symtab == 0 && strtab == 0) {
		printf("Failed to locate dynamic symtab\n");
		return -1;
	}

	if (lseek(fd, 0, SEEK_END) == -1) {
		perror("Failed to seek to end of file");
		return -1;
	}

	write(fd, ".dynamic", strlen(".dynamic") + 1);
	write(fd, ".shstrtab", strlen(".shstrtab") + 1);
	write(fd, ".dynsym", strlen(".dynsym") + 1);
	write(fd, ".dynstr", strlen(".dynstr") + 1);

	shdr_start = lseek(fd, 0, SEEK_CUR);
	if (shdr_start % 16 != 0) {
		shdr_start = lseek(fd, 16 - (shdr_start % 16), SEEK_CUR);
	}

	strtab_len = shdr_start - stat.st_size;

	memset(&dynamic_shdr, 0, sizeof(dynamic_shdr));
	dynamic_shdr.sh_name = 0;
	dynamic_shdr.sh_link = 2;
	dynamic_shdr.sh_type = SHT_NOTE;
	dynamic_shdr.sh_offset = phdrs[i].p_offset;
	dynamic_shdr.sh_addr = phdrs[i].p_vaddr;
	dynamic_shdr.sh_size = phdrs[i].p_filesz;
	dynamic_shdr.sh_addralign = phdrs[i].p_align;
	write(fd, &dynamic_shdr, sizeof(dynamic_shdr));

	memset(&dynsym_shdr, 0, sizeof(dynsym_shdr));
	dynsym_shdr.sh_name = strlen(".dynamic") + strlen(".shstrtab") + 2;
	dynsym_shdr.sh_link = 2;
	dynsym_shdr.sh_type = SHT_DYNSYM;
	dynsym_shdr.sh_offset = dhdrs[symtab].d_un.d_ptr;
	dynsym_shdr.sh_addr = dhdrs[symtab].d_un.d_ptr;
	dynsym_shdr.sh_entsize = sizeof(Elf64_Sym);
	dynsym_shdr.sh_size = numsyms * sizeof(Elf64_Sym);
	dynsym_shdr.sh_info = (dynsymsize / sizeof(Elf64_Sym)) + 1;
	write(fd, &dynsym_shdr, sizeof(dynsym_shdr));

	memset(&dynstr_shdr, 0, sizeof(dynsym_shdr));
	dynstr_shdr.sh_name = strlen(".dynamic") + strlen(".shstrtab") + strlen(".dynsym") + 3;
	dynstr_shdr.sh_type = SHT_STRTAB;
	dynstr_shdr.sh_offset = dhdrs[strtab].d_un.d_ptr;
	dynstr_shdr.sh_addr = dhdrs[strtab].d_un.d_ptr;
	dynstr_shdr.sh_size = dynstrsize;
	write(fd, &dynstr_shdr, sizeof(dynstr_shdr));

	memset(&string_shdr, 0, sizeof(string_shdr));
	string_shdr.sh_name = strlen(".dynamic") + 1;
	string_shdr.sh_type = SHT_STRTAB;
	string_shdr.sh_offset = stat.st_size;
	string_shdr.sh_size = strtab_len;
	string_shdr.sh_addralign = 1;
	write(fd, &string_shdr, sizeof(string_shdr));

	ehdr.e_shnum = 4;
	ehdr.e_shoff = shdr_start;
	ehdr.e_shentsize = sizeof(Elf64_Shdr);
	ehdr.e_shstrndx = 3;

	lseek(fd, 0, SEEK_SET);
	write(fd, &ehdr, sizeof(ehdr));
	close(fd);

	return 0;
}
