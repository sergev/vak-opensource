Example of simple program for chipKIT Max32 board.  
Using clang compiler and pic32prog programming utility.


# Build and run

See the LLVM installation instructions in the top of `Makefile`.

You'll also need QEMU for PIC32: https://github.com/sergev/qemu/issues/17

Ensure you have an alias to QEMU for `qemu.sh` to work. ie: `qemu-pic32` needs to exist as an alias pointing to `qemu/mipsel-softmmu/qemu-system-mipsel`. 

Then: 
```bash
./build_and_run.sh
```
