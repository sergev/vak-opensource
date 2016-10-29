/*
 * Implementing STK500/AVRISP protocol.
 * Documented in AVR068 application note.
 */
#include <stdio.h>
#include <stdlib.h>
#ifndef WIN32
#   include <unistd.h>
#endif
#include <fcntl.h>
#include <string.h>
#include "avr.h"
#include "stk500.h"

#if defined(__WIN32__) || defined(WIN32)
#   include <winsock.h>
#else
#   include <sys/ioctl.h>
#   include <termios.h>
#   include <errno.h>
#endif

extern int debug;

struct _avr_t {
	char		*name;
	unsigned char	sequence_number;
	unsigned char	have_fuse;
//	unsigned char	have_checksum_cmd;
//	unsigned char	vendor_code;
//	unsigned char	part_family;
//	unsigned char	part_number;
	unsigned char	stk_flag;
//	unsigned char	hardware_version;
//	unsigned char	software_major;
//	unsigned char	software_minor;
//	unsigned char	sck_duration;
//	unsigned char	topcard;
//	unsigned char	target_voltage;
//	unsigned char	aref_voltage;
//	unsigned char	osc_pscale;
//	unsigned char	osc_cmatch;
	unsigned char	page_addr_fetched;
	u_int32_t	flash_size;
	unsigned short	page_size;
	unsigned short	page_delay;
	u_int32_t	page_addr;
	u_int32_t	last_load_addr;
	unsigned char	page [256];

#if defined(__WIN32__) || defined(WIN32)
	void *fd;
	DCB saved_mode;
#else
	int fd;
	struct termios saved_mode;
#endif
};

#if 0
#define DEBUG(x)	{ if (debug) printf x; }
#else
#define DEBUG(x) 	/* void */
#endif

#if defined(__WIN32__) || defined(WIN32)
int usleep (u_int32_t usec);
#endif

char *avr_name (avr_t *avr)
{
	if (! avr->name)
		return "Unknown";
	return avr->name;
}

u_int32_t avr_flash_size (avr_t *avr)
{
	return avr->flash_size;
}

/*
 * Receive data from device.
 */
static int avr_receive (avr_t *avr, unsigned char *data, int len)
{
#if defined(__WIN32__) || defined(WIN32)
	DWORD got;

	if (! ReadFile (avr->fd, data, len, &got, 0)) {
		fprintf (stderr, "avr-send: read error\n");
		exit (-1);
	}
#else
	struct timeval timeout, to2;
	long got;
	fd_set rfds;

	timeout.tv_sec = 5;
	timeout.tv_usec = 0;
	to2 = timeout;
again:
	FD_ZERO (&rfds);
	FD_SET (avr->fd, &rfds);

	got = select (avr->fd + 1, &rfds, 0, 0, &to2);
	if (got < 0) {
		if (errno == EINTR || errno == EAGAIN) {
			if (debug > 1)
				printf ("avr-send: programmer is not responding\n");
			goto again;
		}
		fprintf (stderr, "avr-send: select error: %s\n", strerror (errno));
		exit (1);
	}
#endif
	if (got == 0) {
		if (debug > 1)
			printf ("avr-send: programmer is not responding\n");
		return 0;
	}

#if ! defined(__WIN32__) && !defined(WIN32)
	got = read (avr->fd, data, (len > 1024) ? 1024 : len);
	if (got < 0) {
		fprintf (stderr, "avr-send: read error\n");
		exit (-1);
	}
#endif
	return got;
}


/*
 * Send the command sequence and get back a response.
 */
static int avr_send (avr_t *avr, unsigned char *cmd, int cmdlen,
	unsigned char *response, int reply_len)
{
	unsigned char *p, sum, hdr [5];
	int len, i, got, rlen;

	/*
	 * Prepare header and checksum.
	 */
	hdr[0] = MESSAGE_START;
	hdr[1] = ++avr->sequence_number;
	hdr[2] = cmdlen >> 8;
	hdr[3] = cmdlen;
	hdr[4] = TOKEN;
	sum = hdr[0] ^ hdr[1] ^ hdr[2] ^ hdr[3] ^ hdr[4];
	for (i=0; i<cmdlen; ++i)
		sum ^= cmd[i];

	/*
	 * Send command.
	 */
	if (debug > 1) {
		printf ("send [%d] %x-%x-%x-%x-%x",
			5 + cmdlen + 1, hdr[0], hdr[1], hdr[2], hdr[3], hdr[4]);
		for (i=0; i<cmdlen; ++i)
			printf ("-%x", cmd[i]);
		printf ("-%x\n", sum);
	}
#if defined(__WIN32__) || defined(WIN32)
	{
	DWORD written;

	if (! WriteFile (avr->fd, hdr, 5, &written, 0) ||
	    ! WriteFile (avr->fd, cmd, cmdlen, &written, 0) ||
	    ! WriteFile (avr->fd, &sum, 1, &written, 0)) {
		fprintf (stderr, "avr-send: write error\n");
		exit (-1);
	}
	}
#else
	if (write (avr->fd, hdr, 5) < 0 ||
	    write (avr->fd, cmd, cmdlen) < 0 ||
	    write (avr->fd, &sum, 1) < 0) {
		fprintf (stderr, "avr-send: write error\n");
		exit (-1);
	}
#endif
	/*
	 * Get header.
	 */
	p = hdr;
	len = 0;
	while (len < 5) {
		got = avr_receive (avr, p, 5 - len);
		if (! got)
			return 0;

		p += got;
		len += got;
	}
	if (hdr[0] != MESSAGE_START || hdr[1] != avr->sequence_number ||
	    hdr[4] != TOKEN) {
		printf ("got invalid header: %x-%x-%x-%x-%x\n",
			hdr[0], hdr[1], hdr[2], hdr[3], hdr[4]);
flush_input:	{
		/* Skip all incoming data. */
		unsigned char buf [300];
		avr_receive (avr, buf, sizeof (buf));
		}
		return 0;
	}
	rlen = hdr[2] << 8 | hdr[3];
	if (rlen == 0 || rlen > reply_len) {
		printf ("invalid reply length=%d, expecting %d bytes\n",
			rlen, reply_len);
		goto flush_input;
	}

	/*
	 * Get response.
	 */
	p = response;
	len = 0;
	while (len < rlen) {
		got = avr_receive (avr, p, rlen - len);
		if (! got)
			return 0;

		p += got;
		len += got;
	}

	/*
	 * Get sum.
	 */
	p = &sum;
	len = 0;
	while (len < 1) {
		got = avr_receive (avr, p, 1);
		if (! got)
			return 0;
		++len;
	}

	if (debug > 1) {
		printf (" got [%d] %x-%x-%x-%x-%x",
			5 + rlen + 1, hdr[0], hdr[1], hdr[2], hdr[3], hdr[4]);
		for (i=0; i<rlen; ++i)
			printf ("-%x", response[i]);
		printf ("-%x\n", sum);
	}

	/* Check sum. */
	sum ^= hdr[0] ^ hdr[1] ^ hdr[2] ^ hdr[3] ^ hdr[4];
	for (i=0; i<rlen; ++i)
		sum ^= response[i];
	if (sum != 0) {
		printf ("invalid reply checksum\n");
		goto flush_input;
	}
	return 1;
}

static void avr_prog_enable (avr_t *avr)
{
	unsigned char cmd [12] = { CMD_ENTER_PROGMODE_ISP,
		200,	/* timeout in msec */
		100,	/* pin stabilization delay in msec */
		25,	/* command execution delay in msec */
		32,	/* number of synchronization loops */
		0,	/* per byte delay */
		0x53,	/* poll value, 53h for AVR, 69h for AT89xx */
		3,	/* poll index, 3 for AVR, 4 for AT89xx */
		0xAC, 0x53, 0x00, 0x00 };
	unsigned char response [2];

	if (! avr_send (avr, cmd, 12, response, 2) || response[0] != cmd[0] ||
	    response[1] != STATUS_CMD_OK) {
		fprintf (stderr, "Cannot enter programming mode.\n");
		exit (-1);
	}
}

static void avr_prog_disable (avr_t *avr)
{
	unsigned char cmd [3] = { CMD_LEAVE_PROGMODE_ISP,
		1,	/* pre-delay in msec */
		1,	/* post-delay in msec */
		};
	unsigned char response [2];

	if (! avr_send (avr, cmd, 3, response, 2) || response[0] != cmd[0] ||
	    response[1] != STATUS_CMD_OK) {
		fprintf (stderr, "Cannot leave programming mode.\n");
		exit (-1);
	}
}

#if 0
static unsigned char avr_read_signature (avr_t *avr, unsigned char addr)
{
	unsigned char cmd [6] = { CMD_READ_FUSE_ISP,
		4, 0x30, 0, addr, 0 };
	unsigned char response [4];

	if (! avr_send (avr, cmd, 6, response, 4) || response[0] != cmd[0] ||
	    response[1] != STATUS_CMD_OK || response[3] != STATUS_CMD_OK) {
		fprintf (stderr, "Read signature failed.\n");
		exit (-1);
	}
	return response[2];
}

static unsigned char avr_get_param (avr_t *avr, unsigned char addr)
{
	unsigned char cmd [2] = { CMD_GET_PARAMETER, addr };
	unsigned char response [3];

	if (avr_send (avr, cmd, 2, response, 3) && response[0] == cmd[0] &&
	    response[1] == STATUS_CMD_OK)
		return response [2];
	return 0;
}

static void avr_set_param (avr_t *avr, unsigned char addr, unsigned char val)
{
	unsigned char cmd [3] = { CMD_SET_PARAMETER, addr, val };
	unsigned char response [2];

	if (! avr_send (avr, cmd, 3, response, 2) || response[0] != cmd[0] ||
	    response[1] != STATUS_CMD_OK) {
		fprintf (stderr, "Cannot set parameter %02x to %02x.\n",
			addr, val);
		exit (-1);
	}
}
#endif

static int avr_detect (avr_t *avr)
{
	int retry_count, ok_count;
	unsigned char response [12];

	/* Synchronize */
	retry_count = ok_count = 0;
	while (ok_count < 3 && retry_count < 7) {
		/* Send CMD_SIGN_ON. */
		if (! avr_send (avr, (unsigned char*)"\1", 1, response, 11))
			goto retry;

		if (memcmp (response, "\1\0\10STK500_2", 11) == 0) {
			avr->stk_flag = 1;
detected:		++ok_count;
			if (debug > 1)
				printf ("avr-probe: ok %d\n", ok_count);
			continue;
		}
		if (memcmp (response, "\1\0\10AVRISP_2", 11) == 0) {
			avr->stk_flag = 0;
			goto detected;
		}
retry:
		++retry_count;
		if (debug > 1)
			printf ("avr-probe: error %d, ok %d\n",
				retry_count, ok_count);
	}
	if (retry_count >= 7)
		return 0;
#if 0
	avr_set_param (avr, PARAM_SCK_DURATION, 2);

	/* Get programmer parameters */
	avr->hardware_version = avr_get_param (avr, PARAM_HW_VER);
	avr->software_major = avr_get_param (avr, PARAM_SW_MAJOR);
	avr->software_minor = avr_get_param (avr, PARAM_SW_MINOR);
	avr->sck_duration = avr_get_param (avr, PARAM_SCK_DURATION);
	if (avr->stk_flag) {
		/* For STK500 only, not for AVRISP. */
		avr->topcard = avr_get_param (avr, PARAM_TOPCARD_DETECT);
		avr->target_voltage = avr_get_param (avr, PARAM_VTARGET);
		avr->aref_voltage = avr_get_param (avr, PARAM_VADJUST);
		avr->osc_pscale = avr_get_param (avr, PARAM_OSC_PSCALE);
		avr->osc_cmatch = avr_get_param (avr, PARAM_OSC_CMATCH);
	}
#endif
	avr_prog_enable (avr);
	avr->last_load_addr = -1;
#if 0
	/* Get AVR Info */
	avr->vendor_code = avr_read_signature (avr, 0);	/* 0x1e is Atmel */
	avr->part_family = avr_read_signature (avr, 1);
	avr->part_number = avr_read_signature (avr, 2);

	/* Assume checksum command is available. */
	avr->have_checksum_cmd = 1;

	if (debug) {
		printf ("Hardware Version: %d\n", avr->hardware_version);
		printf ("Firmware Version: %d.%d\n",
			avr->software_major, avr->software_minor);
		printf ("      SCK Period: %.1f usec\n",
			avr->sck_duration * 8.0e6 / STK500_XTAL + 0.05);
		if (avr->stk_flag) {
			char *topcard_name = "Unknown";

			switch (avr->topcard) {
			case 0xFF: topcard_name = "Not installed"; break;
			case 0xAA: topcard_name = "STK501"; break;
			case 0x55: topcard_name = "STK502"; break;
			case 0xFA: topcard_name = "STK503"; break;
			case 0xEE: topcard_name = "STK504"; break;
			case 0xE4: topcard_name = "STK505"; break;
			case 0xDD: topcard_name = "STK520"; break;
			}
			printf ("        Top Card: %s\n", topcard_name);
			printf ("  Target Voltage: %.1f V\n",
				avr->target_voltage / 10.0);
			printf ("    AREF Voltage: %.1f V\n",
				avr->aref_voltage / 10.0);

			printf ("      Oscillator: ");
			if (avr->osc_pscale == 0)
				printf("Off\n");
			else {
				double f;

				f = STK500_XTAL / 2 / (avr->osc_cmatch + 1);
				switch (avr->osc_pscale) {
				case 2: f /= 8; break;
				case 3: f /= 32; break;
				case 4: f /= 64; break;
				case 5: f /= 128; break;
				case 6: f /= 256; break;
				case 7: f /= 1024; break;
				}
				if (f > 1e6) {
					printf ("%.3f MHz\n", f / 1000000);
				} else if (f > 1e3) {
					printf ("%.3f kHz\n", f / 1000);
				} else
					printf ("%.3f Hz\n", f);
			}
		}
		printf ("Vendor Code = %x, Part Family = %x, Part Number = %x\n",
			avr->vendor_code, avr->part_family, avr->part_number);
	}
#endif
	/* Identify device according to family.
	 * Assume ATmega128. */
	avr->flash_size = 0xfe00 * 2;
//	avr->have_fuse = 1;
	avr->page_delay = 5;
	avr->page_size = 256;
#if 0
	/* Identify device according to family. */
	switch (avr->part_family) {
	case 0x01:
		avr->have_fuse = 0;
		avr->flash_size = 0x20000;
		avr->page_delay = 56;
		avr->name = "Old ATmega103";
		if (avr->vendor_code == 0 && avr->part_number == 2) {
			if (debug)
				printf ("Device is protected, assuming ATmega103.\n");
			avr->name = "Protected";
		}
		break;

	case 0x90:
		avr->flash_size = 1024;
		avr->page_size = 32;
		switch (avr->part_number) {
		case 0x01:
			avr->name = "AT90s1200";
			avr->page_size = 0;
			break;
		case 0x04:
			avr->name = "ATtiny11";
			avr->page_size = 0;
			break;
		case 0x05:
			avr->name = "ATtiny12";
			avr->page_size = 0;
			break;
		case 0x06:
			avr->name = "ATtiny15";
			avr->page_size = 0;
			break;
		case 0x07:
			avr->name = "ATtiny13";
			break;
		default:
			if (debug)
				printf ("Unknown part number %#x, assuming ATtiny13.\n",
					avr->part_number);
			avr->name = "Unknown";
			break;
		}
		break;

	case 0x91:
		avr->flash_size = 2048;
		avr->page_size = 32;
		switch (avr->part_number) {
		case 0x01:
			avr->name = "AT90s2313";
			avr->page_size = 0;
			break;
		case 0x02:
			avr->name = "AT90s2323";
			avr->page_size = 0;
			break;
		case 0x03:
			avr->name = "AT90s2343";
			avr->page_size = 0;
			break;
		case 0x06:
			avr->name = "ATtiny22";
			avr->page_size = 0;
			break;
		case 0x07:
			avr->name = "ATtiny28";
			avr->page_size = 0;
			break;
		case 0x08:
			avr->name = "ATtiny25";
			break;
		case 0x09:
			avr->name = "ATtiny26";
			break;
		case 0x0A:
			avr->name = "ATtiny2313";
			break;
		case 0x0B:
			avr->name = "ATtiny24";
			break;
		case 0x0C:
			avr->name = "ATtiny261";
			break;
		default:
			if (debug)
				printf ("Unknown part number %#x, assuming ATtiny24.\n",
					avr->part_number);
			avr->name = "Unknown";
			break;
		}
		break;

	case 0x92:
		avr->flash_size = 4096;
		avr->page_size = 64;
		switch (avr->part_number) {
		case 0x01:
			avr->name = "AT90s4414";
			avr->page_size = 0;
			break;
		case 0x03:
			avr->name = "AT90s4433";
			avr->page_size = 0;
			break;
		case 0x05:
			avr->name = "ATmega48";
			break;
		case 0x06:
			avr->name = "ATtiny45";
			break;
		case 0x07:
			avr->name = "ATtiny44";
			avr->page_size = 32;
			break;
		case 0x08:
			avr->name = "ATtiny461";
			avr->page_size = 32;
			break;
		default:
			if (debug)
				printf ("Unknown part number %#x, assuming ATtiny44.\n",
					avr->part_number);
			avr->name = "Unknown";
			break;
		}
		break;

	case 0x93:
		avr->flash_size = 0xf80*2;
		avr->page_size = 64;
		switch (avr->part_number) {
		case 0x01:
			avr->name = "AT90s8515";
			avr->page_size = 0;
			break;
		case 0x03:
			avr->name = "AT90s8535";
			avr->page_size = 0;
			break;
		case 0x06:
			avr->name = "ATmega8515";
			break;
		case 0x07:
			avr->name = "ATmega8";
			break;
		case 0x08:
			avr->name = "ATmega8535";
			break;
		case 0x0A:
			avr->name = "ATmega88";
			break;
		case 0x0B:
			avr->name = "ATtiny85";
			break;
		case 0x0C:
			avr->name = "ATtiny84";
			break;
		case 0x0D:
			avr->name = "ATtiny861";
			break;
		default:
			if (debug)
				printf ("Unknown part number %#x, assuming ATmega8.\n",
					avr->part_number);
			avr->name = "Unknown";
			break;
		}
		break;

	case 0x94:
		avr->flash_size = 0x1f80*2;
		avr->page_size = 128;
		switch (avr->part_number) {
		case 0x01:
			avr->name = "ATmega161";
			break;
		case 0x02:
			avr->name = "ATmega163";
			break;
		case 0x03:
			avr->name = "ATmega16";
			break;
		case 0x04:
			avr->name = "ATmega162";
			break;
		case 0x05:
			avr->name = "ATmega169";
			break;
		case 0x06:
			avr->name = "ATmega168";
			break;
		case 0x07:
			avr->name = "ATmega165";
			break;
		default:
			if (debug)
				printf ("Unknown part number %#x, assuming ATmega16.\n",
					avr->part_number);
			avr->name = "Unknown";
			break;
		}
		break;

	case 0x95:
		avr->flash_size = 0x3f00*2;
		avr->page_size = 128;
		switch (avr->part_number) {
		case 0x01:
			avr->name = "ATmega323";
			break;
		case 0x02:
			avr->name = "ATmega32";
			break;
		case 0x03:
			avr->name = "ATmega329";
			break;
		case 0x04:
			avr->name = "ATmega3290";
			break;
		case 0x05:
			avr->name = "ATmega325";
			break;
		case 0x06:
			avr->name = "ATmega3250";
			break;
		case 0x07:
			avr->name = "ATmega406";
			break;
		default:
			if (debug)
				printf ("Unknown part number %#x, assuming ATmega32.\n",
					avr->part_number);
			avr->name = "Unknown";
			break;
		}
		break;

	case 0x96:
		avr->flash_size = 0x7e00*2;
		switch (avr->part_number) {
		case 0x01:
			avr->name = "ATmega603";
			avr->flash_size = 0x10000;
			avr->have_fuse = 0;
			avr->page_delay = 56;
			break;
		case 0x02:
			avr->name = "ATmega64";
			break;
		case 0x03:
			avr->name = "ATmega649";
			break;
		case 0x04:
			avr->name = "ATmega6490";
			break;
		case 0x05:
			avr->name = "ATmega645";
			break;
		case 0x06:
			avr->name = "ATmega6450";
			break;
		case 0x08:
			avr->name = "ATmega640";
			break;
		case 0x09:
			avr->name = "ATmega644";
			break;
		default:
			if (debug)
				printf ("Unknown part number %#x, assuming ATmega64.\n",
					avr->part_number);
			avr->name = "Unknown";
			break;
		}
		break;

	case 0x97:
		switch (avr->part_number) {
		case 0x01:
			avr->name = "ATmega103";
			avr->flash_size = 0x20000;
			avr->have_fuse = 0;
			avr->page_delay = 56;
			break;
		case 0x02:
			avr->name = "ATmega128";
			break;
		case 0x03:
			avr->name = "ATmega1280";
			break;
		case 0x04:
			avr->name = "ATmega1281";
			break;
		default:
			if (debug)
				printf ("Unknown part number %#x, assuming ATmega128.\n",
					avr->part_number);
			avr->name = "Unknown";
			break;
		}
		break;

	case 0x98:
		avr->flash_size = 0x3E000; // LY: use only first 248K, see ERRATA;
		switch (avr->part_number) {
		case 0x01:
			avr->name = "ATmega2560";
			break;
		case 0x02:
			avr->name = "ATmega2561";
			break;
		default:
			if (debug)
				printf ("Unknown part number %#x, assuming ATmega256.\n",
					avr->part_number);
			avr->name = "Unknown";
			break;
		}
		break;

	default:
		if (debug)
			printf ("Unknown part family %#x, assuming ATmega128.\n",
				avr->part_family);
		avr->name = "Unknown";
		break;
	}
#endif
	return 1;
}

/*
 * Open the device.
 */
avr_t *avr_open (char *devname)
{
	avr_t *avr;
#if defined(__WIN32__) || defined(WIN32)
	DCB new_mode;
	COMMTIMEOUTS ctmo;
#else
	struct termios new_mode;
#endif
	avr = calloc (1, sizeof (avr_t));
	if (! avr) {
		fprintf (stderr, "Out of memory\n");
		exit (-1);
	}

#if defined(__WIN32__) || defined(WIN32)
	/* Open port */
	avr->fd = CreateFile (devname, GENERIC_READ | GENERIC_WRITE,
		0, 0, OPEN_EXISTING, 0, 0);
	if (avr->fd == INVALID_HANDLE_VALUE) {
		fprintf (stderr, "%s: Cannot open\n", devname);
		exit (-1);
	}

	/* Set serial attributes */
	memset (&avr->saved_mode, 0, sizeof(avr->saved_mode));
	if (! GetCommState (avr->fd, &avr->saved_mode)) {
		fprintf (stderr, "%s: Cannot get state\n", devname);
		exit (-1);
	}

	new_mode = avr->saved_mode;

	new_mode.BaudRate = CBR_9600;
	new_mode.ByteSize = 8;
	new_mode.StopBits = ONESTOPBIT;
	new_mode.Parity = 0;
	new_mode.fParity = FALSE;
	new_mode.fOutX = FALSE;
	new_mode.fInX = FALSE;
	new_mode.fOutxCtsFlow = FALSE;
	new_mode.fOutxDsrFlow = FALSE;
	new_mode.fRtsControl = RTS_CONTROL_ENABLE;
	new_mode.fNull = FALSE;
	new_mode.fAbortOnError = FALSE;
	new_mode.fBinary = TRUE;
	if (! SetCommState (avr->fd, &new_mode)) {
		fprintf (stderr, "%s: Cannot set state\n", devname);
		exit (-1);
	}

	memset (&ctmo, 0, sizeof(ctmo));
	ctmo.ReadIntervalTimeout = 0;
	ctmo.ReadTotalTimeoutMultiplier = 0;
	ctmo.ReadTotalTimeoutConstant = 500;
	if (! SetCommTimeouts (avr->fd, &ctmo)) {
		fprintf (stderr, "%s: Cannot set timeouts\n", devname);
		exit (-1);
	}
#else
	/* Open port */
	avr->fd = open (devname, O_RDWR | O_NONBLOCK);
	if (avr->fd < 0) {
		perror (devname);
		exit (-1);
	}

	/* Set serial attributes */
	memset (&avr->saved_mode, 0, sizeof(avr->saved_mode));
	tcgetattr (avr->fd, &avr->saved_mode);

	new_mode = avr->saved_mode;

	cfmakeraw (&new_mode);
	cfsetispeed (&new_mode, 9600);
	cfsetospeed (&new_mode, 9600);
	new_mode.c_iflag |= IGNBRK;
	tcsetattr (avr->fd, TCSANOW, &new_mode);

	if (avr_detect (avr))
		goto ok;
#endif

	/* Try another baud rate. */
#if defined(__WIN32__) || defined(WIN32)
	new_mode.BaudRate = CBR_115200;
	SetCommState (avr->fd, &new_mode);
#else
	cfsetispeed (&new_mode, 115200);
	cfsetospeed (&new_mode, 115200);
	tcsetattr (avr->fd, TCSANOW, &new_mode);
#endif
	if (! avr_detect (avr)) {
		fprintf (stderr, "No response from device.\n");
		exit (-1);
	}
ok:
#if defined(__WIN32__) || defined(WIN32)
	ctmo.ReadTotalTimeoutConstant = 5000;
	SetCommTimeouts (avr->fd, &ctmo);
#endif
	return avr;
}

/*
 * Close the device.
 */
void avr_close (avr_t *avr)
{
	if (avr->name) {
		avr_prog_disable (avr);
		avr->name = 0;
	}

#if defined(__WIN32__) || defined(WIN32)
	SetCommState (avr->fd, &avr->saved_mode);
	CloseHandle (avr->fd);
#else
	tcsetattr (avr->fd, TCSANOW, &avr->saved_mode);
	close (avr->fd);
#endif
}

void avr_erase (avr_t *avr)
{
	unsigned char cmd [7] = { CMD_CHIP_ERASE_ISP,
		3 * avr->page_delay * 10, 0, 0xAC, 0x80, 0x00, 0x00 };
	unsigned char response [2];

	printf ("Erasing...");
	fflush (stdout);

	if (! avr_send (avr, cmd, 7, response, 2) || response[0] != cmd[0] ||
	    response[1] != STATUS_CMD_OK) {
		fprintf (stderr, "Chip erase failed.\n");
		exit (-1);
	}
	printf (" done\n");
	avr_detect (avr);
}

static void avr_load_address (avr_t *avr, u_int32_t addr)
{
	unsigned char cmd [5] = { CMD_LOAD_ADDRESS,
		addr >> 24, addr >> 16, addr >> 8, addr };
	unsigned char response [2];

	if (avr->last_load_addr == addr)
		return;

	if (debug > 1)
		printf ("Load address: %#x\n", addr);

	/* Extended address flag. */
	if (avr->flash_size > 0x10000)
		cmd[1] |= 0x80;

	if (! avr_send (avr, cmd, 5, response, 2) || response[0] != cmd[0] ||
	    response[1] != STATUS_CMD_OK) {
		fprintf (stderr, "Load address failed.\n");
		exit (-1);
	}
	avr->last_load_addr = addr;
}

/*
 * PAGE MODE PROGRAMMING:
 * Cache page address. When current address is out of the page address,
 * flush page buffer and continue programming.
 */
void avr_write_byte (avr_t *avr, u_int32_t addr, unsigned char byte)
{
	int page_bytes = avr->page_size;

	if (page_bytes == 0) {
		/* Word mode. */
		page_bytes = 256;
	}
	if (debug > 2)
		printf ("Loading to address: %#x (page_addr_fetched=%s)\n",
			addr, avr->page_addr_fetched ? "Yes" : "No");

	if (avr->page_addr / page_bytes != addr / page_bytes)
		avr_flush_write_buffer (avr);

	if (! avr->page_addr_fetched) {
		avr->page_addr = addr / page_bytes * page_bytes;
		avr->page_addr_fetched = 1;
	}
	avr->page [addr % page_bytes] = byte;
}

void avr_flush_write_buffer (avr_t *avr)
{
	unsigned char cmd [10+256] = { CMD_PROGRAM_FLASH_ISP,
		avr->page_size >> 8, avr->page_size,
		0xA1,		/* mode */
		avr->page_delay * 3/2,	/* delay */
		0x40,		/* Load Page command */
		0x4c,		/* Write Program Memory Page command */
		0x20,		/* Read Program Memory command */
		0xFF,		/* poll value 1 */
		0 };		/* poll value 2 (for EEPROM only) */
	unsigned char response [2];
	int page_bytes = avr->page_size;

	if (! avr->page_addr_fetched)
		return;

	avr_load_address (avr, avr->page_addr >> 1);

	if (debug > 1)
		printf ("Programming page: %#x\n", avr->page_addr);

	if (page_bytes == 0) {
		/* Word mode. */
		page_bytes = 256;
		cmd [1] = page_bytes >> 8;
		cmd [2] = page_bytes;
		cmd [3] = 4;		/* mode */
		cmd [6] = 0x20;		/* Read Program Memory command */
		cmd [8] = 0x7F;		/* poll value 1 */
	}
	memcpy (cmd+10, avr->page, page_bytes);
	if (! avr_send (avr, cmd, 10+page_bytes, response, 2) ||
	    response[0] != cmd[0]) {
		fprintf (stderr, "Program flash failed.\n");
		exit (-1);
	}
	if (response[1] != STATUS_CMD_OK)
		printf ("Programming flash: timeout at %#x\n", avr->page_addr);

	usleep (avr->page_delay * 1000L);
	avr->page_addr_fetched = 0;
	avr->last_load_addr += page_bytes / 2;
}

unsigned char avr_read_byte (avr_t *avr, u_int32_t addr)
{
	unsigned char cmd [4] = { CMD_READ_FLASH_ISP, 0, 2, 0x20 };
	unsigned char response [5];

	avr_load_address (avr, addr >> 1);
	if (! avr_send (avr, cmd, 4, response, 5) || response[0] != cmd[0] ||
	    response[1] != STATUS_CMD_OK || response[4] != STATUS_CMD_OK) {
		fprintf (stderr, "Read byte failed.\n");
		exit (-1);
	}
	return (addr & 1) ? response[3] : response[2];
}

void avr_write_block (avr_t *avr, u_int32_t addr,
	unsigned char *buf, u_int32_t bytes)
{
	unsigned short i;

	for (i=0; i<bytes; ++i) {
		avr_write_byte (avr, addr+i, buf[i]);
	}
	avr_flush_write_buffer (avr);
}

#if 0
static int avr_get_checksum (avr_t *avr, u_int32_t addr,
	u_int32_t bytes, unsigned short *sum)
{
	unsigned char cmd [4] = { CMD_READ_FLASH_ISP | 0x80, 1, 0, 0x20 };
	unsigned char response [4];

	avr_load_address (avr, addr >> 1);
	if (debug > 1)
		printf ("Get checksum: %#x\n", addr);

	if (! avr_send (avr, cmd, 4, response, 4) ||
	    response[0] != cmd[0] ||
	    response[1] != STATUS_CMD_OK) {
		return 0;
	}
	*sum = response[2] << 8 | response[3];

	avr->last_load_addr += 256 / 2;
	return 1;
}

/*
 * Calculate a new sum given the current sum and the new data.
 * Use 0xffff as the initial sum value.
 * Do not forget to invert the final checksum value.
 */
static unsigned short
crc16 (unsigned const char *buf, unsigned short len)
{
	static const unsigned short poly_tab [16] = {
		0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401,
		0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400,
	};
	unsigned short sum;

	sum = 0;
	for (; len--; ++buf) {
		/* compute checksum of lower four bits of *buf */
		sum = ((sum >> 4) & 0x0FFF) ^
			poly_tab [sum & 0xF] ^
			poly_tab [*buf & 0xF];

		/* now compute checksum of upper four bits of *buf */
		sum = ((sum >> 4) & 0x0FFF) ^
			poly_tab [sum & 0xF] ^
			poly_tab [(*buf >> 4) & 0xF];
	}
	return sum;
}
#endif

int avr_check_block (avr_t *avr, u_int32_t addr,
	unsigned char *buf, u_int32_t bytes)
{
	unsigned short i;
	unsigned char page [256];
#if 0
	unsigned short sum, memsum;
	if (avr->have_checksum_cmd &&
	    avr_get_checksum (avr, addr, bytes, &memsum)) {
		sum = crc16 (buf, bytes);
		if (memsum != sum) {
			printf ("\nchecksum error at address %#x: file=%#x, mem=%#x\n",
				addr, sum, memsum);
			return 0;
		}
		return 1;
	}
	/* No checksum command available. */
	avr->have_checksum_cmd = 0;
#endif
	avr_read_block (avr, addr, page, 256);
	for (i=0; i<bytes; ++i) {
		if (page[i] != buf[i]) {
			printf ("\nerror at address %#x: file=%#x, mem=%#x\n",
				addr+i, buf[i], page[i]);
			return 0;
		}
	}
	return 1;
}

void avr_read_block (avr_t *avr, u_int32_t addr,
	unsigned char *buf, u_int32_t bytes)
{
	unsigned char cmd [4] = { CMD_READ_FLASH_ISP, 1, 0, 0x20 };
	unsigned char response [3+256];

	avr_load_address (avr, addr >> 1);
again:
	if (debug > 1)
		printf ("Read page: %#x\n", addr);

	if (! avr_send (avr, cmd, 4, response, 3+256) ||
	    response[0] != cmd[0] ||
	    response[1] != STATUS_CMD_OK ||
	    response[2+256] != STATUS_CMD_OK) {
		fprintf (stderr, "Read page failed.\n");
		exit (-1);
	}
	memcpy (buf, response+2, 256);
	bytes -= 256;
	if (bytes > 0) {
		addr += 256;
		buf += 256;
		goto again;
	}
	avr->last_load_addr += 256 / 2;
}

#if 0
void avr_lock (avr_t *avr)
{
	unsigned char cmd [6] = { CMD_READ_FUSE_ISP,
		0, 0xAC, 0xF9, 0x00, 0x00 };
	unsigned char response [4];

	if (debug)
		printf ("Writing lock bits...\n");

	if (! avr_send (avr, cmd, 6, response, 4) || response[0] != cmd[0] ||
	    response[1] != STATUS_CMD_OK || response[3] != STATUS_CMD_OK) {
		fprintf (stderr, "Chip lock failed.\n");
		exit (-1);
	}
}
#endif

unsigned char avr_have_fuse (avr_t *avr)
{
	return avr->have_fuse;
}

#if 0
unsigned char avr_read_fuse (avr_t *avr)
{
	unsigned char cmd [6] = { CMD_READ_FUSE_ISP,
		4, 0x50, 0, 0, 0 };
	unsigned char response [4];

	if (! avr->have_fuse)
		return 0xff;

	if (! avr_send (avr, cmd, 6, response, 4) || response[0] != cmd[0] ||
	    response[1] != STATUS_CMD_OK || response[3] != STATUS_CMD_OK) {
		fprintf (stderr, "Read fuse failed.\n");
		exit (-1);
	}
	return response[2];
}

unsigned char avr_read_fuse_high (avr_t *avr)
{
	unsigned char cmd [6] = { CMD_READ_FUSE_ISP,
		4, 0x58, 0x08, 0, 0 };
	unsigned char response [4];

	if (! avr->have_fuse)
		return 0xff;

	if (! avr_send (avr, cmd, 6, response, 4) || response[0] != cmd[0] ||
	    response[1] != STATUS_CMD_OK || response[3] != STATUS_CMD_OK) {
		fprintf (stderr, "Read fuse failed.\n");
		exit (-1);
	}
	return response[2];
}

unsigned char avr_read_fuse_extended (avr_t *avr)
{
	unsigned char cmd [6] = { CMD_READ_FUSE_ISP,
		4, 0x50, 0x08, 0, 0 };
	unsigned char response [4];

	if (! avr->have_fuse)
		return 0xff;

	if (! avr_send (avr, cmd, 6, response, 4) || response[0] != cmd[0] ||
	    response[1] != STATUS_CMD_OK || response[3] != STATUS_CMD_OK) {
		fprintf (stderr, "Read fuse failed.\n");
		exit (-1);
	}
	return response[2];
}

void avr_write_fuse (avr_t *avr, unsigned char val)
{
	unsigned char cmd [5] = { CMD_PROGRAM_FUSE_ISP,
		0xAC, 0xA0, 0, val };
	unsigned char response [3];

	if (! avr->have_fuse)
		return;

	if (! avr_send (avr, cmd, 5, response, 3) || response[0] != cmd[0] ||
	    response[1] != STATUS_CMD_OK || response[2] != STATUS_CMD_OK) {
		fprintf (stderr, "Write fuse failed.\n");
		exit (-1);
	}
}

void avr_write_fuse_high (avr_t *avr, unsigned char val)
{
	unsigned char cmd [5] = { CMD_PROGRAM_FUSE_ISP,
		0xAC, 0xA8, 0, val };
	unsigned char response [3];

	if (! avr->have_fuse)
		return;

	if (! avr_send (avr, cmd, 5, response, 3) || response[0] != cmd[0] ||
	    response[1] != STATUS_CMD_OK || response[2] != STATUS_CMD_OK) {
		fprintf (stderr, "Write fuse failed.\n");
		exit (-1);
	}
}

void avr_write_fuse_extended (avr_t *avr, unsigned char val)
{
	unsigned char cmd [5] = { CMD_PROGRAM_FUSE_ISP,
		0xAC, 0xA4, 0, val };
	unsigned char response [3];

	if (! avr->have_fuse)
		return;

	if (! avr_send (avr, cmd, 5, response, 3) || response[0] != cmd[0] ||
	    response[1] != STATUS_CMD_OK || response[2] != STATUS_CMD_OK) {
		fprintf (stderr, "Write fuse failed.\n");
		exit (-1);
	}
}
#endif
