/*
SPLITMRB - splits MRB into SHG/BMP/WMF and SHG into BMP/WMF files - Version 1.5
M.Winterhoff, Geschw.-Scholl-Ring 17, 38444 Wolfsburg, Germany, CIS 100326,2776

usage: SPLITMRB filename[.mrb] ...
       SPLITMRB filename.shg ...

Output files are created in the current directory using resolution-dependent
extensions *.EGA,*.VGA,*.CGA,*.854,*.MAC or *.BMP,*.WMF or *.Snn,*.nnn (where
n is a digit from 0 to 9). Discarded hotspot info will be written to stdout.

This program is freeware. Use at your own risk. No part of it may be used
commercially. No fees may be charged on distributing.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef enum {FALSE,TRUE} BOOL;

typedef struct tagBITMAPFILEHEADER
{
    unsigned short bfType;
    unsigned long bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned long bfOffBits;
}
BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
    unsigned long biSize;
    long biWidth;
    long biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned long biCompression;
    unsigned long biSizeImage;
    long biXPelsPerMeter;
    long biYPelsPerMeter;
    unsigned long biClrUsed;
    unsigned long biClrImportant;
}
BITMAPINFOHEADER;

typedef struct tagRECT
{
    short left;
    short top;
    short right;
    short bottom;
}
RECT;

typedef struct tagAPMFILEHEADER
{
    unsigned long dwKey;
    unsigned short hMF;
    RECT rcBBox;
    unsigned short wInch;
    unsigned long dwReserved;
    unsigned short wChecksum;
}
APMFILEHEADER;

unsigned short GetCWord(FILE *f)
{
    unsigned char b;

    b=getc(f);
    if(b&1) return (((unsigned short)getc(f)<<8)|(unsigned short)b)>>1;
    return ((unsigned short)b>>1);
}

unsigned long GetCDWord(FILE *f)
{
    unsigned short w;

    w = getw(f);
    if(w&1) return (((unsigned long)getw(f)<<16)|(unsigned long)w)>>1;
    return ((unsigned long)w>>1);
}

unsigned long GetDWord(FILE *f)
{
    unsigned short w;

    w=getw(f);
    return ((unsigned long)getw(f)<<16)|(unsigned long)w;
}

long copy(FILE *f,long bytes,FILE *out)
{
    long length;
    int size;
    static char buffer[1024];

    for(length=0;length<bytes;length+=size)
    {
	size=(int)(bytes-length>sizeof(buffer)?sizeof(buffer):bytes-length);
	fread(buffer,1,size,f);
	fwrite(buffer,size,1,out);
    }
    return length;
}

signed char count; /* for run len decompression */

int derun(int c,FILE *f) /* expand runlen compressed data */
{
    int i;

    if(count&0x7F)
    {
	if(count&0x80)
	{
	    putc(c,f);
	    count--;
	    return 1;
	}
	for(i=0;i<count;i++)
	{
	    putc(c,f);
	}
	count=0;
	return i;
    }
    count=(signed char)c;
    return 0;
}

long decompress(int method,FILE *f,long bytes,FILE *fTarget)
{
    static unsigned char lzbuffer[0x1000];
    int (*Emit)(int c,FILE *f);
    unsigned char bits,mask;
    int pos,len,back;
    long n;

    n=0;
    if(method&1)
    {
	Emit=derun;
	count=0;
    }
    else
    {
	Emit=fputc;
    }
    if(method&2)
    {
	mask=0;
	pos=0;
	while(bytes-->0L)
	{
	    if(!mask)
	    {
		bits=getc(f);
		mask=1;
	    }
	    else
	    {
		if(bits&mask)
		{
		    if(bytes--==0) break;
		    back=getw(f);
		    len=((back>>12)&15)+3;
		    back=pos-(back&0xFFF)-1;
		    while(len-->0)
		    {
			n+=Emit(lzbuffer[pos++&0xFFF]=lzbuffer[back++&0xFFF],fTarget);
		    }
		}
		else
		{
		    n+=Emit(lzbuffer[pos++&0xFFF]=getc(f),fTarget);
		}
		mask<<=1;
	    }
	}
    }
    else
    {
	while(bytes-->0L) n+=Emit(getc(f),fTarget);
    }
    return n;
}

int GetPackedByte(FILE *f) // RulLen decompression
{
    static unsigned char count,value;

    if(!f) // initialize
    {
	count=value=0;
    }
    else
    {
	if((count&0x7F)==0)
	{
	    count=getc(f);
	    value=getc(f);
	}
	else if(count&0x80)
	{
	    value=getc(f);
	}
	count--;
    }
    return value;
}

void PrintHotspotInfo(FILE *f)
{
    int i,l,n;
    typedef struct
    {
	unsigned char c1,c2,c3;
	unsigned int x,y,w,h;
	unsigned long hash;
    }
    HOTSPOT;
#if sizeof(HOTSPOT)!=15
#error Compile byte aligned !
#endif
    HOTSPOT *hotspot;
    char name[80];
    char buffer[128];

    if(getc(f)==1)
    {
	n=getw(f);
	if(n>0)
	{
	    hotspot=malloc(n*sizeof(HOTSPOT));
	    if(hotspot)
	    {
		l=getw(f); // macro data size
		getw(f);   // hiword ignored
		fread(hotspot,sizeof(HOTSPOT),n,f);
		for(i=0;i<l;i++) getc(f);
		for(i=0;i<n;i++)
		{
		    for(l=0;l<sizeof(name)&&(name[l]=getc(f))!='\0';l++);
		    for(l=0;l<sizeof(buffer)&&(buffer[l]=getc(f))!='\0';l++);
		    printf("'%s' ",buffer);
		    if((hotspot[i].c1&0xF0)==0xC0)
		    {
			printf("Macro ");
		    }
		    else if(hotspot[i].c1&1)
		    {
			printf("Jump ");
		    }
		    else
		    {
			printf("Pop-up ");
		    }
		    if(hotspot[i].c2)
		    {
			printf("Invisible");
		    }
		    else
		    {
			printf("Visible ");
		    }
		    printf(" '%s' %d,%d,%d,%d\n",name,hotspot[i].x,hotspot[i].y,hotspot[i].x+hotspot[i].w,hotspot[i].y+hotspot[i].h);
		}
		free(hotspot);
	    }
	}
    }
}

int main(int argc,char *argv[])
{
    FILE *f;
    FILE *fTarget;
    int i,j,k,l,m,n,colors;
    unsigned char byType,byPacked;
    long dwDataSize,dwHotspotOffset,dwHotspotSize,w,h,offset;
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;
    APMFILEHEADER afh;
    char filename[81];
    BOOL res[7];

    if(argc<2)
    {
	fprintf(stderr,"SPLITMRB - splits MRB into SHG/BMP/WMF and SHG into BMP/WMF files - Version 1.5\n"
		       "M.Winterhoff, Geschw.-Scholl-Ring 17, 38444 Wolfsburg, Germany, CIS 100326,2776\n"
		       "\n"
		       "usage: SPLITMRB filename[.mrb] ...\n"
		       "       SPLITMRB filename.shg ...\n"
		       "\n"
		       "Output files are created in the current directory using resolution-dependent\n"
		       "extensions *.EGA,*.VGA,*.CGA,*.854,*.MAC or *.BMP,*.WMF or *.Snn,*.nnn (where\n"
		       "n is a digit from 0 to 9). Discarded hotspot info will be written to stdout.\n"
		       "\n"
		       "This program is freeware. Use at your own risk. No part of it may be used\n"
		       "commercially. No fees may be charged on distributing.\n");
    }
    else for(i=1;i<argc;i++)
    {
	strcpy(filename,argv[i]);
	l=strlen(filename);
	while(l>0&&filename[l-1]!='\\'&&filename[l-1]!='/'&&filename[l-1]!=':') l--;
	m=l;
	while(filename[l]!='\0'&&filename[l]!='.') l++;
	if(filename[l]=='\0') strcpy(filename+l,".MRB");
	f=fopen(filename,"rb");
	if(!f)
	{
	    fprintf(stderr,"Can not open '%s'\n",filename);
	}
	else
	{
	    if((getw(f)&0xDFFF)!=0x506C)
	    {
		fprintf(stderr,"'%s' is no MRB/SHG bitmap\n",argv[i]);
	    }
	    else
	    {
		n=getw(f);
		res[0]=res[1]=res[2]=res[3]=res[4]=res[5]=res[6]=FALSE;
		for(j=0;j<n;j++)
		{
		    fseek(f,4*(j+1),SEEK_SET);
		    fread(&offset,sizeof(offset),1,f);
		    fseek(f,offset,SEEK_SET);
		    byType=getc(f); // type of picture: 5=DDB, 6=DIB, 8=METAFILE
		    byPacked=getc(f); // packing method: 0=unpacked, 1=RunLen, 2=LZ77
		    if(byType==6||byType==5&&byPacked<2)
		    {
			memset(&bmfh,0,sizeof(bmfh));
			memset(&bmih,0,sizeof(bmih));
			bmfh.bfType=0x4D42; // bitmap magic ("BM")
			bmih.biSize=sizeof(bmih);
			w=GetCDWord(f);
			bmih.biXPelsPerMeter=(w*79L+1)/2;
			h=GetCDWord(f);
			bmih.biYPelsPerMeter=(h*79L+1)/2;
			bmih.biPlanes=GetCWord(f);
			bmih.biBitCount=GetCWord(f);
			bmih.biWidth=GetCDWord(f);
			bmih.biHeight=GetCDWord(f);
			colors=(int)(bmih.biClrUsed=GetCDWord(f));
			if(!colors) colors=1<<bmih.biBitCount;
			bmih.biClrImportant=GetCDWord(f);
			dwDataSize=GetCDWord(f);
			dwHotspotSize=GetCDWord(f);
			GetDWord(f); // dwPictureOffset
			dwHotspotOffset=GetDWord(f);
			if(dwHotspotOffset&&n>1) // dwHotspotOffset
			{
			    // save as SHG
			    sprintf(filename+l,".S%02d",j);
			    fTarget=fopen(filename+m,"wb");
			    if(fTarget)
			    {
				fprintf(stderr,"Extracting Segmented Hotspot Graphic '%s'...\n",filename+m);
				putw(0x506C,fTarget);
				putw(1,fTarget);
				putw(8,fTarget);
				putw(0,fTarget);
				fseek(f,offset,SEEK_SET);
				copy(f,dwHotspotOffset+dwHotspotSize,fTarget);
				fclose(fTarget);
			    }
			    else
			    {
				fprintf(stderr,"Can not create '%s'\n",filename+m);
			    }
			}
			else
			{
			    sprintf(filename+l,".%03d",j);
			    if(w==96&&h==48&&!res[0])
			    {
				strcpy(filename+l,".CGA");
				res[0]=TRUE;
			    }
			    else if(w==96&&h==72&&!res[1])
			    {
				strcpy(filename+l,".EGA");
				res[1]=TRUE;
			    }
			    else if(w==96&&h==96&&!res[2])
			    {
				strcpy(filename+l,".VGA");
				res[2]=TRUE;
			    }
			    else if(w==120&&h==120&&!res[3])
			    {
				strcpy(filename+l,".854");
				res[3]=TRUE;
			    }
			    else if(w==72&&h==72&&!res[4])
			    {
				strcpy(filename+l,".MAC");
				res[4]=TRUE;
			    }
			    else if(!res[6])
			    {
				strcpy(filename+l,".BMP");
				res[6]=TRUE;
			    }
			    fTarget=fopen(filename+m,"wb");
			    if(fTarget)
			    {
				fprintf(stderr,"Extracting %ld x %ld x %u Bitmap '%s' (%ldx%ld dpi)...\n",bmih.biWidth,bmih.biHeight,bmih.biPlanes*bmih.biBitCount,filename+m,w,h);
				fwrite(&bmfh,1,sizeof(bmfh),fTarget);
				fwrite(&bmih,1,sizeof(bmih),fTarget);
				if(byType==6)
				{
				    copy(f,colors*4L,fTarget);
				}
				else
				{
				    long l;

				    l=0x000000L;
				    fwrite(&l,1,sizeof(l),fTarget);
				    l=0xFFFFFFL;
				    fwrite(&l,1,sizeof(l),fTarget);
				}
				bmfh.bfOffBits=sizeof(bmfh)+sizeof(bmih)+colors*4L;
				bmih.biSizeImage=(((bmih.biWidth*bmih.biBitCount+31)/32)*4)*bmih.biHeight;
				if(byType==5) // convert 3.0 DDB to 3.1 DIB
				{
				    long width,length,l;
				    int pad;

				    width=((bmih.biWidth*bmih.biBitCount+15)/16)*2;
				    pad=(int)(((width+3)/4)*4-width);
				    GetPackedByte(NULL);
				    for(l=0;l<bmih.biHeight;l++)
				    {
					if(byPacked==1)
					{
					    for(length=0;length<width;length++) putc(GetPackedByte(f),fTarget);
					}
					else
					{
					    copy(f,width,fTarget);
					}
					if(pad) fwrite("    ",pad,1,fTarget);
				    }
				}
				else
				{
				    decompress(byPacked,f,dwDataSize,fTarget);
				}
				// update bitmap headers
				bmfh.bfSize=ftell(fTarget);
				fseek(fTarget,0L,SEEK_SET);
				fwrite(&bmfh,1,sizeof(bmfh),fTarget);
				fwrite(&bmih,1,sizeof(bmih),fTarget);
				fclose(fTarget);
				if(dwHotspotOffset)
				{
				    fseek(f,offset+dwHotspotOffset,SEEK_SET);
				    PrintHotspotInfo(f);
				}
			    }
			    else
			    {
				fprintf(stderr,"Can not create '%s'\n",filename+m);
			    }
			}
		    }
		    else if(byType==8) // Windows MetaFile
		    {
			APMFILEHEADER afh;
			unsigned short *wp;

			memset(&afh,0,sizeof(afh));
			afh.dwKey=0x9AC6CDD7L;
			GetCWord(f); // mapping mode
			afh.rcBBox.right=getw(f); // width of metafile-picture
			afh.rcBBox.bottom=getw(f); // height of metafile-picture
			GetCDWord(f);
			dwDataSize=GetCDWord(f);
			dwHotspotSize=GetCDWord(f); // dwHotspotSize
			GetDWord(f); // dwPictureOffset
			dwHotspotOffset=GetDWord(f);
			if(dwHotspotOffset&&n>1) // dwHotspotOffset
			{
			    sprintf(filename+l,".S%02d",j);
			    fTarget=fopen(filename+m,"wb");
			    if(fTarget)
			    {
				fprintf(stderr,"Extracting Segmented Hotspot Graphic '%s'...\n",filename+m);
				putw(0x506C,fTarget);
				putw(1,fTarget);
				putw(8,fTarget);
				putw(0,fTarget);
				fseek(f,offset,SEEK_SET);
				copy(f,dwHotspotOffset+dwHotspotSize,fTarget);
				fclose(fTarget);
			    }
			    else
			    {
				fprintf(stderr,"Can not create '%s'\n",filename+m);
			    }
			}
			else
			{
			    afh.wInch=2540;
			    wp=(unsigned short *)&afh;
			    for(k=0;k<10;k++) afh.wChecksum^=*wp++;
			    if(!res[5])
			    {
				strcpy(filename+l,".WMF");
				res[5]=TRUE;
			    }
			    else
			    {
				sprintf(filename+l,".%03d",j);
			    }
			    fTarget=fopen(filename+m,"wb");
			    if(fTarget)
			    {
				fprintf(stderr,"Extracting Metafile '%s'...\n",filename+m);
				fwrite(&afh,1,sizeof(afh),fTarget);
				decompress(byPacked,f,dwDataSize,fTarget);
				fclose(fTarget);
			    }
			    if(dwHotspotOffset)
			    {
				fseek(f,offset+dwHotspotOffset,SEEK_SET);
				PrintHotspotInfo(f);
			    }
			}
		    }
		    else
		    {
			fprintf(stderr,"Picture %d of '%s' unknown type %02x pack=%02x skipped.\n",j,argv[i],byType,byPacked);
		    }
		}
	    }
	    fclose(f);
	}
    }
    return 0;
}
