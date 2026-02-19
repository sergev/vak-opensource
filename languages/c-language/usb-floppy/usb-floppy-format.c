/*
 * usb-floppy-format - Low-level format USB floppy disks on Linux (ufiformat-like)
 *
 * Uses SCSI generic (/dev/sgX) and SG_IO with UFI command set.
 * Requires: modprobe sg; device e.g. /dev/sg0.
 * Does not create filesystems; use mkfs.msdos (or mkfs.vfat) after formatting.
 *
 * Build: cc -Wall -Wextra -o usb-floppy-format usb-floppy-format.c
 */

#define _DEFAULT_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <scsi/sg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define SENSE_LEN 32
#define UFI_CDB_LEN 12
#define FORMAT_TIMEOUT_MS 120000

static const char *progname;

static uint32_t be32_from(const uint8_t *p)
{
	return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 |
	       (uint32_t)p[2] << 8 | p[3];
}

static void be32_to(uint8_t *p, uint32_t v)
{
	p[0] = (v >> 24) & 0xff;
	p[1] = (v >> 16) & 0xff;
	p[2] = (v >> 8) & 0xff;
	p[3] = v & 0xff;
}

static int sg_io(int fd, uint8_t *cdb, int cdb_len,
                 void *dxferp, unsigned int dxfer_len, int direction,
                 uint8_t *sense, unsigned int timeout_ms)
{
	struct sg_io_hdr hdr;
	memset(&hdr, 0, sizeof(hdr));
	hdr.interface_id = 'S';
	hdr.dxfer_direction = direction;
	hdr.cmd_len = cdb_len;
	hdr.cmdp = cdb;
	if (sense) {
		hdr.mx_sb_len = SENSE_LEN;
		hdr.sbp = sense;
	} else {
		hdr.mx_sb_len = 0;
		hdr.sbp = NULL;
	}
	hdr.dxfer_len = dxfer_len;
	hdr.dxferp = dxferp;
	hdr.timeout = timeout_ms;

	int r = ioctl(fd, SG_IO, &hdr);
	if (r < 0)
		return -1;
	if (hdr.host_status || hdr.driver_status)
		return -1;
	if (hdr.status == 0)
		return 0;
	if (hdr.status == 2) /* CHECK CONDITION */
		return -2;
	return -1;
}

static int do_inquiry(int fd, uint8_t *out, size_t len)
{
	uint8_t cdb[UFI_CDB_LEN] = { 0 };
	cdb[0] = 0x12; /* INQUIRY */
	/* UFI INQUIRY: Allocation Length is byte 4 (1 byte) */
	cdb[4] = (len > 0xff) ? 0xff : (uint8_t)len;
	return sg_io(fd, cdb, UFI_CDB_LEN, out, len, SG_DXFER_FROM_DEV, NULL, 5000);
}

static int do_test_unit_ready(int fd)
{
	uint8_t cdb[UFI_CDB_LEN] = { 0 };
	cdb[0] = 0x00; /* TEST UNIT READY */
	return sg_io(fd, cdb, UFI_CDB_LEN, NULL, 0, SG_DXFER_NONE, NULL, 5000);
}

static int do_request_sense(int fd, uint8_t *out, size_t len)
{
	uint8_t cdb[UFI_CDB_LEN] = { 0 };
	cdb[0] = 0x03; /* REQUEST SENSE */
	/* UFI REQUEST SENSE: Allocation Length is byte 4 (1 byte) */
	cdb[4] = (len > 0xff) ? 0xff : (uint8_t)len;
	return sg_io(fd, cdb, UFI_CDB_LEN, out, len, SG_DXFER_FROM_DEV, NULL, 5000);
}

static int do_read_format_capacities(int fd, uint8_t *out, size_t len)
{
	uint8_t cdb[UFI_CDB_LEN] = { 0 };
	cdb[0] = 0x23; /* READ FORMAT CAPACITIES */
	/* UFI READ FORMAT CAPACITIES: Allocation Length is bytes 7-8 */
	cdb[7] = (len >> 8) & 0xff;
	cdb[8] = len & 0xff;
	return sg_io(fd, cdb, UFI_CDB_LEN, out, len, SG_DXFER_FROM_DEV, NULL, 10000);
}

static int do_mode_sense(int fd, uint8_t page, uint8_t *out, size_t len)
{
	uint8_t cdb[UFI_CDB_LEN] = { 0 };
	cdb[0] = 0x5a; /* MODE SENSE (UFI 12-byte command block) */
	cdb[2] = page;
	/* UFI MODE SENSE: Parameter List Length is bytes 7-8 */
	cdb[7] = (len >> 8) & 0xff;
	cdb[8] = len & 0xff;
	return sg_io(fd, cdb, UFI_CDB_LEN, out, len, SG_DXFER_FROM_DEV, NULL, 5000);
}

static int do_format_unit(int fd, uint32_t num_blocks, uint32_t block_len)
{
	uint8_t cdb[UFI_CDB_LEN] = { 0 };
	/* FORMAT UNIT: FmtData=1, CmpList=0, Defect List Format=7 */
	cdb[0] = 0x04;
	cdb[1] = 0x10 | 7; /* FmtData bit, Defect List Format 7 */
	cdb[2] = 0;       /* Track 0 (full disk) */
	cdb[3] = 0;
	cdb[4] = 0;       /* Interleave 0 = default */
	cdb[7] = 0;
	cdb[8] = 12;      /* Parameter list length */

	uint8_t param[12];
	memset(param, 0, sizeof(param));
	param[1] = 0x08;  /* DCRT = 1 (disable certification) */
	param[2] = 0;
	param[3] = 8;     /* Defect list length = 8 (Format Descriptor size) */
	be32_to(param + 4, num_blocks);
	/* param[8] reserved */
	param[9] = (block_len >> 16) & 0xff;
	param[10] = (block_len >> 8) & 0xff;
	param[11] = block_len & 0xff;

	return sg_io(fd, cdb, UFI_CDB_LEN, param, 12, SG_DXFER_TO_DEV, NULL, FORMAT_TIMEOUT_MS);
}

static int do_verify(int fd, uint32_t lba, uint32_t num_blocks)
{
	uint8_t cdb[UFI_CDB_LEN] = { 0 };
	cdb[0] = 0x2f; /* VERIFY */
	cdb[2] = (lba >> 24) & 0xff;
	cdb[3] = (lba >> 16) & 0xff;
	cdb[4] = (lba >> 8) & 0xff;
	cdb[5] = lba & 0xff;
	cdb[7] = (num_blocks >> 8) & 0xff;
	cdb[8] = num_blocks & 0xff;
	return sg_io(fd, cdb, UFI_CDB_LEN, NULL, 0, SG_DXFER_NONE, NULL, 60000);
}

static void sense_message(uint8_t *sense, size_t len, char *buf, size_t bufsiz)
{
	if (len < 14) {
		snprintf(buf, bufsiz, "Sense key 0x%02X", len >= 2 ? sense[2] & 0x0f : 0);
		return;
	}
	uint8_t sk = sense[2] & 0x0f;
	uint8_t asc = sense[12];
	uint8_t ascq = sense[13];
	const char *sk_str = "Unknown";
	const char *asc_str = "";
	if (sk == 0x02) sk_str = "Not Ready";
	else if (sk == 0x05) sk_str = "Illegal Request";
	else if (sk == 0x07) sk_str = "Data Protect";
	else if (sk == 0x03) sk_str = "Medium Error";
	if (asc == 0x3a) asc_str = " (no medium)";
	else if (asc == 0x24 && ascq == 0) asc_str = " (invalid format)";
	else if (asc == 0x27 && ascq == 0) asc_str = " (write protected)";
	snprintf(buf, bufsiz, "%s ASC 0x%02X ASCQ 0x%02X%s", sk_str, asc, ascq, asc_str);
}

static int infer_geometry(uint8_t medium_type, uint32_t num_blocks, uint32_t block_len,
                          uint16_t *cylinders, uint8_t *heads, uint8_t *sec_per_track, uint16_t *bytes_per_sec)
{
	/* Prefer medium type codes when available (UFI Table 17 / Table 35). */
	switch (medium_type) {
	case 0x1e: /* 720 KB (DD) */
		*cylinders = 80; *heads = 2; *sec_per_track = 9; *bytes_per_sec = 512; return 1;
	case 0x93: /* 1.25 MB (HD) */
		*cylinders = 77; *heads = 2; *sec_per_track = 8; *bytes_per_sec = 1024; return 1;
	case 0x94: /* 1.44 MB (HD) */
		*cylinders = 80; *heads = 2; *sec_per_track = 18; *bytes_per_sec = 512; return 1;
	default:
		break;
	}

	/* Fallback to common floppy geometries inferred from current capacity. */
	if (num_blocks == 1440 && block_len == 512) { *cylinders = 80; *heads = 2; *sec_per_track = 9;  *bytes_per_sec = 512;  return 1; }
	if (num_blocks == 2880 && block_len == 512) { *cylinders = 80; *heads = 2; *sec_per_track = 18; *bytes_per_sec = 512;  return 1; }
	if (num_blocks == 1232 && block_len == 1024){ *cylinders = 77; *heads = 2; *sec_per_track = 8;  *bytes_per_sec = 1024; return 1; }
	if (num_blocks == 2400 && block_len == 512) { *cylinders = 80; *heads = 2; *sec_per_track = 15; *bytes_per_sec = 512;  return 1; }

	return 0;
}

static int run_format(int fd, unsigned int capacity_kb, int force, int verify,
                      uint8_t *fmt_buf, size_t fmt_len)
{
	uint8_t sense[SENSE_LEN];
	int r;

	r = do_test_unit_ready(fd);
	if (r != 0) {
		if (r == -2) {
			do_request_sense(fd, sense, SENSE_LEN);
			char msg[128];
			sense_message(sense, SENSE_LEN, msg, sizeof(msg));
			fprintf(stderr, "%s: Media not ready: %s\n", progname, msg);
		} else
			fprintf(stderr, "%s: TEST UNIT READY failed: %s\n", progname, strerror(errno));
		return 2;
	}

	uint8_t mode_buf[64];
	memset(mode_buf, 0, sizeof(mode_buf));
	r = do_mode_sense(fd, 0x05, mode_buf, sizeof(mode_buf));
	if (r == 0 && sizeof(mode_buf) >= 8) {
		/* Mode Parameter Header (Table 16) byte 3 = WP */
		int wp = (mode_buf[3] & 0x80) ? 1 : 0;
		if (wp) {
			fprintf(stderr, "%s: Medium is write-protected.\n", progname);
			return 3;
		}
	}

	if (!force) {
		printf("Format will erase all data. Continue? [y/N] ");
		fflush(stdout);
		char line[32];
		if (!fgets(line, sizeof(line), stdin) || (line[0] != 'y' && line[0] != 'Y')) {
			printf("Aborted.\n");
			return 1;
		}
	}

	uint8_t list_len = fmt_len > 3 ? fmt_buf[3] : 0;
	int num_fmt = list_len >= 8 ? (list_len - 8) / 8 : 0;
	uint32_t num_blocks = 0, block_len = 0;
	for (int i = 0; i < num_fmt; i++) {
		uint8_t *d = fmt_buf + 12 + i * 8;
		uint32_t nb = be32_from(d);
		uint32_t bl = (d[5] << 16) | (d[6] << 8) | d[7];
		unsigned int cap_kb = (unsigned int)((uint64_t)nb * bl / 1024);
		if (cap_kb == capacity_kb) {
			num_blocks = nb;
			block_len = bl;
			break;
		}
	}
	if (num_blocks == 0 || block_len == 0) {
		fprintf(stderr, "%s: Capacity %u KB is not supported by the drive/media.\n",
			progname, capacity_kb);
		return 1;
	}

	memset(sense, 0, SENSE_LEN);
	r = do_format_unit(fd, num_blocks, block_len);
	if (r != 0) {
		do_request_sense(fd, sense, SENSE_LEN);
		char msg[128];
		sense_message(sense, SENSE_LEN, msg, sizeof(msg));
		fprintf(stderr, "%s: FORMAT UNIT failed: %s\n", progname, msg);
		return 1;
	}
	printf("Format completed successfully.\n");

	if (verify) {
		r = do_verify(fd, 0, num_blocks);
		if (r != 0) {
			do_request_sense(fd, sense, SENSE_LEN);
			char msg[128];
			sense_message(sense, SENSE_LEN, msg, sizeof(msg));
			fprintf(stderr, "%s: VERIFY failed: %s\n", progname, msg);
			return 1;
		}
		printf("Verify completed successfully.\n");
	}
	return 0;
}

static void print_info(int fd, uint8_t *inq, size_t inq_len,
                      uint8_t *fmt_buf, size_t fmt_len)
{
	if (inq_len >= 36) {
		printf("Vendor:   %.8s\n", inq + 8);
		printf("Product:  %.16s\n", inq + 16);
		printf("Revision: %.4s\n", inq + 32);
		printf("Removable: %s\n", (inq[1] & 0x80) ? "Yes" : "No");
	}

	printf("\nMedia: ");
	if (do_test_unit_ready(fd) != 0) {
		uint8_t sense[SENSE_LEN];
		do_request_sense(fd, sense, SENSE_LEN);
		char msg[128];
		sense_message(sense, SENSE_LEN, msg, sizeof(msg));
		printf("Not ready (%s)\n", msg);
	} else {
		printf("Ready\n");

		/* Use current capacity as a fallback for geometry if MODE SENSE doesn't include page 0x05. */
		uint32_t cur_blocks = 0;
		uint32_t cur_block_len = 0;
		if (fmt_len >= 12) {
			cur_blocks = be32_from(fmt_buf + 4);
			cur_block_len = (fmt_buf[9] << 16) | (fmt_buf[10] << 8) | fmt_buf[11];
		}

		uint8_t mode_buf[64];
		memset(mode_buf, 0, sizeof(mode_buf));
		if (do_mode_sense(fd, 0x05, mode_buf, sizeof(mode_buf)) == 0) {
			/* UFI MODE SENSE header is 8 bytes (Mode Data Length at bytes 0-1). */
			size_t avail = sizeof(mode_buf);
			if (avail >= 2) {
				size_t mdl = ((size_t)mode_buf[0] << 8) | (size_t)mode_buf[1];
				size_t total = mdl + 2; /* not including itself */
				if (total < avail)
					avail = total;
			}

			if (avail >= 8) {
				uint8_t medium_type = mode_buf[2];
				int wp = (mode_buf[3] & 0x80) ? 1 : 0;
				printf("Write protect: %s\n", wp ? "Yes" : "No");

				/* Parse mode pages starting immediately after the 8-byte header. */
				int printed_geom = 0;
				size_t off = 8;
				while (off + 2 <= avail) {
					uint8_t page_code = mode_buf[off] & 0x3f;
					uint8_t page_len = mode_buf[off + 1];
					size_t page_total = (size_t)page_len + 2;
					if (off + page_total > avail)
						break;

					if (page_code == 0x05 && page_len >= 10) {
						/* Flexible Disk Page (UFI Table 19): heads@4, sec/trk@5, bytes/sec@6-7, cyl@8-9 */
						uint8_t heads = mode_buf[off + 4];
						uint8_t sec_per_track = mode_buf[off + 5];
						uint16_t bytes_per_sec = ((uint16_t)mode_buf[off + 6] << 8) | mode_buf[off + 7];
						uint16_t cylinders = ((uint16_t)mode_buf[off + 8] << 8) | mode_buf[off + 9];
						printf("Geometry: %u cylinders, %u heads, %u sectors/track, %u bytes/sector\n",
						       (unsigned)cylinders, (unsigned)heads, (unsigned)sec_per_track, (unsigned)bytes_per_sec);
						printed_geom = 1;
						break;
					}
					off += page_total;
				}

				if (!printed_geom) {
					uint16_t cylinders = 0, bytes_per_sec = 0;
					uint8_t heads = 0, sec_per_track = 0;
					if (infer_geometry(medium_type, cur_blocks, cur_block_len,
					                   &cylinders, &heads, &sec_per_track, &bytes_per_sec)) {
						printf("Geometry: %u cylinders, %u heads, %u sectors/track, %u bytes/sector\n",
						       (unsigned)cylinders, (unsigned)heads, (unsigned)sec_per_track, (unsigned)bytes_per_sec);
					}
				}
			}
		}
	}

	if (fmt_len < 4) return;
	uint8_t list_len = fmt_buf[3];
	uint32_t num_blocks = be32_from(fmt_buf + 4);
	uint8_t desc_code = fmt_buf[8] & 0x03;
	uint32_t block_len = (fmt_buf[9] << 16) | (fmt_buf[10] << 8) | fmt_buf[11];
	const char *desc_type = "Reserved";
	switch (desc_code) {
	case 0x01: desc_type = "Unformatted (max)"; break;
	case 0x02: desc_type = "Formatted (current)"; break;
	case 0x03: desc_type = "No media (max)"; break;
	}
	printf("\nCapacity: %s - %u blocks x %u bytes\n", desc_type, (unsigned)num_blocks, (unsigned)block_len);
	int num_fmt = list_len >= 8 ? (list_len - 8) / 8 : 0;
	printf("Supported formats:\n");
	for (int i = 0; i < num_fmt; i++) {
		uint8_t *d = fmt_buf + 12 + i * 8;
		uint32_t nb = be32_from(d);
		uint32_t bl = (d[5] << 16) | (d[6] << 8) | d[7];
		unsigned int cap_kb = (unsigned int)((uint64_t)nb * bl / 1024);
		const char *name = "Unknown";
		if (nb == 1440 && bl == 512) name = "720 KB";
		else if (nb == 2880 && bl == 512) name = "1.44 MB";
		else if (nb == 1232 && bl == 1024) name = "1.25 MB";
		else if (nb == 2400 && bl == 512) name = "1.2 MB";
		else if (nb == 640 && bl == 512) name = "640 KB";
		printf("  - %u KB: %u blocks x %u bytes (%s)\n", cap_kb, (unsigned)nb, (unsigned)bl, name);
	}
}

static void usage(void)
{
	printf("Usage: %s [options] <device>\n", progname);
	printf("Low-level format USB floppy disks (UFI over SCSI generic).\n");
	printf("\n");
	printf("Options:\n");
	printf("  -i              Info only: show device and supported formats (no format)\n");
	printf("  -f <capacity>   Format to capacity in KB (e.g. 1440, 720)\n");
	printf("  -F              Force: skip confirmation prompt\n");
	printf("  -V              Verify medium after format\n");
	printf("  -h, --help      Show this help\n");
	printf("\n");
	printf("Device: /dev/sgN (e.g. /dev/sg0). Load sg module: modprobe sg\n");
	printf("After low-level format, create a filesystem: mkfs.msdos /dev/fd0\n");
}

int main(int argc, char **argv)
{
	int opt;
	int info_only = 0;
	int force = 0;
	int verify = 0;
	unsigned int capacity_kb = 0;
	const char *device = NULL;

	progname = argv[0] ? argv[0] : "usb-floppy-format";

	static struct option longopts[] = {
		{ "help", no_argument, NULL, 'h' },
		{ NULL, 0, NULL, 0 }
	};
	while ((opt = getopt_long(argc, argv, "if:FVh", longopts, NULL)) != -1) {
		switch (opt) {
		case 'i': info_only = 1; break;
		case 'f': capacity_kb = (unsigned int)atoi(optarg); break;
		case 'F': force = 1; break;
		case 'V': verify = 1; break;
		case 'h':
		default:
			usage();
			return opt == 'h' ? 0 : 1;
		}
	}
	if (optind >= argc) {
		fprintf(stderr, "%s: missing device\n", progname);
		usage();
		return 1;
	}
	device = argv[optind];

	int fd = open(device, O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "%s: open %s: %s\n", progname, device, strerror(errno));
		return 1;
	}

	uint8_t inq[36];
	uint8_t fmt_buf[256];
	memset(inq, 0, sizeof(inq));
	memset(fmt_buf, 0, sizeof(fmt_buf));
	int r = do_inquiry(fd, inq, sizeof(inq));
	if (r != 0) {
		fprintf(stderr, "%s: INQUIRY failed: %s\n", progname, strerror(errno));
		close(fd);
		return 1;
	}
	r = do_read_format_capacities(fd, fmt_buf, sizeof(fmt_buf));
	if (r != 0) {
		fprintf(stderr, "%s: READ FORMAT CAPACITIES failed\n", progname);
		close(fd);
		return 1;
	}

	if (info_only || capacity_kb == 0) {
		print_info(fd, inq, sizeof(inq), fmt_buf, sizeof(fmt_buf));
		close(fd);
		return 0;
	}

	r = run_format(fd, capacity_kb, force, verify, fmt_buf, sizeof(fmt_buf));
	close(fd);
	return r;
}
