Example of simple program for MikroElektronika MINI-32 board.
Using GCC compiler and pic32prog programming utility.

The specific feature of the board is the UHB bootloader.
User application starts from address 9d00bff0.
48k of flash memory is available for user program (9d000000-9d0c000).

Additional 8k of boot area can be used as well (9fc00000-9fc2000).
In this case, user program should first jump from 9fc00000
to the bootloader entry (9d0c000), and then continue from 9d00bff0.
