#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/syscall.h>

void print_regs(int child)
{
    if (ptrace(PT_GETREGS, child, (caddr_t)1, 0) < 0) {
        perror("PT_STEP");
        exit(-1);
    }
    //TODO: PT_GETFPREGS
}

int main()
{
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
        if (ptrace(PT_TRACE_ME, 0, NULL, 0) < 0) {
            perror("PT_TRACE_ME");
            exit(-1);
        }
        char *const argv[] = { "./hello-amd64-macos", NULL };
        execv(argv[0], argv);

        // Failed to execute.
        perror("execv");
        exit(-1);
    }

    //
    // Parent.
    //
    size_t instr_count = 0;
    for (;;) {
        int status;
        if (wait(&status) < 0) {
            perror("wait");
            exit(-1);
        }
        //printf("status = %#x\n", status);
        printf(" (%zu:%#x)", instr_count, status); fflush(stdout);

        // WIFEXITED(status) - True if the process terminated normally
        // by a call to _exit(2).
        if (WIFEXITED(status)) {
            // WEXITSTATUS(status) - If WIFEXITED(status) is true,
            // evaluates to the low-order 8 bits of the argument passed
            // to _exit(2) by the child.
            printf("Child exited with status %#x\n", WEXITSTATUS(status));
            break;
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
            break;
        }

        // WIFSTOPPED(status) - True if the process has not terminated,
        // but has stopped and can be restarted. This macro can be true
        // only if the wait call specified the WUNTRACED option
        // or if the child process is being traced.
        if (!WIFSTOPPED(status)) {
            printf("Child not stopped?\n");
            break;
        }

        // WSTOPSIG(status) - If WIFSTOPPED(status) is true, evaluates
        // to the number of the signal that caused the process to stop.
        // Must be SIGTRAP for ptrace.
        if (WSTOPSIG(status) != SIGTRAP) {
            printf("Child stopped by signal %d\n", WSTOPSIG(status));
            break;
        }

        print_regs(child);

        // Execute next CPU instruction.
        instr_count += 1;
        if (ptrace(PT_STEP, child, (caddr_t)1, 0) < 0) {
            perror("PT_STEP");
            exit(-1);
        }
    }
}
