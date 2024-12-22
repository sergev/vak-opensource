#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <asm/prctl.h>        /* Definition of ARCH_* constants */

int main()
{
    printf("Start.\n");

    size_t old_fs = 0, old_gs = 0;
    if (syscall(SYS_arch_prctl, ARCH_GET_FS, &old_fs) < 0) {
        perror("ARCH_GET_FS");
    }
    if (syscall(SYS_arch_prctl, ARCH_GET_GS, &old_gs) < 0) {
        perror("ARCH_GET_GS");
    }
    printf("old_fs = 0x%lx\n", old_fs);
    printf("old_gs = 0x%lx\n", old_gs);

    size_t fs = 0, gs = 0;
    syscall(SYS_arch_prctl, ARCH_SET_FS, 0x1122334455);
    syscall(SYS_arch_prctl, ARCH_GET_FS, &fs);
    syscall(SYS_arch_prctl, ARCH_SET_FS, old_fs);
    printf("fs = 0x%lx\n", fs);

    syscall(SYS_arch_prctl, ARCH_SET_GS, 0x1122334455);
    syscall(SYS_arch_prctl, ARCH_GET_GS, &gs);
    syscall(SYS_arch_prctl, ARCH_SET_GS, old_gs);
    printf("gs = 0x%lx\n", gs);

    printf("Done.\n");
}
