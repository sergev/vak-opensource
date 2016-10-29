#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <elf.h>

#include "stream.h"

/*
 * Системная консоль, интерфейс Stream_t.
 * Вывод на stderr.
 */
class Console_t : public Stream_t {
	ADA_COMPATIBLE_IMPLEMENTATION;
public:
	virtual void Put_Char (int c);
	virtual void Put_String (const char *str, int strlim[2] = 0);
	virtual void Put_Integer (long val, int width, int base);
	virtual void Put_Unsigned (unsigned long val, int width, int base);
};

void Console_t::Put_Char (int c)
{
	char ch = c;

	write (2, &ch, 1);
}

void Console_t::Put_String (const char *str, int strlim[2])
{
	write (2, str, strlen (str));
}

/* Max number conversion buffer length: a long in base 2, plus NUL byte. */
#define MAXNBUF	(sizeof(long) * 8 + 2)

void Console_t::Put_Integer (long val, int width, int base)
{
	char nbuf [MAXNBUF], *p;
	int ch, neg = 0;

	if (val < 0) {
		neg = 1;
		val = -val;
	}

	/* Put a NUL-terminated ASCII number (base <= 36) in a buffer
	 * in reverse order; return an optional length and a pointer
	 * to the last character written in the buffer
	 * (i.e., the first character of the string). */
	p = nbuf;
	*p = 0;
	do {
		ch = val % base;
		val /= base;
		*++p = (ch > 9) ? (ch + 'a' - 10) : (ch + '0');
	} while (width > p - nbuf || val != 0);
	if (neg)
		*++p = '-';

	while (width-- > p - nbuf)
		Put_Char (' ');

	while (*p)
		Put_Char (*p--);
}

void Console_t::Put_Unsigned (unsigned long val, int width, int base)
{
	char nbuf [MAXNBUF], *p;
	int ch;

	/* Put a NUL-terminated ASCII number (base <= 36) in a buffer
	 * in reverse order; return an optional length and a pointer
	 * to the last character written in the buffer
	 * (i.e., the first character of the string). */
	p = nbuf;
	*p = 0;
	do {
		ch = val % base;
		val /= base;
		*++p = (ch > 9) ? (ch + 'a' - 10) : (ch + '0');
	} while (width > p - nbuf || val != 0);

	while (width-- > p - nbuf)
		Put_Char ('0');

	while (*p)
		Put_Char (*p--);
}

/*
 * Аварийный останов при вызове отсутствующего метода.
 */
extern "C" {
void __cxa_pure_virtual (void)
{
	const char msg[] = "pure virtual method called\n";

	write (2, msg, sizeof (msg) - 1);
	exit (-1);
}
}

void run (const char *filename, Stream_t *console)
{
	FILE *fd = fopen (filename, "r");
	if (! fd) {
		perror (filename);
		exit (-1);
	}

	/*
	 * Read file header.
	 */
	Elf32_Ehdr *eh = (Elf32_Ehdr*) alloca (sizeof (*eh));
	if (fread (eh, sizeof (*eh), 1, fd) != 1) {
		perror (filename);
		exit (-1);
	}
	if (eh->e_ident[0] != 0x7f || eh->e_ident[1] != 'E' ||
	    eh->e_ident[2] != 'L' || eh->e_ident[3] != 'F') {
		fprintf (stderr, "Invalid file format\n");
		exit (-1);
	}
	if (eh->e_ident[4] != ELFCLASS32) {
		fprintf (stderr, "Invalid ELF class, 32-bit expected\n");
		exit (-1);
	}
	if (eh->e_ident[5] != ELFDATA2LSB) {
		fprintf (stderr, "Invalid ELF data encoding, LSB expected\n");
		exit (-1);
	}
	if (eh->e_machine != EM_386) {
		fprintf (stderr, "Invalid ELF target machine, i386 expected\n");
		exit (-1);
	}
	if (eh->e_type != ET_EXEC) {
		fprintf (stderr, "Invalid ELF file type, EXEC expected\n");
		exit (-1);
	}

	/*
	 * Read program header.
	 */
	Elf32_Phdr *ph = (Elf32_Phdr*) alloca (eh->e_phnum * eh->e_phentsize);
	fseek (fd, eh->e_phoff, 0);
	if (fread (ph, eh->e_phnum * eh->e_phentsize, 1, fd) != 1) {
		perror (filename);
		exit (-1);
	}

	/*
	 * Find code and data segments.
	 */
	unsigned int i;
	Elf32_Phdr *code = 0, *data = 0;
	printf ("Type     Offset   Vaddr    Paddr    Filesz   Memsz    Flags    Align\n");
	for (i=0; i<eh->e_phnum; ++i) {
		printf ("%08x ", ph[i].p_type);
                printf ("%08x ", ph[i].p_offset);
                printf ("%08x ", ph[i].p_vaddr);
                printf ("%08x ", ph[i].p_paddr);
                printf ("%08x ", ph[i].p_filesz);
                printf ("%08x ", ph[i].p_memsz);
                printf ("%08x ", ph[i].p_flags);
                printf ("%08x\n", ph[i].p_align);
		if (ph[i].p_type != PT_LOAD)
			continue;
		if (! (ph[i].p_flags & PF_R))
			continue;
		if (ph[i].p_flags & PF_X) {
			if (code) {
				fprintf (stderr, "Too many code segments\n");
				exit (-1);
			}
			code = &ph[i];
		}
		if (ph[i].p_flags & PF_W) {
			if (data) {
				fprintf (stderr, "Too many data segments\n");
				exit (-1);
			}
			data = &ph[i];
		}
	}
	printf ("code %08x + %08x\n", code->p_vaddr, code->p_memsz);
	printf ("data %08x + %08x\n", data->p_vaddr, data->p_memsz);

	/*
	 * Read section header.
	 */
	Elf32_Shdr *sh = (Elf32_Shdr*) alloca (eh->e_shnum * eh->e_shentsize);
	fseek (fd, eh->e_shoff, 0);
	if (fread (sh, eh->e_shnum * eh->e_shentsize, 1, fd) != 1) {
		perror (filename);
		exit (-1);
	}

	/*
	 * Map code+data as read/executable region.
	 */
	unsigned int length = (data->p_vaddr + data->p_memsz +
		data->p_align - 1) / data->p_align * data->p_align -
		code->p_vaddr;
	unsigned int addr = (unsigned int) mmap (0, length,
		PROT_READ | PROT_EXEC, MAP_PRIVATE,
		fileno (fd), code->p_offset);
	if (addr == (unsigned) -1) {
		fprintf (stderr, "Failed to map code segment\n");
		perror (filename);
		exit (-1);
	}

	/*
	 * Remap data as read/write region.
	 */
	unsigned int addr2 = (addr + ((char*) data->p_vaddr -
		(char*) code->p_vaddr)) / data->p_align * data->p_align;
	unsigned int length2 = addr + length - addr2;
	printf ("addr %08x + %08x, %08x + %08x\n",
		addr, length, addr2, length2);
	if (munmap ((void*) addr2, length2) < 0) {
		fprintf (stderr, "Failed to unmap data segment\n");
		exit (-1);
	}
	int status = (int) mmap ((void*) addr2, length2,
		PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_FIXED, fileno (fd),
		data->p_offset / data->p_align * data->p_align);
	if (status == -1) {
		fprintf (stderr, "Failed to map data segment\n");
		exit (-1);
	}

	/*
	 * Relocate global offset table section.
	 */
	for (i=0; i<eh->e_shnum; ++i) {
		if (sh[i].sh_type == SHT_PROGBITS &&
		    sh[i].sh_flags == (SHF_WRITE | SHF_ALLOC) &&
		    sh[i].sh_entsize != 0) {
			unsigned int *p = (unsigned int*) (sh[i].sh_addr +
				addr - code->p_vaddr);
			printf ("Relocate section %08x + %08x to %08x\n",
				sh[i].sh_addr, sh[i].sh_size, (int) p);

			/* Skip first 3 entries. */
			unsigned int offset = 12;
			p += 3;
			while (offset < sh[i].sh_size) {
				*p++ += addr - code->p_vaddr;
				offset += sh[i].sh_entsize;
			}
		}
	}

	/*
	 * Clear bss segment.
	 */
	if (data->p_memsz > data->p_filesz)
		memset ((char*) addr + ((char*) data->p_vaddr -
			(char*) code->p_vaddr) + data->p_filesz,
			0, data->p_memsz - data->p_filesz);

	/*
	 * Call loaded code.
	 */
	void (*start) (Stream_t*) = (void (*) (Stream_t*))
		(addr + eh->e_entry - code->p_vaddr);
	printf ("Calling %08x\n--------\n", (int) start);
	start (console);
	printf ("--------\n");

	/*
	 * Unload.
	 */
	munmap ((void*) addr2, length2);
	munmap ((void*) addr, length);
	fclose (fd);
}

int main (int argc, char **argv)
{
	Console_t console_data, *console = &console_data;

	if (argc != 2) {
		fprintf (stderr, "Usage: shell filename.bx\n");
		return -1;
	}
	console->Put_String ("Shell started.\n");
	run (argv[1], console);
	console->Put_String ("Shell finisned.\n");
	return 0;
}
