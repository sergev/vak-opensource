#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <scsi/sg.h>
#include <errno.h>

#define DEF_TIMEOUT 5000
#define SENSE_LEN 64

// Generic SCSI send function
static int send_scsi_cmd(int fd,
                         uint8_t *cdb, int cdb_len,
                         void *data, int data_len,
                         int dxfer_dir)
{
    sg_io_hdr_t io;
    uint8_t sense[SENSE_LEN];

    memset(&io, 0, sizeof(io));
    memset(sense, 0, sizeof(sense));

    io.interface_id = 'S';
    io.cmdp = cdb;
    io.cmd_len = cdb_len;
    io.dxferp = data;
    io.dxfer_len = data_len;
    io.dxfer_direction = dxfer_dir;
    io.mx_sb_len = sizeof(sense);
    io.sbp = sense;
    io.timeout = DEF_TIMEOUT;

    if (ioctl(fd, SG_IO, &io) < 0) {
        perror("SG_IO");
        return -1;
    }

    if ((io.info & SG_INFO_OK_MASK) != SG_INFO_OK) {
        printf("SCSI command failed. Sense data:\n");
        for (int i = 0; i < io.sb_len_wr; i++)
            printf("%02x ", sense[i]);
        printf("\n");
        return -1;
    }

    return 0;
}

// Safe FORMAT UNIT test (zero parameter list)
static void test_format_unit_safe(int fd)
{
    uint8_t cdb[6];
    memset(cdb, 0, sizeof(cdb));

    cdb[0] = 0x04;   // FORMAT UNIT
    cdb[1] = 0x00;   // FmtData = 0 (no parameter list)
    cdb[4] = 0x00;   // Allocation length = 0

    printf("Issuing safe FORMAT UNIT (no parameter list)...\n");

    // dxfer_len = 0, SG_DXFER_NONE to avoid writing anything
    int res = send_scsi_cmd(fd, cdb, 6, NULL, 0, SG_DXFER_NONE);

    if (res == 0) {
        printf("FORMAT UNIT accepted (device would use default geometry)\n");
    } else {
        printf("FORMAT UNIT rejected (device does not allow any host formatting)\n");
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

    printf("Opening device: %s\n\n", argv[1]);

    test_format_unit_safe(fd);

    close(fd);
    return 0;
}
