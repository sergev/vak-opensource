/*
 * Boot installator.
 * Author: Serge Vakulenko, <vak@cronyx.ru>
 */
#include <stdio.h>
#include <fcntl.h>
#include <windows.h>
#include <winioctl.h>

#define PartAddr        0x1be	/* Offset to partition table */
#define ValidationAddr  0x1fe	/* Offset to validation bytes */
#define MAGIC		0xaa55	/* Validation tag */

typedef struct PartitionEntry {
	unsigned char	bootIndicator;
	unsigned char	beginHead;
	unsigned char	beginSector;
	unsigned char	beginCyl;
	unsigned char	systemId;
	unsigned char	endHead;
	unsigned char	endSector;
	unsigned char	endCyl;
	unsigned short	relSectorLow;
	unsigned short	relSectorHigh;
	unsigned short	numSectorsLow;
	unsigned short	numSectorsHigh;
} PartitionEntry;

unsigned char bootRecord [512];

char head1 [] =	"здбдддддддддбддддбддддбдддддддддддддддбдддддддддддддддбдддддддддддбддддддддддд©\n";
char head2 [] =	"Ё ЁPartitionЁCan ЁAc- Ё   Beginning   Ё    Ending     Ё  Relative Ё Number of Ё\n";
char head3 [] =	"ЁNЁ  Type   ЁBootЁtiveЁHead  Cyl  SectЁHead  Cyl  SectЁ  Sectors  Ё  Sectors  Ё\n";
char head4 [] =	"цдедддддддддеддддеддддеддддбдддддбддддеддддбдддддбддддедддддддддддеддддддддддд╢\n";
char mid1 []  = "Ё%dЁ%-9.9sЁ%-4.4sЁ%-4.4sЁ%3u Ё%4u Ё%3u Ё%3u Ё%4u Ё%3u Ё%10lu Ё%10lu Ё\n";
char mid2 []  =	"цдедддддддддеддддеддддеддддедддддеддддеддддедддддеддддедддддддддддеддддддддддд╢\n";
char foot []  =	"юдадддддддддаддддаддддаддддадддддаддддаддддадддддаддддадддддддддддаддддддддддды\n";

int diskread (int disk, long lba, char *buf)
{
	HANDLE fd;
	char name [80];
	long locLow, locHigh, cnt;

	if (disk >= '0' && disk <= '9')
		sprintf (name, "\\\\.\\PhysicalDrive%c", disk);
	else
		sprintf (name, "\\\\.\\%c:", disk);

	fd = CreateFile (name, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);

	if (fd == INVALID_HANDLE_VALUE) {
		fprintf (stderr, "Error opening disk `%c'\n", disk);
		CloseHandle (fd);
		return -1;
	}
	locLow = lba << 9;
	locHigh = lba >> 23;

	if (SetFilePointer (fd, locLow, &locHigh, FILE_BEGIN) == 0xFFFFFFFF) {
		fprintf (stderr, "Seek failed\n");
		CloseHandle (fd);
		return -1;
	}
	/* Read sector off of the drive... */
	if (! ReadFile (fd, buf, 512, &cnt, 0)) {
		fprintf (stderr, "Error reading boot record from disk `%c'\n",
			disk);
		CloseHandle (fd);
		return -1;
	}
	CloseHandle (fd);
	return 0;
}

int diskgeom (int disk)
{
	HANDLE fd;
	char name [80];
	long cnt;
	DISK_GEOMETRY geom;

	if (disk >= '0' && disk <= '9')
		sprintf (name, "\\\\.\\PhysicalDrive%c", disk);
	else
		sprintf (name, "\\\\.\\%c:", disk);

	fd = CreateFile (name, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
	if (fd == INVALID_HANDLE_VALUE) {
		fprintf (stderr, "Error opening disk `%c'\n", disk);
		CloseHandle (fd);
		return -1;
	}

	if (! DeviceIoControl (fd, IOCTL_DISK_GET_DRIVE_GEOMETRY,
	    NULL, 0, &geom, sizeof (geom), &cnt, 0)) {
		fprintf (stderr, "Error getting drive geometry\n");
		CloseHandle (fd);
		return -1;
	}

	printf ("           Disk %c = ", disk);
	switch (geom.MediaType) {
	case Unknown:
		printf ("Unknown\n");
		break;
	case F5_1Pt2_512:
		printf ("Floppy 5.25\" with 1.2MB and 512 bytes/sector\n");
		break;
	case F3_1Pt44_512:
		printf ("Floppy 3.5\" with 1.44MB and 512 bytes/sector\n");
		break;
	case F3_2Pt88_512:
		printf ("Floppy 3.5\" with 2.88MB and 512 bytes/sector\n");
		break;
	case F3_20Pt8_512:
		printf ("Floppy 3.5\" with 20.8MB and 512 bytes/sector\n");
		break;
	case F3_720_512:
		printf ("Floppy 3.5\" with 720KB and 512 bytes/sector\n");
		break;
	case F5_360_512:
		printf ("Floppy 5.25\" with 360KB and 512 bytes/sector\n");
		break;
	case F5_320_512:
		printf ("Floppy 5.25\" with 320KB and 512 bytes/sector\n");
		break;
	case F5_320_1024:
		printf ("Floppy 5.25\" with 320KB and 1024 bytes/sector\n");
		break;
	case F5_180_512:
		printf ("Floppy 5.25\" with 180KB and 512 bytes/sector\n");
		break;
	case F5_160_512:
		printf ("Floppy 5.25\" with 160KB and 512 bytes/sector\n");
		break;
	case RemovableMedia:
		printf ("Removable disk\n");
		break;
	case FixedMedia:
		printf ("Fixed hard disk\n");
		break;
	default:
		printf ("%d\n", (int) geom.MediaType);
		break;
	}
	printf ("        Cylinders = %lu\n", geom.Cylinders.u.LowPart);
	printf ("            Heads = %lu\n", geom.TracksPerCylinder);
	printf ("Sectors per track = %lu\n", geom.SectorsPerTrack);
	printf (" Bytes per sector = %lu\n", geom.BytesPerSector);

	CloseHandle (fd);
	return 0;
}

void printtable (int disk)
{
	PartitionEntry *part = (PartitionEntry *) (bootRecord + PartAddr);
	int i, cb;
	long relSectors;
	long numSectors;
	char *typeString, typeData[16], block[512];

	printf ("\n");
	printf (head1);
	printf (head2);
	printf (head3);
	printf (head4);

	for (i=0; i<4; ++i) {
		switch (part->systemId) {
                case 0x00:	typeString = "  empty";   cb = 0; break;
                case 0x01:	typeString = "  dos-12";  cb = 1; break;
                case 0x04:
                case 0x06:	typeString = "  dos-16";  cb = 1; break;
                case 0x05:	typeString = "  extend";  cb = 0; break;
                case 0x07:	typeString = "  hpfs";    cb = 1; break;
                case 0x0A:	typeString = "  os/2";    cb = 1; break;
                case 0x0B:
                case 0x0C:
                case 0x0E:	typeString = " windows";  cb = 1; break;
                case 0x63:	typeString = "  unix";    cb = 1; break;
                case 0x64:	typeString = "novell286"; cb = 1; break;
                case 0x65:	typeString = "novell386"; cb = 1; break;
                case 0x80:	typeString = "  minix";	  cb = 1; break;
                case 0x81:
                case 0x83:	typeString = "  linux";   cb = 1; break;
                case 0x82:	typeString = "linux/swp"; cb = 0; break;
                case 0x93:	typeString = "  amoeba";  cb = 1; break;
                case 0x94:	typeString = "amoebaBBT"; cb = 0; break;
                case 0xA5:	typeString = " freebsd";  cb = 1; break;
                case 0x9f:	typeString = "  bsd/os";  cb = 1; break;
                case 0xB8:	typeString = " bsd/swap"; cb = 0; break;
                case 0xff:	typeString = "  bbt";     cb = 0; break;
		default:	sprintf (typeData, "  0x%02x", part->systemId);
				typeString = typeData;    cb = 1; break;
		}
		relSectors = part->relSectorLow |
			(unsigned long) part->relSectorHigh<<16;
		numSectors = part->numSectorsLow |
			(unsigned long) part->numSectorsHigh<<16;

		if (cb) {
			if (diskread (disk, relSectors, block) < 0)
				cb = 2;
			else if (*(short *) &block [ValidationAddr] !=
			    (short) MAGIC)
				cb = 3;
		}
		printf (mid1, i+1, typeString, cb==3 ? " bad" :
			cb==2 ? " err" : cb ? " yes" : " no",
			(part->bootIndicator & 0x80) ? " yes" : " no",
			part->beginHead, part->beginCyl |
			(part->beginSector<<2 & 0x300),
			part->beginSector & 077, part->endHead,
			part->endCyl | (part->endSector<<2 & 0x300),
			part->endSector & 077, relSectors, numSectors);
		if (i < 3)
			printf (mid2);
		++part;
	}
	printf (foot);
}

int main (int argc, char **argv)
{
	int disk = '0';

	if (argc > 1)
		disk = argv[1][0];

	if (diskread (disk, 0, bootRecord) < 0)
		exit (-1);

	diskgeom (disk);
	printtable (disk);

	if (*(short *) &bootRecord [ValidationAddr] != (short) MAGIC) {
		fprintf (stderr, "Bad master boot record on disk `%c'!\n",
			disk);
		exit (-1);
	}
	return (0);
}
