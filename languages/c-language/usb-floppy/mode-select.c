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

// Test MODE SELECT(10)
static void test_mode_select(int fd)
{
    // Minimal Mode Parameter Header (safe, header only, no pages)
    uint8_t header[8];
    memset(header, 0, sizeof(header));

    // Fill header with dummy length values
    header[0] = 0x00; // Mode Data Length MSB
    header[1] = 0x3E; // Mode Data Length LSB = 62 (like TEAC returned)
    header[2] = 0x00; // Medium Type
    header[3] = 0x80; // Device-specific (write protect bit set)
    header[4] = 0x00; // Reserved
    header[5] = 0x00; // Reserved
    header[6] = 0x00; // Block Descriptor Length MSB
    header[7] = 0x00; // Block Descriptor Length LSB

    uint8_t cdb[10];
    memset(cdb, 0, sizeof(cdb));

    cdb[0] = 0x55;           // MODE SELECT(10)
    cdb[1] = 0x10;           // PF bit = 1 (Parameter List format)
    cdb[7] = sizeof(header) >> 8;
    cdb[8] = sizeof(header) & 0xFF;

    printf("Issuing MODE SELECT(10) with minimal header...\n");

    int res = send_scsi_cmd(fd, cdb, 10, header, sizeof(header), SG_DXFER_TO_DEV);

    if (res == 0) {
        printf("MODE SELECT succeeded (unexpected!)\n");
    } else {
        printf("MODE SELECT failed as expected.\n");
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

    test_mode_select(fd);

    close(fd);
    return 0;
}
