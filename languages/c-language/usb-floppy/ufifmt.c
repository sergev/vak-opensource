#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <scsi/sg.h>
#include <errno.h>
#include <sys/ioctl.h>

#define SENSE_BUFF_LEN 32
#define MAX_CDB_LEN 12

// TEAC allowed formats
typedef struct {
    const char *name;
    int tracks;
    int heads;
    int sectors;
    int block_size;
} teac_format_t;

teac_format_t teac_formats[] = {
    {"1.44M", 80, 2, 18, 512},
    {"1.20M", 80, 2, 15, 512},
    {"1.232M", 77, 2, 8, 1024},
    {NULL, 0, 0, 0, 0}
};

// Send FORMAT UNIT command via SG_IO
int format_floppy(int fd, teac_format_t *fmt) {
    unsigned char cdb[6];
    unsigned char sense[SENSE_BUFF_LEN];
    memset(cdb, 0, sizeof(cdb));
    memset(sense, 0, sizeof(sense));

    // FORMAT UNIT (6-byte) opcode = 0x04
    cdb[0] = 0x04;   // FORMAT UNIT
    cdb[1] = 0x10;   // Densities = default, FmtData = 1
    // cdb[2..4] = parameter list length (UFI often ignores)
    cdb[4] = 0;      // No parameter list for safe UFI
    cdb[5] = 0;      // Control

    struct sg_io_hdr io;
    memset(&io, 0, sizeof(io));
    io.interface_id = 'S';
    io.cmd_len = sizeof(cdb);
    io.mx_sb_len = SENSE_BUFF_LEN;
    io.cmdp = cdb;
    io.sbp = sense;
    io.dxfer_direction = SG_DXFER_NONE;
    io.timeout = 20000; // 20 seconds for format

    if (ioctl(fd, SG_IO, &io) < 0) {
        perror("SG_IO");
        return -1;
    }

    if ((io.info & SG_INFO_OK_MASK) != SG_INFO_OK) {
        printf("FORMAT UNIT failed, sense: ");
        for (int i = 0; i < io.sb_len_wr; i++) printf("%02X ", sense[i]);
        printf("\n");
        return -1;
    }

    return 0;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s /dev/sdX format\n", argv[0]);
        fprintf(stderr, "Formats: 1.44M, 1.20M, 1.232M\n");
        return 1;
    }

    const char *dev = argv[1];
    const char *fmt_name = argv[2];

    teac_format_t *fmt = NULL;
    for (int i = 0; teac_formats[i].name; i++) {
        if (strcmp(fmt_name, teac_formats[i].name) == 0) {
            fmt = &teac_formats[i];
            break;
        }
    }

    if (!fmt) {
        fprintf(stderr, "Unknown format %s\n", fmt_name);
        return 1;
    }

    int fd = open(dev, O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    printf("Formatting %s as %s (%d tracks, %d heads, %d sectors, %d bytes/sector)\n",
           dev, fmt->name, fmt->tracks, fmt->heads, fmt->sectors, fmt->block_size);

    if (format_floppy(fd, fmt) == 0) {
        printf("Format succeeded.\n");
    } else {
        printf("Format failed.\n");
    }

    close(fd);
    return 0;
}
