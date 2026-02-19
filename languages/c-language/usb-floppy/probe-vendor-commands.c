#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <scsi/sg.h>

#define DEF_TIMEOUT 5000
#define SENSE_LEN 64

// Send a SCSI command with no data transfer
static int send_scsi_cmd(int fd, uint8_t *cdb, int cdb_len)
{
    sg_io_hdr_t io;
    uint8_t sense[SENSE_LEN];
    memset(&io, 0, sizeof(io));
    memset(sense, 0, sizeof(sense));

    io.interface_id = 'S';
    io.cmdp = cdb;
    io.cmd_len = cdb_len;
    io.dxferp = NULL;
    io.dxfer_len = 0;
    io.dxfer_direction = SG_DXFER_NONE;
    io.mx_sb_len = sizeof(sense);
    io.sbp = sense;
    io.timeout = DEF_TIMEOUT;

    if (ioctl(fd, SG_IO, &io) < 0) {
        perror("SG_IO");
        return -1;
    }

    if ((io.info & SG_INFO_OK_MASK) == SG_INFO_OK) {
        return 0;  // Command accepted
    } else {
        return io.sb_len_wr ? sense[0] : -1;  // Return first byte of sense if failed
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
            printf("Usage: %s /dev/sdX\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    printf("Probing vendor-specific commands on %s...\n", argv[1]);

    for (uint8_t cmd = 0xC0; cmd <= 0xFF; cmd++) {
        uint8_t cdb[6] = {0};
        cdb[0] = cmd;

        int res = send_scsi_cmd(fd, cdb, 6);

        if (res == 0) {
            printf("0x%02X : ACCEPTED\n", cmd);
        } else if (res == -1) {
            printf("0x%02X : NO RESPONSE / ERROR\n", cmd);
        } else {
            printf("0x%02X : REJECTED, Sense byte 0 = 0x%02X\n", cmd, res);
        }
    }

    close(fd);
    return 0;
}
