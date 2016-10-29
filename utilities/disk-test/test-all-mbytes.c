/*
 * Measure a real size of hard disk.
 *
 * Copyright (c) 2015, Serge Vakulenko
 *
 * Permission to use, copy, modify, and/or distribute this
 * software for any purpose with or without fee is hereby granted,
 * provided that the above copyright notice and this permission
 * notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

#define MAX_BLOCK_SZ    64      /* kbytes */

const char version[] = "1.0";
const char copyright[] = "Copyright (C) 2015 Serge Vakulenko";

char *progname;
int verbose;
int blocksize_kbytes = 4;
int fd;
unsigned block[MAX_BLOCK_SZ*1024 / sizeof(unsigned)];

int write_mbyte(unsigned m)
{
    off_t offset = m * (off_t)1024*1024;
    int nbytes = blocksize_kbytes * 1024;
    int words_per_block = nbytes / sizeof(unsigned);
    int n;

    /* Fill buffer with data, based on block number. */
    for (n=0; n<words_per_block; n++) {
        block[n] = ~m;
    }

    /* Seek to the needed block number. */
    if (lseek(fd, offset, SEEK_SET) != offset) {
        printf("Mbyte #%u: seek failed\n", m);
        return 0;
    }

    /* Write block data. */
    if (write(fd, block, nbytes) != nbytes) {
        printf("Mbyte #%u: write failed\n", m);
        return 0;
    }
    if (m % 100 == 0) {
        if (m == 0)
            printf("Write Mbyte");
        fdatasync(fd);
        printf(" #%u", m);
        fflush(stdout);
    }
    return 1;
}

void verify_mbyte(unsigned m)
{
    off_t offset = m * (off_t)1024*1024;
    int nbytes = blocksize_kbytes * 1024;
    int words_per_block = nbytes / sizeof(unsigned);
    int n;

    /* Seek to the needed block number. */
    if (lseek(fd, offset, SEEK_SET) != offset) {
        printf("Mbyte #%u: seek failed\n", m);
        exit(-1);
    }

    /* Read block data. */
    if (read(fd, block, nbytes) != nbytes) {
        printf("Mbyte #%u: read failed\n", m);
        exit(-1);
    }

    /* Verify data. */
    for (n=0; n<words_per_block; n++) {
        if (block[n] != ~m) {
            printf("Mbyte #%u, word %u: data error: read %x, expected %x\n",
                m, n, block[n], ~m);
            exit(-1);
        }
    }
    if (m % 100 == 0) {
        if (m == 0)
            printf("Read Mbyte");
        printf(" #%u", m);
        fflush(stdout);
    }
}

void usage()
{
    printf("Disk test, Version %s, %s\n", version, copyright);
    printf("Usage:\n");
    printf("    %s [-v] [-b blocksz] /dev/disk\n", progname);
    printf("Options:\n");
    printf("    -v    verbose mode\n");
    printf("    -b #  block size in kbytes, default 4\n");
    exit(-1);
}

int main(int argc, char **argv)
{
    char *filename = 0;
    unsigned n, maxn;

    progname = *argv;
    for (;;) {
        switch (getopt(argc, argv, "vb:")) {
        case EOF:
            break;
        case 'v':
            ++verbose;
            continue;
        case 'b':
            blocksize_kbytes = strtol(optarg, 0, 0);
            continue;
        default:
            usage();
        }
        break;
    }
    argc -= optind;
    argv += optind;

    if (argc != 1)
        usage();
    filename = argv[0];

    /*
     * Verify parameters.
     */
    if (blocksize_kbytes < 1 || blocksize_kbytes > MAX_BLOCK_SZ) {
        printf("Bad block size = %d kbytes.\n", blocksize_kbytes);
        printf("Valid range is 1...%d kbytes.\n", MAX_BLOCK_SZ);
        exit(-1);
    }
    printf("Disk device: %s\n", filename);
    if (blocksize_kbytes != 4)
        printf("Block size: %d kbytes\n", blocksize_kbytes);

    /*
     * Open the file.
     */
    fd = open(filename, O_RDWR);
    if (fd < 0) {
        printf("%s: Cannot open\n", filename);
        exit(-1);
    }

    /*
     * Write data to first block of every megabyte.
     */
    for (n=0; ; n++) {
        if (! write_mbyte(n))
            break;
    }
    maxn = n;
    printf("\nDisk size is %u Mbytes\n", maxn);

    /*
     * Verify contents of written blocks.
     */
    for (n=0; n<maxn; n++) {
        verify_mbyte(n);
    }
    printf("\nDisk is OK\n");

    close(fd);
    return 0;
}
