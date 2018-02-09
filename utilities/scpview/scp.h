/*
 * Read SCP image format.
 */
#ifndef __SCP_H__
#define __SCP_H__

#include <stdint.h>

#define MAXREVS 15                      /* max revolution count */

typedef struct {
    int fd;

    /* Current track number. */
    unsigned int track;

    /* Raw track data. */
    uint16_t *dat;
    unsigned int datsz;

    unsigned int revs;                  /* stored disk revolutions */
    unsigned int dat_idx;               /* current index into dat[] */
    unsigned int index_pos;             /* next index offset */
    int jitter;                         /* accumulated injected jitter */

    unsigned int index_off[MAXREVS];    /* data offsets of each index */
} scp_file_t;

int scp_open(scp_file_t *sf, const char *name);

void scp_close(scp_file_t *sf);

int scp_select_track(scp_file_t *sf, unsigned int tracknr);

void scp_reset(scp_file_t *sf);

int scp_next_flux(scp_file_t *sf, unsigned int data_rpm);

#endif /* __SCP_H__ */
