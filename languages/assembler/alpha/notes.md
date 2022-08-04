Registers
=========

The functions of the registers are defined by the calling-convention of the
operating system rather than the hardware specification.

The following is the OSF/1 (UNIX) convention :

|Register|Name |Description          |Preserved across procedure calls|
|--------|-----|---------------------|--------------------------------|
|       0|v0   |Function result      |No                              |
|       1|t0   |Temporary            |No                              |
|       2|t1   |"                    |No                              |
|       3|t2   |"                    |No                              |
|       4|t3   |"                    |No                              |
|       5|t4   |"                    |No                              |
|       6|t5   |"                    |No                              |
|       7|t6   |"                    |No                              |
|       8|t7   |"                    |No                              |
|       9|s0   |Saved                |Yes                             |
|      10|s1   |"                    |Yes                             |
|      11|s2   |"                    |Yes                             |
|      12|s3   |"                    |Yes                             |
|      13|s4   |"                    |Yes                             |
|      14|s5   |"                    |Yes                             |
|      15|fp/s6|Frame pointer / saved|? / Yes                         |
|      16|a0   |Function arguments   |No                              |
|      17|a1   |"                    |No                              |
|      18|a2   |"                    |No                              |
|      19|a3   |"                    |No                              |
|      20|a4   |"                    |No                              |
|      21|a5   |"                    |No                              |
|      22|t8   |Temporary            |No                              |
|      23|t9   |"                    |No                              |
|      24|t10  |"                    |No                              |
|      25|t11  |"                    |No                              |
|      26|ra   |Return address       |Yes                             |
|      27|pv/t1|Procedure value      |No                              |
|      28|at   |Assembler temporary  |No                              |
|      29|gp   |Global pointer       |No                              |
|      30|sp   |Stack pointer        |Yes                             |
|      31|zero |Always zero          |n/a                             |

Relocations
===========

* !literal
* !literal!N

Used with an ldq instruction to load the address of a symbol from the GOT.
A sequence number N is optional, and if present is used to pair lituse
relocations with this literal relocation. The lituse relocations are used by
the linker to optimize the code based on the final location of the symbol.

* !lituse_base!N

Used with any memory format instruction (e.g. ldl) to indicate that the literal
is used for an address load. The offset field of the instruction must be zero.
During relaxation, the code may be altered to use a gp-relative load.

* !lituse_jsr!N

Used with a register branch format instruction (e.g. jsr) to indicate that the
literal is used for a call. During relaxation, the code may be altered to use
a direct branch (e.g. bsr).

* !lituse_jsrdirect!N

Similar to lituse_jsr, but also that this call cannot be vectored through a PLT
entry. This is useful for functions with special calling conventions which do
not allow the normal call-clobbered registers to be clobbered.

* !lituse_bytoff!N

Used with a byte mask instruction (e.g. extbl) to indicate that only the low 3
bits of the address are relevant. During relaxation, the code may be altered
to use an immediate instead of a register shift.

* !lituse_addr!N

Used with any other instruction to indicate that the original address is in
fact used, and the original ldq instruction may not be altered or deleted.
This is useful in conjunction with lituse_jsr to test whether a weak symbol
is defined.
```Assembly
	ldq  $27,foo($29)   !literal!1
	beq  $27,is_undef   !lituse_addr!1
	jsr  $26,($27),foo  !lituse_jsr!1
```

* !lituse_tlsgd!N

Used with a register branch format instruction to indicate that the literal
is the call to __tls_get_addr used to compute the address of the thread-local
storage variable whose descriptor was loaded with !tlsgd!N.

* !lituse_tlsldm!N

Used with a register branch format instruction to indicate that the literal
is the call to __tls_get_addr used to compute the address of the base of the
thread-local storage block for the current module. The descriptor for the
module must have been loaded with !tlsldm!N.

* !gpdisp!N

Used with ldah and lda to load the GP from the current address, a-la the ldgp
macro. The source register for the ldah instruction must contain the address
of the ldah instruction. There must be exactly one lda instruction paired with
the ldah instruction, though it may appear anywhere in the instruction stream.
The immediate operands must be zero.
```Assembly
	bsr  $26,foo
	ldah $29,0($26)     !gpdisp!1
	lda  $29,0($29)     !gpdisp!1
```

* !gprelhigh

Used with an ldah instruction to add the high 16 bits of a 32-bit displacement
from the GP.

* !gprellow
Used with any memory format instruction to add the low 16 bits of a 32-bit
displacement from the GP.

* !gprel

Used with any memory format instruction to add a 16-bit displacement from
the GP.

* !samegp

Used with any branch format instruction to skip the GP load at the target
address. The referenced symbol must have the same GP as the source object
file, and it must be declared to either not use $27 or perform a standard GP
load in the first two instructions via the .prologue directive.

* !tlsgd
* !tlsgd!N

Used with an lda instruction to load the address of a TLS descriptor for a
symbol in the GOT.  The sequence number N is optional, and if present it used
to pair the descriptor load with both the literal loading the address of the
`__tls_get_addr` function and the `lituse_tlsgd` marking the call to that function.

* !tlsldm
* !tlsldm!N

Used with an lda instruction to load the address of a TLS descriptor for the
current module in the GOT.  Similar in other respects to tlsgd.

* !gotdtprel

Used with an ldq instruction to load the offset of the TLS symbol within its
module's thread-local storage block. Also known as the dynamic thread pointer
offset or dtp-relative offset.

* !dtprelhi
* !dtprello
* !dtprel

Like gprel relocations except they compute dtp-relative offsets.

* !gottprel

Used with an ldq instruction to load the offset of the TLS symbol from the
thread pointer. Also known as the tp-relative offset.

* !tprelhi
* !tprello
* !tprel

Like gprel relocations except they compute tp-relative offsets.


Virtual Memory
==============

On a TLB miss, a PALcode routine is called which loads the TLB entry with
the new virtual-to-physical mapping as defined by the page table structure
pointed to by the PTBR (page-table base register).

Page table structure is thus defined by PALcode implementation rather than the
hardware specification.

The page table layout for OSF/1 (used in Tru64 UNIX and UNIX-like operating
systems such as Linux), using 8k pages is :

```
666555555555554444444|4443333333|3332222222|2221111111|11
321098765432109876543|2109876543|2109876543|2109876543|2109876543210
---------------------+----------+----------+----------+-------------
uuuuuuuuuuuuuuuuuuuuu|aaaaaaaaaa|bbbbbbbbbb|cccccccccc|ooooooooooooo
```

u : unused
a : PTE index of root page table   (10 bits)
b : PTE index of middle page table (10 bits)
c : PTE index of leaf page table   (10 bits)
o : page offset                    (13 bits)

- which allows for a virtual address space of 1G pages == 8TiB

These sizes are defined in /usr/include/asm/osf.h as follows :

```
#define VA_V_SEG1       33
#define VA_M_SEG1       (0x3FF<<VA_V_SEG1)
#define VA_V_SEG2       23
#define VA_M_SEG2       (0x3FF<<VA_V_SEG2)
#define VA_V_SEG3       13
#define VA_M_SEG3       (0x3FF<<VA_V_SEG3)
#define VA_V_OFFSET     0
#define VA_M_OFFSET     0x1FFF

#define VA_S_SIZE       43
#define VA_S_OFF        13
#define VA_S_SEG        10
#define VA_S_PAGE_SIZE  8192
```

Instruction Aliases
===================

Alpha has a very orthogonal instruction set, which together with its flexible
register usage, results in the same opcode being used for multiple operations
which might have seperate opcodes on other architectures.

| Pseudo Instruction | Actual Instruction  |
| ------------------ | ------------------- |
| nop                | bis   $31,$31,$31   |
| fnop               | cpys  $31,$31,$f31  |
| clr                | bis   $31,$31,$x    |
| fclr               | cpys  $f31,$f31,$fx |
| unop               | ldq_u $31,0($x)     |
| sextl  $x,$y       | addl  $31,$x,$y     |
| mov    #lit8,$y    | bis   $31,lit8,$y   |
| mov    $x,$y       | bis   $x,$x,$y      |
| negl   $x,$y       | subl  $31,$x,$y     |
| negq   $x,$y       | subq  $31,$x,$y     |
| not    $x,$y       | ornot $31,$x,$y     |
| or     $x,$y,$z    | bis   $x,$y,$z      |
| andnot $x,$y,$z    | bic   $x,$y,$z      |
| fmov   $fx,$fy     | cpys  $fx,$fx,$fy   |
| fnegs  $fx,$fy     | cpysn $fx,$fx,$fy   |
| fnegt  $fx,$fy     | cpytn $fx,$fx,$fy   |


