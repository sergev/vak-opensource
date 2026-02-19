#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <scsi/sg.h>
#include <errno.h>
#include <sys/ioctl.h>

#define SENSE_BUFF_LEN 32
#define MAX_CDB_LEN 6
#define FORMAT_PARAM_LEN 12

typedef struct {
    const char *name;
    unsigned char param[FORMAT_PARAM_LEN]; // UFI FORMAT UNIT parameter list
} teac_format_t;

// TEAC FD-05PUB allowed formats
teac_format_t teac_formats[] = {
    {"1.44M", {0x00,0x00,0x50,0x00,0x12,0x00,0x02,0x00,0x18,0x00,0x02,0x00}}, // 80x2x18x512
    {"1.20M", {0x00,0x00,0x50,0x00,0x0F,0x00,0x02,0x00,0x15,0x00,0x02,0x00}}, // 80x2x15x512
    {"1.232M",{0x00,0x00,0x31,0x00,0x08,0x00,0x02,0x00,0x08,0x00,0x02,0x00}}, // 77x2x8x1024
    {NULL, {0}}
};

// Send FORMAT UNIT with parameter list
int format_floppy(int fd, teac_format_t *fmt) {
    unsigned char cdb[MAX_CDB_LEN] = {0};
    unsigned char sense[SENSE_BUFF_LEN];
    memset(sense, 0, sizeof(sense));

    // FORMAT UNIT opcode = 0x04
    cdb[0] = 0x04;
    cdb[1] = 0x10; // FMTDATA = 1
    // Parameter list length in CDB[4]
    cdb[4] = FORMAT_PARAM_LEN;

    struct sg_io_hdr io;
    memset(&io, 0, sizeof(io));
    io.interface_id = 'S';
    io.cmd_len = sizeof(cdb);
    io.mx_sb_len = SENSE_BUFF_LEN;
    io.dxfer_direction = SG_DXFER_TO_DEV;
    io.dxfer_len = FORMAT_PARAM_LEN;
    io.cmdp = cdb;
    io.sbp = sense;
    io.dxferp = fmt->param;
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

    printf("Formatting %s as %s...\n", dev, fmt->name);

    if (format_floppy(fd, fmt) == 0) {
        printf("Format succeeded.\n");
    } else {
        printf("Format failed.\n");
    }

    close(fd);
    return 0;
}
