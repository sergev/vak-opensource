#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define IMAGESEP 0x2c           /* ',' - image separator */
#define INTERLACEMASK 0x40      /* image is interlaced */
#define COLORMAPMASK 0x80       /* file contains colormap */

#define NEXTBYTE (*ptr++)

typedef unsigned char PIXEL;

FILE *fp;

int BitOffset = 0;              /* Bit Offset of next code */
int XC = 0, YC = 0;             /* Output X and Y coordsof current pixel */
int Pass = 0;                   /* Used by output routine if interlaced pic */
int OutCount = 0;               /* Decompressor output 'stack count' */
int RWidth, RHeight;            /* screen dimensions */
int Width, Height;              /* image dimensions */
int LeftOfs, TopOfs;            /* image offset */
int BitsPerPixel;               /* Bits per pixel, read from GIF header */
int ColorMapSize;               /* number of colors */
int CodeSize;                   /* Code size, read from GIF header */
int InitCodeSize;               /* Starting code size, used during Clear */
int Code;                       /* Value returned by ReadCode */
int MaxCode;                    /* limiting value for current code size */
int ClearCode;                  /* GIF clear code */
int EOFCode;                    /* GIF end-of-information code */
int CurCode, OldCode, InCode;   /* Decompressor variables */
int FirstFree;                  /* First free code, generated per GIF spec */
int FreeCode;                   /* Decompressor, next free slot in hash table */
int FinChar;                    /* Decompressor variable */
int BitMask;                    /* AND mask for data size */
int ReadMask;                   /* Code AND mask for current code size */

int Interlace, HasColormap;
int Verbose = 0;

PIXEL **pixels;                 /* The pixel array */

unsigned char *RawGIF;          /* The heap array to hold it, raw */
unsigned char *Raster;          /* The raster data stream, unblocked */

/* The hash table used by the decompressor */
int Prefix[4096], Suffix[4096];

/* An output array used by the decompressor */
int OutCode[1025];

/* The color map, read from the GIF header */
unsigned char Red[256], Green[256], Blue[256];

char *id = "GIF87a";

void Error (char *str)
{
	fprintf (stderr, "%s\n", str);
	exit (1);
}

void Error1 (char *str, char *a)
{
	fprintf (stderr, str, a);
	fprintf (stderr, "\n");
	exit (1);
}

/* Allocate memory for image of width w, height h. */
PIXEL **AllocImage (int w, int h)
{
	PIXEL **tab;
	register i;

	tab = (PIXEL **) malloc (sizeof (PIXEL *) * h);
	if (! tab)
		Error ("Out of memory");
	for (i=0; i<h; ++i) {
		tab [i] = (PIXEL *) malloc (sizeof (PIXEL) * w);
		if (! tab [i])
			Error ("Out of memory");
	}
	return (tab);
}

void WriteImage (PIXEL **pix, int w, int h)
{
	/* For example:
	 * int x, y, p;
	 * for (y=0; y<h; ++y)
	 *   for (x=0; x<w; ++x) {
	 *     p = pix[y][x];
	 *     printf ("%d %d %d\n", Red[p], Green[p], Blue[p]);
	 *   }
	 */
	printf ("width=%d, height=%d\n", w, h);
}

/* Fetch the next code from the raster data stream.  The codes can be
 * any length from 3 to 12 bits, packed into 8-bit bytes, so we have to
 * maintain our location in the Raster array as a BIT Offset.  We
 * compute the byte Offset into the raster array by dividing this by 8,
 * pick up three bytes, compute the bit Offset into our 24-bit chunk,
 * shift to bring the desired code to the bottom, then mask it off and
 * return it. */
int ReadCode ()
{
	int RawCode, ByteOffset;

	ByteOffset = BitOffset / 8;
	RawCode = Raster[ByteOffset] + (Raster[ByteOffset + 1] << 8);
	if (CodeSize >= 8)
		RawCode += Raster[ByteOffset + 2] << 16;
	RawCode >>= (BitOffset % 8);
	BitOffset += CodeSize;
	return (RawCode & ReadMask);
}

void AddToPixel (unsigned char Index)
{
	if (YC < Height)
		pixels[YC][XC] = Index;

	/* Update the X-coordinate, and if it overflows, update the
	 * Y-coordinate */

	if (++XC < Width)
		return;

	/* If a non-interlaced picture, just increment YC to
	 * the next scan line. If it's interlaced, deal with
	 * the interlace as described in the GIF spec.  Put the
	 * decoded scan line out to the screen if we haven't
	 * gone past the bottom of it */

	XC = 0;
	if (! Interlace) {
		YC++;
		return;
	}

	switch (Pass) {
	case 0: YC += 8; if (YC >= Height) { Pass++; YC = 4; } break;
	case 1: YC += 8; if (YC >= Height) { Pass++; YC = 2; } break;
	case 2: YC += 4; if (YC >= Height) { Pass++; YC = 1; } break;
	case 3: YC += 2; break;
	default: Error ("can't happen");
	}
}

int main (int argc, char **argv)
{
	char *inf = NULL;
	int filesize;
	register unsigned char ch, ch1;
	register unsigned char *ptr, *ptr1;
	register int i;
	char *usage = "[-h] [-v] [- | GIFfile]";

	setbuf (stderr, NULL);

	while (--argc) {
		if ((++argv)[0][0] == '-') {
			switch (argv[0][1]) {
			case 'v':
				Verbose = 1;
				break;
			case '\0':
				if (inf)
					Error (usage);
				inf = "-";
				break;
			case 'h':
			default:
				Error (usage);
			}
			continue;
		}
		if (inf)
			Error (usage);
		inf = argv[0];
	}
	if (!inf || !strcmp (inf, "-")) {
		inf = "Standard Input";
		fp = stdin;
	} else if (! (fp = fopen (inf, "r")))
		Error1 ("%s not found", inf);

	/* find the size of the file */
	fseek (fp, 0L, 2);
	filesize = ftell (fp);
	fseek (fp, 0L, 0);

	ptr = RawGIF = (unsigned char *) malloc (filesize);
	Raster = (unsigned char *) malloc (filesize);
	if (!ptr || !Raster)
		Error ("not enough memory to read gif file");

	if (fread (ptr, filesize, 1, fp) != 1)
		Error ("GIF data read failed");

	if (memcmp (ptr, id, 6))
		Error1 ("%s is not a GIF file", inf);
	ptr += 6;

	/* Get variables from the GIF screen descriptor */

	ch = NEXTBYTE;
	RWidth = ch + (NEXTBYTE << 8);  /* screen dimensions... not used. */
	ch = NEXTBYTE;
	RHeight = ch + (NEXTBYTE << 8);

	if (Verbose)
		fprintf (stderr, "screen dims: %dx%d.\n", RWidth, RHeight);

	ch = NEXTBYTE;
	HasColormap = ((ch & COLORMAPMASK) ? 1 : 0);

	BitsPerPixel = (ch & 7) + 1;
	ColorMapSize = 1 << BitsPerPixel;
	BitMask = ColorMapSize - 1;

	ch = NEXTBYTE;                 /* background color... not used. */

	if (NEXTBYTE)                   /* supposed to be zero */
		Error1 ("%s is a corrupt GIF file (nonzero)", inf);

	/* Read in global colormap. */

	if (HasColormap) {
		if (Verbose)
			fprintf (stderr, "%s is %d bits per pixel, (%d colors).\n",
				 inf, BitsPerPixel, ColorMapSize);
		for (i=0; i<ColorMapSize; ++i) {
			Red[i] = NEXTBYTE;
			Green[i] = NEXTBYTE;
			Blue[i] = NEXTBYTE;
		}
		/* PPMS: Don't have to do anything special here. */
	} else {
		fprintf (stderr, "%s does not have a colormap - making one up\n", inf);
		Red[0] = Green[0] = Blue[0] = 0;
		Red[1] = Green[1] = Blue[1] = 255;
	}

	/* Check for image separator */

	if (NEXTBYTE != IMAGESEP)
		Error1 ("%s is a corrupt GIF file (nosep)", inf);

	/* Now read in values from the image descriptor */

	ch = NEXTBYTE;
	LeftOfs = ch + (NEXTBYTE << 8);
	ch = NEXTBYTE;
	TopOfs = ch + (NEXTBYTE << 8);
	ch = NEXTBYTE;
	Width = ch + (NEXTBYTE << 8);
	ch = NEXTBYTE;
	Height = ch + (NEXTBYTE << 8);
	Interlace = ((NEXTBYTE & INTERLACEMASK) ? 1 : 0);

	if (Verbose)
		fprintf (stderr, "Reading a %d by %d %sinterlaced image...",
			 Width, Height, (Interlace) ? "" : "non-");


	/* Note that I ignore the possible existence of a local color
	 * map. I'm told there aren't many files around that use them,
	 * and the spec says it's defined for future use.  This could
	 * lead to an error reading some files. */

	/* Start reading the raster data. First we get the intial code
	 * size and compute decompressor constant values, based on this
	 * code size. */

	CodeSize = NEXTBYTE;
	ClearCode = (1 << CodeSize);
	EOFCode = ClearCode + 1;
	FreeCode = FirstFree = ClearCode + 2;

	/* The GIF spec has it that the code size is the code size used
	 * to compute the above values is the code size given in the
	 * file, but the code size used in compression/decompression is
	 * the code size given in the file plus one. (thus the ++). */

	CodeSize++;
	InitCodeSize = CodeSize;
	MaxCode = (1 << CodeSize);
	ReadMask = MaxCode - 1;

	/* Read the raster data.  Here we just transpose it from the
	 * GIF array to the Raster array, turning it from a series of
	 * blocks into one long data stream, which makes life much
	 * easier for ReadCode(). */

	ptr1 = Raster;
	do {
		ch = ch1 = NEXTBYTE;
		while (ch--)
			*ptr1++ = NEXTBYTE;
		if ((ptr1 - Raster) > filesize)
			Error1 ("%s is a corrupt GIF file (unblock)", inf);
	} while (ch1);

	free (RawGIF);          /* We're done with the raw data now... */

	if (Verbose) {
		fprintf (stderr, "done.\n");
		fprintf (stderr, "Decompressing...");
	}

	pixels = AllocImage (Width, Height);

	/* Decompress the file, continuing until you see the GIF EOF
	 * code. One obvious enhancement is to add checking for corrupt
	 * files here. */

	Code = ReadCode ();
	while (Code != EOFCode) {

		/* Clear code sets everything back to its initial
		 * value, then reads the immediately subsequent code as
		 * uncompressed data. */

		if (Code == ClearCode) {
			CodeSize = InitCodeSize;
			MaxCode = (1 << CodeSize);
			ReadMask = MaxCode - 1;
			FreeCode = FirstFree;
			CurCode = OldCode = Code = ReadCode ();
			FinChar = CurCode & BitMask;
			AddToPixel (FinChar);
		} else {

			/* If not a clear code, then must be data: save
			 * same as CurCode and InCode */

			CurCode = InCode = Code;

			/* If greater or equal to FreeCode, not in the
			 * hash table yet; repeat the last character
			 * decoded */

			if (CurCode >= FreeCode) {
				CurCode = OldCode;
				OutCode[OutCount++] = FinChar;
			}
			/* Unless this code is raw data, pursue the
			 * chain pointed to by CurCode through the hash
			 * table to its end; each code in the chain
			 * puts its associated output code on the
			 * output queue. */

			while (CurCode > BitMask) {
				if (OutCount > 1024)
					Error1 ("%s is a corrupt GIF file (OutCount)", inf);
				OutCode[OutCount++] = Suffix[CurCode];
				CurCode = Prefix[CurCode];
			}

			/* The last code in the chain is treated as raw
			 * data. */

			FinChar = CurCode & BitMask;
			OutCode[OutCount++] = FinChar;

			/* Now we put the data out to the Output
			 * routine. It's been stacked LIFO, so deal
			 * with it that way... */

			for (i=OutCount-1; i>=0; --i)
				AddToPixel (OutCode[i]);
			OutCount = 0;

			/* Build the hash table on-the-fly. No table is
			 * stored in the file. */

			Prefix[FreeCode] = OldCode;
			Suffix[FreeCode] = FinChar;
			OldCode = InCode;

			/* Point to the next slot in the table.  If we
			 * exceed the current MaxCode value, increment
			 * the code size unless it's already 12.  If it
			 * is, do nothing: the next code decompressed
			 * better be CLEAR */

			FreeCode++;
			if (FreeCode >= MaxCode) {
				if (CodeSize < 12) {
					CodeSize++;
					MaxCode *= 2;
					ReadMask = (1 << CodeSize) - 1;
				}
			}
		}
		Code = ReadCode ();
	}

	free (Raster);

	if (Verbose)
		fprintf (stderr, "done.\n");

	if (fp != stdin)
		fclose (fp);

	if (Verbose)
		fprintf (stderr, "Writing rasterfile...");

	WriteImage (pixels, Width, Height);

	if (Verbose)
		fprintf (stderr, "done.\n");

	if (fp != stdout)
		fclose (fp);

	return (0);
}
