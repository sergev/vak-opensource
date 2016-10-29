/* HELPDEC1.C - HELPDECO supporting functions */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
//#include <conio.h>
#include <ctype.h>
#include "helpdeco.h"

extern BOOL overwrite; /* ugly: declared in HELPDECO.C */

void error(char *format,...)
{
    va_list arg;

    fputs("\n",stderr);
    va_start(arg,format);
    vfprintf(stderr,format,arg);
    va_end(arg);
    //fputs("\nPress CR to continue at your own risk, any other key to exit.\n",stderr);
    //if(getch()!='\r')
    exit(1);
}

#if 0
size_t strlcpy(char *dest,char *src,size_t len) /* limited string copy */
{
    size_t i;

    if(!dest) return 0;
    for(i=0;i<len-1&&src&&src[i];i++) dest[i]=src[i];
    dest[i]='\0';
    return i;
}
#endif

void *my_malloc(long bytes) /* save malloc function */
{
    void *ptr;

    if(bytes<1L||((size_t)bytes!=bytes)||(ptr=malloc((size_t)bytes))==NULL)
    {
	fprintf(stderr,"Allocation of %ld bytes failed. File too big.\n",bytes);
	exit(1);
    }
    return ptr;
}

void *my_realloc(void *ptr,long bytes) /* save realloc function */
{
    if(!ptr) return my_malloc(bytes);
    if(bytes<1L||bytes!=(size_t)bytes||(ptr=realloc(ptr,(size_t)bytes))==NULL)
    {
	fprintf(stderr,"Reallocation to %ld bytes failed. File too big.\n",bytes);
	exit(1);
    }
    return ptr;
}

char *my_strdup(char *ptr) /* save strdup function */
{
    size_t len;
    char *dup;

    if(!ptr) return NULL;
    len=strlen(ptr);
    dup=my_malloc(len+1);
    strcpy(dup,ptr);
    return dup;
}

size_t my_fread(void *ptr,long bytes,FILE *f) /* save fread function */
{
    size_t result;

    if(bytes==0) return 0;
    if(bytes<0||bytes!=(size_t)bytes||(result=fread(ptr,1,(size_t)bytes,f))!=bytes)
    {
	error("my_fread(%ld) at %ld failed",bytes,ftell(f));
    }
    return result;
}

size_t my_gets(char *ptr,size_t size,FILE *f)  /* read nul terminated string from regular file */
{
    size_t i;
    int c;

    i=0;
    while((c=getc(f))>0)
    {
	if(i>=size-1)
	{
	    fputs("String length exceeds decompiler limit.\n",stderr);
	    exit(1);
	}
	ptr[i++]=c;
    }
    ptr[i]='\0';
    return i;
}

void my_fclose(FILE *f) /* checks if disk is full */
{
    if(ferror(f)!=0)
    {
	fputs("File write error. Program aborted.\n",stderr);
	exit(2);
    }
    fclose(f);
}

FILE *my_fopen(const char *filename,const char *mode) /* save fopen function */
{
    FILE *f;
    char ch;

    if(!overwrite)
    {
	f=fopen(filename,"rb");
	if(f)
	{
	    fclose(f);
#if 0
	    fprintf(stderr,"File %s already exists. Overwrite (Y/N/All/Quit) ? Y\b",filename);
	    do
	    {
		ch=toupper(getch());
		if(ch=='\r') ch='Y'; else if(ch=='\x1B') ch='N';
	    }
	    while(ch!='Q'&&ch!='A'&&ch!='Y'&&ch!='N');
	    printf("%c\n",ch);
	    if(ch=='Q') exit(0);
	    if(ch=='A') overwrite=TRUE;
	    if(ch=='N') return NULL;
#else
	    fprintf(stderr, "File %s already exists: overwritten.\n", filename);
	    overwrite = TRUE;
#endif
	}
    }
    f=fopen(filename,mode);
    if(!f)
    {
	error("Can not create '%s'.",filename);
    }
    else
    {
	fprintf(stderr,"Creating %s...\n",filename);
    }
    return f;
}

unsigned short my_getw(FILE *f) /* get 16 bit quantity */
{
    int ch;

    ch=getc(f);
    return ch|(getc(f)<<8);
}

unsigned long getdw(FILE *f) /* get long */
{
    unsigned short w;

    w=my_getw(f);
    return ((unsigned long)my_getw(f)<<16)|(unsigned long)w;
}

void my_putw(unsigned short w,FILE *f) /* write 16 bit quantity */
{
    putc((w&0xFF),f);
    putc((w>>8),f);
}

void putdw(unsigned long x,FILE *f) /* write long to file */
{
    fwrite(&x,4,1,f);
}

void putcdw(unsigned long x,FILE *f) /* write compressed long to file */
{
    if(x>32767L)
    {
	my_putw((unsigned int)(x<<1)+1,f);
	my_putw(x>>15,f);
    }
    else
    {
	my_putw(x<<1,f);
    }
}

void putcw(unsigned int x,FILE *f) /* write compressed word to file */
{
    if(x>127)
    {
	my_putw((x<<1)+1,f);
    }
    else
    {
	putc(x<<1,f);
    }
}

/* HELPDECO sometimes has to work off the help file, sometimes needs to do
// the same with (decompressed) information stored in memory. MFILE and the
// memory mapped file functions allow to write the same code for both, but
// this approach needs some declarations first... */
int MemoryPut(MFILE *f,char c) /* put char to memory mapped file */
{
    if(f->ptr>=f->end) return 0;
    *f->ptr++=c;
    return 1;
}

int FilePut(MFILE *f,char c) /* put char to regular file */
{
    if(putc(c,f->f)==-1) return 0;
    return 1;
}

int MemoryGet(MFILE *f) /* get char from memory mapped file */
{
    if(f->ptr>=f->end) return -1;
    return *(unsigned char *)f->ptr++;
}

int FileGet(MFILE *f) /* get char from regular file */
{
    return getc(f->f);
}

size_t MemoryRead(MFILE *f,void *ptr,long bytes) /* read function for memory mapped file */
{
    if(bytes<0||bytes>f->end-f->ptr)
    {
	error("read(%ld) failed",bytes);
	bytes=f->end-f->ptr;
    }
    memcpy(ptr,f->ptr,bytes);
    f->ptr+=bytes;
    return bytes;
}

size_t FileRead(MFILE *f,void *ptr,long bytes) /* read function for regular file */
{
    return my_fread(ptr,bytes,f->f);
}

long MemoryTell(MFILE *f) /* tell for memory mapped file */
{
    return (long)f->ptr;
}

long FileTell(MFILE *f) /* tell for regular file */
{
    return ftell(f->f);
}

void MemorySeek(MFILE *f,long offset) /* seek in memory mapped file */
{
    f->ptr=(char *)offset;
}

void FileSeek(MFILE *f,long offset) /* seek in regular file */
{
    fseek(f->f,offset,SEEK_SET);
}

MFILE *CreateMap(char *ptr,size_t size) /* assign a memory mapped file */
{
    MFILE *f;

    f=my_malloc(sizeof(MFILE));
    f->f=NULL;
    f->ptr=ptr;
    f->end=ptr+size;
    f->get=MemoryGet;
    f->put=MemoryPut;
    f->read=MemoryRead;
    f->tell=MemoryTell;
    f->seek=MemorySeek;
    return f;
}

MFILE *CreateVirtual(FILE *f)  /* assign a real file */
{
    MFILE *mf;

    mf=my_malloc(sizeof(MFILE));
    mf->f=f;
    mf->ptr=mf->end=NULL;
    mf->get=FileGet;
    mf->put=FilePut;
    mf->read=FileRead;
    mf->tell=FileTell;
    mf->seek=FileSeek;
    return mf;
}

void CloseMap(MFILE *f) /* close a MFILE */
{
    if(f) free(f);
}

int GetWord(MFILE *f) /* read 16 bit value from memory mapped file or regular file */
{
    unsigned char b;

    b=f->get(f);
    return ((unsigned short)(f->get(f))<<8)|(unsigned short)b;
}

unsigned short GetCWord(MFILE *f) /* get compressed word from memory mapped file or regular file */
{
    unsigned char b;

    b=f->get(f);
    if(b&1) return (((unsigned short)(f->get(f))<<8)|(unsigned short)b)>>1;
    return ((unsigned short)b>>1);
}

unsigned long GetCDWord(MFILE *f) /* get compressed long from memory mapped file or regular file */
{
    unsigned short w;

    w=GetWord(f);
    if(w&1) return (((unsigned long)GetWord(f)<<16)|(unsigned long)w)>>1;
    return ((unsigned long)w>>1);
}

unsigned long GetDWord(MFILE *f) /* get long from memory mapped file or regular file */
{
    unsigned short w;

    w=GetWord(f);
    return ((unsigned long)GetWord(f)<<16)|(unsigned long)w;
}

size_t StringRead(char *ptr,size_t size,MFILE *f) /* read nul terminated string from memory mapped or regular file */
{
    size_t i;
    int c;

    i=0;
    while((c=f->get(f))>0)
    {
	if(i>=size-1)
	{
	    fputs("String length exceeds decompiler limit.\n",stderr);
	    exit(1);
	}
	ptr[i++]=c;
    }
    ptr[i]='\0';
    return i;
}

long copy(FILE *f,long bytes,FILE *out)
{
    long length;
    int size;
    static char buffer[512];

    for(length=0;length<bytes;length+=size)
    {
	size=(int)(bytes-length>sizeof(buffer)?sizeof(buffer):bytes-length);
	my_fread(buffer,size,f);
	fwrite(buffer,size,1,out);
    }
    return length;
}

long CopyBytes(MFILE *f,long bytes,FILE *out)
{
    long length;
    int size;
    static char buffer[512];

    for(length=0;length<bytes;length+=size)
    {
	size=(int)(bytes-length>sizeof(buffer)?sizeof(buffer):bytes-length);
	f->read(f,buffer,size);
	fwrite(buffer,size,1,out);
    }
    return length;
}

signed char count; /* for run len decompression */

int DeRun(MFILE *f,char c) /* expand runlen compressed data */
{
    int i;

    if(count&0x7F)
    {
	if(count&0x80)
	{
	    f->put(f,c);
	    count--;
	    return 1;
	}
	for(i=0;i<count;i++)
	{
	    f->put(f,c);
	}
	count=0;
	return i;
    }
    count=(signed char)c;
    return 0;
}

/* copies bytes from (memory mapped or regular file) f to (memory mapped or
// regular file) fTarget, decompressed using method
// 0: copy (no decompression)
// 1: runlen decompression
// 2: LZ77 decompression
// 3: runlen and LZ77 decompression
// returns number of bytes copied to fTarget. Doesn't complain if fTarget
// is a memory mapped file and buffer is full, just stops writing */
long decompress(int method,MFILE *f,long bytes,MFILE *fTarget)
{
    static unsigned char lzbuffer[0x1000];
    int (*Emit)(MFILE *f,char c);
    unsigned char bits,mask;
    int pos,len,back;
    long n;

    n=0;
    if(method&1)
    {
	Emit=DeRun;
	count=0;
    }
    else
    {
	Emit=fTarget->put;
    }
    if(method&2)
    {
	mask=0;
	pos=0;
	while(bytes-->0L)
	{
	    if(!mask)
	    {
		bits=f->get(f);
		mask=1;
	    }
	    else
	    {
		if(bits&mask)
		{
		    if(bytes--==0) break;
		    back=GetWord(f);
		    len=((back>>12)&15)+3;
		    back=pos-(back&0xFFF)-1;
		    while(len-->0)
		    {
			n+=Emit(fTarget,lzbuffer[pos++&0xFFF]=lzbuffer[back++&0xFFF]);
		    }
		}
		else
		{
		    n+=Emit(fTarget,lzbuffer[pos++&0xFFF]=f->get(f));
		}
		mask<<=1;
	    }
	}
    }
    else
    {
	while(bytes-->0L) n+=Emit(fTarget,f->get(f));
    }
    return n;
}

long DecompressIntoBuffer(int method,FILE *HelpFile,long bytes,char *ptr,long size)
{
    MFILE *f;
    MFILE *mf;

    f=CreateMap(ptr,size);
    mf=CreateVirtual(HelpFile);
    bytes=decompress(method,mf,bytes,f);
    CloseMap(mf);
    CloseMap(f);
    return bytes;
}

long DecompressIntoFile(int method,MFILE *f,long bytes,FILE *fTarget)
{
    MFILE *mf;

    mf=CreateVirtual(fTarget);
    bytes=decompress(method,f,bytes,mf);
    CloseMap(mf);
    return bytes;
}

void HexDump(FILE *f,long FileLength,long offset)
{
    unsigned char b[16];
    long l;
    int n,i;

    puts("[-Addr-] [--------------------Data---------------------] [-----Text-----]");
    fseek(f,offset,SEEK_CUR);
    for(l=offset;l<FileLength;l+=16)
    {
	printf("%08lX ",l);
	n=(int)(FileLength-l>16?16:FileLength-l);
	for(i=0;i<n;i++) printf("%02X ",b[i]=getc(f));
	while(i++<16) printf("	 ");
	for(i=0;i<n;i++) putchar(isprint(b[i])?b[i]:'.');
	putchar('\n');
    }
}

void HexDumpMemory(unsigned char *bypMem,unsigned int FileLength)
{
    unsigned char b[16];
    unsigned int l;
    int n,i;

    puts("[-Addr-] [--------------------Data---------------------] [-----Text-----]");
    for(l=0;l<FileLength;l+=16)
    {
	printf("%08X ",l);
	n=(int)(FileLength-l>16?16:FileLength-l);
	for(i=0;i<n;i++) printf("%02X ",b[i]=*bypMem++);
	while(i++<16) printf("	 ");
	for(i=0;i<n;i++) putchar(isprint(b[i])?b[i]:'.');
	putchar('\n');
    }
}

/* write str to stdout, replacing nonprintable characters with hex codes,
// returning str+len. PrintString doesn't stop at NUL characters */
char *PrintString(char *str,unsigned int len)
{
    while(len-->0)
    {
	if(isprint((unsigned char)*str))
	{
	    putchar(*str);
	}
	else
	{
	    printf("(%02x)",*(unsigned char *)str);
	}
	str++;
    }
    return str;
}

/* get next bit (lsb first) from 32 bit words in f, initialized if f = NULL */
/* important to read longs to stop at right position */
BOOL GetBit(FILE *f)
{
    static unsigned long mask;
    static unsigned long value;

    if(f)
    {
	mask<<=1;
	if(!mask)
	{
	    fread(&value,4,1,f);
	    mask=1L;
	}
    }
    else
    {
	mask=0L; /* initialize */
    }
    return (value&mask)!=0L;
}
/* output str to RTF file, escaping neccessary characters */
void putrtf(FILE *rtf,char *str)
{
    if(rtf) while(*str)
    {
	if(*str=='{'||*str=='}'||*str=='\\')
	{
	    putc('\\',rtf);
	    putc(*str++,rtf);
	}
	else if(isprint((unsigned char)*str))
	{
	    putc(*str++,rtf);
	}
	else
	{
	    fprintf(rtf, "{\\u%u}", cp1251_to_unicode(*str++));
	}
    }
}

/* scan-functions for reading compressed values from LinkData1 */
short scanint(char **ptr) /* scan a compressed short */
{
    short val;

    if (*(*ptr) & 1) {
        val = ((*(unsigned short*) *ptr) >> 1) - 0x4000;
        *ptr += 2;
    } else {
        val = ((*(unsigned char *) *ptr) >> 1) - 0x40;
        *ptr += 1;
    }
    return val;
}

unsigned short scanword(char **ptr) /* scan a compressed unsigned short */
{
    unsigned short val;

    if (*(*ptr) & 1) {
        val = (*(unsigned short*) *ptr) >> 1;
        *ptr += 2;
    } else {
        val = (*(unsigned char *) *ptr) >> 1;
        *ptr += 1;
    }
    return val;
}

long scanlong(char **ptr)  /* scan a compressed long */
{
    long val;

    if (*(*ptr) & 1) {
        val = ((*(unsigned long*) *ptr) >> 1) - 0x40000000L;
        *ptr += 4;
    } else {
        val = ((*(unsigned short*) *ptr) >> 1) - 0x4000;
        *ptr += 2;
    }
    return val;
}

/* locates internal file FileName or internal directory if FileName is NULL
// reads FILEHEADER and returns TRUE with current position in HelpFile set
// to first byte of data of FileName or returns FALSE if not found. Stores
// UsedSpace (that's the file size) in FileLength if FileLength isn't NULL */
BOOL SearchFile(FILE *HelpFile,char *FileName,long *FileLength)
{
    HELPHEADER Header;
    FILEHEADER FileHdr;
    BTREEHEADER BtreeHdr;
    BTREENODEHEADER CurrNode;
    long offset;
    char TempFile[19];
    int i,n;

    fseek(HelpFile,0L,SEEK_SET);
    my_fread(&Header,sizeof(Header),HelpFile);
    if(Header.Magic!=0x00035F3FL) return FALSE;
    fseek(HelpFile,Header.DirectoryStart,SEEK_SET);
    my_fread(&FileHdr,sizeof(FileHdr),HelpFile);
    if(!FileName)
    {
	if(FileLength) *FileLength=FileHdr.UsedSpace;
	return TRUE;
    }
    my_fread(&BtreeHdr,sizeof(BtreeHdr),HelpFile);
    offset=ftell(HelpFile);
    fseek(HelpFile,offset+BtreeHdr.RootPage*(long)BtreeHdr.PageSize,SEEK_SET);
    for(n=1;n<BtreeHdr.NLevels;n++)
    {
	my_fread(&CurrNode,sizeof(BTREEINDEXHEADER),HelpFile);
	for(i=0;i<CurrNode.NEntries;i++)
	{
	    my_gets(TempFile,sizeof(TempFile),HelpFile);
	    if(strcmp(FileName,TempFile)<0) break;
	    CurrNode.PreviousPage=my_getw(HelpFile);
	}
	fseek(HelpFile,offset+CurrNode.PreviousPage*(long)BtreeHdr.PageSize,SEEK_SET);
    }
    my_fread(&CurrNode,sizeof(CurrNode),HelpFile);
    for(i=0;i<CurrNode.NEntries;i++)
    {
	my_gets(TempFile,sizeof(TempFile),HelpFile);
	offset=getdw(HelpFile);
	if(strcmp(TempFile,FileName)==0)
	{
	    fseek(HelpFile,offset,SEEK_SET);
	    my_fread(&FileHdr,sizeof(FileHdr),HelpFile);
	    if(FileLength) *FileLength=FileHdr.UsedSpace;
	    return TRUE;
	}
    }
    return FALSE;
}

/* read first (and next) page from B+ tree. HelpFile must be positioned
// at start of internal file prior calling GetFirstPage. It will be
// positioned at first B+ tree entry after return from GetFirstPage.
// Number of TotalBtreeEntries stored in TotalEntries if pointer is
// not NULL, NumberOfEntries of first B+ tree page returned.
// buf stores position, so GetNextPage will seek to position itself. */
short GetFirstPage(FILE *HelpFile,BUFFER *buf,long *TotalEntries)
{
    int CurrLevel;
    BTREEHEADER BTreeHdr;
    BTREENODEHEADER CurrNode;

    my_fread(&BTreeHdr,sizeof(BTreeHdr),HelpFile);
    if(TotalEntries) *TotalEntries=BTreeHdr.TotalBtreeEntries;
    if(!BTreeHdr.TotalBtreeEntries) return 0;
    buf->FirstLeaf=ftell(HelpFile);
    buf->PageSize=BTreeHdr.PageSize;
    fseek(HelpFile,buf->FirstLeaf+BTreeHdr.RootPage*(long)BTreeHdr.PageSize,SEEK_SET);
    for(CurrLevel=1;CurrLevel<BTreeHdr.NLevels;CurrLevel++)
    {
	my_fread(&CurrNode,sizeof(BTREEINDEXHEADER),HelpFile);
	fseek(HelpFile,buf->FirstLeaf+CurrNode.PreviousPage*(long)BTreeHdr.PageSize,SEEK_SET);
    }
    my_fread(&CurrNode,sizeof(CurrNode),HelpFile);
    buf->NextPage=CurrNode.NextPage;
    return CurrNode.NEntries;
}

short GetNextPage(FILE *HelpFile,BUFFER *buf) /* walk Btree */
{
    BTREENODEHEADER CurrNode;

    if(buf->NextPage==-1) return 0;
    fseek(HelpFile,buf->FirstLeaf+buf->NextPage*(long)buf->PageSize,SEEK_SET);
    my_fread(&CurrNode,sizeof(CurrNode),HelpFile);
    buf->NextPage=CurrNode.NextPage;
    return CurrNode.NEntries;
}

/* reads next record from |SYSTEM file, returns NULL if no more available
// Use last system record as parameter SysRec (saves filehandle and pos) */
SYSTEMRECORD *GetNextSystemRecord(SYSTEMRECORD *SysRec)
{
    if(SysRec->Remaining<4)
    {
	free(SysRec);
	return NULL;
    }
    fseek(SysRec->File,SysRec->SavePos,SEEK_SET);
    SysRec->RecordType=my_getw(SysRec->File);
    SysRec->DataSize=my_getw(SysRec->File);
    SysRec->Remaining-=4;
    if(SysRec->Remaining<SysRec->DataSize)
    {
	free(SysRec);
	return NULL;
    }
    SysRec=my_realloc(SysRec,sizeof(SYSTEMRECORD)+SysRec->DataSize);
    my_fread(SysRec->Data,SysRec->DataSize,SysRec->File);
    SysRec->Data[SysRec->DataSize]='\0';
    SysRec->Remaining-=SysRec->DataSize;
    SysRec->SavePos=ftell(SysRec->File);
    return SysRec;
}

/* reads first record from |SYSTEM file, returns NULL if none found */
SYSTEMRECORD *GetFirstSystemRecord(FILE *HelpFile)
{
    SYSTEMHEADER SysHdr;
    SYSTEMRECORD *SysRec;
    long FileLength;

    if(!SearchFile(HelpFile,"|SYSTEM",&FileLength)) return NULL;
    my_fread(&SysHdr,sizeof(SysHdr),HelpFile);
    if(SysHdr.Major!=1||SysHdr.Minor<16) return NULL;
    SysRec=my_malloc(sizeof(SYSTEMRECORD));
    SysRec->File=HelpFile;
    SysRec->SavePos=ftell(HelpFile);
    SysRec->Remaining=FileLength-sizeof(SYSTEMHEADER);
    return GetNextSystemRecord(SysRec);
}

void ListFiles(FILE *HelpFile) /* display internal directory */
{
    BUFFER buf;
    char FileName[20];
    int j,i,n;

    puts("FileName		  FileOffset | FileName 	       FileOffset");
    puts("-----------------------------------+-----------------------------------");
    j=0;
    for(n=GetFirstPage(HelpFile,&buf,NULL);n;n=GetNextPage(HelpFile,&buf))
    {
	for(i=0;i<n;i++)
	{
	    my_gets(FileName,sizeof(FileName),HelpFile);
	    printf("%-23s 0x%08lX",FileName,getdw(HelpFile));
	    if(j++&1) putchar('\n'); else printf(" | ");
	}
    }
    if(j&1) putchar('\n');
}

void ListBaggage(FILE *HelpFile,FILE *hpj,BOOL before31) /* writes out [BAGGAGE] section */
{
    BOOL headerwritten;
    char *leader;
    char FileName[20];
    long FileLength;
    BUFFER buf;
    int i,n;
    FILE *f;
    long savepos;

    headerwritten=FALSE;
    leader="|bm"+before31;
    SearchFile(HelpFile,NULL,NULL);
    for(n=GetFirstPage(HelpFile,&buf,NULL);n;n=GetNextPage(HelpFile,&buf))
    {
	for(i=0;i<n;i++)
	{
	    my_gets(FileName,sizeof(FileName),HelpFile);
	    getdw(HelpFile);
	    if(FileName[0]!='|'&&memcmp(FileName,leader,strlen(leader))!=0&&!strstr(FileName,".GRP")&&!strstr(FileName,".tbl"))
	    {
		savepos=ftell(HelpFile);
		if(SearchFile(HelpFile,FileName,&FileLength))
		{
		    if(!headerwritten)
		    {
			fputs("[BAGGAGE]\n",hpj);
			headerwritten=TRUE;
		    }
		    fprintf(hpj,"%s\n",FileName);
		    f=my_fopen(FileName,"wb");
		    if(f)
		    {
			copy(HelpFile,FileLength,f);
			my_fclose(f);
		    }
		}
		fseek(HelpFile,savepos,SEEK_SET);
	    }
	}
    }
    if(headerwritten) putc('\n',hpj);
}

void PrintWindow(FILE *hpj,SECWINDOW *SWin)
{
    if(SWin->Flags&WSYSFLAG_NAME) fprintf(hpj,"%s",SWin->Name);
    putc('=',hpj);
    if(SWin->Flags&WSYSFLAG_CAPTION) fprintf(hpj,"\"%s\"",SWin->Caption);
    putc(',',hpj);
    if(SWin->Flags&(WSYSFLAG_X|WSYSFLAG_Y|WSYSFLAG_WIDTH|WSYSFLAG_HEIGHT))
    {
	putc('(',hpj);
	if(SWin->Flags&WSYSFLAG_X) fprintf(hpj,"%d",SWin->X);
	putc(',',hpj);
	if(SWin->Flags&WSYSFLAG_Y) fprintf(hpj,"%d",SWin->Y);
	putc(',',hpj);
	if(SWin->Flags&WSYSFLAG_WIDTH) fprintf(hpj,"%d",SWin->Width);
	putc(',',hpj);
	if(SWin->Flags&WSYSFLAG_HEIGHT) fprintf(hpj,"%d",SWin->Height);
	putc(')',hpj);
    }
    putc(',',hpj);
    if(SWin->Maximize) fprintf(hpj,"%u",SWin->Maximize);
    putc(',',hpj);
    if(SWin->Flags&WSYSFLAG_RGB) fprintf(hpj,"(%d,%d,%d)",SWin->Rgb[0],SWin->Rgb[1],SWin->Rgb[2]);
    putc(',',hpj);
    if(SWin->Flags&WSYSFLAG_RGBNSR) fprintf(hpj,"(%d,%d,%d)",SWin->RgbNsr[0],SWin->RgbNsr[1],SWin->RgbNsr[2]);
    if(SWin->Flags&(WSYSFLAG_TOP|WSYSFLAG_AUTOSIZEHEIGHT))
    {
	if(SWin->Flags&WSYSFLAG_AUTOSIZEHEIGHT)
	{
	    if(SWin->Flags&WSYSFLAG_TOP)
	    {
		fputs(",f3",hpj);
	    }
	    else
	    {
		fputs(",f2",hpj);
	    }
	}
	else fputs(",1",hpj);
    }
    putc('\n',hpj);
}

void PrintMVBWindow(FILE *hpj,MVBWINDOW *SWin)
{
    fprintf(hpj,"%s",SWin->Name);
    putc('=',hpj);
    fprintf(hpj,"\"%s\"",SWin->Caption);
    putc(',',hpj);
    putc('(',hpj);
    fprintf(hpj,"%d",SWin->X);
    putc(',',hpj);
    fprintf(hpj,"%d",SWin->Y);
    putc(',',hpj);
    fprintf(hpj,"%d",SWin->Width);
    putc(',',hpj);
    fprintf(hpj,"%d",SWin->Height);
    putc(',',hpj);
    fprintf(hpj,"%d",SWin->Maximize);
    putc(')',hpj);
    putc(',',hpj);
    putc('(',hpj);
    fprintf(hpj,"%d",0);
    putc(')',hpj);
    putc(',',hpj);
    fprintf(hpj,"(%d,%d,%d)",SWin->TopRgb[0],SWin->TopRgb[1],SWin->TopRgb[2]);
    putc(',',hpj);
    fprintf(hpj,"(%d,%d,%d)",SWin->RgbNsr[0],SWin->RgbNsr[1],SWin->RgbNsr[2]);
    putc(',',hpj);
    fprintf(hpj,"(%d,%d,%d)",SWin->Rgb[0],SWin->Rgb[1],SWin->Rgb[2]);
    putc(',',hpj);
    putc('(',hpj);
    fprintf(hpj,"%d",SWin->X2);
    putc(',',hpj);
    fprintf(hpj,"%d",SWin->Y2);
    putc(',',hpj);
    fprintf(hpj,"%d",SWin->Width2);
    putc(',',hpj);
    fprintf(hpj,"%d",SWin->Height2);
    putc(',',hpj);
    fprintf(hpj,"%d",0);
    putc(')',hpj);
    putc(',',hpj);
    putc('(',hpj);
    fprintf(hpj,"%d",1);
    putc(',',hpj);
    fprintf(hpj,"%d",SWin->X3);
    putc(',',hpj);
    fprintf(hpj,"%d",SWin->Y3);
    putc(')',hpj);
    putc(',',hpj);
    putc('(',hpj);
    fprintf(hpj,"%d",0);
    putc(')',hpj);
    putc(',',hpj);
    putc('(',hpj);
    fprintf(hpj,"%d",1);
    putc(')',hpj);
    putc('\n',hpj);
}

void ToMapDump(FILE *HelpFile,long FileLength)
{
    long i;

    for(i=0;i*4L<FileLength;i++)
    {
	printf("TopicNum: %-12ld TopicOffset: 0x%08lX\n",i,getdw(HelpFile));
    }
}

void GroupDump(FILE *HelpFile)
{
    GROUPHEADER GroupHeader;
    char *ptr;
    unsigned long i;

    my_fread(&GroupHeader,sizeof(GroupHeader),HelpFile);
    switch(GroupHeader.GroupType)
    {
    case 2:
	ptr=my_malloc(GroupHeader.BitmapSize);
	my_fread(ptr,GroupHeader.BitmapSize,HelpFile);
    case 1:
	for(i=GroupHeader.FirstTopic;i<=GroupHeader.LastTopic;i++)
	{
	    if(GroupHeader.GroupType==1||ptr[i>>3]&(1<<(i&7))) printf("TopicNumber: %lu\n",i);
	}
	break;
    default:
	fprintf(stderr,"GroupHeader GroupType %ld unknown\n",GroupHeader.GroupType);
    }
}

void KWMapDump(FILE *HelpFile)
{
    unsigned short n,i;
    KWMAPREC KeywordMap;

    n=my_getw(HelpFile);
    for(i=0;i<n;i++)
    {
	my_fread(&KeywordMap,sizeof(KWMAPREC),HelpFile);
	printf("Keyword: %-12ld LeafPage: %u\n",KeywordMap.FirstRec,KeywordMap.PageNum);
    }
}

void KWDataDump(FILE *HelpFile,long FileLength)
{
    long i;

    for(i=0;i<FileLength;i+=4)
    {
	printf("KWDataAddress: 0x%08lx TopicOffset: 0x%08lX\n",i,getdw(HelpFile));
    }
}

void CatalogDump(FILE *HelpFile)
{
    CATALOGHEADER catalog;
    long n;

    my_fread(&catalog,sizeof(catalog),HelpFile);
    for(n=0;n<catalog.entries;n++)
    {
	printf("Topic: %-12ld TopicOffset: 0x%08lx\n",n+1,getdw(HelpFile));
    }
}

void CTXOMAPDump(FILE *HelpFile)
{
    CTXOMAPREC CTXORec;
    unsigned short n,i;

    n=my_getw(HelpFile);
    for(i=0;i<n;i++)
    {
	my_fread(&CTXORec,sizeof(CTXORec),HelpFile);
	printf("MapId: %-12ld TopicOffset: 0x%08lX\n",CTXORec.MapID,CTXORec.TopicOffset);
    }
}

void LinkDump(FILE *HelpFile)
{
    long data[3];
    int n,i;

    n=my_getw(HelpFile);
    for(i=0;i<n;i++)
    {
	my_fread(data,sizeof(data),HelpFile);
	printf("Annotation for topic 0x%08lx 0x%08lx 0x%08lx\n",data[0],data[1],data[2]);
    }
}

void AnnotationDump(FILE *HelpFile,long FileLength,char *name)
{
    long l;

    printf("Annotation %s for topic 0x%08lx:\n",name,atol(name));
    for(l=0;l<FileLength;l++) putchar(getc(HelpFile));
    putchar('\n');
}
