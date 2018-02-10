/*
 * stream/supercard_scp.c
 *
 * Parse SuperCard Pro SCP flux format.
 *
 * Written in 2014 by Simon Owen, based on code by Keir Fraser
 */
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <err.h>
#include "scp.h"

#ifdef __APPLE__
#   include <libkern/OSByteOrder.h>
#   define be16toh(x) OSSwapBigToHostInt16(x)
#   define le16toh(x) OSSwapLittleToHostInt16(x)
#   define be32toh(x) OSSwapBigToHostInt32(x)
#   define le32toh(x) OSSwapLittleToHostInt32(x)
#endif

static void read_exact(int fd, void *buf, size_t count)
{
    ssize_t done;
    char *_buf = buf;

    while (count > 0) {
        done = read(fd, _buf, count);
        if (done < 0) {
            if ((errno == EAGAIN) || (errno == EINTR))
                continue;
            err(1, NULL);
        }
        if (done == 0) {
            memset(_buf, 0, count);
            done = count;
        }
        count -= done;
        _buf += done;
    }
}

/*
 * Open the SCP file.
 * Read disk header.
 */
int scp_open(scp_file_t *sf, const char *name)
{
    memset(sf, 0, sizeof(*sf));
    sf->fd = open(name, O_RDONLY);
    if (sf->fd < 0)
        err(1, "%s", name);

    read_exact(sf->fd, &sf->header, sizeof(sf->header));

    if (memcmp(sf->header.sig, "SCP", 3) != 0)
        errx(1, "%s: Not SCP file", name);

    if (sf->header.nr_revolutions == 0 || sf->header.nr_revolutions > REV_MAX)
        errx(1, "%s: Invalid revolution count = %u", name, sf->header.nr_revolutions);

    if (sf->header.cell_width != 0 && sf->header.cell_width != 16)
        errx(1, "%s: Unsupported cell width = %u", name, sf->header.cell_width);

    /* Convert to host byte order. */
    int i;
    for (i = 0; i < TRACK_MAX; i++) {
        sf->header.track_offset[i] = le32toh(sf->header.track_offset[i]);
    }

    return 0;
}

/*
 * Close the SCP file.
 * Free any allocated memory.
 */
void scp_close(scp_file_t *sf)
{
    close(sf->fd);
    if (sf->dat) {
        free(sf->dat);
        sf->dat = 0;
    }
}

/*
 * Select a track by index.
 * Read track header.
 */
int scp_select_track(scp_file_t *sf, unsigned int tn)
{
    /* Track already loaded? */
    if (sf->dat && (sf->track.track_nr == tn))
        return 0;

    /* Free data from previous track. */
    free(sf->dat);
    sf->dat = NULL;
    sf->datsz = 0;

    /* Read track header. */
    unsigned tdh_offset = sf->header.track_offset[tn];
    if (lseek(sf->fd, tdh_offset, SEEK_SET) != tdh_offset)
        return -1;

    read_exact(sf->fd, &sf->track, 4 + 12 * sf->header.nr_revolutions);
    if (memcmp(sf->track.sig, "TRK", 3) != 0)
        return -1;

    if (sf->track.track_nr != tn)
        return -1;

    /* Convert to host byte order.
     * Make offset from the start of the file.
     * Compute total data size. */
    unsigned int rev;
    for (rev = 0; rev < sf->header.nr_revolutions; rev++) {
        sf->track.rev[rev].duration_25ns = le32toh(sf->track.rev[rev].duration_25ns);
        sf->track.rev[rev].nr_samples = le32toh(sf->track.rev[rev].nr_samples);
        sf->track.rev[rev].offset = tdh_offset + le32toh(sf->track.rev[rev].offset);
        sf->datsz += sf->track.rev[rev].nr_samples;
    }

    /* Allocate data. */
    sf->dat = calloc(sf->datsz, sizeof(sf->dat[0]));
    if (! sf->dat)
        err(1, NULL);

    /* Read data. */
    sf->datsz = 0;
    for (rev = 0; rev < sf->header.nr_revolutions; rev++) {
        if (lseek(sf->fd, sf->track.rev[rev].offset, SEEK_SET) != sf->track.rev[rev].offset)
            return -1;
        read_exact(sf->fd, &sf->dat[sf->datsz],
                   sf->track.rev[rev].nr_samples * sizeof(sf->dat[0]));
        sf->datsz += sf->track.rev[rev].nr_samples;
        sf->index_ptr[rev] = sf->datsz;
    }
    return 0;
}

/*
 * Reset read pointers.
 */
void scp_reset(scp_file_t *sf)
{
    sf->iter_ptr = 0;
    sf->iter_limit = 0;
}

unsigned scp_next_flux(scp_file_t *sf, unsigned int rev)
{
    unsigned val = 0;

    for (;;) {
        if (sf->iter_ptr >= sf->iter_limit) {
            sf->iter_limit = sf->index_ptr[rev];
            sf->iter_ptr = rev ? sf->index_ptr[rev-1] : 0;
            val = 0;
        }

        unsigned t = be16toh(sf->dat[sf->iter_ptr++]);
        if (t != 0) {
            val += t;
            return val;
        }

        /* overflow */
        val += 0x10000;
    }
}

void scp_print_disk_header(scp_file_t *sf)
{
    printf("Disk Header:\n");
    printf("    Signature: %c%c%c\n", sf->header.sig[0], sf->header.sig[1], sf->header.sig[2]);
    printf("  SCP Version: %d.%d\n", sf->header.version >> 4, sf->header.version & 0xf);

    printf("    Disk Type: ");
    switch (sf->header.disk_type) {
    case 0:  printf("CBM\n");       break;
    case 1:  printf("AMIGA\n");     break;
    case 2:  printf("APPLE II\n");  break;
    case 3:  printf("ATARI ST\n");  break;
    case 4:  printf("ATARI 800\n"); break;
    case 5:  printf("MAC 800\n");   break;
    case 6:  printf("360K/720K\n"); break;
    case 7:  printf("1.44MB\n");    break;
    default: printf("%d\n", sf->header.disk_type); break;
    }

    printf("  Revolutions: %d\n", sf->header.nr_revolutions);
    printf("       Tracks: %d - %d\n", sf->header.start_track, sf->header.end_track);

    printf("        Flags: %x <", sf->header.flags);
    if (sf->header.flags & FLAG_TPI)    printf("96TPI");  else printf("48TPI");
    if (sf->header.flags & FLAG_RPM)    printf(" 360RPM"); else printf(" 300RPM");
    if (sf->header.flags & FLAG_INDEX)  printf(" Index");
    if (sf->header.flags & FLAG_TYPE)   printf(" Normalized");
    if (sf->header.flags & FLAG_MODE)   printf(" Writeable");
    if (sf->header.flags & FLAG_FOOTER) printf(" Footer");
    printf(">\n");

    printf("   Cell Width: %d\n", sf->header.cell_width ? sf->header.cell_width : 16);

    printf("        Sides: ");
    switch (sf->header.sides) {
    case SIDE_BOTH:   printf("Both\n");        break;
    case SIDE_BOTTOM: printf("Bottom only\n"); break;
    case SIDE_TOP:    printf("Top only\n");    break;
    default: printf("%d\n", sf->header.sides); break;
    }

    printf("     Checksum: %08x\n", sf->header.checksum);

    printf("Track Offsets:");
    int i;
    for (i = 0; i < TRACK_MAX; i++) {
        printf(" %d", sf->header.track_offset[i]);
        if (i % 10 == 9)
            printf("\n              ");
    }
    printf("\n");
}

void scp_print_track(scp_file_t *sf)
{
    int rev;

    printf("Track %d:\n", sf->track.track_nr);
    for (rev = 0; rev < sf->header.nr_revolutions; rev++) {
        scp_reset(sf);
        unsigned f1 = scp_next_flux(sf, rev);
        unsigned f2 = scp_next_flux(sf, rev);
        unsigned f3 = scp_next_flux(sf, rev);
        unsigned f4 = scp_next_flux(sf, rev);

        printf("  Revolution %d: %u samples, %f msec, offset %u, data %u-%u-%u-%u...\n",
            rev, sf->track.rev[rev].nr_samples,
            sf->track.rev[rev].duration_25ns * 0.000025,
            sf->track.rev[rev].offset, f1, f2, f3, f4);
    }
}
