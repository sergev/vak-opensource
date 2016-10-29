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

int write_block(unsigned bno)
{
    off_t nbytes = blocksize_kbytes * 1024;
    int words_per_block = nbytes / sizeof(unsigned);
    int n;

    /* Fill buffer with data, based on block number. */
    for (n=0; n<words_per_block; n++) {
        block[n] = ~bno;
    }

    /* Seek to the needed block number. */
    if (lseek(fd, bno * nbytes, SEEK_SET) != bno * nbytes) {
        printf("Block #%u: seek failed\n", bno);
        return 0;
    }

    /* Write block data. */
    if (write(fd, block, nbytes) != nbytes) {
        printf("Block #%u: write failed\n", bno);
        return 0;
    }
    printf("Write block #%u\n", bno);
    return 1;
}

void verify_block(unsigned bno)
{
    off_t nbytes = blocksize_kbytes * 1024;
    int words_per_block = nbytes / sizeof(unsigned);
    int n;

    /* Seek to the needed block number. */
    if (lseek(fd, bno * nbytes, SEEK_SET) != bno * nbytes) {
        printf("Block #%u: seek failed\n", bno);
        exit(-1);
    }

    /* Read block data. */
    if (read(fd, block, nbytes) != nbytes) {
        printf("Block #%u: read failed\n", bno);
        exit(-1);
    }

    /* Verify data. */
    for (n=0; n<words_per_block; n++) {
        if (block[n] != ~bno) {
            printf("Block #%u, word %u: data error: read %x, expected %x\n",
                bno, n, block[n], ~bno);
            exit(-1);
        }
    }
    printf("Read block #%u: OK\n", bno);
}

void verify_written_blocks()
{
    if (! verify_block(0))
        return 0;
    for (n=1; n<maxn; n<<=1) {
        verify_block(n);
    }
}

void find_last_block(unsigned last_good, unsigned first_bad)
{
    unsigned n = (last_good + first_bad) / 2;

    if (n == last_good) {
        printf("Last valid block is #%u\n", last_good);
        printf("Disk size is %u kbytes, or %u Mbytes, or %u Gbytes\n",
            first_bad * blocksize_kbytes,
            first_bad * blocksize_kbytes / 1024
            first_bad * blocksize_kbytes / 1024 / 1024);
        return;
    }
    write_block(n);
    if (verify_written_blocks()) {
    } else {
    }
}

void usage()
{
    printf("Disk size test, Version %s, %s\n", version, copyright);
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
     * Write data to block 0,1,2,4,8, ... 2^k.
     */
    if (! write_block(0)) {
        printf("Block #0 damaged.\n");
        exit(-1);
    }
    for (maxn=1; maxn>0; maxn<<=1) {
        if (! write_block(maxn))
            break;
    }

    /*
     * Verify contents of written blocks.
     */
    if (! verify_written_blocks()) {
        printf("Disk is damaged.\n");
        exit(-1);
    }

    /*
     * Find last valid block.
     */
    find_last_block(maxn>>1, maxn);

    close(fd);
    return 0;
}
