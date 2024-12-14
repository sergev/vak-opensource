#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <sys/syscall.h>
//#include <sys/personality.h> TODO
#include <capstone/capstone.h>

// Handle for disassembler.
csh disasm;

//
// Print current CPU instruction.
//
void print_cpu_instruction(int child, unsigned long long address)
{
    // Read opcode from child process.
    // Max instruction size for x86-64 architecture is 16 bytes.
    uint64_t code[2];
    code[0] = ptrace(PTRACE_PEEKTEXT, child, (void*)address, NULL);
    code[1] = ptrace(PTRACE_PEEKTEXT, child, (void*)(address + 8), NULL);

    // Disassemble one instruction.
    cs_insn *insn = NULL;
    size_t count = cs_disasm(disasm, (uint8_t*)code, sizeof(code), address, 1, &insn);
    printf("0x%016llx: ", address);
    if (count == 0) {
        printf("(unknown)\n");
    } else {
        unsigned n;
        for (n = 0; n < insn[0].size; n++) {
            printf(" %02x", insn[0].bytes[n]);
        }
        while (n++ < 7) {
            printf("   ");
        }
        printf("  %s %s\n", insn[0].mnemonic, insn[0].op_str);
        cs_free(insn, count);
    }
    fflush(stdout);
}

//
// Get CPU state.
// Print program counter, disassembled instruction and changed registers.
//
void print_cpu_state(int child)
{
    struct user_regs_struct regs;
    struct user_fpregs_struct fpregs;

    if (ptrace(PTRACE_GETREGS, child, NULL, &regs) < 0) {
        perror("PTRACE_GETREGS");
        exit(-1);
    }
    print_cpu_instruction(child, regs.rip);

    if (ptrace(PTRACE_GETFPREGS, child, NULL, &fpregs) < 0) {
        perror("PTRACE_GETFPREGS");
        exit(-1);
    }
}

//
// Wait for child process to stop on next instruction.
// Return true when the child process is still running.
// Return false when in terminated for some reason.
//
bool child_alive()
{
    int status;
    if (wait(&status) < 0) {
        perror("wait");
        exit(-1);
    }
    //printf("%zu: status %#x\n", instr_count, status);

    // WIFEXITED(status) - True if the process terminated normally
    // by a call to _exit(2).
    if (WIFEXITED(status)) {
        // WEXITSTATUS(status) - If WIFEXITED(status) is true,
        // evaluates to the low-order 8 bits of the argument passed
        // to _exit(2) by the child.
        if (WEXITSTATUS(status) == 0) {
            printf("Process exited normally.\n");
        } else {
            printf("Process exited with status %d\n", WEXITSTATUS(status));
        }
        return false;
    }

    // WIFSIGNALED(status) - True if the process terminated
    // due to receipt of a signal.
    if (WIFSIGNALED(status)) {
        // WTERMSIG(status) - If WIFSIGNALED(status) is true,
        // evaluates to the number of the signal that caused
        // the termination of the process.
        printf("Child killed by signal %d\n", WTERMSIG(status));

        // WCOREDUMP(status) - If WIFSIGNALED(status) is true,
        // evaluates as true if the termination of the process
        // was accompanied by the creation of a core file containing
        // an image of the process when the signal was received.
        if (WCOREDUMP(status)) {
            printf("Core dumped.\n");
        }
        return false;
    }

    // WIFSTOPPED(status) - True if the process has not terminated,
    // but has stopped and can be restarted. This macro can be true
    // only if the wait call specified the WUNTRACED option
    // or if the child process is being traced.
    if (!WIFSTOPPED(status)) {
        printf("Child not stopped?\n");
        return false;
    }

    // WSTOPSIG(status) - If WIFSTOPPED(status) is true, evaluates
    // to the number of the signal that caused the process to stop.
    // Must be SIGTRAP for ptrace.
    if (WSTOPSIG(status) != SIGTRAP) {
        printf("Child stopped by signal %d\n", WSTOPSIG(status));
        return false;
    }
    return true;
}

int main()
{
    // Initialize disassembler.
    if (cs_open(CS_ARCH_X86, CS_MODE_64, &disasm) != CS_ERR_OK)
        return -1;

    // Create child.
    pid_t child = fork();
    if (child < 0) {
        // Cannot fork
        perror("fork");
        exit(-1);
    }

    if (child == 0) {
        //
        // Child: start target program.
        //
        errno = 0;
        if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) < 0) {
            perror("PTRACE_TRACEME");
            exit(-1);
        }
        //personality(ADDR_NO_RANDOMIZE);
        char *const argv[] = { "./hello-amd64-linux", NULL };
        printf("Starting program: %s\n", argv[0]);
        execv(argv[0], argv);

        // Failed to execute.
        perror(argv[0]);
        exit(-1);
    }

    //
    // Parent.
    //
    size_t instr_count = 0;
    while (child_alive()) {

        print_cpu_state(child);
        instr_count += 1;

        // Execute next CPU instruction.
        if (ptrace(PTRACE_SINGLESTEP, child, NULL, NULL) < 0) {
            perror("PTRACE_SINGLESTEP");
            exit(-1);
        }
    }
    cs_close(&disasm);
}
