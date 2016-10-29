/*
 * Write master boot record.
 * Author: Serge Vakulenko, <vak@cronyx.ru>
 */
#include <stdio.h>
#include <fcntl.h>
#include <getopt.h>
#include <windows.h>
#include <winioctl.h>

#define SAVEFILE	"mbrsave.bin"

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

unsigned char mbr [512];
unsigned char bootcode [512];
char geomdata [4096];
unsigned long disksectors;
int create_partition;
int ncylinders;
int nheads;
int nsectors;

int diskgeom (int disk)
{
	HANDLE fd;
	char name [80];
	long cnt;
	DISK_GEOMETRY *geom;

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

	cnt = 0;
	if (! DeviceIoControl (fd, IOCTL_DISK_GET_DRIVE_GEOMETRY,
	    NULL, 0, geomdata, sizeof (geomdata), &cnt, 0)) {
		fprintf (stderr, "Error getting drive geometry, cnt=%ld\n", cnt);
		CloseHandle (fd);
		return -1;
	}
	geom = (DISK_GEOMETRY*) geomdata;

	printf ("Disk %c:\n", disk);
	printf ("Media type = ");
	switch (geom->MediaType) {
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
		printf ("%d\n", (int) geom->MediaType);
		break;
	}
	printf ("Cylinders = %lu\n", geom->Cylinders.u.LowPart);
	printf ("Tracks per cylinder = %lu\n", geom->TracksPerCylinder);
	printf ("Sectors per track = %lu\n", geom->SectorsPerTrack);
	printf ("Bytes per sector = %lu\n", geom->BytesPerSector);
	if (nsectors == 0)
		nsectors = geom->SectorsPerTrack;
	if (nheads == 0)
		nheads = geom->TracksPerCylinder;
	if (ncylinders == 0)
		ncylinders = geom->Cylinders.u.LowPart *
			geom->TracksPerCylinder * geom->SectorsPerTrack /
			nsectors / nheads;
	disksectors = ncylinders * nsectors * nheads;

	CloseHandle (fd);
	return 0;
}

int yes ()
{
	char reply [80];

	printf (" (yes/no) ");
	fgets (reply, sizeof (reply), stdin);
	return (!stricmp (reply, "y\n") || !stricmp (reply, "yes\n"));
}

int install (int disk, char *file, char *savefile)
{
	HANDLE h;
	int fd;
	char name [80];
	long locLow, locHigh, cnt;

	if (disk >= '0' && disk <= '9')
		sprintf (name, "\\\\.\\PhysicalDrive%c", disk);
	else
		sprintf (name, "\\\\.\\%c:", disk);

	h = CreateFile (name, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);

	if (h == INVALID_HANDLE_VALUE) {
		fprintf (stderr, "Error opening disk `%c'\n", disk);
		CloseHandle (h);
		return -1;
	}
	locLow = 0;
	locHigh = 0;

	if (SetFilePointer (h, locLow, &locHigh, FILE_BEGIN) == 0xFFFFFFFF) {
		fprintf (stderr, "Seek failed\n");
		CloseHandle (h);
		return -1;
	}
	/* Read sector off of the drive... */
	if (! ReadFile (h, mbr, 512, &cnt, 0)) {
		fprintf (stderr, "Error reading boot record from disk `%c'\n",
			disk);
		CloseHandle (h);
		return -1;
	}

	/* Read MBR file. */
	fd = open (file, O_RDONLY|O_BINARY);
	if (fd < 0) {
		fprintf (stderr, "Cannot read file %s\n", file);
		return (-1);
	}
	if (read (fd, bootcode, 512) != 512) {
		fprintf (stderr, "Error reading %s\n", file);
		return (-1);
	}
	close (fd);
	if (*(short *) &bootcode [ValidationAddr] != (short) MAGIC) {
		fprintf (stderr, "Bad boot image in file %s\n", file);
		return (-1);
	}

	printf ("\nInstall new master boot record?");
	if (! yes ()) {
		CloseHandle (h);
		return (0);
	}

	/* Save old MBR. */
	close (creat (savefile, 0664));
	fd = open (savefile, O_WRONLY|O_BINARY);
	if (fd < 0) {
		fprintf (stderr, "Cannot write to file %s\n", savefile);
		return (-1);
	}
	if (write (fd, mbr, 512) != 512) {
		fprintf (stderr, "Error writing to %s\n", savefile);
		return (-1);
	}
	close (fd);

	/* Write new boot record. */
	memcpy (mbr, bootcode, PartAddr);
	memcpy (mbr + ValidationAddr, bootcode + ValidationAddr, 2);
	if (create_partition) {
		/* Create partition. */
		PartitionEntry *part = (PartitionEntry*) (mbr + PartAddr);

		memset (mbr + PartAddr, 0, 4 * 16);
		part->bootIndicator = 0x80;
		part->beginHead = 0;
		part->beginSector = 2;
		part->beginCyl = 0;
		part->systemId = create_partition;
		part->endHead = nheads - 1;
		part->endSector = nsectors;
		part->endCyl = disksectors / nheads / nsectors - 1;
		part->relSectorLow = 1;
		part->relSectorHigh = 0;
		part->numSectorsLow = (unsigned short) (disksectors - 1);
		part->numSectorsHigh = (disksectors - 1) >> 16;
		printf ("Partition created.\n");
	}
	if (SetFilePointer (h, locLow, &locHigh, FILE_BEGIN) == 0xFFFFFFFF) {
		fprintf (stderr, "Seek failed\n");
		CloseHandle (h);
		return -1;
	}
	if (! WriteFile (h, mbr, 512, &cnt, 0)) {
		fprintf (stderr, "Error writing boot record to disk `%c'\n",
			disk);
		CloseHandle (h);
		return -1;
	}

	printf ("New boot record successfully installed.\n");
	CloseHandle (h);
	return 0;
}

void usage ()
{
	printf ("Master boot record installer, version 1.8\n");
	printf ("Copyright (C) Serge Vakulenko, www.vak.ru\n");
	printf ("\nUsage:\n");
	printf ("\twritembr [-p# [-c#] [-h#] [-s#]] disk# bootfile.bin\n\n");
	printf ("It installs boot block from bootfile to the specified disk drive.\n");
	printf ("Old boot sector is saved to mbrsave.bin.\n");
	printf ("\nOptions:\n");
	printf ("\t-p #\t\tcreate a partition of given type on a whole disk\n");
	printf ("\t-c #\t\tnumber of cylinders\n");
	printf ("\t-h #\t\tnumber of heads\n");
	printf ("\t-s #\t\tnumber of sectors\n");
	exit (0);
}

int main (int argc, char **argv)
{
	char *bootfile;
	int ch, disk;

	disk = '0';
	while ((ch = getopt(argc, argv, "c:h:p:s:")) != -1)
		switch (ch) {
		case 'p':
			create_partition = strtol (optarg, 0, 0);
			break;
		case 'c':
			ncylinders = strtol (optarg, 0, 0);
			break;
		case 'h':
			nheads = strtol (optarg, 0, 0);
			break;
		case 's':
			nsectors = strtol (optarg, 0, 0);
			break;
		default:
			fprintf (stderr, "Incorrect option: `%c'\n", ch);
			usage ();
		}
	argc -= optind;
	argv += optind;

	if (argc != 2)
		usage ();
	disk = argv[0][0];
	bootfile = argv[1];

	if (access (bootfile, 4) < 0) {
		perror (bootfile);
		return (-1);
	}

	diskgeom (disk);

	install (disk, bootfile, SAVEFILE);

	return (0);
}
