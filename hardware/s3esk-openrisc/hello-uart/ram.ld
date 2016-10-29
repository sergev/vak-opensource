MEMORY
        {
        vectors : ORIGIN = 0x00000000, LENGTH = 0x00002000 
        ram     : ORIGIN = 0x00002000, LENGTH = 0x00200000 - 0x00002000
        }
 
SECTIONS
{
        .vectors :
        {
        *(.vectors)
        } > vectors

        .text :
        {
        *(.text)
        } > ram

        .data :
        {
        *(.data)
        } > ram

        .rodata :
        {
        *(.rodata)
        } > ram

        .bss :
        {
        *(.bss)
        } > ram

        .stack :
        {
        *(.stack)
        _src_addr = .;
        } > ram
}
