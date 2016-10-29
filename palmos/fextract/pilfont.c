/*  PilFont - A crude font decompiler for PalmOS
 *  Copyright (C) 1998 David Turnbull
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 *
 *  Send bug reports to: dpt@newsguy.com
 */

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

typedef struct 
{
   char  offset;
   char  width;
} FontCharInfoType;
             
typedef struct{
   short fontType;           // font type
   short firstChar;          // ASCII code of first character
   short lastChar;           // ASCII code of last character
   short maxWidth;           // maximum character width
   short kernMax;            // negative of maximum character kern
   short nDescent;           // negative of descent
   short fRectWidth;         // width of font rectangle
   short fRectHeight;        // height of font rectangle
   short owTLoc;             // offset to offset/width table
   short ascent;             // ascent
   short descent;            // descent
   short leading;            // leading
   short rowWords;           // row width of bit image / 2
} FontType;  


/* This is the most portable and fastest way I could think of */
unsigned short bit[8]={128,64,32,16,8,4,2,1};


short ptoh(short i)
{
   /* Convert Pilot short to local short */
   return (((unsigned char *)&i)[0] << 8) + (short)((unsigned char *)&i)[1];
}


char* decode(char* in, char* out, long int offset)
{
   char *err, **bitmap;
   FILE *fin, *fout;
   FontType font;
   unsigned short int loctable[257], col, col2;
   FontCharInfoType glyph[256];
   short i, j,chars;
   long int mark;
   
   /* Prepare for input */   
   if(!(fin=fopen(in, "rb"))) return "Could not open input file.";

   /* Seek to specified location */
   fseek(fin, offset, SEEK_SET);

   /* Load the font header */
   fread(&font, sizeof(FontType), 1, fin);

   /* Get this placeholder for calculating owTLoc */
   mark=ftell(fin) - 10;

   /* Convert header to local machine byte order */
   font.fontType = ptoh(font.fontType);
   font.firstChar = ptoh(font.firstChar);
   font.lastChar = ptoh(font.lastChar);
   font.maxWidth = ptoh(font.maxWidth);
   font.kernMax = ptoh(font.kernMax);
   font.nDescent = ptoh(font.nDescent);
   font.fRectWidth = ptoh(font.fRectWidth);
   font.fRectHeight = ptoh(font.fRectHeight);
   font.owTLoc = ptoh(font.owTLoc);
   font.ascent = ptoh(font.ascent);
   font.descent = ptoh(font.descent);
   font.leading = ptoh(font.leading);
   font.rowWords = ptoh(font.rowWords);

   /* How many characters */
   chars = font.lastChar - font.firstChar + 1;

   /* Load the bitmap image */
   bitmap=(void*)malloc(font.fRectHeight*sizeof(void*));
   for(i=0;i<font.fRectHeight;i++)
   {
      bitmap[i]=(void*)malloc(font.rowWords*2);
      fread(bitmap[i], font.rowWords*2, 1, fin);
   }
   
   /* Load the column table */
   fread(&loctable[font.firstChar], sizeof(unsigned short int) * (chars+1), 1 ,fin);

   /* Seek to owTLoc */
   fseek(fin, mark + font.owTLoc * 2, SEEK_SET);
   
   /* Load the offset/width table */
   fread(&glyph[font.firstChar], sizeof(FontCharInfoType) * chars, 1, fin);

   /* Finished loading */
   fclose(fin);

   /* Prepare for output */
   if(!(fout=fopen(out, "w"))) return "Could not open output file.";
   
   /* Output the header */
   fprintf(fout, 
      "fontType %hu\n"\
      "maxWidth %hd\n"\
      "kernMax %hd\n"\
      "nDescent %hd\n"\
      "fRectWidth %hd\n"\
      "fRectHeight %hd\n"\
      "ascent %hd\n"\
      "descent %hd\n"\
      "leading %hd\n\n",
      font.fontType, font.maxWidth,
      font.kernMax, font.nDescent,
      font.fRectWidth, font.fRectHeight,
      font.ascent, font.descent, font.leading);

   /* Output the glyphs */
   i=font.firstChar;
   while(i<=font.lastChar)
   {
      col=ptoh(loctable[i]);
      col2=ptoh(loctable[i+1]);
      if(col<col2)
      {
         if(i>32 && i!=39 && i!=92 && i<127) fprintf(fout, "GLYPH '%c'\n",i);
         else fprintf(fout, "GLYPH %d\n", i);
         if(glyph[i].width != col2-col) fprintf(fout, "WIDTH %d\n", glyph[i].width);
         if(glyph[i].offset) fprintf(fout, "OFFSET %d\n", glyph[i].offset);
         for(j=0;j<font.fRectHeight;j++)
         {
            for(col=ptoh(loctable[i]);col<col2;col++)
            {
               if(bitmap[j][col>>3] & bit[col&0x7]) fputc('#', fout);
               else fputc('-', fout);
            }
            fprintf(fout, "\n");
         }
         fprintf(fout, "\n");
      }
      i++;
   }

   /* Finished outputing */   
   fclose(fout);

   /* Success, print warm fuzzy */   
   printf("%d glyphs exported.\n\n", chars);
   return 0;
}


int main(int argc, char *argv[])
{
   char * s;
   long int offset=0;
   
   printf("PilFont V1.0, Copyright (C) 1998 David Turnbull\n" \
          "PilFont comes with ABSOLUTELY NO WARRANTY\n\n");
            
   if(argc<3 || argc>4) goto help;
   
   if(argc==4) offset=atol(argv[3]);
   
   s=decode(argv[1], argv[2], offset);
   
   if(!s) exit(0);
   
   printf("ERROR: %s\n\n", s);
   exit(1);


help:

   printf(
   "Extracts a Pilot font resource to an ASCII\n"\
   "file that can be compiled with PilRC.\n\n"
   " usage:  pilfont infile outfile {offset}\n\n"\
   );
   exit(1);
}
