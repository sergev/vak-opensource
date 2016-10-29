/* HELPDECO.H - declares functions stored external in HELPDEC1.C */
#ifndef HELPDECO_H
#define HELPDECO_H
#include <time.h>
//#include <malloc.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
//#include <conio.h>
#include <ctype.h>

#define _MAX_DIR    512
#define _MAX_FNAME  256
#define _MAX_EXT    32
#define _MAX_DRIVE  8
#define _cdecl      /*empty*/

#ifdef __TURBOC__
typedef struct { char a,b,c; } align;
#if sizeof(align)!=3
#error Compile bytealigned !
#endif
#else
#pragma pack(1)
#endif

typedef enum {FALSE,TRUE} BOOL;

typedef struct               /* structure at beginning of help file */
{
    long Magic;              /* 0x00035F3F */
    long DirectoryStart;     /* offset of FILEHEADER of internal direcory */
    long FreeChainStart;     /* offset of FILEHEADER or -1L */
    long EntireFileSize;     /* size of entire help file in bytes */
}
HELPHEADER;

typedef struct FILEHEADER    /* structure at FileOffset of each internal file */
{
    long ReservedSpace;      /* reserved space in help file incl. FILEHEADER */
    long UsedSpace;          /* used space in help file excl. FILEHEADER */
    unsigned char FileFlags; /* normally 4 */
}
FILEHEADER;

typedef struct BTREEHEADER   /* structure after FILEHEADER of each Btree */
{
    unsigned short Magic;    /* 0x293B */
    unsigned short Flags;    /* bit 0x0002 always 1, bit 0x0400 1 if direcory */
    unsigned short PageSize; /* 0x0400=1k if directory, 0x0800=2k else */
    unsigned char Structure[16]; /* string describing structure of data */
    short MustBeZero;        /* 0 */
    short PageSplits;        /* number of page splits Btree has suffered */
    short RootPage;          /* page number of Btree root page */
    short MustBeNegOne;      /* 0xFFFF */
    short TotalPages;        /* number of Btree pages */
    short NLevels;           /* number of levels of Btree */
    long TotalBtreeEntries;  /* number of entries in Btree */
}
BTREEHEADER;

typedef struct BTREEINDEXHEADER /* structure at beginning of every index-page */
{
    unsigned short Unknown;  /* sorry, no ID to identify an index-page */
    short NEntries;          /* number of entries in this index-page */
    short PreviousPage;      /* page number of previous page */
}
BTREEINDEXHEADER;

typedef struct BTREENODEHEADER /* structure at beginning of every leaf-page */
{
    unsigned short Unknown;  /* Sorry, no ID to identify a leaf-page */
    short NEntries;          /* number of entires in this leaf-page */
    short PreviousPage;      /* page number of preceeding leaf-page or -1 */
    short NextPage;          /* page number of next leaf-page or -1 */
}
BTREENODEHEADER;

typedef struct SYSTEMHEADER  /* structure at beginning of |SYSTEM file */
{
    unsigned short Magic;    /* 0x036C */
    unsigned short Minor;    /* help file format version number */
    unsigned short Major;    /* 1 */
    time_t GenDate;          /* date/time the help file was generated or 0 */
    unsigned short Flags;    /* tells you how the help file is compressed */
}
SYSTEMHEADER;

typedef struct               /* internal structure */
{
    FILE *File;
    long SavePos;
    long Remaining;
    unsigned short RecordType; /* type of data in record */
    unsigned short DataSize;   /* size of data */
    char Data[10];
}
SYSTEMRECORD;

typedef struct SECWINDOW     /* structure of data following RecordType 6 */
{
    unsigned short Flags;    /* flags (see below) */
    char Type[10];           /* type of window */
    char Name[9];            /* window name */
    char Caption[51];        /* caption for window */
    short X;                 /* x coordinate of window (0..1000) */
    short Y;                 /* y coordinate of window (0..1000) */
    short Width;             /* width of window (0..1000) */
    short Height;            /* height of window (0..1000) */
    short Maximize;          /* maximize flag and window styles */
    unsigned char Rgb[3];    /* color of scrollable region */
    unsigned char Unknown1;
    unsigned char RgbNsr[3]; /* color of non-scrollable region */
    unsigned char Unknown2;
}
SECWINDOW;

typedef struct
{
    unsigned short Flags;    /* flags (see below) */
    char Type[10];           /* type of window */
    char Name[9];            /* window name */
    char Caption[51];        /* caption for window */
    unsigned char MoreFlags;
    short X;                 /* x coordinate of window (0..1000) */
    short Y;                 /* y coordinate of window (0..1000) */
    short Width;             /* width of window (0..1000) */
    short Height;            /* height of window (0..1000) */
    short Maximize;          /* maximize flag and window styles */
    unsigned char TopRgb[3];
    unsigned char Unknown0;
    unsigned char Unknown[25];
    unsigned char Rgb[3];    /* color of scrollable region */
    unsigned char Unknown1;
    unsigned char RgbNsr[3]; /* color of non-scrollable region */
    unsigned char Unknown2;
    short X2;
    short Y2;
    short Width2;
    short Height2;
    short X3;
    short Y3;
}
MVBWINDOW;

typedef struct               /* structure of data following RecordType 14 */
{
    char btreename[10];
    char mapname[10];
    char dataname[10];
    char title[80];
}
KEYINDEX;

#define WSYSFLAG_TYPE           0x0001  /* Type is valid */
#define WSYSFLAG_NAME           0x0002  /* Name is valid */
#define WSYSFLAG_CAPTION        0x0004  /* Caption is valid */
#define WSYSFLAG_X              0x0008  /* X is valid */
#define WSYSFLAG_Y              0x0010  /* Y is valid */
#define WSYSFLAG_WIDTH          0x0020  /* Width is valid */
#define WSYSFLAG_HEIGHT         0x0040  /* Height is valid */
#define WSYSFLAG_MAXIMIZE       0x0080  /* Maximize is valid */
#define WSYSFLAG_RGB            0x0100  /* Rgb is valid */
#define WSYSFLAG_RGBNSR         0x0200  /* RgbNsr is valid */
#define WSYSFLAG_TOP            0x0400  /* On top was set in HPJ file */
#define WSYSFLAG_AUTOSIZEHEIGHT 0x0800  /* Auto-Size Height */

typedef struct PHRINDEXHDR   /* structure of beginning of |PhrIndex file */
{
    long always4A01;              /* sometimes 0x0001 */
    long entries;                 /* number of phrases */
    long compressedsize;          /* size of PhrIndex file */
    long phrimagesize;            /* size of decompressed PhrImage file */
    long phrimagecompressedsize;  /* size of PhrImage file */
    long always0;
    unsigned short bits:4;
    unsigned short unknown:12;
    unsigned short always4A00;    /* sometimes 0x4A01, 0x4A02 */
}
PHRINDEXHDR;

typedef struct FONTHEADER    /* structure of beginning of |FONT file */
{
    unsigned short NumFacenames;       /* number of face names */
    unsigned short NumDescriptors;     /* number of font descriptors */
    unsigned short FacenamesOffset;    /* offset of face name array */
    unsigned short DescriptorsOffset;  /* offset of descriptors array */
    unsigned short NumFormats;         /* only if FacenamesOffset >= 12 */
    unsigned short FormatsOffset;      /* offset of formats array */
    unsigned short NumCharmaps;        /* only if FacenamesOffset >= 16 */
    unsigned short CharmapsOffset;     /* offset of charmapnames array */
}
FONTHEADER;

typedef struct FONTDESCRIPTOR /* internal font descriptor */
{
    unsigned char Bold;
    unsigned char Italic;
    unsigned char Underline;
    unsigned char StrikeOut;
    unsigned char DoubleUnderline;
    unsigned char SmallCaps;
    unsigned char HalfPoints;
    unsigned char FontFamily;
    unsigned short FontName;
    unsigned char textcolor;
    unsigned char backcolor;
    unsigned short style;
    short expndtw;
    signed char up;
}
FONTDESCRIPTOR;

typedef struct                /* non-Multimedia font descriptor */
{
    unsigned char Attributes; /* Font Attributes See values below */
    unsigned char HalfPoints; /* PointSize * 2 */
    unsigned char FontFamily; /* Font Family. See values below */
    unsigned short FontName;  /* Number of font in Font List */
    unsigned char FGRGB[3];   /* RGB values of foreground */
    unsigned char BGRGB[3];   /* unused background RGB Values */
}
OLDFONT;

typedef struct NEWFONT        /* structure located at DescriptorsOffset */
{
    unsigned char unknown1;
    short FontName;
    unsigned char FGRGB[3];
    unsigned char BGRGB[3];
    unsigned char unknown5;
    unsigned char unknown6;
    unsigned char unknown7;
    unsigned char unknown8;
    unsigned char unknown9;
    long Height;
    unsigned char mostlyzero[12];
    short Weight;
    unsigned char unknown10;
    unsigned char unknown11;
    unsigned char Italic;
    unsigned char Underline;
    unsigned char StrikeOut;
    unsigned char DoubleUnderline;
    unsigned char SmallCaps;
    unsigned char unknown17;
    unsigned char unknown18;
    unsigned char PitchAndFamily;
}
NEWFONT;

typedef struct
{
    unsigned short StyleNum;
    unsigned short BasedOn;
    NEWFONT font;
    char unknown[35];
    char StyleName[65];
}
NEWSTYLE;

typedef struct MVBFONT        /* structure located at DescriptorsOffset */
{
    short FontName;
    short expndtw;
    unsigned short style;
    unsigned char FGRGB[3];
    unsigned char BGRGB[3];
    long Height;
    unsigned char mostlyzero[12];
    short Weight;
    unsigned char unknown10;
    unsigned char unknown11;
    unsigned char Italic;
    unsigned char Underline;
    unsigned char StrikeOut;
    unsigned char DoubleUnderline;
    unsigned char SmallCaps;
    unsigned char unknown17;
    unsigned char unknown18;
    unsigned char PitchAndFamily;
    unsigned char unknown20;
    signed char up;
}
MVBFONT;

typedef struct
{
    unsigned short StyleNum;
    unsigned short BasedOn;
    MVBFONT font;
    char unknown[35];
    char StyleName[65];
}
MVBSTYLE;

typedef struct
{
    unsigned short Magic;     /* 0x5555 */
    unsigned short Size;
    unsigned short Unknown1;
    unsigned short Unknown2;
    unsigned short Entries;
    unsigned short Ligatures;
    unsigned short LigLen;
    unsigned short Unknown[13];
}
CHARMAPHEADER;
/* Font Attributes */
#define FONT_NORM 0x00 /* Normal */
#define FONT_BOLD 0x01 /* Bold */
#define FONT_ITAL 0x02 /* Italics */
#define FONT_UNDR 0x04 /* Underline */
#define FONT_STRK 0x08 /* Strike Through */
#define FONT_DBUN 0x10 /* Dbl Underline */
#define FONT_SMCP 0x20 /* Small Caps */
/* Font Families */
#define FAM_MODERN 0x01
#define FAM_ROMAN  0x02
#define FAM_SWISS  0x03
#define FAM_TECH   0x03
#define FAM_NIL    0x03
#define FAM_SCRIPT 0x04
#define FAM_DECOR  0x05

typedef struct KWMAPREC       /* structure of |xWMAP leaf-page entries */
{
    long FirstRec;            /* index number of first keyword on leaf page */
    unsigned short PageNum;   /* page number that keywords are associated with */
}
KWMAPREC;

typedef long TOPICPOS;        /* TOPICPOS/DecompressSize = block number, TOPICPOS%DecompressSize = offset into decompression buffer (including sizeof(TOPICBLOCKHEADER)) */

typedef long TOPICOFFSET;     /* TOPICOFFSET/0x8000 = block number, TOPICOFFSET/0x8000 = number of characters and hotspots counting from first TOPICLINK of this block */

typedef struct                /* structure every TopicBlockSize in |TOPIC */
{
    TOPICPOS LastTopicLink;   /* points to last TOPICLINK in previous block */
    TOPICPOS FirstTopicLink;  /* points to first TOPICLINK in this block */
    TOPICPOS LastTopicHeader; /* points to TOPICLINK of last TOPICHEADER */
}
TOPICBLOCKHEADER;

typedef struct                /* structure pointed to by FirstTopicLink */
{
    long BlockSize;           /* size of this link + LinkData1 + LinkData2 */
    long DataLen2;            /* length of decompressed LinkData2 */
    TOPICPOS PrevBlock;
    /* Windows 3.0 (HC30): number of bytes the TOPICLINK of the previous
    // block is located before this TOPICLINK, that is the block size of
    // the previous TOPICLINK plus eventually skipped TOPICBLOCKHEADER.
    // Windows 3.1 (HC31): TOPICPOS of previous TOPICLINK */
    TOPICPOS NextBlock;
    /* Windows 3.0 (HC30): number of bytes the TOPICLINK of the next block
    // is located behind this block, including skipped TOPICBLOCKHEADER.
    // Windows 3.1 (HC31): TOPICPOS of next TOPICLINK */
    long DataLen1;            /* includes size of TOPICLINK */
    unsigned char RecordType; /* See below */
}
TOPICLINK;
/* Known RecordTypes for TOPICLINK */
#define TL_DISPLAY30 0x01     /* version 3.0 displayable information */
#define TL_TOPICHDR  0x02     /* topic header information */
#define TL_DISPLAY   0x20     /* version 3.1 displayable information */
#define TL_TABLE     0x23     /* version 3.1 table */

typedef struct                /* structure of LinkData1 of RecordType 2 */
{
    long BlockSize;        /* size of topic, including internal topic links */
    TOPICOFFSET BrowseBck; /* topic offset for prev topic in browse sequence */
    TOPICOFFSET BrowseFor; /* topic offset for next topic in browse sequence */
    long TopicNum;         /* topic Number */
    TOPICPOS NonScroll;    /* start of non-scrolling region (topic offset) or -1 */
    TOPICPOS Scroll;       /* start of scrolling region (topic offset) */
    TOPICPOS NextTopic;    /* start of next type 2 record */
}
TOPICHEADER;

typedef struct                /* structure of LinkData1 of RecordType 2 */
{
    long BlockSize;
    short PrevTopicNum;
    short unused1;
    short NextTopicNum;
    short unused2;
}
TOPICHEADER30;

typedef struct                /* structure of |CTXOMAP file entries */
{
    long MapID;
    long TopicOffset;
}
CTXOMAPREC;

typedef struct                /* structure of |CONTEXT leaf-page entry */
{
    long HashValue;           /* Hash value of context id */
    TOPICOFFSET TopicOffset;  /* Topic offset */
}
CONTEXTREC;

typedef struct                /* structure of *.GRP file header */
{
    unsigned long Magic;      /* 0x000A3333 */
    unsigned long BitmapSize;
    unsigned long LastTopic;
    unsigned long FirstTopic;
    unsigned long TopicsUsed;
    unsigned long TopicCount;
    unsigned long GroupType;
    unsigned long Unknown1;
    unsigned long Unknown2;
    unsigned long Unknown3;
}
GROUPHEADER;

typedef struct                /* internal use */
{
    GROUPHEADER GroupHeader;
    char *Name;
    unsigned char *Bitmap;
}
GROUP;

typedef struct                /* structure of STOPn.STP header */
{
    unsigned long Magic;      /* 0x00082222 */
    unsigned short BytesUsed;
    unsigned short Unused[17];
}
STOPHEADER;

typedef struct                /* structure of |VIOLA leaf-page entry */
{
    TOPICOFFSET TopicOffset;  /* topic offset */
    long WindowNumber;        /* number of window assigned to topic */
}
VIOLAREC;

typedef struct                /* structure of |CATALOG header */
{
   unsigned short magic;      /* 0x1111 */
   unsigned short always8;
   unsigned short always4;
   long entries;
   unsigned char zero[30];
}
CATALOGHEADER;

typedef struct                /* structure of Windows Bitmap BMP file */
{
    unsigned short bfType;
    unsigned long bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned long bfOffBits;
}
BITMAPFILEHEADER;

typedef struct                /* structure of Windows Bitmap header */
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

typedef struct tagRECT        /* Windows rectangle */
{
    short left;
    short top;
    short right;
    short bottom;
}
RECT;

typedef struct                /* Windows Aldus placeable metafile header */
{
    unsigned long dwKey;
    unsigned short hMF;
    RECT rcBBox;
    unsigned short wInch;
    unsigned long dwReserved;
    unsigned short wChecksum;
}
APMFILEHEADER;

typedef struct                /* structure of hotspot info */
{
    unsigned char id0,id1,id2;
    unsigned short x,y,w,h;
    unsigned long hash;
}
HOTSPOT;

typedef struct                /* structure used as buf of GetFirstPage */
{
    long FirstLeaf;
    unsigned short PageSize;
    short NextPage;
}
BUFFER;

typedef struct                /* internal use. 16 bit: max. 3640 */
{
    long StartTopic;
    long NextTopic;
    long PrevTopic;
    short BrowseNum;
    short Start;
    short Count;
}
BROWSE;

typedef struct                /* internal use. 16 bit: max. 8191 */
{
    long StartTopic;
    short BrowseNum;
    short Start;
}
START;

typedef struct                /* internal use. 16 bit: max. 6553 */
{
    char *name;
    long hash;
    BOOL derived;
}
HASHREC;

typedef struct                /* internal use to store keyword definitions */
{
    BOOL KeyIndex;
    char Footnote;
    char *Keyword;
    long TopicOffset;
}
KEYWORDREC;

typedef struct placerec       /* internal use to store external references */
{
   struct placerec *next;
   char topicname[1];
}
PLACEREC;

typedef struct checkrec       /* internal use to store external references */
{
    struct checkrec *next;
    enum { TOPIC, CONTEXT } type;
    long hash;
    char *id;
    PLACEREC *here;
}
CHECKREC;

typedef struct fileref        /* internal use to store external references */
{
    struct fileref *next;
    CHECKREC *check;
    char filename[1];
}
FILEREF;

typedef struct                /* internal use */
{
    TOPICOFFSET TopicOffset;
    TOPICOFFSET OtherTopicOffset;
}
ALTERNATIVE;

typedef struct mfile          /* a class would be more appropriate */
{
    FILE *f;
    char *ptr;
    char *end;
    int (*get)(struct mfile *);
    int (*put)(struct mfile *,char);
    size_t (*read)(struct mfile *,void *,long);
    long (*tell)(struct mfile *);
    void (*seek)(struct mfile *,long);
}
MFILE;

extern void error(char *format,...);
extern void *my_malloc(long bytes); /* save malloc function */
extern void *my_realloc(void *ptr,long bytes); /* save realloc function */
extern char *my_strdup(char *ptr); /* save strdup function */
extern size_t my_fread(void *ptr,long bytes,FILE *f); /* save fread function */
extern size_t my_gets(char *ptr,size_t size,FILE *f);  /* read nul terminated string from regular file */
extern void my_fclose(FILE *f); /* checks if disk is full */
extern FILE *my_fopen(const char *filename,const char *mode); /* save fopen function */
extern unsigned short my_getw(FILE *f); /* get 16 bit quantity */
extern unsigned long getdw(FILE *f); /* get long */
extern void my_putw(unsigned short w,FILE *f); /* write 16 bit quantity */
extern void putdw(unsigned long x,FILE *f); /* write long to file */
extern void putcdw(unsigned long x,FILE *f); /* write compressed long to file */
extern void putcw(unsigned int x,FILE *f); /* write compressed word to file */
extern int MemoryPut(MFILE *f,char c); /* put char to memory mapped file */
extern int FilePut(MFILE *f,char c); /* put char to regular file */
extern int MemoryGet(MFILE *f); /* get char from memory mapped file */
extern int FileGet(MFILE *f); /* get char from regular file */
extern size_t MemoryRead(MFILE *f,void *ptr,long bytes); /* read function for memory mapped file */
extern size_t FileRead(MFILE *f,void *ptr,long bytes); /* read function for regular file */
extern long MemoryTell(MFILE *f); /* tell for memory mapped file */
extern long FileTell(MFILE *f); /* tell for regular file */
extern void MemorySeek(MFILE *f,long offset); /* seek in memory mapped file */
extern void FileSeek(MFILE *f,long offset); /* seek in regular file */
extern MFILE *CreateMap(char *ptr,size_t size); /* assign a memory mapped file */
extern MFILE *CreateVirtual(FILE *f);  /* assign a real file */
extern void CloseMap(MFILE *f); /* close a MFILE */
extern int GetWord(MFILE *f); /* read 16 bit value from memory mapped file or regular file */
extern unsigned short GetCWord(MFILE *f); /* get compressed word from memory mapped file or regular file */
extern unsigned long GetCDWord(MFILE *f); /* get compressed long from memory mapped file or regular file */
extern unsigned long GetDWord(MFILE *f); /* get long from memory mapped file or regular file */
extern size_t StringRead(char *ptr,size_t size,MFILE *f); /* read nul terminated string from memory mapped or regular file */
extern long copy(FILE *f,long bytes,FILE *out);
extern long CopyBytes(MFILE *f,long bytes,FILE *out);
extern long decompress(int method,MFILE *f,long bytes,MFILE *fTarget);
extern long DecompressIntoBuffer(int method,FILE *HelpFile,long bytes,char *ptr,long size);
extern long DecompressIntoFile(int method,MFILE *f,long bytes,FILE *fTarget);
extern void HexDump(FILE *f,long FileLength,long offset);
extern void HexDumpMemory(unsigned char *bypMem,unsigned int FileLength);
extern char *PrintString(char *str,unsigned int len);
extern BOOL GetBit(FILE *f);
extern void putrtf(FILE *rtf,char *str);
extern short scanint(char **ptr); /* scan a compressed short */
extern unsigned short scanword(char **ptr); /* scan a compressed unsiged short */
extern long scanlong(char **ptr);  /* scan a compressed long */
extern BOOL SearchFile(FILE *HelpFile,char *FileName,long *FileLength);
extern short GetFirstPage(FILE *HelpFile,BUFFER *buf,long *TotalEntries);
extern short GetNextPage(FILE *HelpFile,BUFFER *buf); /* walk Btree */
extern SYSTEMRECORD *GetNextSystemRecord(SYSTEMRECORD *SysRec);
extern SYSTEMRECORD *GetFirstSystemRecord(FILE *HelpFile);
extern void ListFiles(FILE *HelpFile); /* display internal directory */
extern void ListBaggage(FILE *HelpFile,FILE *hpj,BOOL before31); /* writes out [BAGGAGE] section */
extern void PrintWindow(FILE *hpj,SECWINDOW *SWin);
extern void PrintMVBWindow(FILE *hpj,MVBWINDOW *SWin);
extern void ToMapDump(FILE *HelpFile,long FileLength);
extern void GroupDump(FILE *HelpFile);
extern void KWMapDump(FILE *HelpFile);
extern void KWDataDump(FILE *HelpFile,long FileLength);
extern void CatalogDump(FILE *HelpFile);
extern void CTXOMAPDump(FILE *HelpFile);
extern void LinkDump(FILE *HelpFile);
extern void AnnotationDump(FILE *HelpFile,long FileLength,char *name);

#endif
