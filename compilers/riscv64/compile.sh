#!/bin/sh
set -x
riscv64-elf-gcc -O3 -g -march=rv64imafdc -Wall -fpie -ffunction-sections -fdata-sections -S -nostartfiles hello-world-libc.c -o hello-world-libc.s
riscv64-elf-gcc -O3 -g -march=rv64imafdc -Wall -fpie -ffunction-sections -fdata-sections -c hello-world-libc.c -o hello-world-libc.o
riscv64-elf-gcc -O3 -g -march=rv64imafdc -Wall -fpie -ffunction-sections -fdata-sections hello-world-libc.o -o hello-world-libc
riscv64-elf-strip hello-world-libc -o hello-world-libc-strip
