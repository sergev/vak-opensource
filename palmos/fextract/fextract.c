/*
 * FontExtract - A utility for extracting fonts from PalmOS PRC files.
 * Copyright (C) 2002-2003 Serge Vakulenko
 * Based on sources of pilfont utility, by David Turnbull.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 *
 * $Id: fextract.c,v 1.9 2003/02/06 17:04:35 vak Exp $
 */
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	char offset;
	char width;
} charinfo_t;

typedef struct {
	unsigned short fontType;		/* font type */
#define FONT_STD	((unsigned short) 0x9000)
#define FONT_EXTENDED	((unsigned short) 0x9200)
#define FONT_EXTSWAPPED	((unsigned short) 0x0092)

	short firstChar;	/* ASCII code of first character */
	short lastChar;		/* ASCII code of last character */
	short maxWidth;		/* maximum character width */
	short kernMax;		/* negative of maximum character kern */
	short nDescent;		/* negative of descent */
	short fRectWidth;	/* width of font rectangle */
	short fRectHeight;	/* height of font rectangle */
	short owTLoc;		/* offset to offset / width table */
	short ascent;		/* ascent */
	short descent;		/* descent */
	short leading;		/* leading */
	short rowWords;		/* row width of bit image / 2 */

	/* For extended font (V2). */
	short version;		/* version on extended font, should be 1 */
	short densityCount;	/* number of densities, should be 2 */
	short densities0_density;
	unsigned long densities0_glyphBitsOffset;
	short densities1_density;
	unsigned long densities1_glyphBitsOffset;
} font_t;

/*
 * Swap two bytes of short integer value.
 * This is the most portable and fastest way I could think of
 */
unsigned short bit[8] = {128, 64, 32, 16, 8, 4, 2, 1};

char font_index = 'a';

unsigned short swap_short (short i)
{
	/* Convert Pilot short to local short */
	return (((unsigned char *)&i)[0] << 8) + ((unsigned char *)&i)[1];
}

/*
 * Read the font header from the file.
 * Return 0 on i/o error.
 */
int font_read (font_t *font, FILE *fin, long offset)
{
	/* Seek to specified location */
	/* printf ("offset = %d\n", offset); */
	fseek (fin, offset, SEEK_SET);

	/* Load the font header (V1). */
	if (fread (font, 26, 1, fin) != 1)
		return 0;

	if (swap_short (font->fontType) == FONT_STD) {
		/* Font version 1, convert to local byte order. */
		font->fontType = swap_short (font->fontType);
		font->firstChar = swap_short (font->firstChar);
		font->lastChar = swap_short (font->lastChar);
		font->maxWidth = swap_short (font->maxWidth);
		font->kernMax = swap_short (font->kernMax);
		font->nDescent = swap_short (font->nDescent);
		font->fRectWidth = swap_short (font->fRectWidth);
		font->fRectHeight = swap_short (font->fRectHeight);
		font->owTLoc = swap_short (font->owTLoc);
		font->ascent = swap_short (font->ascent);
		font->descent = swap_short (font->descent);
		font->leading = swap_short (font->leading);
		font->rowWords = swap_short (font->rowWords);
		return 1;
	}
	if (font->fontType == FONT_EXTENDED) {
		/* Font version 2, load the rest of extended header. */
		struct {
			short densityCount;
			short dummy1;
			short densities0_density;
			short dummy2;
			long densities0_glyphBitsOffset;
			short densities1_density;
			short dummy3;
			long densities1_glyphBitsOffset;
		} hext;

		if (fread (&font->version, 2, 1, fin) != 1)
			return 0;
		if (fread (&hext, 20, 1, fin) != 1)
			return 0;

		font->densityCount = hext.densityCount;
		font->densities0_density = hext.densities0_density;
		font->densities1_density = hext.densities1_density;
		font->densities0_glyphBitsOffset = hext.densities0_glyphBitsOffset;
		font->densities1_glyphBitsOffset = hext.densities1_glyphBitsOffset;
		return 1;
	}
	if (font->fontType == FONT_EXTSWAPPED) {
		/* Font version 2, but byte swapped. */
		struct {
			short version;		/* version on extended font, should be 1 */
			short densityCount;	/* number of densities, should be 2 */
			short densities0_density;
			short densities0_glyphBitsOffset_high;
			short densities0_glyphBitsOffset_low;
			short densities1_density;
			short densities1_glyphBitsOffset_high;
			short densities1_glyphBitsOffset_low;
		} hext;

		font->firstChar = swap_short (font->firstChar);
		font->lastChar = swap_short (font->lastChar);
		font->maxWidth = swap_short (font->maxWidth);
		font->kernMax = swap_short (font->kernMax);
		font->nDescent = swap_short (font->nDescent);
		font->fRectWidth = swap_short (font->fRectWidth);
		font->fRectHeight = swap_short (font->fRectHeight);
		font->owTLoc = swap_short (font->owTLoc);
		font->ascent = swap_short (font->ascent);
		font->descent = swap_short (font->descent);
		font->leading = swap_short (font->leading);
		font->rowWords = swap_short (font->rowWords);
		if (fread (&hext, 16, 1, fin) != 1)
			return 0;

		font->version = swap_short (hext.version);
		font->densityCount = swap_short (hext.densityCount);
		font->densities0_density =
			swap_short (hext.densities0_density);
		font->densities1_density =
			swap_short (hext.densities1_density);
		font->densities0_glyphBitsOffset =
			swap_short (hext.densities0_glyphBitsOffset_low) |
			swap_short (hext.densities0_glyphBitsOffset_high) << 16;
		font->densities1_glyphBitsOffset =
			swap_short (hext.densities1_glyphBitsOffset_low) |
			swap_short (hext.densities1_glyphBitsOffset_high) << 16;
#if 0
printf ("%ld - fontType = %#x, version = %#hx, densityCount = %#hx\n",
offset, font->fontType, font->version, font->densityCount);
printf ("%ld - offset0 = %#lx, offset1 = %#lx\n",
offset, font->densities[0].glyphBitsOffset, font->densities[1].glyphBitsOffset);
#endif
		return 1;
	}
	return 1;
}

/*
 * Check the correctness of font header.
 */
int valid_font (font_t *font)
{
	if (font->fontType != FONT_STD &&
	    font->fontType != FONT_EXTENDED &&
	    font->fontType != FONT_EXTSWAPPED)
		return 0;
#if 0
printf ("firstChar = %d, lastChar = %d, maxWidth = %d, kernMax = %d, nDescent = %d, fRectWidth = %d, fRectHeight = %d, owTLoc = %d, ascent = %d, descent = %d, leading = %d\n",
font->firstChar, font->lastChar, font->maxWidth, font->kernMax,
font->nDescent, font->fRectWidth, font->fRectHeight,
font->owTLoc, font->ascent, font->descent, font->leading);
if (font->fontType != FONT_STD)
printf ("offset0 = %08lx, offset1 = %08lx\n",
font->densities0_glyphBitsOffset, font->densities1_glyphBitsOffset);
#endif
	if (font->firstChar < 0 || font->firstChar > 255)
		return 0;
	if (font->lastChar < 0 || font->lastChar > 255)
		return 0;
	if (font->maxWidth < 0 || font->maxWidth > 50)
		return 0;
	if (font->kernMax < -50 || font->kernMax > 50)
		return 0;
	if (font->nDescent < -50 || font->nDescent > 50)
		return 0;
	if (font->fRectWidth < 1 || font->fRectWidth > 50)
		return 0;
	if (font->fRectHeight < 1 || font->fRectHeight > 50)
		return 0;
	if (font->owTLoc < 0 || font->owTLoc > 30000)
		return 0;
	if (font->ascent < 0 || font->ascent > 50)
		return 0;
	if (font->descent < 0 || font->descent > 50)
		return 0;
	if (font->leading < 0 || font->leading > 50)
		return 0;
	if (font->fontType != FONT_STD &&
	    (font->version != 1 || font->densityCount != 2))
		return 0;
	return 1;
}

/*
 * Write the PilRC-compatible text representation
 * of the font to file.
 */
void output_font (font_t *font, unsigned short *loctable,
	charinfo_t *glyph, char **bitmap)
{
	char outname [80];
	FILE *fout;
	short i, j;
	unsigned short col, col2;

	/* Prepare for output */
	sprintf (outname, "%c-%dx%d.pfn", font_index,
		font->fRectWidth, font->fRectHeight);
	fout = fopen (outname, "w");
	if (! fout) {
		perror (outname);
		exit (1);
	}

	/* Output the header */
	fprintf (fout,
	    "fontType %hu\n" \
	    "maxWidth %hd\n" \
	    "kernMax %hd\n" \
	    "nDescent %hd\n" \
	    "fRectWidth %hd\n" \
	    "fRectHeight %hd\n" \
	    "ascent %hd\n" \
	    "descent %hd\n" \
	    "leading %hd\n\n",
	    font->fontType, font->maxWidth,
	    font->kernMax, font->nDescent,
	    font->fRectWidth, font->fRectHeight,
	    font->ascent, font->descent, font->leading);

	/* Output the glyphs */
	for (i = font->firstChar; i <= font->lastChar; i++) {
		col = loctable[i];
		col2 = loctable[i + 1];
		if (col >= col2)
			continue;

		fprintf (fout, "GLYPH %d\n", i);
		if (glyph[i].width != col2 - col)
			fprintf (fout, "WIDTH %d\n", glyph[i].width);
		if (glyph[i].offset)
			fprintf (fout, "OFFSET %d\n", glyph[i].offset);
		for (j = 0; j < font->fRectHeight; j++) {
			for (col = loctable[i]; col < col2; col++) {
				if (bitmap[j][col >> 3] & bit[col & 0x7])
					fputc ('#', fout);
				else
					fputc ('-', fout);
			}
			fprintf (fout, "\n");
		}
		fprintf (fout, "\n");
	}
	fclose (fout);
}

void write_short (short val, FILE *fout)
{
	fputc (val >> 8, fout);
	fputc (val, fout);
}

/*
 * Write the PilotFontEditor-compatible binary representation
 * of the font to file.
 */
void output_pft (font_t *font, unsigned short *loctable,
	charinfo_t *glyph, char **bitmap)
{
	char outname [80];
	FILE *fout;
	short i, chars, owtloc;

	/* Prepare for output */
	sprintf (outname, "%c-%dx%d.pft", font_index,
		font->fRectWidth, font->fRectHeight);
	fout = fopen (outname, "wb");
	if (! fout) {
		perror (outname);
		exit (1);
	}

	/* How many characters */
	chars = font->lastChar - font->firstChar + 1;
	owtloc = font->rowWords * font->fRectHeight +
		font->lastChar - font->firstChar + 8;

	/* Output the header */
	write_short (FONT_STD, fout);
	write_short (font->firstChar, fout);
	write_short (font->lastChar, fout);
	write_short (font->maxWidth, fout);
	write_short (font->kernMax, fout);
	write_short (font->nDescent, fout);
	write_short (font->fRectWidth, fout);
	write_short (font->fRectHeight, fout);
	write_short (owtloc, fout);
	write_short (font->ascent, fout);
	write_short (font->descent, fout);
	write_short (font->leading, fout);
	write_short (font->rowWords, fout);

	/* Output the bitmap image */
	for (i = 0; i < font->fRectHeight; i++) {
		fwrite (bitmap[i], font->rowWords * 2, 1, fout);
	}

	/* Output the column table */
	for (i=font->firstChar; i<=font->lastChar+2; ++i) {
		write_short (loctable[i], fout);
	}

	/* Seek to owTLoc */
	fseek (fout, 16 + (owtloc * 2), SEEK_SET);

	/* Load the offset/width table */
	fwrite (&glyph[font->firstChar], sizeof (charinfo_t) * (chars+1), 1, fout);
	fwrite ("\377\377", sizeof (charinfo_t), 1, fout);

	fclose (fout);
}

/*
 * Try to extract a font from the binary file, starting from given offset.
 * When valid font is detected, write it to file.
 * Return the new value of offset (for the next try).
 * On i/o error, return -1.
 */
long decode (FILE *fin, long offset)
{
	font_t font;
	unsigned short loctable [258];
	charinfo_t glyph [257];
	char *bitmap [50], *bitmap_high [50];
	short i, chars;

	if (! font_read (&font, fin, offset)) {
		/* Read error -- finished scanning. */
		return -1;
	}

	if (! valid_font (&font))
		return offset + 1;

	/* How many characters */
	chars = font.lastChar - font.firstChar + 1;

	if (font.fontType == FONT_STD) {
		/* Load the bitmap image */
		for (i = 0; i < font.fRectHeight; i++) {
			bitmap[i] = (char*) malloc (font.rowWords * 2);
			fread (bitmap[i], font.rowWords * 2, 1, fin);
		}
	}

	/* Load the column table */
	fread (&loctable[font.firstChar], sizeof (short) * (chars+2), 1, fin);
	if (font.fontType != FONT_EXTENDED) {
		for (i=font.firstChar; i<=font.lastChar+2; ++i)
			loctable[i] = swap_short (loctable[i]);
	}

	/* Load the offset/width table */
	if (font.fontType == FONT_STD) {
		/* Seek to owTLoc */
		fseek (fin, offset + 16 + font.owTLoc * 2, SEEK_SET);
	}
	fread (&glyph[font.firstChar], sizeof (charinfo_t) * (chars+1), 1, fin);

	if (font.fontType != FONT_STD) {
		/* Load the bitmap image */
		fseek (fin, offset + font.densities0_glyphBitsOffset, SEEK_SET);
		for (i = 0; i < font.fRectHeight; i++) {
			bitmap[i] = (char*) malloc (font.rowWords * 2);
			fread (bitmap[i], font.rowWords * 2, 1, fin);
		}
		fseek (fin, offset + font.densities1_glyphBitsOffset, SEEK_SET);
		for (i = 0; i < font.fRectHeight * 2; i++) {
			bitmap_high[i] = (char*) malloc (font.rowWords * 4);
			fread (bitmap_high[i], font.rowWords * 4, 1, fin);
		}
	}

	/* Success, print warm fuzzy */
	printf ("Font %dx%d, %d glyphs exported.\n",
		font.fRectWidth, font.fRectHeight, chars);

	output_font (&font, loctable, glyph, bitmap);
	output_pft (&font, loctable, glyph, bitmap);
	if (font.fontType != FONT_STD) {
		font.maxWidth *= 2;
		font.kernMax *= 2;
		font.nDescent *= 2;
		font.fRectWidth *= 2;
		font.fRectHeight *= 2;
		font.ascent *= 2;
		font.descent *= 2;
		font.leading *= 2;
		font.rowWords *= 2;
		for (i=font.firstChar; i<=font.lastChar+1; ++i) {
			loctable[i] *= 2;
			if (glyph[i].width > 0)
				glyph[i].width *= 2;
			if (glyph[i].offset > 0)
				glyph[i].offset *= 2;
		}
		loctable[font.lastChar+2] *= 2;

		output_font (&font, loctable, glyph, bitmap_high);
		output_pft (&font, loctable, glyph, bitmap_high);
	}
	++font_index;

	return ftell (fin);
}

int main (int argc, char *argv[])
{
	FILE *fin;
	long offset;

	printf ("Palm Font Extractor, Version 1.3\n"
		"Copyright (C) 2002-2003 Serge Vakulenko\n\n");

	if (argc != 2) {
		printf ("Extracts Pilot font resources to ASCII files that can be\n"
			"compiled with PilRC, and binary files that can be edited\n"
			"with Pilot Font Editor.\n"
			"\nUsage:\n\tfextract infile\n\n"
			"For every font it creates text files named `a-WxH.pfn',\n"
			"and binary files named `a-WxH.pft'. Here W and H are\n"
			"the font width and height respectively. The first letter\n"
			"is `a' for first font, `b' for second font and so on.\n"
			);
		exit (1);
	}
	/* Prepare for input */
	fin = fopen (argv[1], "rb");
	if (! fin) {
		perror (argv [1]);
		exit (1);
	}
	offset = 0;
	while (offset >= 0)
		offset = decode (fin, offset);

	fclose (fin);
	exit (0);
}
/*
 * $Log: fextract.c,v $
 * Revision 1.9  2003/02/06 17:04:35  vak
 * Changed version number.
 *
 * Revision 1.8  2003/02/06 17:02:47  vak
 * Added support for FontBucket (PalmOS 5 fonts).
 *
 * Revision 1.7  2002/12/24 18:30:40  vak
 * Fixed bug with structure alignment.
 *
 * Revision 1.6  2002/12/24 17:37:11  vak
 * Added support for PalmOS 5 extended fonts.
 *
 * Revision 1.5  2002/12/23 18:07:55  vak
 * Fixed grammatics errors.
 *
 * Revision 1.4  2002/12/23 17:05:38  vak
 * Usage message updated
 *
 * Revision 1.3  2002/12/23 17:01:02  vak
 * Extracted font files are created with names a-, b-, c- etc,
 * all files having different names.
 * PilRC-style files now have ".pfn" extension, compatible with xFont editor.
 *
 * Revision 1.2  2002/12/15 17:35:21  vak
 * Binary files were incorrectly saved under Windows.
 */
