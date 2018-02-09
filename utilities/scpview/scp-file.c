/*
 * stream/supercard_scp.c
 *
 * Parse SuperCard Pro SCP flux format.
 *
 * Written in 2014 by Simon Owen, based on code by Keir Fraser
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <err.h>

#include "scp.h"

#define SCK_NS_PER_TICK (25u)

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

int scp_open(scp_file_t *sf, const char *name)
{
    struct stat sbuf;
    uint8_t header[0x10], revs;
    int fd;

    if (stat(name, &sbuf) < 0)
        return -errno;

    fd = open(name, O_RDONLY);
    if (fd < 0)
        err(1, "%s", name);

    read_exact(fd, header, sizeof(header));

    if (memcmp(header, "SCP", 3) != 0)
        errx(1, "%s is not a SCP file!", name);

    revs = header[5];
    if (revs == 0 || revs > MAXREVS)
        errx(1, "%s has an invalid revolution count (%u)!", name, header[5]);

    if (header[9] != 0 && header[9] != 16)
        errx(1, "%s has unsupported bit cell time width (%u)", name, header[9]);

    memset(sf, 0, sizeof(*sf));
    sf->fd = fd;
    sf->revs = revs;

    return 0;
}

void scp_close(scp_file_t *sf)
{
    close(sf->fd);
    if (sf->dat) {
        free(sf->dat);
        sf->dat = 0;
    }
}

int scp_select_track(scp_file_t *sf, unsigned int tracknr)
{
    uint8_t trk_header[4];
    uint32_t longwords[3];
    unsigned int rev, trkoffset[sf->revs];
    uint32_t hdr_offset, tdh_offset;

    if (sf->dat && (sf->track == tracknr))
        return 0;

    free(sf->dat);
    sf->dat = NULL;
    sf->datsz = 0;

    hdr_offset = 0x10 + tracknr*sizeof(uint32_t);

    if (lseek(sf->fd, hdr_offset, SEEK_SET) != hdr_offset)
        return -1;

    read_exact(sf->fd, longwords, sizeof(uint32_t));
    tdh_offset = le32toh(longwords[0]);

    if (lseek(sf->fd, tdh_offset, SEEK_SET) != tdh_offset)
        return -1;

    read_exact(sf->fd, trk_header, sizeof(trk_header));
    if (memcmp(trk_header, "TRK", 3) != 0)
        return -1;

    if (trk_header[3] != tracknr)
        return -1;

    for (rev = 0 ; rev < sf->revs ; rev++) {
        read_exact(sf->fd, longwords, sizeof(longwords));
        trkoffset[rev] = tdh_offset + le32toh(longwords[2]);
        sf->index_off[rev] = le32toh(longwords[1]);
        sf->datsz += sf->index_off[rev];
    }

    /* Allocate data. */
    sf->dat = calloc(sf->datsz, sizeof(sf->dat[0]));
    if (! sf->dat)
        err(1, NULL);

    sf->datsz = 0;

    for (rev = 0 ; rev < sf->revs ; rev++) {
        if (lseek(sf->fd, trkoffset[rev], SEEK_SET) != trkoffset[rev])
            return -1;
        read_exact(sf->fd, &sf->dat[sf->datsz],
                   sf->index_off[rev] * sizeof(sf->dat[0]));
        sf->datsz += sf->index_off[rev];
        sf->index_off[rev] = sf->datsz;
    }

    sf->track = tracknr;
    return 0;
}

void scp_reset(scp_file_t *sf)
{
    sf->jitter = 0;
    sf->dat_idx = 0;
    sf->index_pos = 0;
}

#if 0
int scp_next_flux(scp_file_t *sf, unsigned int data_rpm)
{
    uint32_t val = 0, t;
    unsigned int nr_index_seen = 0;

    for (;;) {
        if (sf->dat_idx >= sf->index_pos) {
            uint32_t rev = s->nr_index % sf->revs; //TODO
            sf->index_pos = sf->index_off[rev];
            sf->dat_idx = rev ? sf->index_off[rev-1] : 0;
            s->ns_to_index = s->flux; //TODO
            /* Some drives return no flux transitions for tracks >= 160.
             * Bail if we see no flux transitions in a complete revolution. */
            if (nr_index_seen++)
                break;
            val = 0;
        }

        t = be16toh(sf->dat[sf->dat_idx++]);

        if (t == 0) { /* overflow */
            val += 0x10000;
            continue;
        }

        val += t;
        break;
    }

    /* If we are replaying a single revolution then jitter it a little to
     * trigger weak-bit variations. */
    if (sf->revs == 1) {
        int32_t jitter = rnd16(&s->prng_seed) & 3; //TODO
        if ((sf->jitter >= 4) || (sf->jitter <= -4)) {
            /* Already accumulated significant jitter; adjust for it. */
            jitter = sf->jitter / 2;
        } else if (jitter & 1) {
            /* Add one bit of jitter. */
            jitter >>= 1;
        } else {
            /* Subtract one bit of jitter. */
            jitter >>= 1;
            jitter = -jitter;
        }
        sf->jitter -= jitter;
        val += jitter;
    }

    val = ((uint64_t)val * SCK_NS_PER_TICK * s->drive_rpm) / data_rpm; //TODO

    /* If we are replaying a single revolution then randomly ignore
     * very short pulses (<1us). */
    if ((sf->revs == 1) && (val < 1000) && (rnd16(&s->prng_seed) & 1)) { //TODO
        sf->jitter += val;
        val = 0;
    }

    s->flux += val; //TODO
    return 0;
}
#endif
