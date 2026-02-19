#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <scsi/sg.h>
#include <scsi/scsi.h>
#include <sys/ioctl.h>

#define MAX_DATA_LEN 1024

int send_ufi_command(int fd, unsigned char *cdb, int cdb_len, unsigned char *data, int data_len, int direction) {
    sg_io_hdr_t io_hdr;
    unsigned char sense_buffer[32];

    memset(&io_hdr, 0, sizeof(io_hdr));
    io_hdr.interface_id = 'S';
    io_hdr.cmdp = cdb;
    io_hdr.cmd_len = cdb_len;
    io_hdr.sbp = sense_buffer;
    io_hdr.mx_sb_len = sizeof(sense_buffer);
    io_hdr.dxferp = data;
    io_hdr.dxfer_len = data_len;
    io_hdr.dxfer_direction = direction ? SG_DXFER_TO_DEV : SG_DXFER_FROM_DEV;
    io_hdr.timeout = 20000;  // 20 seconds

    int ret = ioctl(fd, SG_IO, &io_hdr);
    if (ret < 0) {
        perror("ioctl SG_IO");
        return -1;
    }

    if (io_hdr.status != 0) {
        printf("SCSI status error: 0x%02x\n", io_hdr.status);
        if (io_hdr.sb_len_wr > 0) {
            printf("Sense data: ");
            for (int i = 0; i < io_hdr.sb_len_wr; i++) {
                printf("%02x ", sense_buffer[i]);
            }
            printf("\n");
        }
        return -1;
    }

    return io_hdr.dxfer_len;  // Return transferred length
}

void print_inquiry_data(unsigned char *data) {
    char vendor[9], product[17], revision[5];

    strncpy(vendor, (char *)data + 8, 8);
    vendor[8] = '\0';
    strncpy(product, (char *)data + 16, 16);
    product[16] = '\0';
    strncpy(revision, (char *)data + 32, 4);
    revision[4] = '\0';

    printf("Vendor: %s\n", vendor);
    printf("Product: %s\n", product);
    printf("Revision: %s\n", revision);
    printf("Peripheral Device Type: 0x%02x (Direct Access Block Device)\n", data[0]);
    printf("Removable Media: %s\n", (data[1] & 0x80) ? "Yes" : "No");
    printf("ANSI Version: %d\n", data[2] & 0x07);
}

void print_format_capacities(unsigned char *data, int len) {
    if (len < 4) return;

    int list_len = data[3];
    printf("Capacity List Length: %d bytes\n", list_len);

    int offset = 4;
    while (offset + 8 <= len && list_len >= 8) {
        unsigned int blocks = (data[offset] << 24) | (data[offset+1] << 16) | (data[offset+2] << 8) | data[offset+3];
        int type = data[offset+4] >> 6;  // Bits 7-6 of byte 4
        unsigned int block_size = (data[offset+5] << 16) | (data[offset+6] << 8) | data[offset+7];

        const char *type_str;
        switch (type) {
            case 0: type_str = "Unformatted"; break;
            case 1: type_str = "Formatted"; break;
            case 2: type_str = "Formattable"; break;
            default: type_str = "Reserved"; break;
        }

        printf("- %s Capacity: %u blocks of %u bytes (%u KB total)\n", type_str, blocks, block_size, (blocks * block_size) / 1024);

        offset += 8;
        list_len -= 8;
    }
}

void print_read_capacity(unsigned char *data) {
    unsigned int blocks = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
    unsigned int block_size = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];

    printf("Media Capacity: %u blocks of %u bytes (%u KB total)\n", blocks + 1, block_size, ((blocks + 1) * block_size) / 1024);
}

void print_mode_sense_data(unsigned char *data, int len) {
    if (len < 8) return;  // MODE SENSE(10) header is 8 bytes

    int mode_data_len = (data[0] << 8) | data[1];
    printf("Mode Data Length: %d\n", mode_data_len);
    printf("Medium Type: 0x%02x\n", data[2]);
    printf("Device Specific Parameter: 0x%02x\n", data[3]);
    printf("Reserved: 0x%02x%02x\n", data[4], data[5]);
    int block_desc_len = (data[6] << 8) | data[7];
    printf("Block Descriptor Length: %d\n", block_desc_len);

    int offset = 8 + block_desc_len;  // Skip header and block descriptors
    while (offset < len) {
        int page_code = data[offset] & 0x3F;
        int page_len = data[offset + 1];

        if (offset + page_len + 2 > len) break;  // Prevent overflow

        if (page_len != 0) {
            printf("Mode Page 0x%02x (Length: %d):\n", page_code, page_len);
            for (int i = 0; i < page_len; i++) {
                printf("%02x ", data[offset + 2 + i]);
                if ((i + 1) % 16 == 0) printf("\n");
            }
            if (page_len % 16 != 0) printf("\n");

            if (page_code == 0x05) {  // Flexible Disk Page
                printf("  Density Code: 0x%02x\n", data[offset + 2]);
                printf("  Number of Tracks: %u\n", (data[offset + 3] << 8) | data[offset + 4]);
                printf("  Sectors per Track: %u\n", (data[offset + 5] << 8) | data[offset + 6]);
                printf("  Data Bytes per Sector: %u\n", (data[offset + 7] << 8) | data[offset + 8]);
                // More fields can be added
            } else if (page_code == 0x0B) {  // Medium Types Supported Page (vendor-extended)
                printf("  Medium Types Supported Page (Vendor-Specific Interpretation):\n");
                // Standard fields (first 4 types, but extended)
                printf("  Standard Supported Medium Types:\n");
                printf("    Type 1: 0x%02x\n", data[offset + 2]);
                printf("    Type 2: 0x%02x\n", data[offset + 3]);
                printf("    Type 3: 0x%02x\n", data[offset + 4]);
                printf("    Type 4: 0x%02x\n", data[offset + 5]);

                // Parse extended data as 8-byte entries: [00 00 type 00 00 00 cap_hi cap_lo] ...
                // Starting from byte 0 of page data
                int entry_offset = offset + 2;  // Start of page data
                int remaining = page_len;
                int entry_num = 1;
                while (remaining >= 8) {
                    unsigned char entry[8];
                    memcpy(entry, &data[entry_offset], 8);
                    unsigned int cap = (entry[6] << 8) | entry[7];
                    if (cap > 0) {  // Only print non-zero capacities
                        unsigned char type = entry[2];
                        printf("  Supported Format %d:\n", entry_num);
                        printf("    Medium Type Code: 0x%02x\n", type);
                        printf("    Capacity: %u KB\n", cap);
                    }
                    entry_offset += 8;
                    remaining -= 8;
                    entry_num++;
                }
            } else if (page_code == 0x20) {  // Vendor-Specific Page
                printf("  Vendor-Specific Page (0x20):\n");
                char product_info[33];
                memcpy(product_info, &data[offset + 2], 32);
                product_info[32] = '\0';
                // Trim trailing spaces and nulls
                for (int i = 31; i >= 0; i--) {
                    if (product_info[i] == '\0' || product_info[i] == ' ') {
                        product_info[i] = '\0';
                    } else {
                        break;
                    }
                }
                printf("  Product Info: '%s'\n", product_info);
            }
        }
        offset += page_len + 2;
    }
}

void print_sense_data(unsigned char *data, int len) {
    if (len < 14) return;

    int sense_key = data[2] & 0x0F;
    int asc = data[12];
    int ascq = data[13];

    printf("Sense Key: 0x%02x\n", sense_key);
    printf("Additional Sense Code: 0x%02x\n", asc);
    printf("Additional Sense Code Qualifier: 0x%02x\n", ascq);

    // Basic interpretation
    if (sense_key == 0x02 && asc == 0x3A) {
        printf("Interpretation: Medium Not Present\n");
    } else if (sense_key == 0x06 && asc == 0x28) {
        printf("Interpretation: Not Ready to Ready Transition\n");
    } else {
        printf("Interpretation: Unknown\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s /dev/sgX\n", argv[0]);
        return 1;
    }

    const char *device = argv[1];
    int fd = open(device, O_RDWR | O_NONBLOCK);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    unsigned char data[MAX_DATA_LEN];
    int ret;

    printf("Querying USB Floppy Device: %s\n\n", device);

    // TEST UNIT READY
    unsigned char tur_cdb[6] = {0x00, 0, 0, 0, 0, 0};
    ret = send_ufi_command(fd, tur_cdb, 6, NULL, 0, 0);
    int media_ready = (ret == 0);
    printf("Test Unit Ready: %s\n", media_ready ? "Ready" : "Not Ready");

    if (!media_ready) {
        // REQUEST SENSE
        unsigned char req_sense_cdb[6] = {0x03, 0, 0, 0, 18, 0};  // Alloc 18
        memset(data, 0, sizeof(data));
        ret = send_ufi_command(fd, req_sense_cdb, 6, data, 18, 0);
        if (ret >= 0) {
            printf("\nRequest Sense Data:\n");
            print_sense_data(data, ret);
        }
    }

    // INQUIRY
    unsigned char inquiry_cdb[6] = {0x12, 0, 0, 0, 96, 0};
    memset(data, 0, sizeof(data));
    ret = send_ufi_command(fd, inquiry_cdb, 6, data, 96, 0);
    if (ret >= 0) {
        printf("\nInquiry Data:\n");
        print_inquiry_data(data);
    }

    // READ FORMAT CAPACITIES
    unsigned char fmt_cap_cdb[10] = {0x23, 0, 0, 0, 0, 0, 0, (255 >> 8) & 0xFF, 255 & 0xFF, 0};
    memset(data, 0, sizeof(data));
    ret = send_ufi_command(fd, fmt_cap_cdb, 10, data, 255, 0);
    if (ret >= 0) {
        printf("\nFormat Capacities:\n");
        print_format_capacities(data, ret);
    }

    if (media_ready) {
        // READ CAPACITY
        unsigned char read_cap_cdb[10] = {0x25, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        memset(data, 0, sizeof(data));
        ret = send_ufi_command(fd, read_cap_cdb, 10, data, 8, 0);
        if (ret >= 0) {
            printf("\nRead Capacity:\n");
            print_read_capacity(data);
        }
    }

    // MODE SENSE (10-byte)
    unsigned char mode_sense_cdb[10] = {0x5A, 0, 0x3F, 0, 0, 0, 0, (255 >> 8) & 0xFF, 255 & 0xFF, 0};  // PC=0, All pages
    memset(data, 0, sizeof(data));
    ret = send_ufi_command(fd, mode_sense_cdb, 10, data, 255, 0);
    if (ret >= 0) {
        printf("\nMode Sense Data (All Pages):\n");
        print_mode_sense_data(data, ret);
    }

    close(fd);
    return 0;
}
