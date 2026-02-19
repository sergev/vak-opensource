#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

// USB Mass Storage BOT structures
typedef struct {
    uint32_t dCBWSignature;          // 0x43425355 ('USBC')
    uint32_t dCBWTag;                // Arbitrary tag
    uint32_t dCBWDataTransferLength; // Data length
    uint8_t bmCBWFlags;              // 0x80 for IN, 0x00 for OUT
    uint8_t bCBWLUN;                 // LUN 0
    uint8_t bCBWCBLength;            // CDB length (6 or 10)
    uint8_t CBWCB[16];               // CDB (padded to 16)
} __attribute__((packed)) CBW_t;

typedef struct {
    uint32_t dCSWSignature;  // 0x53425355 ('USBS')
    uint32_t dCSWTag;        // Matches CBW tag
    uint32_t dCSWDataResidue;// Remaining data
    uint8_t bCSWStatus;      // 0=Passed, 1=Failed, 2=Phase Error
} __attribute__((packed)) CSW_t;

// Function to send SCSI command
int send_scsi_command(libusb_device_handle *handle, uint8_t ep_out, uint8_t ep_in,
                      uint8_t *cdb, uint8_t cdb_len, uint8_t *data, uint32_t data_len,
                      uint8_t direction) {  // direction: 0x80 IN, 0x00 OUT
    CBW_t cbw;
    memset(&cbw, 0, sizeof(cbw));
    cbw.dCBWSignature = 0x43425355;
    cbw.dCBWTag = rand();  // Simple arbitrary tag
    cbw.dCBWDataTransferLength = data_len;
    cbw.bmCBWFlags = direction;
    cbw.bCBWLUN = 0;
    cbw.bCBWCBLength = cdb_len;
    memcpy(cbw.CBWCB, cdb, cdb_len);

    int transferred;
    int r = libusb_bulk_transfer(handle, ep_out, (unsigned char*)&cbw, 31, &transferred, 5000);
    if (r < 0 || transferred != 31) {
        printf("CBW send error: %d\n", r);
        return -1;
    }

    if (data_len > 0) {
        if (direction == 0x00) {  // OUT
            r = libusb_bulk_transfer(handle, ep_out, data, data_len, &transferred, 5000);
        } else {  // IN
            r = libusb_bulk_transfer(handle, ep_in, data, data_len, &transferred, 5000);
        }
        if (r < 0 || transferred != (int)data_len) {
            printf("Data transfer error: %d (transferred %d)\n", r, transferred);
            return -1;
        }
    }

    CSW_t csw;
    r = libusb_bulk_transfer(handle, ep_in, (unsigned char*)&csw, 13, &transferred, 5000);
    if (r < 0 || transferred != 13) {
        printf("CSW recv error: %d\n", r);
        return -1;
    }
    if (csw.dCSWSignature != 0x53425355 || csw.dCSWTag != cbw.dCBWTag || csw.bCSWStatus != 0) {
        printf("Command failed, status: %d\n", csw.bCSWStatus);
        return -1;
    }

    return 0;
}

// USB Mass Storage BOT Reset Recovery (spec 5.3.4)
// Some devices (e.g. TEAC floppy) stall the 0xFF request (LIBUSB_ERROR_PIPE -9); skip it and only clear halt.
static int ms_reset_recovery(libusb_device_handle *handle, uint8_t ep_in, uint8_t ep_out) {
    // (a) Bulk-Only Mass Storage Reset: bmRequestType=0x21, bRequest=0xFF (optional; device may stall)
    int r = libusb_control_transfer(handle,
        LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
        0xFF, 0, 0, NULL, 0, 5000);
    if (r < 0 && r != LIBUSB_ERROR_PIPE) return r;  /* device may stall 0xFF (not supported) */

    // (b) Clear HALT on Bulk-In
    r = libusb_clear_halt(handle, ep_in);
    if (r < 0) return r;

    // (c) Clear HALT on Bulk-Out
    r = libusb_clear_halt(handle, ep_out);
    if (r < 0) return r;

    return 0;
}

// Helper to get big-endian uint32
uint32_t be32_to_cpu(uint8_t *buf) {
    return (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
}

// Helper to get big-endian uint16
uint16_t be16_to_cpu(uint8_t *buf) {
    return (buf[0] << 8) | buf[1];
}

int main() {
    libusb_context *ctx = NULL;
    libusb_device_handle *handle = NULL;
    libusb_device *dev = NULL;
    struct libusb_config_descriptor *config = NULL;
    int r;

    srand(time(NULL));  // For random tag

    r = libusb_init(&ctx);
    if (r < 0) { printf("Init error: %d\n", r); return 1; }

    // Open TEAC device (VID:PID 0x0644:0x0000)
    handle = libusb_open_device_with_vid_pid(ctx, 0x0644, 0x0000);
    if (!handle) { printf("Unable to open device\n"); goto cleanup; }
    dev = libusb_get_device(handle);

    // Get config descriptor to find endpoints
    r = libusb_get_config_descriptor(dev, 0, &config);
    if (r < 0) { printf("Config desc error: %d\n", r); goto cleanup; }

    uint8_t ep_out = 0, ep_in = 0;
    const struct libusb_interface_descriptor *intf = &config->interface[0].altsetting[0];
    for (int i = 0; i < intf->bNumEndpoints; i++) {
        uint8_t addr = intf->endpoint[i].bEndpointAddress;
        if (addr & LIBUSB_ENDPOINT_IN) {
            ep_in = addr;
        } else {
            ep_out = addr;
        }
    }
    if (ep_out == 0 || ep_in == 0) { printf("Endpoints not found\n"); goto cleanup; }
    libusb_free_config_descriptor(config);
    config = NULL;

    // Detach kernel driver if active (Linux-specific)
    if (libusb_kernel_driver_active(handle, 0) == 1) {
        r = libusb_detach_kernel_driver(handle, 0);
        if (r < 0) { printf("Detach error: %d\n", r); goto cleanup; }
    }

    // Claim interface 0
    r = libusb_claim_interface(handle, 0);
    if (r < 0) { printf("Claim error: %d\n", r); goto cleanup; }

    r = ms_reset_recovery(handle, ep_in, ep_out);
    if (r < 0) { printf("Reset recovery error: %d\n", r); goto cleanup; }

    printf("=== TEAC USB Floppy Drive Information ===\n");

    // INQUIRY
    uint8_t inq_cdb[6] = {0x12, 0, 0, 0, 36, 0};
    uint8_t inq_data[36] = {0};
    if (send_scsi_command(handle, ep_out, ep_in, inq_cdb, 6, inq_data, 36, 0x80) == 0) {
        printf("Vendor: %.*s\n", 8, inq_data + 8);
        printf("Product: %.*s\n", 16, inq_data + 16);
        printf("Revision: %.*s\n", 4, inq_data + 32);
        printf("Removable Media: %s\n", (inq_data[1] & 0x80) ? "Yes" : "No");
    } else {
        printf("INQUIRY failed\n");
    }

    // TEST UNIT READY
    uint8_t tur_cdb[6] = {0x00, 0, 0, 0, 0, 0};
    int media_ready = (send_scsi_command(handle, ep_out, ep_in, tur_cdb, 6, NULL, 0, 0) == 0);

    if (!media_ready) {
        // REQUEST SENSE
        uint8_t sense_cdb[6] = {0x03, 0, 0, 0, 18, 0};
        uint8_t sense_data[18] = {0};
        if (send_scsi_command(handle, ep_out, ep_in, sense_cdb, 6, sense_data, 18, 0x80) == 0) {
            uint8_t sense_key = sense_data[2] & 0x0F;
            uint8_t asc = sense_data[12];
            uint8_t ascq = sense_data[13];
            printf("Media Status: Not Ready (Sense Key: 0x%02X, ASC: 0x%02X, ASCQ: 0x%02X)\n", sense_key, asc, ascq);
            if (asc == 0x3A) {
                printf(" - No medium inserted.\n");
            }
        } else {
            printf("REQUEST SENSE failed\n");
        }
    } else {
        printf("Media Status: Ready\n");

        // READ CAPACITY
        uint8_t cap_cdb[10] = {0x25, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        uint8_t cap_data[8] = {0};
        if (send_scsi_command(handle, ep_out, ep_in, cap_cdb, 10, cap_data, 8, 0x80) == 0) {
            uint32_t last_lba = be32_to_cpu(cap_data);
            uint32_t block_len = be32_to_cpu(cap_data + 4);
            uint64_t capacity_kb = ((uint64_t)(last_lba + 1) * block_len) / 1024;
            printf("Current Capacity: %u blocks of %u bytes each (%llu KB)\n", last_lba + 1, block_len, capacity_kb);
        } else {
            printf("READ CAPACITY failed\n");
        }
    }

    // READ FORMAT CAPACITIES (always query, even if no media)
    uint8_t fmt_cdb[10] = {0x23, 0, 0, 0, 0, 0, 0, 0, 255, 0};  // Alloc len 255
    uint8_t fmt_data[255] = {0};
    if (send_scsi_command(handle, ep_out, ep_in, fmt_cdb, 10, fmt_data, 255, 0x80) == 0) {
        uint8_t list_len = fmt_data[3];
        // Current/Max Descriptor
        uint32_t num_blocks = be32_to_cpu(fmt_data + 4);
        uint8_t desc_code = fmt_data[8] & 0x03;
        uint32_t block_len = (fmt_data[9] << 16) | (fmt_data[10] << 8) | fmt_data[11];
        const char *desc_type;
        switch (desc_code) {
            case 0x01: desc_type = "Unformatted Media (Max Capacity)"; break;
            case 0x02: desc_type = "Formatted Media (Current Capacity)"; break;
            case 0x03: desc_type = "No Media (Max Capacity)"; break;
            default: desc_type = "Reserved"; break;
        }
        printf("Capacity Descriptor: %s - %u blocks of %u bytes\n", desc_type, num_blocks, block_len);

        // Formattable Descriptors (starting at byte 12)
        int num_fmt = (list_len - 8) / 8;
        printf("Supported Formats (%d):\n", num_fmt);
        for (int i = 0; i < num_fmt; i++) {
            uint8_t *desc = fmt_data + 12 + i * 8;
            uint32_t fmt_blocks = be32_to_cpu(desc);
            uint32_t fmt_len = (desc[5] << 16) | (desc[6] << 8) | desc[7];
            const char *fmt_name = "Unknown";
            if (fmt_blocks == 1440 && fmt_len == 512) fmt_name = "720 KB (DD)";
            else if (fmt_blocks == 1232 && fmt_len == 1024) fmt_name = "1.25 MB";
            else if (fmt_blocks == 2880 && fmt_len == 512) fmt_name = "1.44 MB (HD)";
            printf(" - %u blocks of %u bytes (%s)\n", fmt_blocks, fmt_len, fmt_name);
        }
    } else {
        printf("READ FORMAT CAPACITIES failed\n");
    }

    // MODE SENSE(6) for Flexible Disk Page (0x05)
    uint8_t mode_cdb[6] = {0x1A, 0, 0x05, 0, 32, 0};  // PC=0 (current), alloc 32
    uint8_t mode_data[32] = {0};
    if (send_scsi_command(handle, ep_out, ep_in, mode_cdb, 6, mode_data, 32, 0x80) == 0) {
        // Header (4 bytes for MODE SENSE(6))
        uint8_t med_type = mode_data[1];
        uint8_t wp = (mode_data[2] & 0x80) ? 1 : 0;
        // Page starts at byte 4
        uint16_t transfer_rate = be16_to_cpu(mode_data + 4 + 2);
        uint8_t heads = mode_data[4 + 4];
        uint8_t sec_per_track = mode_data[4 + 5];
        uint16_t bytes_per_sec = be16_to_cpu(mode_data + 4 + 6);
        uint16_t cylinders = be16_to_cpu(mode_data + 4 + 8);
        uint16_t med_rotation = be16_to_cpu(mode_data + 4 + 28);

        printf("Flexible Disk Parameters:\n");
        printf(" - Medium Type: 0x%02X (%s)\n", med_type, wp ? "Write-Protected" : "Writable");
        printf(" - Transfer Rate: %u kbit/s\n", transfer_rate);
        printf(" - Heads: %u\n", heads);
        printf(" - Sectors per Track: %u\n", sec_per_track);
        printf(" - Bytes per Sector: %u\n", bytes_per_sec);
        printf(" - Cylinders (Tracks): %u\n", cylinders);
        printf(" - Medium Rotation Rate: %u RPM\n", med_rotation);
    } else {
        printf("MODE SENSE failed\n");
    }

cleanup:
    if (config) libusb_free_config_descriptor(config);
    if (handle) {
        libusb_release_interface(handle, 0);
        libusb_close(handle);
    }
    libusb_exit(ctx);
    return 0;
}
