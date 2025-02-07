#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>

void spawn_with_in_out(char *argv[], int *fd_in, int *fd_out)
{
    // Create pipes
    int pipe_in[2], pipe_out[2];
    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1) {
        err(EXIT_FAILURE, "pipe");
    }

    pid_t pid = fork();
    if (pid < 0) {
        err(EXIT_FAILURE, "fork");
    }

    if (pid == 0) {
        //
        // Child process
        //

        // Close unused ends of pipes
        close(pipe_in[1]);
        close(pipe_out[0]);

        // Redirect stdin from the read end of the input pipe
        dup2(pipe_in[0], STDIN_FILENO);
        close(pipe_in[0]);

        // Redirect stdout to the write end of the output pipe
        dup2(pipe_out[1], STDOUT_FILENO);
        close(pipe_out[1]);

        // Execute the wc command
        execvp(argv[0], argv);
        err(EXIT_FAILURE, "%s", argv[0]);
    }

    //
    // Parent process
    //

    // Close unused ends of pipes
    close(pipe_in[0]);
    close(pipe_out[1]);

    // Return pipe descriptors.
    *fd_out = pipe_in[1];
    *fd_in = pipe_out[0];
}

void print_binary_data(const char *str, unsigned nbytes)
{
    putchar('"');
    for (; nbytes-- > 0; str++) {
        if (*str >= ' ' && *str <= '~') {
            putchar(*str);
        } else {
            printf("\\x%02X", (unsigned char)*str);
        }
    }
    putchar('"');
}

void cross_connect(int receive_in, int receive_out, int transmit_in, int transmit_out)
{
    fd_set readfds;
    struct timeval timeout;
    int retval;

    printf("sz              rz\n");
    printf("------------------------\n");
    while (receive_in >= 0 || transmit_in >= 0) {
        // Initialize the file descriptor set
        FD_ZERO(&readfds);
        if (receive_in >= 0) {
            FD_SET(receive_in, &readfds);
        }
        if (transmit_in >= 0) {
            FD_SET(transmit_in, &readfds);
        }

        // Set the timeout one second.
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        // Select for readability on both file descriptors
        retval = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);

        if (retval == -1) {
            err(EXIT_FAILURE, "select");
        } else if (retval == 0) {
            printf("--- timeout occurred\n");
        } else {
            // Check if data is available on receive_in
            if (receive_in >= 0 && FD_ISSET(receive_in, &readfds)) {
                char buffer[1024];
                ssize_t bytes_read = read(receive_in, buffer, sizeof(buffer));
                if (bytes_read <= 0) {
                    // Handle the closure of receive_in
                    printf("--- rz finished\n");
                    close(receive_in);
                    receive_in = -1;
                    continue;
                }

                // Send data to transmit_out.
                printf("                ");
                print_binary_data(buffer, bytes_read);
                printf(" [%zd bytes]\n", bytes_read);
                ssize_t bytes_written = write(transmit_out, buffer, bytes_read);
                if (bytes_written != bytes_read) {
                    printf("Bad write from rz to sz: %zd bytes instead of %zd\n", bytes_written, bytes_read);
                    exit(EXIT_FAILURE);
                }
            }

            // Check if data is available on transmit_in
            if (transmit_in >= 0 && FD_ISSET(transmit_in, &readfds)) {
                char buffer[1024];
                ssize_t bytes_read = read(transmit_in, buffer, sizeof(buffer));
                if (bytes_read <= 0) {
                    printf("--- sz finished\n");
                    // Handle the closure of transmit_in
                    close(transmit_in);
                    transmit_in = -1;
                    continue;
                }

                // Send data to receive_out.
                print_binary_data(buffer, bytes_read);
                printf(" [%zd bytes]\n", bytes_read);
                ssize_t bytes_written = write(receive_out, buffer, bytes_read);
                if (bytes_written != bytes_read) {
                    printf("Bad write from sz to rz: %zd bytes instead of %zd\n", bytes_written, bytes_read);
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
}

//
// Run sz to send a file and rz to receive it.
//
int main()
{
    // Start sz.
    char *transmit_argv[] = { "sz", "--zmodem", "--verbose", "abc.txt", NULL };
    int transmit_in, transmit_out;
    if (chdir("transmit") < 0) {
        err(EXIT_FAILURE, "transmit");
    }
    spawn_with_in_out(transmit_argv, &transmit_in, &transmit_out);

    // Start rz.
    char *receive_argv[] = { "rz", "--zmodem", "--verbose", "--overwrite", NULL };
    int receive_in, receive_out;
    if (chdir("../receive") < 0) {
        err(EXIT_FAILURE, "receive");
    }
    spawn_with_in_out(receive_argv, &receive_in, &receive_out);

    cross_connect(receive_in, receive_out, transmit_in, transmit_out);

    wait(NULL);
}
