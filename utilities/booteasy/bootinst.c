/*
 * Boot installator.
 * Author: Serge Vakulenko, <vak@cronyx.ru>
 */

#include <stdio.h>
#include <fcntl.h>
#include <dos.h>

#define BOOTFILE	"boot.bin"
#define SAVEFILE	"bootsav.bin"
#define SAVE2FILE	"boot2sav.bin"

#define PartAddr        0x1be	/* Offset to partition table */
#define ValidationAddr  0x1fe	/* Offset to validation bytes */
#define MAGIC		0xaa55	/* Validation tag */

#define DISK1		0
#define DISK2		1
#define READ		0
#define WRITE		1

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
unsigned char bootProg [512];

char head1 [] =	"здбдддддддддбддддбддддбдддддддддддддддбдддддддддддддддбдддддддддбддддддддд©\n";
char head2 [] =	"Ё ЁPartitionЁCan ЁBootЁ   Beginning   Ё    Ending     Ё RelativeЁNumber ofЁ\n";
char head3 [] =	"ЁNЁ  Type   ЁBootЁPartЁHead  Cyl  SectЁHead  Cyl  SectЁ Sectors Ё Sectors Ё\n";
char head4 [] =	"цдедддддддддеддддеддддеддддбдддддбддддеддддбдддддбддддедддддддддеддддддддд╢\n";
char mid1 []  = "Ё%dЁ%-9.9sЁ%-4.4sЁ%-4.4sЁ%3u Ё%4u Ё%3u Ё%3u Ё%4u Ё%3u Ё%8lu Ё%8lu Ё\n";
char mid2 []  =	"цдедддддддддеддддеддддеддддедддддеддддеддддедддддеддддедддддддддеддддддддд╢\n";
char foot []  =	"юдадддддддддаддддаддддаддддадддддаддддаддддадддддаддддадддддддддаддддддддды\n";

void printtable (disk)
{
	PartitionEntry *part = (PartitionEntry *) (bootRecord + PartAddr);
	int i, cb;
	long relSectors;
	long numSectors;
	char *typeString, block [512];

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
		default:	typeString = "  ?????";   cb = 1; break;
		}
		relSectors = part->relSectorLow |
			(unsigned long) part->relSectorHigh<<16;
		numSectors = part->numSectorsLow |
			(unsigned long) part->numSectorsHigh<<16;

		if (cb && (bootio (disk, READ, part->beginHead, part->beginCyl,
		    part->beginSector, block) != 0 ||
		    *(short *) &block [ValidationAddr] != (short) MAGIC))
			cb = 0;

		printf (mid1, i+1, typeString, cb ? " yes" : " no",
			(part->bootIndicator & 0x80) ? " yes" : " no",
			part->beginHead, part->beginCyl |
			part->beginSector<<2 & 0x300,
			part->beginSector & 077, part->endHead,
			part->endCyl | part->endSector<<2 & 0x300,
			part->endSector & 077, relSectors, numSectors);
		if (i < 3)
			printf (mid2);
		++part;
	}
	printf (foot);
}

int yes ()
{
	char reply [80];

	printf (" (yes/no) ");
	fgets (reply, sizeof (reply), stdin);
	return (!stricmp (reply, "y\n") || !stricmp (reply, "yes\n"));
}

int install (disk, file, savefile)
char *file, *savefile;
{
	int rc, fd;

	rc = bootio (disk, READ, 0, 0, 1, bootRecord);
	if (rc) {
		fprintf (stderr, "Error %d reading boot record from disk %d\n",
			rc, disk);
		return (-1);
	}
	if (*(short *) &bootRecord [ValidationAddr] != (short) MAGIC) {
		fprintf (stderr, "Bad master boot record on disk %d!\n",
			disk);
		return (-1);
	}

	printtable (disk);

	fd = open (file, O_RDONLY|O_BINARY);
	if (fd < 0) {
		fprintf (stderr, "Cannot read file %s\n", file);
		return (-1);
	}
	if (read (fd, bootProg, 512) != 512) {
		fprintf (stderr, "Error reading %s\n", file);
		return (-1);
	}
	close (fd);
	if (*(short *) &bootProg [ValidationAddr] != (short) MAGIC) {
		fprintf (stderr, "Bad boot image in file %s\n", file);
		return (-1);
	}

	printf ("\nAre you sure you want to install new boot on disk %d?",
		disk);
	if (! yes ())
		return (0);

	close (creat (savefile, 0664));
	fd = open (savefile, O_WRONLY|O_BINARY);
	if (fd < 0) {
		fprintf (stderr, "Cannot write to file %s\n", savefile);
		return (-1);
	}
	if (write (fd, bootRecord, 512) != 512) {
		fprintf (stderr, "Error writing to %s\n", savefile);
		return (-1);
	}
	close (fd);

	memcpy (bootRecord, bootProg, PartAddr);

	rc = bootio (disk, WRITE, 0, 0, 1, bootRecord);
	if (rc) {
		fprintf (stderr, "Error %d updating boot record on disk %d\n",
			rc, disk);
		return (-1);
	}
	printf ("New boot record successfully installed.\n");
	return (0);
}

bootio (disk, op, head, cyl, sect, buf)
char *buf;
{
	return (biosdisk (op==WRITE ? 3 : 2, disk==DISK1 ? 0x80 : 0x81,
		head, cyl, sect, 1, buf));
}

int main (argc, argv)
char **argv;
{
	char *bootfile;
	char *boot2file;
	int disk2present;

	printf ("Boot installer, version 1.8, Copyright (C) Serge Vakulenko\n\n");

	printf ("This utility allows you to install new boot program on your\n");
	printf ("hard disk.  The usage is:\n\n");
	printf ("        bootinst [ bootfile [ boot2file ] ]\n\n");
	printf ("It installs boot block from bootfile (default boot.bin) to the first\n");
	printf ("drive and, if second drive is present, to the second drive.\n");
	printf ("Old boot sectors are saved to bootsav.bin and boot2sav.bin respectively.\n");
	printf ("\nYou can copy and distribute this software free of charge,\n");
	printf ("provided that sources are included.  No warranty, of course.\n\n");

	if (argc > 3)
		return (-1);

	bootfile = argc>1 ? argv[1] : BOOTFILE;
	boot2file = argc>2 ? argv[2] : bootfile;
	if (access (bootfile, 4) < 0) {
		perror (bootfile);
		return (-1);
	}
	if (access (boot2file, 4) < 0) {
		perror (boot2file);
		return (-1);
	}

	/*
	 * Is second disk present?  Try to read boot sector,
	 * check that it is bootable and has valid partition table.
	 */
	disk2present = (bootio (DISK2, READ, 0, 0, 1, bootRecord) == 0 &&
		*(short *) &bootRecord [ValidationAddr] == (short) MAGIC &&
		(bootRecord [PartAddr] == 0 || bootRecord [PartAddr] == 0x80) &&
		(bootRecord [PartAddr+16] == 0 || bootRecord [PartAddr+16] == 0x80) &&
		(bootRecord [PartAddr+32] == 0 || bootRecord [PartAddr+32] == 0x80) &&
		(bootRecord [PartAddr+48] == 0 || bootRecord [PartAddr+48] == 0x80));

	printf ("Continue with installation?");
	if (! yes ())
		return (0);

	install (DISK1, bootfile, SAVEFILE);

	if (disk2present)
		install (DISK2, boot2file, SAVE2FILE);

	return (0);
}
