#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/syscall.h>

int main()
{
    // Create child.
    pid_t child = fork();

    if (child == 0) {
        errno = 0;
        if (ptrace(PT_TRACE_ME, 0, NULL, 0) < 0) {
            perror("PT_TRACE_ME");
            exit(-1);
        }
        char *const argv[] = { "./hello-amd64-macos", NULL };
        execv(argv[0], argv);
        perror("execv");
    } else {
        int status;
        long long ins_count = 0;
        while (1) {
            // stop tracing if child terminated successfully
            wait(&status);
            if (WIFEXITED(status))
                break;
            //printf("status = %#x\n", status);
            printf(" %#x", status); fflush(stdout);

            ins_count++;
            ptrace(PT_STEP, child, (caddr_t)1, 0);
        }

        printf("\n%lld Instructions executed.\n", ins_count);
    }

    return 0;
}
