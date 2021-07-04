#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <arpa/telnet.h>
#include "vtty.h"

struct termios Vtty::tios, Vtty::tios_orig;

Vtty *Vtty::first = nullptr;

pthread_t Vtty::thread{};

//
// Create console (when p=0) or TCP port.
//
Vtty::Vtty(const std::string &n, int p)
    : name(n), tcp_port(p)
{
    pthread_mutex_init(&lock, nullptr);

    if (tcp_port == 0) {
        // Console on stdout.
        fd = term_init();
        select_fd = &fd;
        output_stream = stdout;
    } else {
        // Telnet port.
        use_telnet_protocol = true;
        tcp_conn_wait();
    }

    next = first;
    first = this;
}

//
// Delete a virtual tty
//
Vtty::~Vtty()
{
    if (output_stream || tcp_port) {
        if (output_stream && output_stream != stdout) {
            fclose(output_stream);
        }
        output_stream = nullptr;

        // We don't close FD 0 since it is stdin
        if (fd > 0) {
            std::cerr << name << ": closing FD " << fd << std::endl;
            close(fd);
            fd = 0;
        }

        if (accept_fd >= 0) {
            std::cerr << name << ": closing accept FD " << accept_fd << std::endl;
            close(accept_fd);
            accept_fd = -1;
        }
        tcp_port = 0;
    }
}

//
// VTTY thread
//
void *Vtty::thread_main(void *arg)
{
    int fd;
    fd_set rfds{};

    for (;;) {
	if (! wait(&rfds))
	    continue;

        // Examine active FDs and call user handlers.
        for (auto vtty = first; vtty != nullptr; vtty = vtty->next) {
            if (! vtty->select_fd)
                continue;

            fd = *vtty->select_fd;
            if (fd < 0)
                continue;

            if (FD_ISSET(fd, &rfds)) {
                vtty->read_and_store();
            }

            // Flush any pending output
            if (vtty->output_stream) {
                fflush(vtty->output_stream);
            }
        }
    }
    return nullptr;
}

//
// Initialize the VTTY thread
//
void Vtty::start()
{
    if (pthread_create(&thread, nullptr, thread_main, nullptr)) {
        perror("vtty: pthread_create");
        exit(1);
    }
}

//
// Initialize TTY device.
//
int Vtty::term_init()
{
    int fd = open("/dev/tty", O_RDWR);
    if (fd < 0) {
        perror("/dev/tty");
        exit(-1);
    }

    tcgetattr(fd, &tios);

    memcpy(&tios_orig, &tios, sizeof(struct termios));
    atexit(term_reset);

    tios.c_cc[VTIME] = 0;
    tios.c_cc[VMIN] = 1;

    /* Disable Ctrl-C, Ctrl-S, Ctrl-Q and Ctrl-Z */
    tios.c_cc[VINTR] = 0;
    tios.c_cc[VSTART] = 0;
    tios.c_cc[VSTOP] = 0;
    tios.c_cc[VSUSP] = 0;

    tios.c_lflag &= ~(ICANON | ECHO);
    tios.c_iflag &= ~ICRNL;
    tcsetattr(fd, TCSANOW, &tios);
    tcflush(fd, TCIFLUSH);

    return fd;
}

//
// Restore TTY original settings
//
void Vtty::term_reset()
{
    // Restore stdin.
    tcsetattr(0, TCSANOW, &tios_orig);
}

//
// Returns TRUE if a character is available in buffer
//
bool Vtty::is_char_avail()
{
    pthread_mutex_lock(&lock);
    bool status = (read_ptr != write_ptr);
    pthread_mutex_unlock(&lock);
    return status;
}

//
// Read a character from the buffer (-1 if the buffer is empty)
//
int Vtty::get_char()
{
    unsigned char c;

    pthread_mutex_lock(&lock);

    if (read_ptr == write_ptr) {
        pthread_mutex_unlock(&lock);
        return -1;
    }

    c = buffer[read_ptr++];

    if (read_ptr == BUFFER_SIZE)
        read_ptr = 0;

    pthread_mutex_unlock(&lock);
    return c;
}

//
// Put char to vtty
//
void Vtty::put_char(char ch)
{
    if (tcp_port != 0) {
        if (tcp_state == Tcp_State::RUNNING &&
            fwrite(&ch, 1, 1, output_stream) != 1)
        {
            std::cerr << name << ": put char 0x" << std::hex << (int)(uint8_t)ch
                      << " failed: " << strerror(errno) << std::endl;
        }
    } else if (output_stream != nullptr) {
        if (write(fd, &ch, 1) != 1)
            perror("write");
    } else {
        std::cerr << name << ": not configured" << std::endl;
    }
}

//
// Store a character in the FIFO buffer
//
int Vtty::store(uint8_t c)
{
    pthread_mutex_lock(&lock);

    auto next_write_ptr = write_ptr + 1;
    if (next_write_ptr == BUFFER_SIZE)
        next_write_ptr = 0;

    if (next_write_ptr == read_ptr) {
        // No space in buffer.
        pthread_mutex_unlock(&lock);
        return -1;
    }

    buffer[write_ptr] = c;
    write_ptr = next_write_ptr;
    pthread_mutex_unlock(&lock);
    return 0;
}

//
// Read a character from the virtual TTY.
//
// If the VTTY is a TCP connection, restart it in case of error.
//
int Vtty::read()
{
    if (tcp_port != 0)
        return tcp_read();

    return term_read();
}

//
// Read a character from the terminal.
//
int Vtty::term_read()
{
    uint8_t c;

    if (::read(fd, &c, 1) != 1) {
        perror("read from vtty failed");
        return -1;
    }
    return c;
}

//
// Read a character (until one is available) and store it in buffer
//
void Vtty::read_and_store()
{
    int c;

    // Wait until we get a character input.
    c = read();

    /* if read error, do nothing */
    if (c < 0)
        return;

    if (! use_telnet_protocol) {
        store(c);
        return;
    }

    switch (input_state) {
    case Input_State::TEXT:
        switch (c) {
        case 0x1b:
            input_state = Input_State::IVT1;
            return;

            /* Ctrl + ']' (0x1d, 29), or Alt-Gr + '*' (0xb3, 179) */
        case 0x1d:
        case 0xb3:
            input_state = Input_State::REMOTE;
            return;
        case IAC:
            input_state = Input_State::TELNET;
            return;
        case 0:                /* NULL - Must be ignored - generated by Linux telnet */
        case 10:               /* LF (Line Feed) - Must be ignored on Windows platform */
            return;
        default:
            /* Store a standard character */
            store(c);
            return;
        }

    case Input_State::IVT1:
        switch (c) {
        case 0x5b:
            input_state = Input_State::IVT2;
            return;
        default:
            store(0x1b);
            store(c);
        }
        input_state = Input_State::TEXT;
        return;

    case Input_State::IVT2:
        switch (c) {
        case 0x41:             /* Up Arrow */
            store(16);
            break;
        case 0x42:             /* Down Arrow */
            store(14);
            break;
        case 0x43:             /* Right Arrow */
            store(6);
            break;
        case 0x44:             /* Left Arrow */
            store(2);
            break;
        default:
            store(0x5b);
            store(0x1b);
            store(c);
            break;
        }
        input_state = Input_State::TEXT;
        return;

    case Input_State::REMOTE:
        //remote_control(c);
        input_state = Input_State::TEXT;
        return;

    case Input_State::TELNET:
        telnet_cmd = c;
        switch (c) {
        case WILL:
        case WONT:
        case DO:
        case DONT:
            input_state = Input_State::TELNET_IYOU;
            return;
        case SB:
            telnet_cmd = c;
            input_state = Input_State::TELNET_SB1;
            return;
        case SE:
            break;
        case IAC:
            store(IAC);
            break;
        }
        input_state = Input_State::TEXT;
        return;

    case Input_State::TELNET_IYOU:
        telnet_opt = c;
        // if telnet client can support ttype, ask it to send ttype string
        if ((telnet_cmd == WILL) && (telnet_opt == TELOPT_TTYPE)) {
            put_char((char) IAC);
            put_char((char) SB);
            put_char((char) TELOPT_TTYPE);
            put_char((char) TELQUAL_SEND);
            put_char((char) IAC);
            put_char((char) SE);
        }
        input_state = Input_State::TEXT;
        return;

    case Input_State::TELNET_SB1:
        telnet_opt = c;
        input_state = Input_State::TELNET_SB2;
        return;

    case Input_State::TELNET_SB2:
        telnet_qual = c;
        if ((telnet_opt == TELOPT_TTYPE)
            && (telnet_qual == TELQUAL_IS))
            input_state = Input_State::TELNET_SB_TTYPE;
        else
            input_state = Input_State::TELNET_NEXT;
        return;

    case Input_State::TELNET_SB_TTYPE:
#if 0
        // parse ttype string: first char is sufficient
        // if client is xterm or vt, set the title bar
        if (c=='x' || c=='X' || c=='v' || c=='V') {
            fprintf(output_stream, "\033]0;pic32sim %s\07", name);
        }
#endif
        input_state = Input_State::TELNET_NEXT;
        return;

    case Input_State::TELNET_NEXT:
        // ignore all chars until next IAC
        if (c == IAC)
            input_state = Input_State::TELNET;
        return;
    }
}

//
// Read a character from the TCP connection.
//
int Vtty::tcp_read()
{
    uint8_t c;

    switch (tcp_state) {
    case Tcp_State::RUNNING:
        if (::read(fd, &c, 1) == 1)
            return c;

        // Problem with the connection: Re-enter wait mode.
        shutdown(fd, 2);
        fclose(output_stream);
        close(fd);
        output_stream = nullptr;
        fd = -1;
        select_fd = &accept_fd;
        tcp_state = Tcp_State::WAITING;
        return -1;

    case Tcp_State::WAITING:
        // A new connection has arrived.
        tcp_conn_accept();
        return -1;

    case Tcp_State::INVALID:
        // Shouldn't happen...
        return -1;
    }
}

//
// Accept a TCP connection
//
int Vtty::tcp_conn_accept()
{
    fd = accept(accept_fd, nullptr, nullptr);
    if (fd < 0) {
        fprintf(stderr,
            "tcp_conn_accept: accept on port %d failed %s\n",
            tcp_port, strerror(errno));
        return -1;
    }

    std::cerr << name << " is now connected (accept_fd=" << accept_fd
              << ", conn_fd=" << fd << ")" << std::endl;

    // Negotiate Telnet settings
    if (use_telnet_protocol) {
        telnet_do_ttype();
        telnet_will_echo();
        telnet_will_suppress_go_ahead();
        telnet_dont_linemode();
        input_state = Input_State::TELNET;
    }

    output_stream = fdopen(fd, "wb");
    if (! output_stream) {
        close(fd);
        fd = -1;
        return -1;
    }

    std::cerr << "Connected to VTTY - " << name << "\r\n\r" << std::endl;

    select_fd = &fd;
    tcp_state = Tcp_State::RUNNING;
    return 0;
}

//
// Send Telnet command: WILL TELOPT_ECHO
//
void Vtty::telnet_will_echo()
{
    uint8_t cmd[] = { IAC, WILL, TELOPT_ECHO };
    if (write(fd, cmd, sizeof(cmd)) < 0)
        perror("telnet_will_echo");
}

//
// Send Telnet command: Suppress Go-Ahead
//
void Vtty::telnet_will_suppress_go_ahead()
{
    uint8_t cmd[] = { IAC, WILL, TELOPT_SGA };
    if (write(fd, cmd, sizeof(cmd)) < 0)
        perror("telnet_will_suppress_go_ahead");
}

//
// Send Telnet command: Don't use linemode
//
void Vtty::telnet_dont_linemode()
{
    uint8_t cmd[] = { IAC, DONT, TELOPT_LINEMODE };
    if (write(fd, cmd, sizeof(cmd)) < 0)
        perror("telnet_dont_linemode");
}

//
// Send Telnet command: does the client support terminal type message?
//
void Vtty::telnet_do_ttype()
{
    uint8_t cmd[] = { IAC, DO, TELOPT_TTYPE };
    if (write(fd, cmd, sizeof(cmd)) < 0)
        perror("telnet_do_ttype");
}

//
// Wait for a TCP connection
//
int Vtty::tcp_conn_wait()
{
    struct sockaddr_in serv;
    int one = 1;

    tcp_state = Tcp_State::INVALID;

    accept_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (accept_fd < 0) {
        perror("tcp_conn_wait: socket");
        return -1;
    }

    if (setsockopt(accept_fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) < 0) {
        perror("tcp_conn_wait: setsockopt(SO_REUSEADDR)");
        goto error;
    }

    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    serv.sin_port = htons(tcp_port);

    if (bind(accept_fd, (struct sockaddr *) &serv, sizeof(serv)) < 0) {
        perror("tcp_conn_wait: bind");
        goto error;
    }

    if (listen(accept_fd, 1) < 0) {
        perror("tcp_conn_wait: listen");
        goto error;
    }

    std::cerr << name << ": waiting connection on tcp port " << tcp_port
              << " (FD " << accept_fd << ")" << std::endl;

    select_fd = &accept_fd;
    tcp_state = Tcp_State::WAITING;
    return 0;

error:
    close(accept_fd);
    accept_fd = -1;
    select_fd = nullptr;
    return -1;
}

//
// Wait for input and return a bitmask of file descriptors.
//
int Vtty::wait(fd_set *rfdp)
{
    struct timeval tv;
    int fd, fd_max, res;

    /* Build the FD set */
    FD_ZERO(rfdp);
    fd_max = -1;
    for (auto vtty = Vtty::first; vtty != nullptr; vtty = vtty->next) {
	if (! vtty->select_fd)
	    continue;

	fd = *vtty->select_fd;
	if (fd < 0)
	    continue;

	if (fd > fd_max)
	    fd_max = fd;
	FD_SET(fd, rfdp);
    }
    if (fd_max < 0) {
	/* No vttys created yet. */
	usleep(200000);
	return 0;
    }

    /* Wait for incoming data */
    tv.tv_sec = 0;
    tv.tv_usec = 10000; /* 10 ms */
    res = select(fd_max + 1, rfdp, nullptr, nullptr, &tv);

    if (res == -1) {
	if (errno != EINTR) {
	    perror("Vtty::wait: select");
            for (auto vtty = Vtty::first; vtty != nullptr; vtty = vtty->next) {
                std::cerr << "   " << vtty->name
                          << ": FD " << vtty->fd << std::endl;
	    }
	}
	return 0;
    }
    return 1;
}
