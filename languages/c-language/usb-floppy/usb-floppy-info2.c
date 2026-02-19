#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <scsi/sg.h>
#include <errno.h>

#define SENSE_LEN 64
#define DEF_TIMEOUT 5000

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

static void print_inquiry(int fd)
{
    uint8_t cdb[6] = {0x12, 0, 0, 0, 96, 0};
    uint8_t data[96];

    if (send_scsi_cmd(fd, cdb, 6, data, sizeof(data),
                      SG_DXFER_FROM_DEV) < 0)
        return;

    char vendor[9] = {0};
    char product[17] = {0};
    char revision[5] = {0};

    memcpy(vendor,  &data[8],  8);
    memcpy(product, &data[16], 16);
    memcpy(revision,&data[32], 4);

    printf("=== INQUIRY ===\n");
    printf("Vendor:   %s\n", vendor);
    printf("Product:  %s\n", product);
    printf("Revision: %s\n", revision);
    printf("Peripheral Device Type: 0x%02x\n", data[0] & 0x1f);
    printf("\n");
}

static void test_unit_ready(int fd)
{
    uint8_t cdb[6] = {0x00,0,0,0,0,0};

    printf("=== TEST UNIT READY ===\n");
    if (send_scsi_cmd(fd, cdb, 6, NULL, 0, SG_DXFER_NONE) == 0)
        printf("Device is ready.\n\n");
    else
        printf("Device not ready.\n\n");
}

static void read_capacity(int fd)
{
    uint8_t cdb[10] = {0x25,0,0,0,0,0,0,0,0,0};
    uint8_t data[8];

    if (send_scsi_cmd(fd, cdb, 10, data, sizeof(data),
                      SG_DXFER_FROM_DEV) < 0)
        return;

    uint32_t last_lba =
        (data[0]<<24)|(data[1]<<16)|(data[2]<<8)|data[3];
    uint32_t block_size =
        (data[4]<<24)|(data[5]<<16)|(data[6]<<8)|data[7];

    uint64_t total_blocks = (uint64_t)last_lba + 1;
    uint64_t total_bytes = total_blocks * block_size;

    printf("=== READ CAPACITY (10) ===\n");
    printf("Last LBA: %u\n", last_lba);
    printf("Block size: %u bytes\n", block_size);
    printf("Total blocks: %llu\n", (unsigned long long)total_blocks);
    printf("Total size: %.2f MB\n",
           total_bytes / (1024.0*1024.0));
    printf("\n");
}

static void read_format_capacities(int fd)
{
    uint8_t cdb[10] = {0x23,0,0,0,0,0,0,0,0,0};
    uint8_t data[252];

    cdb[7] = sizeof(data) >> 8;
    cdb[8] = sizeof(data) & 0xff;

    if (send_scsi_cmd(fd, cdb, 10, data, sizeof(data),
                      SG_DXFER_FROM_DEV) < 0)
        return;

    uint32_t list_len =
        (data[0]<<24)|(data[1]<<16)|(data[2]<<8)|data[3];

    printf("=== READ FORMAT CAPACITIES ===\n");
    printf("Capacity List Length: %u bytes\n", list_len);

    int offset = 4;
    while (offset < list_len + 4) {
        uint32_t blocks =
            (data[offset]<<24)|(data[offset+1]<<16)|
            (data[offset+2]<<8)|data[offset+3];
        uint32_t block_size =
            (data[offset+5]<<16)|
            (data[offset+6]<<8)|data[offset+7];

        uint8_t desc_code = data[offset+4] >> 2;

        printf("  Blocks: %u\n", blocks);
        printf("  Block size: %u\n", block_size);
        printf("  Descriptor code: %u\n", desc_code);
        printf("  Capacity: %.2f MB\n",
               (blocks * block_size) / (1024.0*1024.0));
        printf("\n");

        offset += 8;
    }
}

static void mode_sense_10(int fd)
{
    uint8_t cdb[10] = {0};
    uint8_t data[512];

    memset(cdb, 0, sizeof(cdb));
    memset(data, 0, sizeof(data));

    cdb[0] = 0x5A;        // MODE SENSE(10)
    cdb[1] = 0x08;        // DBD=1 (Disable Block Descriptors)
    cdb[2] = 0x3F;        // Request all pages
    cdb[7] = sizeof(data) >> 8;
    cdb[8] = sizeof(data) & 0xFF;

    if (send_scsi_cmd(fd, cdb, 10, data, sizeof(data),
                      SG_DXFER_FROM_DEV) < 0) {
        printf("MODE SENSE(10) failed.\n\n");
        return;
    }

    printf("=== MODE SENSE (10) ===\n");

    uint16_t mode_data_len =
        (data[0] << 8) | data[1];

    printf("Mode Data Length: %u\n", mode_data_len);

    printf("Medium Type: 0x%02x\n", data[2]);
    printf("Device-Specific Parameter: 0x%02x\n", data[3]);

    uint16_t block_desc_len =
        (data[6] << 8) | data[7];

    printf("Block Descriptor Length: %u\n", block_desc_len);

    int total_len = mode_data_len + 2;
    int offset = 8 + block_desc_len;

    while (offset + 1 < total_len) {

        uint8_t page_code = data[offset] & 0x3F;
        uint8_t page_len  = data[offset + 1];

        if (page_len == 0)
            break;

        printf("\nPage 0x%02X (length %u)\n",
               page_code, page_len);

        for (int i = 0; i < page_len; i++)
            printf("%02x ", data[offset + 2 + i]);

        printf("\n");

        offset += 2 + page_len;
    }
}

static void mode_sense(int fd)
{
    uint8_t cdb[6] = {0x1A,0,0x3F,0,192,0};
    uint8_t data[192];

    if (send_scsi_cmd(fd, cdb, 6, data, sizeof(data),
                      SG_DXFER_FROM_DEV) < 0)
        return;

    printf("=== MODE SENSE (6) ===\n");
    printf("Mode Data Length: %u\n", data[0]);
    printf("Medium Type: 0x%02x\n", data[1]);
    printf("Device-Specific Parameter: 0x%02x\n", data[2]);
    printf("\n");
}

static void flexible_disk_page(int fd)
{
    uint8_t cdb[6] = {0x1A, 0, 0x05, 0, 192, 0};
    uint8_t data[192];

    if (send_scsi_cmd(fd, cdb, 6, data, sizeof(data),
                      SG_DXFER_FROM_DEV) < 0)
        return;

    int header_len = 4;  // MODE SENSE(6) header
    uint8_t *page = &data[header_len];

    if ((page[0] & 0x3F) != 0x05) {
        printf("Flexible Disk Page not returned.\n\n");
        return;
    }

    printf("=== Flexible Disk Mode Page (0x05) ===\n");

    uint16_t transfer_rate =
        (page[2] << 8) | page[3];

    uint8_t heads = page[4];
    uint8_t sectors = page[5];

    uint16_t bytes_per_sector =
        (page[6] << 8) | page[7];

    uint16_t cylinders =
        (page[8] << 8) | page[9];

    printf("Transfer rate: %u kbit/s\n", transfer_rate);
    printf("Heads: %u\n", heads);
    printf("Sectors/track: %u\n", sectors);
    printf("Bytes/sector: %u\n", bytes_per_sector);
    printf("Cylinders: %u\n", cylinders);

    printf("Approx capacity: %.2f MB\n",
        (double)heads * sectors * cylinders *
        bytes_per_sector / (1024.0*1024.0));

    printf("\n");
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s /dev/sdX\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    printf("Opening device: %s\n\n", argv[1]);

    print_inquiry(fd);
    test_unit_ready(fd);
    read_capacity(fd);
    read_format_capacities(fd);
    mode_sense_10(fd);
    //flexible_disk_page(fd);

    close(fd);
    return 0;
}
