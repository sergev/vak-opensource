/*
Various functions, for DEC Alpha EV4 architecture with GNU assembler.
*/

#include <asm/unistd.h>
#include <asm/pal.h>

.arch ev4
.set noreorder
.set nomacro
.set noat
.set volatile

/* code section, align to 4 bytes (size of instruction) */
.section .text
.balign 4

/* long rol(long x, long count); */
/* rotate x left by count bits, return result */
rol:
	lda   $1, 63($31)
	and   $1, $17, $2
	sll   $16, $2, $3
	lda   $4, 64($31)
	subq  $4, $2, $5
	sra   $16, $5, $6
	bis   $6, $3, $0
	ret


/* long ror(long x, long count); */
/* rotate x right by count bits, return result */
ror:
	lda   $1, 63($31)
	and   $1, $17, $2
	sra   $16, $2, $3
	lda   $4, 64($31)
	subq  $4, $2, $5
	sll   $16, $5, $6
	bis   $6, $3, $0
	ret


/* size_t strlen(const char *s); */
strlen:
	lda   $0, 0($31)          /* char count */
	lda   $1, 7($31)          /* quad mask */
strlen$quadloop:
	ldq_u $5, 0($16)          /* read quad */
strlen$byteloop:
	and   $16, $1, $3         /* byte offset within quad */
	extbl $5, $3, $6          /* get byte */
	beq   $6, strlen$done     /* byte 0 => done */
	addq  $0, 1, $0           /* increment count */
	addq  $16, 1, $16         /* increment address */
	cmpeq $3, 7, $7           /* if last offset */
	beq   $7, strlen$quadloop /* read new quad */
	br    strlen$byteloop     /* else loop over all bytes in quad */
strlen$done:
	ret


/* void puts(const char *s); */
/* write string to standard output - linux sysycalls */
puts_linux:
	lda   $sp,-16($sp) /* allocate space for ra copy */
	stq   $26,0($sp)   /* save ra */
	stq   $16,8($sp)   /* save a0 */
	bsr   $26,strlen

	lda   $18,0($0)
	lda   $0, __NR_write($31)          
	ldq   $17,8($sp)   /* load saved a0 */
	lda   $16,1($31)
	callsys

	ldq   $26,0($sp)   /* restore ra */
	lda   $sp,16($sp)  /* cleanup stack */
	ret


unaligned_read:
    ldq_u $2, ($16)   /* Load lower quadword */
    ldq_u $3, 3($16)  /* Load upper quadword */
    extll $2, $16, $2 /* Extract lower part */
    extlh $3, $16, $3 /* Extract upper part */
    bis   $2, $3, $0  /* Join lower and upper part */
    sextl $0, $0      /* Extend sign bit */


load_immediate_64:
/* gcc with -mbuild-constants results in this type of code: */
	ldah  $0,4660
	lda   $0,22137($0)
	sll   $0,0x20,$0
	ldah  $0,-25923($0)
	lda   $0,-8464($0)
	ret
