#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#include <iostream>

#ifdef __linux__
#include <pty.h>
#else // Assume macOS or other POSIX systems
#include <util.h>
#endif

pid_t child_pid = 0;  // Global to handle signals
int master_fd   = -1; // Global for SIGWINCH handler

// Signal handler to forward signals to child
void forward_signal(int sig)
{
    if (child_pid > 0) {
        kill(child_pid, sig);
    }
}

// Signal handler for SIGWINCH to update slave window size
void handle_sigwinch(int sig)
{
    if (master_fd == -1) {
        std::cerr << "SIGWINCH: master_fd invalid" << std::endl;
        return;
    }

    struct winsize ws;
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) == -1) {
        std::cerr << "SIGWINCH: Error getting window size: " << strerror(errno) << std::endl;
        return;
    }

    if (ioctl(master_fd, TIOCSWINSZ, &ws) == -1) {
        std::cerr << "SIGWINCH: Error setting slave window size: " << strerror(errno) << std::endl;
        return;
    }

    std::cerr << "SIGWINCH: Updated slave size to " << ws.ws_col << "x" << ws.ws_row << std::endl;

    // Notify child process of window size change
    if (child_pid > 0) {
        kill(child_pid, SIGWINCH);
    }
}

void set_raw_terminal()
{
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO | ISIG); // Disable canonical mode, echo, and signal handling
    term.c_cc[VMIN]  = 1;
    term.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void restore_terminal()
{
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= ICANON | ECHO | ISIG; // Restore canonical mode, echo, and signal handling
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

int main()
{
    char *slave_name;
    struct termios orig_termios, slave_termios;
    struct winsize ws;

    // Save original terminal settings
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
        std::cerr << "Error getting terminal attributes: " << strerror(errno) << std::endl;
        return 1;
    }

    // Get parent terminal window size
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) == -1) {
        std::cerr << "Error getting initial window size: " << strerror(errno) << std::endl;
        return 1;
    }

    // Open pseudo-terminal
    master_fd = posix_openpt(O_RDWR | O_NOCTTY);
    if (master_fd == -1) {
        std::cerr << "Error opening pseudo-terminal: " << strerror(errno) << std::endl;
        return 1;
    }

    // Grant access to slave (safe to call on both Linux and macOS)
    if (grantpt(master_fd) == -1) {
        std::cerr << "Warning: grantpt failed: " << strerror(errno) << std::endl;
        // Continue, as macOS may not require this
    }

    // Unlock pseudo-terminal (safe to call on both Linux and macOS)
    if (unlockpt(master_fd) == -1) {
        std::cerr << "Warning: unlockpt failed: " << strerror(errno) << std::endl;
        // Continue, as macOS may not require this
    }

    // Get slave name
    slave_name = ptsname(master_fd);
    if (!slave_name) {
        std::cerr << "Error getting slave name: " << strerror(errno) << std::endl;
        close(master_fd);
        return 1;
    }

    // Set up terminal attributes for slave
    tcgetattr(STDIN_FILENO, &slave_termios);
    slave_termios.c_lflag |= ISIG;          // Enable signal handling
    slave_termios.c_iflag |= ICRNL;         // Map CR to NL on input
    slave_termios.c_oflag |= OPOST | ONLCR; // Enable output processing, map NL to CR-NL

    // Fork to create child process
    child_pid = fork();
    if (child_pid == -1) {
        std::cerr << "Error forking: " << strerror(errno) << std::endl;
        close(master_fd);
        return 1;
    }

    if (child_pid == 0) { // Child process
        // Close master in child
        close(master_fd);

        // Create new session
        if (setsid() == -1) {
            std::cerr << "Error setting session: " << strerror(errno) << std::endl;
            _exit(1);
        }

        // Open slave
        int slave_fd = open(slave_name, O_RDWR);
        if (slave_fd == -1) {
            std::cerr << "Error opening slave: " << strerror(errno) << std::endl;
            _exit(1);
        }

        // Set slave as controlling terminal
        if (ioctl(slave_fd, TIOCSCTTY, 0) == -1) {
            std::cerr << "Error setting controlling terminal: " << strerror(errno) << std::endl;
            _exit(1);
        }

        // Set slave window size
        if (ioctl(slave_fd, TIOCSWINSZ, &ws) == -1) {
            std::cerr << "Error setting initial slave window size: " << strerror(errno)
                      << std::endl;
            _exit(1);
        }

        // Set terminal attributes for slave
        if (tcsetattr(slave_fd, TCSANOW, &slave_termios) == -1) {
            std::cerr << "Error setting slave terminal attributes: " << strerror(errno)
                      << std::endl;
            _exit(1);
        }

        // Redirect stdin, stdout, stderr to slave
        dup2(slave_fd, STDIN_FILENO);
        dup2(slave_fd, STDOUT_FILENO);
        dup2(slave_fd, STDERR_FILENO);
        if (slave_fd > 2)
            close(slave_fd);

        // Execute /bin/sh
        execl("/bin/sh", "sh", nullptr);
        std::cerr << "Error executing shell: " << strerror(errno) << std::endl;
        _exit(1);
    } else { // Parent process
        // Set up signal handlers using sigaction for reliability
        struct sigaction sa;
        sa.sa_handler = forward_signal;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sigaction(SIGINT, &sa, nullptr);
        sigaction(SIGTERM, &sa, nullptr);
        sigaction(SIGQUIT, &sa, nullptr);

        sa.sa_handler = handle_sigwinch;
        sigaction(SIGWINCH, &sa, nullptr);

        set_raw_terminal(); // Set terminal to raw mode

        fd_set read_fds;
        char buffer[1024];
        int status;

        while (1) {
            FD_ZERO(&read_fds);
            FD_SET(STDIN_FILENO, &read_fds);
            FD_SET(master_fd, &read_fds);

            int max_fd = std::max(STDIN_FILENO, master_fd) + 1;
            if (select(max_fd, &read_fds, nullptr, nullptr, nullptr) == -1) {
                if (errno == EINTR)
                    continue;
                std::cerr << "Error in select: " << strerror(errno) << std::endl;
                break;
            }

            // Read from master (shell output)
            if (FD_ISSET(master_fd, &read_fds)) {
                ssize_t bytes = read(master_fd, buffer, sizeof(buffer));
                if (bytes <= 0)
                    break; // EOF or error
                write(STDOUT_FILENO, buffer, bytes);
            }

            // Read from stdin (user input)
            if (FD_ISSET(STDIN_FILENO, &read_fds)) {
                ssize_t bytes = read(STDIN_FILENO, buffer, sizeof(buffer));
                if (bytes <= 0)
                    break; // EOF or error
                write(master_fd, buffer, bytes);
            }

            // Check if child has exited
            if (waitpid(child_pid, &status, WNOHANG) > 0) {
                break;
            }
        }

        restore_terminal(); // Restore terminal settings
        close(master_fd);

        // Wait for child to exit
        waitpid(child_pid, &status, 0);
    }

    return 0;
}
