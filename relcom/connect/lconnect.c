#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <sys/ioctl.h>

struct termios tc, tt, ott;
char *comname = "/dev/cuaa1";
char *termname = "/dev/tty";
int speed = 9600;
int term, com;
int dtr, rts;

void quit()
{
    if (com >= 0) {
        close(com);
        fprintf(stderr, "Disconnected.\n");
    }
    if (term >= 0) {
        tcsetattr(term, 0, &ott);
        close(term);
    }
    exit(0);
}

void run(char *av, ...)
{
    int status;
    int pid;

    pid = fork();
    if (pid < 0) {
        perror("fork");
        return;
    }
    if (pid == 0) {
        /* child */
        close(0);
        dup(com);
        close(1);
        dup(com);
        close(com);
        close(term);
        execvp(av, &av);
        exit(-1);
    }
    /* parent */
    signal(SIGTERM, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGINT, SIG_IGN);

    while (pid != wait(&status))
        continue;

    signal(SIGTERM, quit);
    signal(SIGQUIT, quit);
    signal(SIGINT, quit);
}

int filter(int len, unsigned char *str)
{
    static int state = 0;
    static char *prefix = "\30B000000";
    int i;

    for (i=0; i<len; ++i) {
        if (str[i] == prefix[state])
            ++state;
        else
            state = 0;

        if (! prefix[state]) {
            run("rz", "-t", "10", (char*) 0);
            state = 0;
        }
    }
    return len;
}

void showstatus()
{
    int s;

    if (ioctl(com, TIOCMGET, &s) < 0) {
        perror("TIOCMGET");
        return;
    }
    printf("DTR\tRTS\tCTS\tDSR\tRING\n");
    printf("%c\t%c\t%c\t%c\t%c\n",
        (s & TIOCM_DTR) ? '+' : '-',
        (s & TIOCM_RTS) ? '+' : '-',
        (s & TIOCM_CTS) ? '+' : '-',
        (s & TIOCM_DSR) ? '+' : '-',
        (s & TIOCM_RI) ? '+' : '-');
}

int cmd(int len, unsigned char *str)
{
    static int state = 0;
    static char *prefix = "\36c";
    int i, val;

    for (i=0; i<len; ++i) {
        if (state) {
            switch (str[i]) {
            default:
                continue;
            case 'q':
                quit();
                break;
            case 's':
                showstatus();
                break;
            case 'h':
                printf("Hanging up...");
                fflush(stdout);
                ioctl(com, TIOCCDTR, 0);
                sleep(1);
                printf("done\n");
                ioctl(com, dtr ? TIOCSDTR : TIOCCDTR, 0);
                break;
            case 'b':
                printf("Sending break...");
                fflush(stdout);
                tcsendbreak(com, 0);
                printf("done\n");
                break;
            case 'd':
                printf("Set DTR = %s...",
                    (dtr ^= 1) ? "ON" : "OFF");
                fflush(stdout);
                ioctl(com, dtr ? TIOCSDTR : TIOCCDTR, 0);
                printf("done\n");
                break;
            case 'r':
                printf("Set RTS = %s...",
                    (rts ^= 1) ? "ON" : "OFF");
                fflush(stdout);
                val = (rts ? TIOCM_RTS : 0) |
                     (dtr ? TIOCM_DTR : 0);
                ioctl(com, TIOCMSET, &val);
                printf("done\n");
                break;
            }
            state = 0;
        } else if (str[i] == prefix[state]) {
            ++state;
            printf("Command:  S-Status  B-send Break  H-Hangup  D-change DTR  R-change RTS  Q-Quit\n");
        } else
            continue;
        bcopy(str+i+1, str+i, len-i-1);
        --len;
        --i;
    }
    return len;
}

int copy(int a, int b, char *name, int (*dofilter)())
{
    unsigned char buf [512];
    int len;

    len = read(a, buf, sizeof(buf));
    if (len > 0) {
        if (dofilter)
            len = (*dofilter)(len, buf);
        if (len > 0) {
#if 0
            int i;
            fprintf(stderr, "<%02x", buf[0]);
            for (i=1; i<len; i++)
                fprintf(stderr, "-%02x", buf[i]);
            fprintf(stderr, "> ");
            fflush(stderr);
#endif
            if (write(b, buf, len) != len)
                perror("write");
            if (b == com && buf[len-1] == '\r') {
                /* Append \n after \r. */
                if (write(b, "\n", 1) != 1)
                    perror("write");
                write(term, "\n", 1);
            }
        }
    } else if (len < 0) {
        perror(name);
    } else {
        fprintf(stderr, "EOF on %s\n", name);
    }
    return len;
}

void connect(int a, int b, char *aname, char *bname)
{
    fd_set mask, m;
    int idle;

    FD_ZERO(&mask);
    FD_SET(a, &mask);
    FD_SET(b, &mask);
    dtr = rts = 1;

    for (;;) {
        m = mask;
        if (select(32, &m, 0, 0, 0) < 0) {
            perror("select");
            return;
        }
        idle = 1;
        if (FD_ISSET(b, &m)) {
            if (copy(b, a, bname, filter) < 0)
                return;
            idle = 0;
        }
        if (FD_ISSET(a, &m)) {
            if (copy(a, b, aname, cmd) < 0)
                return;
            idle = 0;
        }
        if (idle)
            fprintf(stderr, "unexpected return from select()\n");
    }
}

int main(int argc, char **argv)
{
    if (argc > 1)
        comname = argv[1];
    if (argc > 2)
        speed = atoi(argv[2]);

    signal(SIGTERM, quit);
    signal(SIGQUIT, quit);
    signal(SIGINT, quit);

    term = open(termname, O_RDWR);
    if (term < 0) {
        perror(termname);
        exit(-1);
    }

    fprintf(stderr, "Opening `%s'...", comname);
    com = open(comname, O_RDWR | O_NONBLOCK);
    if (com < 0) {
        perror(comname);
        exit(-1);
    }
    fprintf(stderr, "done.\n");

    tcgetattr(com, &tc);
    cfmakeraw(&tc);
    cfsetispeed(&tc, speed);
    cfsetospeed(&tc, speed);
    tc.c_iflag |= IGNBRK;
    tc.c_cflag |= CLOCAL;
    tcsetattr(com, TCSAFLUSH, &tc);

    tcgetattr(term, &tt);
    ott = tt;
    tt.c_lflag &= ~(ICANON | ECHOCTL);
    tt.c_iflag &= ~ICRNL;
    tt.c_iflag |= INLCR;
    tcsetattr(term, TCSAFLUSH, &tt);

    fprintf(stderr, "Connected at %d baud.  Press ^^q to quit.\n", speed);
    fcntl(com, F_SETFL, 0);
    connect(term, com, termname, comname);

    quit();
    return 0;
}
