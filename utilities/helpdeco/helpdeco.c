/*
HELPDECO - Utility-Programm zum Zerlegen von Windows Hilfedateien
HELPDECO - utility program to dissect Windows help files

HELPDECO zerlegt HLP-Hilfedateien von Windows 3.0, 3.1, 3.11 und '95 und
viele MVB-Dateien des Multimedia-Viewers in alle fr den jeweiligen
Hilfecompiler HC30, HC31, HCP, HCW, HCRTF, WMVC, MMVC oder MVC zum
erneuten Zusammenbau erforderlichen Dateien. Dazu geh”ren:
HPJ - die Projektdatei, als Parameter fr den Hilfecompiler anzugeben
MVP - die Multimediaprojektdatei, als Parameter fr den MM-Compiler
RTF - die Textdatei mit dem gesamten Hilfetext und allen Fuánoten
PH  - die Phrasen-Datei (wie sie auch vom Hilfecompiler erzeugt wird)
ICO - ein eventuell der Hilfedatei zugeordnetes Icon
BMP/WMF/SHG/MRB - alle Bilder in Dateien mit passendem Format
Baggage - alle als Baggage in der Hilfedatei enthaltenen Dateien

HELPDECO dissects HLP help files of Windows 3.0, 3.1, 3.11, and '95 and
many MVB multi media viewer titles into all files required for a rebuild
using the appropriate help compiler HC30, HC31, HCP, HCW, HCRTF, WMVC,
MMVC or MVC:
HPJ - help project file, use as parameter when calling help compiler
MVP - multi media project file, parameter for multi media help compiler
RTF - text file containing whole content of help file and all footnotes
PH  - phrases file (same as produced by help compiler)
ICO - icon of help file if embedded
BMP/WMF/SHG/MRB - embedded pictures in appropriate format
Baggage - all baggage files contained in help file

HELPDECO wird von der MS-DOS Kommandozeile aus mit dem Namen der zu
bearbeitenden Datei, eventuell dem Namen einer internen Datei und
eventuellen Optionen aufgerufen. HELPDECO l„uft von der Kommandozeile
von Windows 95 oder Windows NT als 32-bit Applikation zur Bearbeitung
gr”áerer Hilfedateien.
Call HELPDECO from MS-DOS command line. Supply name of help file to use,
optional name of internal file, and options if appropriate.
HELPDECO runs from Windows 95 or Windows NT command line as 32-bit
application to handle larger help files.

HELPDECO
Zeigt Benutzungshinweise
Displays usage

HELPDECO helpfilename
Zerlegt die Hilfedatei in alle zum erneuten Zusammenbau ben”tigten Dateien.
Diese Dateien werden im aktuellen (m”glichst leeren) Verzeichnis abgelegt.
Existierende Dateien werden ohne Rckfrage berschrieben wenn die Option /y
angegeben wird.
Decompiles help file into all sources needed for a rebuild. All files are
created in current directory (should be empty). Existing files will be
overwritten without asking if option /y was specified.

Hinter dem Kommando k”nnen durch Leerzeichen abgesetzte Optionen angeben
werden:
Options may be appended to the command, separated using blanks, as follows:

/m
kann verwendet werden, um das Durchsuchen von macros nach Topicnamen zu
verhindern, wenn dabei Probleme auftreten. Hilfecompiler wird Warnung 4131
melden.
May be used to stop parsing macros for topic names. Help compiler will emit
Warning 4131.

/b
kann verwendet werden, um das Aufl”sen von Browse-Sequenzen zu verhindern,
wenn dabei Probleme auftreten. Hilfequelltextdatei enth„lt dann keine +
Fuánoten.
May be used to stop resolving browse sequences. Help source file than
contains no + footnotes.

/g
Schaltet das zeitintensive Raten von Kontextnamen aus. Anzuwenden wenn
HELPDECO 'no context ids found' meldet oder man nicht an lesbaren
Kontextnamen interessiert ist. Die Funktionsweise von HELPDECO wird dadurch
nicht beeintr„chtigt.
Zum Raten versucht HELPDECO aus Kapitelberschriften und Schlsselw”rten
die Kontextnamen zu rekonstruieren, was bei einigen Hilfedateien sehr gut,
bei anderen gar nicht funktioniert, je nach Hilfedateierstellungswerkzeug
oder Arbeitsmethode des Hilfeautors.
Turns off time consuming guessing of context names. Applicable when
HELPDECO reports 'no context ids found' or when you are not interested in
legible context ids. This doesn't affect functionality.
During guessing HELPDECO tries to reconstruct context ids from topic titles
and keywords. This may produce good results on some help files and no result
at all on others, depending on autoring tool used or working method of help
author.

/i
Wenn man sehen m”chte, welche Kontextnamen HELPDECO err„t...
If you want to see which context ids are guessed by HELPDECO...

/hprefix
HELPDECO versucht aus Kapitelberschriften und Schlsselw”rtern Kontextnamen
zu erraten, wobei auch idh_ oder helpid_ vorangestellt werden.
Wenn die Hilfedatei mit anderen bekannten Prefixen erstellt wurde, kann
die Tabelle durch die Option /h prefix erweitert werden.
HELPDECO tries to guess context ids from topic titles and keywords, possibly
prefixed by idh_ or helpid_. If the help file was created using other
prefixes, they may be added to the table using the /h prefix option.

/a [annotationfilename.ANN]
Fgt zus„tzlich alle Anmerkungen aus der angegebenen Anmerkungsdatei als
Anmerkungen des Benutzers ANN in die RTF-Datei ein. Fehlt der annotation-
filename, verwendet HELPDECO helpfilename.ANN dafr.
Adds all annotations from annotationfile as annotations of user ANN into
RTF file. Default annotationfilename is helpfilename.ANN.

/s n
Zerteilt die Ausgabe alle n Seiten in eine eigene RTF Datei, z.B. zerlegt
  HELPDECO helpdemo /s 100
die Datei helpdemo.hlp und erzeugt RTF-Dateien mit den Namen helpdem1.rtf,
helpdem2.rtf,.. die jeweils h”chstens 100 Seiten enthalten.
Splits output every n pages into a separate RTF file. For example
  HELPDECO helpdemo /s 100
decompiles helpdeco.hlp and creates different RTF files helpdem1.rtf,
helpdem2.rtf,.. each containing no more than 100 topics.

HELPDECO helpfilename /r
Erzeugt aus der Hilfedatei eine RTF-Datei, die von WinWord geladen dasselbe
Aussehen hat wie die von WinHelp angezeigten Hilfeseiten. Damit kann eine
Hilfedatei komplett gedruckt oder weiterverarbeitet werden. Zus„tzlich
Option /n angeben, wenn an Kapitelgrenzen kein Seitenwechsel stattfinden soll.
Converts help file into RTF file of same appearance if loaded into WinWord
as if displayed by WinHelp. To print or work with complete content. Specify
additional option /n, if no page breaks should separate topics.

HELPDECO helpfilename /c
Erzeugt aus der Hilfedatei eine *.CNT-Datei fr WinHlp32, die alle Kapitel
mit šberschriften in der Reihenfolge enth„lt, in der sie in der Hilfedatei
auftreten. Die Datei muá dann mit HCW 4.00 oder einem Texteditor in eine
hierarchische Struktur berarbeitet werden.
Generates a *.CNT file used by WinHlp32, containing all chapters that have
titles assigned in the order they appear in the helpfile. This file should
then be edited using HCW 4.00 or any text editor into a hierarchical order.

HELPDECO helpfilename /e
Zeigt alle Referenzen auf externe Hilfedateien.
Lists all references to external help files.

HELPDECO helpfilename /e /f
Zeigt alle Referenzen auf externe Hilfedateien und die Titel der Topics in
denen sie auftraten.
Lists all references to external help files and titles of topics that
contained these references.

HELPDECO helpfilename /p
Prft Referenzen auf externe Hilfedateien. Die referenzierten Hilfedateien
mssen fr HELPDECO zugreifbar sein. Dieser Aufruf erzeugt keine neuen
Dateien und modifiziert keine existierenden. Fehler werden auf stdout (den
Bildschirm) geschrieben.
Checks references to external help files. Referenced help file need to be
available to HELPDECO. This call doesn't produce any new files and doesn't
modify existing files. Errors are reported to stdout (screen).

HELPDECO helpfilename /d
Zeigt das interne Inhaltsverzeichnis der Hilfedatei. Es kann auch eine
*.MVB,*.M??,*.ANN,*.CAC,*.AUX Datei anstelle der *.HLP-Datei angegeben
werden.
Displays internal directory of help file. You may supply a *.MVB,*.M??,
*.ANN,*.CAC,*.AUX file instead of a *.HLP file.

HELPDECO helpfilename /x
Zeigt das interne Inhaltsverzeichnis als HexDump
Displays hex dump of internal directory

HELPDECO helpfilename "internalfilename"
Zeigt die genannte interne Datei in einem passenden Format an, soweit die
interne Datei anzeigbar ist, sonst als HexDump. Sie k”nnen die Ausgabe in
eine Datei umleiten durch anh„ngen von >outfile.
Displays internal file in appropriate format if known, else hex dump. You
may redirect output into a file using >outfile.

HELPDECO helpfilename "internalfilename" /x
Zeigt die genannte interne Datei als HexDump
Displays hex dump of internal file

HELPDECO helpfilename "internalfilename" /x /t offset
Zeigt die genannte interne Datei als HexDump beginnend bei Position
offset, der als Dezimalzahl oder mit vorangestelltem 0x als Hexadezimal-
zahl akzeptiert wird.
Displays hex dump of internal file starting at offset, which may be
specified in decimal or preceeded with 0x in hex.

HELPDECO helpfilename "internalfilename" filename
Exportiert die genannte interne Datei in filename
Exports internal file into filename

*.ANN, *.CAC, *.AUX
Diese Dateien sind auch wie Hilfedateien formatiert, HELPDECO kann aber nur
verwendet werden, um ihr Inhaltsverzeichnis anzuzeigen oder um einzelne
Dateien anzuzeigen und zu exportieren.
These files are formatted like helpfiles, but HELPDECO can only be used to
display their internal directory or display or export internal files.

HELPDECO wurde erstellt von / was written by
Manfred Winterhoff, Geschw.-Scholl-Ring 17, 38444 Wolfsburg, Germany
CIS 100326,2776

Wenn Sie weitere Fragen, Probleme (oder Antworten!) haben, k”nnen Sie
durch eine EMail an 100326.2776@compuserve.com mit mir in Kontakt treten.
If you have more questions, problems (or answers!), please feel free to
send me an EMail to 100326.2776@compuserve.com

HELPDECO basiert auf HELPDUMP von Pete Davis ver”ffentlicht in:
HELPDECO is based upon HELPDUMP from Pete Davis published in:
The Windows Help File Format, Dr. Dobbs Journal, Sep/Oct 1993
Thanks to Holger Haase, who did a lot of work on picture file formats.
And thanks to Jrgen Mller for pointing out 32-bit differences.
Thanks to Bent Lynggaard for the information on help file free lists
and his contribution on context id guessing.

HELPFILE.TXT enth„lt eine Beschreibung des Windows Hilfedateiformats
wie HELPDECO es versteht und weiterer Dateiformate wie MRB/SHG.
See HELPFILE.TXT for a description of the Windows help file format
as parsed by HELPDECO and related file formats like MRB/SHG.

Die neueste Version von HELPDECO befindet sich stets in:
The newest public version of HELPDECO is always available at:
CompuServe: Dr. Dobbs Journal DDJFOR Undocumented Corner HELPDCxx.ZIP
InterNet: ftp://gmutant.wrlc.org/pub/winhelp

HELPDECO ist Freeware. Der Einsatz erfolgt auf eigene Gefahr. Kein
Programmteil darf kommerziell verwendet werden. Fr das Kopieren drfen
keine Gebhren verlangt werden (Sharewarehandel Finger weg).
HELPDECO is freeware. Use at your own risk. No part of the program may be
used commercially. No fees may be charged on distributing the program
(shareware distributors keep off).

Die Verwendung von Inhalten zerlegter Hilfedateien kann eine Verletzung
des Urheberrechtes bedeuten.
The use of parts of decompiled help files may constitute a violation of
copyright law.

Version 2.1:
macros may contain rtf meta characters
Win95 topic names may contain (nearly) all characters
0x8000 in SWin->Maximize allowed
ForeHelp creates PhrIndex/PhrImage _and_ _empty_ Phrases file
HCRTF complained about > footnotes before # footnotes
macro parsing changed again

Version 2.0:
Guess context ids from titles and keywords based on idea of Bent Lynggaard
Recompiled 16 bit EXE without register calling convention. BC++ 3.1 bug.
Doesn't print last (stray) topic of HC30 help files
Can list entry points into this help file (option /l)

Version 1.9: faster & better than ever...
changed TopicPos, TopicOffset, Keyword maintenance
changed unhash to 40 bit integer arithmetic
fixed keyword footnotes [Bent Lynggaard]
no [ALIAS] in MVP files
no hidden text in option /r RTF files
some changes in font and stylesheet handling
corrects rounding error of HC31 on negative values
handles non-underlined topic jumps

Version 1.8: used some spare days to clean up the to-do list...
better tracking of TopicOffset during decompilation
lists and checks references to external files, shows referencing topics
can add annotations from .ANN file to decompiled .RTF file
fixed bug in handling of pictures containing JumpId-macro hotspots
changed parsing of macros (3rd attempt to guess what Microsoft did)
fixed bug in popup/jump to external file / secondary window
fixed bug in > footnote / |VIOLA internal file handling
fixed bug in keyword assignment
now removes LZ77 compression from exported SHGs/MRBs
recreates Win 95 (HCW 4.00) [MACROS] section from internal |Rose file
32 bit version available
handles LANGUAGE, [CHARTAB] and [GROUP] section of media view files

Version 1.7
removed unneccessary output statement

Version 1.6 can now check references to external help files plus:
duplicate macro names preceeding picture hotspot info skipped
does not write Win95 commands to multi-media help project files
changed unhash to circumvent Microsoft-C++ float rounding error
handles keywords defined inside topic text

Version 1.5
fixed static on buffer of TopicName function (affected HC30 files)

Version 1.4 fixes some bugs reported by different users:
buffer overflow in expanding LZ77&RunLen (byPacked 3) images fixed
embedded images {bmxwd} larger than 32k supported
extract topic names from jump into external file if no file specified
handles more phrases on HCRTF generated (Win95) help files
Windows 3.1 (HC31) |Phrases always Zeck compressed
LinkData2 buffer enlarged 1 byte to store trailing NUL character

Version 1.3
parses examples of {bmc} etc. statements contained in help text correctly
can now generate a *.CNT content file for Windows 95 / WinHlp32
Microsoft C: ctype macros (isalnum/isprint) don't work with signed char

Version 1.2 fixes some severe bugs introduced in version 1.1 and:
tells you which help compiler to use
collects multiple keyword footnotes into single lines
handles \r\n in COPYRIGHT
converts SPC-macro (but only in [CONFIG] section)
does not generate duplicate MAP-statements if possible
{button} and {mci,mci_left,mci_right} commands supported
[BITMAP]-section in HCRTF help files irritated transparent bitmaps

Version 1.1 now supports more features of Win95/HCRTF 4.00/WinHlp32:
Supports LCID, CHARSET, AUTO-SIZE HEIGHT, CNT, INDEX_SEPARATORS
Additional Win95 Macros (to extract original topic names)
[CONFIG:n] of Win95 supported (internal file |CFn)
Secondary windows with > footnote supported (internal file |VIOLA)
Transparent bitmaps supported (bmct,bmlt,bmrt)
Expanded internal limits as HCRTF allows larger items
Now does RunLen compressed device dependend bitmaps
Bugs in handling of metafiles removed
Bug in placement of pack(1) removed
Parsing of macros changed (is it really better now ?)
*/
#include "helpdeco.h"

/* neccessary compiler options for 16 bit version using Borland C/C++:
//   bcc -ml -K -Os -p helpdeco.c helpdec1.c
// Don't compile using Register Calling Convention in BC3.1: compiler bug.
// To compile 32 bit version using Microsoft VC4.0 create a new workspace
// for a WIN32 console application, insert HELPDECO.C and HELPDEC1.C into
// it and compile. Use /KNOWEAS 16 bit version as stub to create dual-mode
// application.
// Byte align ! Portable to little endian machines only.
*/

FILEREF *external;
char HelpFileName[81];
char name[_MAX_FNAME];
char ext[_MAX_EXT];
FILE *AnnoFile;
HASHREC *hashrec;
int hashrecs=0;
BROWSE *browse;
int browses;
int browsenums;
long scaling;
int rounderr;
START *start;
int starts;
BOOL lzcompressed,Hall;
BOOL before31,after31;
BOOL win95;
BOOL mvp,multi;
BOOL warnings,missing;
long *Topic;
int Topics;			    /* 16 bit: max. 16348 Topics */
GROUP *group;
int groups;
CONTEXTREC *ContextRec;
int ContextRecs;		    /* 16 bit: max. 8191 Context Records */
ALTERNATIVE *alternative;
int alternatives;
BOOL overwrite=FALSE;
BOOL exportLZ77=FALSE;
BOOL extractmacros=TRUE;
BOOL guessing=TRUE;
long guessed=0L;
BOOL listtopic=FALSE;
BOOL nopagebreak=FALSE;
BOOL resolvebrowse=TRUE;
BOOL reportderived=FALSE;
BOOL checkexternal=FALSE;
BOOL exportplain=FALSE;
#define MAXKEYWORDS (64<<(sizeof(int)*2)) /* 16 bit: 1024, 32 bit: 16348 */
int NextKeywordRec,KeywordRecs;
KEYWORDREC *KeywordRec;
TOPICOFFSET NextKeywordOffset;
char helpcomp[13];
char HelpFileTitle[81];
char TopicTitle[256];
char *Phrases;
unsigned int *PhraseOffsets;
unsigned int PhraseCount;
long TopicFileLength;
int TopicBlockSize; /* 2k or 4k */
int DecompressSize; /* 4k or 16k */
char buffer[4096];
char keyword[512];
char index_separators[40]=",;";
char *extension;
int extensions=0;
/* index into bmpext: bit 0=multiresolution bit 1=bitmap, bit 2=metafile, bit 3=hotspot data, bit 4=embedded, bit 5=transparent */
char *bmpext[]={"???","MRB","BMP","MRB","WMF","MRB","MRB","MRB","SHG","MRB","SHG","MRB","SHG","MRB","SHG","MRB"};
char **stopwordfilename;
int stopwordfiles;
char **fontname;
int fontnames;
unsigned char DefFont;
FONTDESCRIPTOR *font;
int fonts;
unsigned char lookup[]={0,3,1,2,4,5}; /* to translate font styles */
struct { unsigned char r,g,b; } color[128];
int colors;
char **windowname;
int windownames;
BOOL NotInAnyTopic;
int TopicsPerRTF;
BOOL lists['z'-'0'+1];
BOOL keyindex['z'-'0'+1];
static signed char table[256]=
{
    '\x00', '\xD1', '\xD2', '\xD3', '\xD4', '\xD5', '\xD6', '\xD7', '\xD8', '\xD9', '\xDA', '\xDB', '\xDC', '\xDD', '\xDE', '\xDF',
    '\xE0', '\xE1', '\xE2', '\xE3', '\xE4', '\xE5', '\xE6', '\xE7', '\xE8', '\xE9', '\xEA', '\xEB', '\xEC', '\xED', '\xEE', '\xEF',
    '\xF0', '\x0B', '\xF2', '\xF3', '\xF4', '\xF5', '\xF6', '\xF7', '\xF8', '\xF9', '\xFA', '\xFB', '\xFC', '\xFD', '\x0C', '\xFF',
    '\x0A', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x09', '\x0A', '\x0B', '\x0C', '\x0D', '\x0E', '\x0F',
    '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1A', '\x1B', '\x1C', '\x1D', '\x1E', '\x1F',
    '\x20', '\x21', '\x22', '\x23', '\x24', '\x25', '\x26', '\x27', '\x28', '\x29', '\x2A', '\x0B', '\x0C', '\x0D', '\x0E', '\x0D',
    '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1A', '\x1B', '\x1C', '\x1D', '\x1E', '\x1F',
    '\x20', '\x21', '\x22', '\x23', '\x24', '\x25', '\x26', '\x27', '\x28', '\x29', '\x2A', '\x2B', '\x2C', '\x2D', '\x2E', '\x2F',
    '\x50', '\x51', '\x52', '\x53', '\x54', '\x55', '\x56', '\x57', '\x58', '\x59', '\x5A', '\x5B', '\x5C', '\x5D', '\x5E', '\x5F',
    '\x60', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67', '\x68', '\x69', '\x6A', '\x6B', '\x6C', '\x6D', '\x6E', '\x6F',
    '\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77', '\x78', '\x79', '\x7A', '\x7B', '\x7C', '\x7D', '\x7E', '\x7F',
    '\x80', '\x81', '\x82', '\x83', '\x0B', '\x85', '\x86', '\x87', '\x88', '\x89', '\x8A', '\x8B', '\x8C', '\x8D', '\x8E', '\x8F',
    '\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97', '\x98', '\x99', '\x9A', '\x9B', '\x9C', '\x9D', '\x9E', '\x9F',
    '\xA0', '\xA1', '\xA2', '\xA3', '\xA4', '\xA5', '\xA6', '\xA7', '\xA8', '\xA9', '\xAA', '\xAB', '\xAC', '\xAD', '\xAE', '\xAF',
    '\xB0', '\xB1', '\xB2', '\xB3', '\xB4', '\xB5', '\xB6', '\xB7', '\xB8', '\xB9', '\xBA', '\xBB', '\xBC', '\xBD', '\xBE', '\xBF',
    '\xC0', '\xC1', '\xC2', '\xC3', '\xC4', '\xC5', '\xC6', '\xC7', '\xC8', '\xC9', '\xCA', '\xCB', '\xCC', '\xCD', '\xCE', '\xCF'
};
char oldtable[256];
unsigned char untable[]={0,'1','2','3','4','5','6','7','8','9','0',0,'.','_',0,0,0,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
char *prefix[]={"","idh_","helpid_",NULL,NULL,NULL,NULL,NULL};
long prefixhash[sizeof(prefix)/sizeof(prefix[0])];
FONTDESCRIPTOR CurrentFont;

long hash(char *name) /* convert 3.1/'95 topic name to hash value */
{
    long hash;
    unsigned char *ptr;

    if(*name=='\0') return 1L;
    for(hash=0L,ptr=(unsigned char *)name;*ptr;ptr++)
    {
	hash=hash*43L+table[*ptr];
    }
    return hash;
}

char *unhash(unsigned long hash) /* deliver 3.1 context id that fits hash value */
{
    static char buffer[15];
    int i,j,k;
    unsigned long hashlo,divlo,result,mask;
    unsigned char hashhi,divhi;
    char ch;

    i=0;
    j=hashrecs;
    while(i<j)
    {
	k=(i+j)/2;
	if(hashrec[k].hash<(long)hash)
	{
	    i=k+1;
	}
	else if(hashrec[k].hash>(long)hash)
	{
	    j=k;
	}
	else return hashrec[k].name;
    }
    for(i=0;i<43;i++)
    {
	buffer[j=14]='\0';
	hashlo=hash;
	hashhi=i;
	while(1)
	{
	    divhi=21;
	    divlo=0x80000000UL;
	    result=0UL;
	    for(mask=0x80000000UL;mask;mask>>=1)
	    {
		if(hashhi>divhi||hashhi==divhi&&hashlo>=divlo)
		{
		    result|=mask;
		    hashhi-=divhi;
		    if(divlo>hashlo) hashhi--;
		    hashlo-=divlo;
		}
		divlo>>=1;
		if(divhi&1) divlo|=0x80000000UL;
		divhi>>=1;
	    }
	    ch=untable[(int)hashlo];
	    if(!ch) break;
	    buffer[--j]=ch;
	    if(result==0) return buffer+j;
	    hashlo=result;
	}
    }
    /* should never happen */
    error("Can not find a matching string for hash value %08lx",hash);
    sprintf(buffer,"HASH%08lx",hash);
    return buffer;
}

char *ContextId(unsigned long hash) /* unhash and verify for legal entry point */
{
    char *ptr;
    int i;

    for(i=0;i<ContextRecs;i++)
    {
	if(ContextRec[i].HashValue==hash)
	{
	    return unhash(hash);
	}
    }
    ptr=unhash(hash);
    printf("Help Compiler will issue Warning 4113: Unresolved jump or popup '%s'\n",ptr);
    return ptr;
}

void AddTopic(char *TopicName,BOOL derived) /* adds a known topic name to hash decode list */
{
    long x;
    int i,j,k;

    x=hash(TopicName);
    i=0;
    j=hashrecs;
    while(i<j)
    {
	k=(i+j)/2;
	if(hashrec[k].hash<x)
	{
	    i=k+1;
	}
	else if(hashrec[k].hash>x)
	{
	    j=k;
	}
	else
	{
	    if (strcasecmp(TopicName, hashrec[k].name) != 0)
	    {
		if(!hashrec[k].derived)
		{
		    if(!derived) fprintf(stderr,"ContextId %s already defined as %s\n",TopicName,hashrec[k].name);
		    return;
		}
		if(derived) return;
		free(hashrec[k].name);
		hashrec[k].name=my_strdup(TopicName);
	    }
	    if(!derived&&hashrec[k].derived)
	    {
		guessed--;
		hashrec[k].derived=FALSE;
	    }
	    return;
	}
    }
    /* %100 to decrease memory fragmentation */
    if(hashrecs%100==0) hashrec=my_realloc(hashrec,(hashrecs+100)*sizeof(HASHREC));
    if(i<hashrecs) memmove(hashrec+i+1,hashrec+i,sizeof(HASHREC)*(hashrecs-i));
    hashrec[i].name=my_strdup(TopicName);
    hashrec[i].derived=derived;
    hashrec[i].hash=x;
    if(derived) guessed++;
    hashrecs++;
}

/* ContextRec is sorted by TopicOffset. Binary search for an entry for
// TopicOffset topic. If more than one entry with identical TopicOffset
// is stored, return index of the first one, if not found return -1 */
int FindContext(long topic)
{
    int i,lwb,upb;

    lwb=0;
    upb=ContextRecs;
    while(lwb<upb)
    {
	i=(lwb+upb)/2;
	if(ContextRec[i].TopicOffset>topic)
	{
	    upb=i;
	}
	else if(ContextRec[i].TopicOffset<topic)
	{
	    lwb=i+1;
	}
	else
	{
	    while(i>0&&ContextRec[i-1].TopicOffset==topic) i--;
	    return i;
	}
    }
    return -1;
}

/* Look if buf contains str (caseinsensitive) and update case of str so it is
// matching case appearance in buf and return TRUE, FALSE if not found */
int FindString(unsigned char *buf,int buflen,unsigned char *str,int len)
{
    int i,j;
    unsigned char ch;

    buflen-=len;
    for(i=0;i<=buflen;i++)
    {
        for(j=0;j<len;j++)
        {
	    if(table[ch=buf[j]]!=table[str[j]]) break;
            str[j]=ch;
        }
        if(j==len)
        {
            return TRUE;
        }
	buf++;
    }
    return FALSE;
}

/* Many people and authoring systems create context ids from topic titles,
// either by replacing every illegal char with _ or . or leaving it out,
// sometimes using only part of the topic title or prefixing it with idh_
// or helpid_. This function tries all variants and may find a context id
// matching the hash value.
// And if it doesn't derive a context id, don't bother, unhash delivers
// one matching the hash code. This code is just to please the user.
// Win95 allows for nearly every char to be used as a context ident. */
BOOL Derive(unsigned char *str,unsigned long desiredhash,char *buffer)
{
    int i,j,k,l,m,n,o,p,s;
    unsigned long hash,h,x,y;
    char *ptr;
    unsigned char ch;

    l=2;
    s=strlen(str);
    for(i=!win95;i<l;i++) /* three variants what to do with illegal characters */
    {                /* but only if an illegal character found (see below) */
	for(j=0;prefix[j];j++)
	{
	    k=0;
	    while(str[k])
	    {
		hash=prefixhash[j];
		strcpy(buffer,prefix[j]);
		ptr=strchr(buffer,'\0');
		for(m=k;str[m];m++)
		{
		    ch=str[m];
		    if(i)
		    {
			n=oldtable[ch];
			if(n==0)
			{
			    if(i==2)
			    {
				n=oldtable[ch='_'];
			    }
			    else
			    {
				continue;
			    }
			}
		    }
		    else /* Win95 */
		    {
			n=table[ch];
			if(n==0) continue;
		    }
		    *ptr++=ch;
		    hash=43L*hash+n;
		    for(x=1L,o=0;o<6;o++,x*=43L)
		    {
			h=desiredhash-x*hash;
			if(h<x)
			{
			    y=x;
			    p=0;
			    while(h)
			    {
				y/=43L;
				ch=untable[(unsigned char)(h/y)];
                                if(!ch) break;
				h%=y;
				ptr[p++]=ch;
			    }
			    if(h==0&&(FindString(str,s,ptr,p)||p<3))
			    {
				ptr[p]='\0';
				return TRUE;
			    }
			}
		    }
		}
		if(i)
		{
		    while(oldtable[ch=str[k]]) k++;
		    if(ch) l=3;
		    while((ch=str[k])!='\0'&&!oldtable[ch]) k++;
		}
		else /* win95 */
		{
		    while(table[ch=str[k]]) k++;
		    if(ch) l=3;
		    while((ch=str[k])!='\0'&&!table[ch]) k++;
		}
	    }
	}
    }
    return FALSE;
}

void Guess(char *str,TOPICOFFSET topic)
{
    int i,j,k,m;
    long hash;

    i=FindContext(topic);
    if(i!=-1) /* iff there is a # footnote assigned to that topic offset */
    {
	do
	{
	    hash=ContextRec[i].HashValue;
	    j=0;
	    k=hashrecs;
	    while(j<k)
	    {
		m=(j+k)/2;
		if(hashrec[m].hash<hash)
		{
		    j=m+1;
		}
		else if(hashrec[m].hash>hash)
		{
		    k=m;
		}
		else break;
	    }
	    if(j>=k) if(Derive(str,hash,buffer))
	    {
		if(reportderived) printf("Derived %s\n",buffer);
		AddTopic(buffer,TRUE);
	    }
	}
	while(++i<ContextRecs&&ContextRec[i].TopicOffset==topic);
    }
}

void SysLoad(FILE *HelpFile) /* gets global values from SYSTEM file */
{
    SYSTEMRECORD *SysRec;
    SYSTEMHEADER SysHdr;
    SECWINDOW *SWin;
    int i;
    char kwdata[10];
    char kwbtree[10];

    if(!SearchFile(HelpFile,"|SYSTEM",NULL))
    {
	fputs("Internal |SYSTEM file not found. Can't continue.\n",stderr);
	exit(1);
    }
    my_fread(&SysHdr,sizeof(SysHdr),HelpFile);
    before31=SysHdr.Minor<16;
    after31=SysHdr.Minor>21;
    multi=SysHdr.Minor==27;
    lzcompressed=!before31&&(SysHdr.Flags==4||SysHdr.Flags==8);
    win95=SysHdr.Minor==33&&!mvp;
    windownames=0;
    windowname=NULL;
    if(before31)
    {
	DecompressSize=TopicBlockSize=2048;
    }
    else
    {
	DecompressSize=0x4000;
	if(SysHdr.Flags==8)
	{
	    TopicBlockSize=2048;
	}
	else
	{
	    TopicBlockSize=4096;
	}
    }
    if(before31)
    {
	my_gets(HelpFileTitle,33,HelpFile);
    }
    else
    {
	for(SysRec=GetFirstSystemRecord(HelpFile);SysRec;SysRec=GetNextSystemRecord(SysRec))
	{
	    switch(SysRec->RecordType)
	    {
	    case 0x0001:
		strlcpy(HelpFileTitle,SysRec->Data,sizeof(HelpFileTitle));
		break;
	    case 0x0006:
		SWin=(SECWINDOW *)SysRec->Data;
		windowname=my_realloc(windowname,(windownames+1)*sizeof(char *));
		windowname[windownames]=NULL;
		if(SWin->Flags&WSYSFLAG_NAME)
		{
		    windowname[windownames]=my_strdup(SWin->Name);
		}
		windownames++;
		break;
	    case 0x000E:
		keyindex[SysRec->Data[1]-'0']=TRUE;
		break;
	    }
	}
    }
    for(i='A';i<='z';i++)
    {
	sprintf(kwdata,"|%cWDATA",i);
	sprintf(kwbtree,"|%cWBTREE",i);
	if(SearchFile(HelpFile,kwdata,NULL)&&SearchFile(HelpFile,kwbtree,NULL))
	{
	    lists[i-'0']=TRUE;
	}
    }
}

/*********************************************************************************

  GRAPHICS STUFF
  ==============
  ExtractBitmap(..) extracts the graphics stored as |bmXXX in the Windows
  HelpFile to separate files. In this version multiple resolution pictures
  (with and without hotspots) are saved as MRB's, pictures with hotspots are
  converted to the SHG-format, single resolution Bitmaps without hotspots
  to standard BMP-format and all Metafiles without hotspots to the Aldus
  Placeable Metafile WMF format.

  GENERAL NOTES ABOUT |bmXXX
  --------------------------

  |bmXXX contains one MRB-File. This can be directly recompiled. If hotspots
  in the graphics contained in the MRB-File shall be added or changed with
  SHED, the MRB-File has to be split into one SHG file for each graphic. This
  is very easy as the graphics-data will only be slightly rewrapped. If the
  graphics themselves shall be edited with a standard drawing program, the
  SHG files have to be converted into bitmaps or metafiles (as flagged in
  the SHG). The hotspot data is lost by this step.

  MRB-FILES
  ---------

  MRBC takes the input-file(s), converts them to "lp"-Files if needed, adds
  a resolution specification if none is given in the file and finally outputs
  a "lp"-File with the ending ".MRB". Depending on the given display type
  MRBC sets the following resolutions:

	       |	   |		   | x-resolution | y-resolution
  display-type | extension | std-extension |	(ppi)	  |    (ppi)
  -------------+-----------+---------------+--------------+-------------
       CGA     |   ".C*"   |	 ".CGA"    |	  96	  |	 48
       EGA     |   ".E*"   |	 ".EGA"    |	  96	  |	 72
       VGA     |   ".V*"   |	 ".VGA"    |	  96	  |	 96
       8514    |   ".8*"   |	 ".854"    |	 120	  |	120

  SHG-Files
  ---------

  Structure of SHG-Data:

   Offset    | Size	  | Ref | Description
   ----------+------------+-----+-----------------------------------------------
      0      | 1 Byte	  |	| always 0x01, maybe a version-number
      1      | 1 Word	  |  N	| the number of hotspots
      3      | 1 DWord	  |  M	| size of macro-data (in bytes)
      7      | 15xN Bytes | HSD | Hotspot-Data (see below)
    7 + 15*N | M Bytes	  |	| Macro-Data (ASCIIZ-Strings)
    7+15*N+M | 2*N*ASCIIZ |	| Hotspot-Id and Context- or Macro-Name as
	     |		  |	| ASCIIZ-String

  Structure of Hotspot-Data:

   Offset    | Size	  | Ref | Description
   ----------+------------+-----+-------------------------------------------------
      0      | 3 Bytes	  |	| Hotspot-Type: 0xE3 0x00 0x00 = jump, visible
	     |		  |	|		0xE7 0x04 0x00 = jump, invisible
	     |		  |	|		0xE2 0x00 0x00 = pop-up, visible
	     |		  |	|		0xE6 0x04 0x00 = pop-up, invisible
	     |		  |	|		0xC8 0x00 0x00 = macro, visible
	     |		  |	|		0xCC 0x04 0x00 = macro, invisible
      3      | 1 Word	  |	| Left border of hotspot
      5      | 1 Word	  |	| Top border of hotspot
      7      | 1 Word	  |	| Right border - Left border
      9      | 1 Word	  |	| Bottom border - Top border
     11      | 1 DWord	  |	| 0x00000000 => nothing
	     |		  |	| 0x00000001 => this is a macro-hotspot
	     |		  |	|   others   => hash-value of the context-string
	     |		  |	|		according to the WinHelp standard

  03-06-1995 Holger Haase
**********************************************************************************/

/* compares filename a[.HLP] and filename b[.HLP], returning 0 if match */
int filenamecmp(const char *a,const char *b)
{
    char aname[_MAX_FNAME],bname[_MAX_FNAME];
    char aext[_MAX_EXT],bext[_MAX_EXT];
    int i;

    _splitpath(a,NULL,NULL,aname,aext);
    _splitpath(b,NULL,NULL,bname,bext);
    if(aext[0]=='\0') strcpy(aext,".HLP");
    if(bext[0]=='\0') strcpy(bext,".HLP");
    i = strcasecmp(aname, bname);
    if(i) return i;
    return strcasecmp(aext, bext);
}

/* store external reference in list, checked later */
void StoreReference(char *filename,int type,char *id,long hash)
{
    CHECKREC *ptr;
    FILEREF *ref;
    PLACEREC *place;

    for(ref=external;ref;ref=ref->next)
    {
	if(filenamecmp(filename,ref->filename)==0) break;
    }
    if(!ref)
    {
	ref=my_malloc(sizeof(FILEREF)+strlen(filename));
	strcpy(ref->filename,filename);
	ref->check=NULL;
	ref->next=external;
	external=ref;
    }
    for(ptr=ref->check;ptr;ptr=ptr->next)
    {
	if(ptr->type==type&&ptr->hash==hash) break;
    }
    if(!ptr)
    {
	ptr=my_malloc(sizeof(CHECKREC));
	ptr->type=type;
	ptr->hash=hash;
	ptr->id=my_strdup(id);
	ptr->here=NULL;
	ptr->next=ref->check;
	ref->check=ptr;
    }
    if(listtopic&&TopicTitle[0])
    {
	place=my_malloc(sizeof(PLACEREC)+strlen(TopicTitle));
	strcpy(place->topicname,TopicTitle);
	place->next=ptr->here;
	ptr->here=place;
    }
}

/* validate each entry in list of external references */
void CheckReferences(void)
{
    FILEREF *ref;
    CHECKREC *ptr;
    FILE *f;
    BOOL found;
    long offset;
    int i,n;
    CTXOMAPREC CTXORec;
    BTREEHEADER BTreeHdr;
    BTREENODEHEADER CurrNode;
    CONTEXTREC ContextRec;

    for(ref=external;ref;ref=ref->next)
    {
	f=fopen(ref->filename,"rb");
	if(!f)
	{
	    printf("%s not found\n",ref->filename);
	}
	else
	{
	    if(SearchFile(f,NULL,NULL))
	    {
		for(ptr=ref->check;ptr;ptr=ptr->next)
		{
		    found=FALSE;
		    if(ptr->type==CONTEXT)
		    {
			if(SearchFile(f,"|CTXOMAP",NULL))
			{
			    n=my_getw(f);
			    for(i=0;i<n;i++)
			    {
				my_fread(&CTXORec,sizeof(CTXORec),f);
				if(CTXORec.MapID==ptr->hash) /* hash is context id */
				{
				    found=TRUE;
				    break;
				}
			    }
			}
		    }
		    else
		    {
			if(SearchFile(f,"|CONTEXT",NULL))
			{
			    my_fread(&BTreeHdr,sizeof(BTreeHdr),f);
			    offset=ftell(f);
			    CurrNode.PreviousPage=BTreeHdr.RootPage;
			    for(n=1;n<BTreeHdr.NLevels;n++)
			    {
				fseek(f,offset+CurrNode.PreviousPage*(long)BTreeHdr.PageSize,SEEK_SET);
				my_fread(&CurrNode,sizeof(BTREEINDEXHEADER),f);
				for(i=0;i<CurrNode.NEntries;i++)
				{
				    ContextRec.HashValue=getdw(f);
				    if(ContextRec.HashValue>ptr->hash) break;
				    CurrNode.PreviousPage=my_getw(f); /* Page */
				}
			    }
			    fseek(f,offset+CurrNode.PreviousPage*(long)BTreeHdr.PageSize,SEEK_SET);
			    my_fread(&CurrNode,sizeof(BTREENODEHEADER),f);
			    for(i=0;i<CurrNode.NEntries;i++)
			    {
				my_fread(&ContextRec,sizeof(ContextRec),f);
				if(ContextRec.HashValue==ptr->hash) found=TRUE;
				if(ContextRec.HashValue>=ptr->hash) break;
			    }
			}
		    }
		    if(!found)
		    {
			if(ptr->id)
			{
			    printf("%s@%s not found\n",ptr->id,ref->filename);
			}
			else
			{
			    printf("0x%08lx@%s not found\n",ptr->hash,ref->filename);
			}
			while(ptr->here)
			{
			    printf("  %s\n",ptr->here->topicname);
			    ptr->here=ptr->here->next;
			}
		    }
		}
	    }
	    else
	    {
		printf("%s isn't a valid WinHelp file !\n",ref->filename);
	    }
	    fclose(f);
	}
    }
}

/* list each entry in list of external references */
void ListReferences(void)
{
    FILEREF *ref;
    CHECKREC *ptr;

    hashrecs=0;
    for(ref=external;ref;ref=ref->next)
    {
	for(ptr=ref->check;ptr;ptr=ptr->next)
	{
	    printf("%s ",ref->filename);
	    switch(ptr->type)
	    {
	    case TOPIC:
		printf("topic id ");
		if(ptr->id)
		{
		    printf("'%s'",ptr->id);
		}
		else
		{
		    printf("0x%08lx='%s'",ptr->hash,unhash(ptr->hash));
		}
		break;
	    case CONTEXT:
		printf("[MAP] id ");
		if(ptr->id)
		{
		    printf("'%s'",ptr->id);
		}
		else
		{
		    printf("0x%08lx=(%ld)",ptr->hash,ptr->hash);
		}
		break;
	    }
	    if(ptr->here)
	    {
		printf(" referenced from:");
		while(ptr->here)
		{
		    printf("\n	%s",ptr->here->topicname);
		    ptr->here=ptr->here->next;
		}
	    }
	    putchar('\n');
	}
    }
}

void unquote(char *ptr)
{
    char *dest;

    dest=ptr;
    do
    {
        if(*ptr=='\\'&&ptr[1]!='\0')
        {
            ptr++;
        }
    }
    while((*dest++=*ptr++)!='\0');
}

char *findMatchingChar(char *p) /* searches for matching '`', '"', or '('. */
{
    char match = '\'';

    if(*p=='"') match = '"';
    else if(*p=='(') match = ')';
    else if(*p!='`') return p; /* no match for this string */
    for(p++;*p!='\0'&&*p!=match;p++)
    {
	if(*p=='\\'&&p[1]!='\0')
        {
	    p++;
        }
	else if(*p=='`'||*p=='"'||*p=='(')
        {
	    if(!*(p=findMatchingChar(p))) break; /* error: end of string */
        }
    }
    return p;
}

/* scan macro string for topic names and external references */
BOOL CheckMacroX(char *ptr)
{
    static char *macro[]=
    {
	"AddAccelerator(ssm","AA(ssm",
	"AppendItem(sssm","AI(sssm",
	"AL(ssts",
	"ChangeButtonBinding(sm","CBB(sm",
	"CB(ssm",
	"CE(sm",
	"ChangeItemBinding(sm","CIB(sm",
	"ExtInsertItem(sssmss",
	"IfThen(mm","IT(mm",
	"IfThenElse(mmm","IE(mmm",
	"ExecFile(sssm","EF(sssm",
	"InsertItem(sssms","II(sssms",
	"JumpContext(x","JC(x","JumpContext(fx","JC(fx",
	"JumpHash(h","JH(h","JumpHash(fh","JH(fh",
	"JumpId(c","JI(c","JumpId(fc","JI(fc",
	"KL(ssts",
	"MPrintID(c",
	"Not(m",
	"PopupId(c","PI(c","PopupId(fc","PI(fc",
	"PopupContext(x","PC(x","PopupContext(fx","PC(fx",
	"PopupHash(h","PopupHash(fh",
	"SetContents(fx",
	"SE(sssssc",
	"SH(sssst",
	"UpdateWindow(fc","UW(fc"
    };
    char *name;
    int namelen,parms,i,n,l;
    char *parm[20];

    while(1)
    {
	while(*ptr==' ') ptr++;
	if(*ptr=='\0') return TRUE;
	if(!isalpha(*ptr)) return FALSE;
	for(name=ptr,namelen=0;isalnum(*ptr)||*ptr=='_';namelen++) ptr++;
	while(*ptr==' ') ptr++;
	if(*ptr!='(') return FALSE;
        ptr++;
	parms=0;
	while(1)
	{
	    while(*ptr==' ') ptr++;
	    parm[parms]=ptr; /* save after skip spaces */
	    if(*ptr=='`'||*ptr=='"')
            {
		parm[parms++]=ptr+1; /* advance past quote */
                ptr=findMatchingChar(ptr);
		if(*ptr=='\0') return FALSE; /* unexpected end of string */
		*ptr++='\0'; /* zap terminating quote */
	        while(*ptr==' '||*ptr=='\t') ptr++;
	    }
            else if(*ptr!=')'&&*ptr!='\0')
            {
	        parm[parms++]=ptr;
	        while(*ptr!=','&&*ptr!=')'&&*ptr!='\0')
                {
	            if(*ptr=='('||*ptr=='`'||*ptr=='"') /* `, " probably not needed */
                    {
                        ptr=findMatchingChar(ptr);
		        if(*ptr=='\0') return FALSE;
                    }
	            ptr++;
                }
	    }
	    if(*ptr==')') break;
	    if(*ptr!=',') return FALSE;
	    *ptr++='\0';
	}
	*ptr++='\0';
	for(i=0;i<sizeof(macro)/sizeof(macro[0]);i++)
	{
	    if (strcspn(macro[i], "(") == namelen &&
                strncasecmp(macro[i], name, namelen) == 0 &&
                strlen(macro[i] + namelen + 1) >= parms)
                break;
	}
	if(i<sizeof(macro)/sizeof(macro[0])) /* macro of interest */
	{
	    char *at;

	    for(n=0;n<parms;n++) unquote(parm[n]);
	    for(n=0;n<parms;n++)
	    {
		if(macro[i][namelen+1+n]=='m')
		{
		    CheckMacroX(parm[n]); /* recursive */
		}
		else if(macro[i][namelen+1+n]=='c')
		{
		    if(extractmacros)
		    {
			while(parm[n][0]==' ') parm[n]++;
			for(l=strlen(parm[n]);l>0&&parm[n][l-1]==' ';l--) ;
			parm[n][l]='\0';
			AddTopic(parm[n],FALSE);
		    }
		}
		else if(macro[i][namelen+1+n]=='f')
		{
		    at=strchr(parm[n],'>');
		    if(at)
		    {
			if(filenamecmp(parm[n],name)!=0)
			{
			    if(macro[i][namelen+2+n]=='c')
			    {
				StoreReference(parm[n],TOPIC,parm[n+1],hash(parm[n+1]));
				n++;
			    }
			    else if(macro[i][namelen+2+n]=='x')
			    {
				StoreReference(parm[n],CONTEXT,parm[n+1],strtoul(parm[n+1],NULL,0));
				n++;
			    }
			    else if(macro[i][namelen+2+n]=='h')
			    {
				StoreReference(parm[n],TOPIC,parm[n+1],strtoul(parm[n+1],NULL,0));
				n++;
			    }
			}
		    }
		}
		else if(macro[i][namelen+1+n]=='t')
		{
		    at=strchr(parm[n],'@');
		    if(at)
		    {
			if(filenamecmp(at+1,name)!=0)
			{
			    *at='\0';
			    StoreReference(at+1,TOPIC,parm[n],hash(parm[n]));
			}
			else
			{
			    AddTopic(parm[n],FALSE);
			}
		    }
		    else
		    {
			AddTopic(parm[n],FALSE);
		    }
		}
	    }
	}
	while(*ptr==' ') ptr++;
	if(*ptr!=':'&&*ptr!=';') break;
	ptr++;
    }
    return TRUE;
}

void CheckMacro(char *ptr)
{
    char *temp;

    if(!multi)
    {
	temp=my_strdup(ptr);
	if(!CheckMacroX(temp)) fprintf(stderr,"Bad macro: %s\n",ptr);
	free(temp);
    }
}

/* check hotspot info of bitmap for topic names and external references and
// extract (write to file) if checkexternal not set, write out first bitmap
// or metafile only (no SHG/MRB) if exportplain (create lookalike rtf) set */
int ExtractBitmap(char *szFilename,MFILE *f)
{
    FILE *fTarget;
    char *filename;
    int type;
    unsigned int i,n,j;
    unsigned char byType,byPacked;
    long l,pos,offset,nextpict,FileStart;
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;
    APMFILEHEADER afh;
    unsigned short *wp;
    unsigned short wMagic,mapmode,colors;
    unsigned long dwRawSize,dwDataSize,dwHotspotOffset,dwOffsBitmap,dwHotspotSize,dwPictureOffset,xPels,yPels;

    FileStart=f->tell(f);
    wMagic=GetWord(f);
    if(wMagic!=0x506C /* SHG */ &&wMagic!=0x706C /* MRB */)
    {
	error("Unknown magic 0x%04X (%c%c)",wMagic,wMagic&0x00FF,wMagic>>8);
	return 0;
    }
    fTarget=NULL;
    n=GetWord(f);
    type=!exportplain&&n>1; /* contains multiple resolutions */
    /* do not depend on wMagic, because it is sometimes incorrect */
    nextpict=4+4*n;
    for(j=0;j<n;j++)
    {
	f->seek(f,FileStart+4+4*j);
	dwOffsBitmap=GetDWord(f);
	f->seek(f,FileStart+dwOffsBitmap);
	byType=f->get(f); /* type of picture: 5=DDB, 6=DIB, 8=METAFILE */
	byPacked=f->get(f); /* packing method: 0=unpacked, 1=RunLen, 2=LZ77, 3=both */
	if(byType==6&&byPacked<4||byType==5&&byPacked<2)
	{
	    type|=2; /* contains bitmap */
	    memset(&bmfh,0,sizeof(bmfh));
	    memset(&bmih,0,sizeof(bmih));
	    bmfh.bfType=0x4D42; /* bitmap magic ("BM") */
	    bmih.biSize=sizeof(bmih);
	    xPels=GetCDWord(f);
	    /* HC30 doesn't like certain PelsPerMeter */
	    if(!before31) bmih.biXPelsPerMeter=(xPels*79L+1)/2;
	    yPels=GetCDWord(f);
	    if(!before31) bmih.biYPelsPerMeter=(yPels*79L+1)/2;
	    bmih.biPlanes=GetCWord(f);
	    bmih.biBitCount=GetCWord(f);
	    bmih.biWidth=GetCDWord(f);
	    bmih.biHeight=GetCDWord(f);
	    colors=(int)(bmih.biClrUsed=GetCDWord(f));
	    if(!colors) colors=(unsigned short)1<<bmih.biBitCount;
	    bmih.biClrImportant=GetCDWord(f);
	    if(after31&&bmih.biClrImportant==1) type|=0x20; /* contains transparent bitmap */
	    dwDataSize=GetCDWord(f);
	    dwHotspotSize=GetCDWord(f);
	    dwPictureOffset=GetDWord(f);
	    dwHotspotOffset=GetDWord(f);
	    if(exportplain||n==1&&(dwHotspotOffset==0L||dwHotspotSize==0L))
	    {
		if(checkexternal) break;
		strcat(szFilename,".BMP");
		fTarget=my_fopen(szFilename,"wb");
		if(fTarget)
		{
		    fwrite(&bmfh,1,sizeof(bmfh),fTarget);
		    fwrite(&bmih,1,sizeof(bmih),fTarget);
		    if(byType==6)
		    {
			CopyBytes(f,colors*4L,fTarget);
		    }
		    else
		    {
			putdw(0x000000L,fTarget);
			putdw(0xFFFFFFL,fTarget);
		    }
		    bmfh.bfOffBits=sizeof(bmfh)+sizeof(bmih)+colors*4L;
		    bmih.biSizeImage=(((bmih.biWidth*bmih.biBitCount+31)/32)*4)*bmih.biHeight;
		    if(byType==5) /* convert DDB to DIB */
		    {
			long width,length;
			unsigned char count,value;
			int pad;

			width=((bmih.biWidth*bmih.biBitCount+15)/16)*2;
			pad=(int)(((width+3)/4)*4-width);
			count=value=0;
			for(l=0;l<bmih.biHeight;l++)
			{
			    if(byPacked==1)
			    {
				for(length=0;length<width;length++)
				{
				    if((count&0x7F)==0)
				    {
					count=f->get(f);
					value=f->get(f);
				    }
				    else if(count&0x80)
				    {
					value=f->get(f);
				    }
				    putc(value,fTarget);
				    count--;
				}
			    }
			    else
			    {
				CopyBytes(f,width,fTarget);
			    }
			    if(pad) fwrite(buffer,pad,1,fTarget);
			}
		    }
		    else
		    {
			DecompressIntoFile(byPacked,f,dwDataSize,fTarget);
		    }
		    /* update bitmap headers */
		    bmfh.bfSize=ftell(fTarget);
		    fseek(fTarget,0L,SEEK_SET);
		    fwrite(&bmfh,1,sizeof(bmfh),fTarget);
		    fwrite(&bmih,1,sizeof(bmih),fTarget);
		}
		break;
	    }
	}
	else if(byType==8&&byPacked<4) /* Windows MetaFile */
	{
	    type|=4; /* contains metafile */
	    memset(&afh,0,sizeof(afh));
	    mapmode=GetCWord(f); /* mapping mode */
	    afh.rcBBox.right=GetWord(f); /* width of metafile-picture */
	    afh.rcBBox.bottom=GetWord(f); /* height of metafile-picture */
	    dwRawSize=GetCDWord(f);
	    dwDataSize=GetCDWord(f);
	    dwHotspotSize=GetCDWord(f);
	    dwPictureOffset=GetDWord(f);
	    dwHotspotOffset=GetDWord(f);
	    if(exportplain||n==1&&(dwHotspotOffset==0L||dwHotspotSize==0L))
	    {
		if(checkexternal) break;
		afh.dwKey=0x9AC6CDD7L;
		afh.wInch=2540;
		wp=(unsigned short *)&afh;
		for(i=0;i<10;i++) afh.wChecksum^=*wp++;
		strcat(szFilename,".WMF");
		fTarget=my_fopen(szFilename,"wb");
		if(fTarget)
		{
		    fwrite(&afh,1,sizeof(afh),fTarget);
		    DecompressIntoFile(byPacked,f,dwDataSize,fTarget);
		}
		break;
	    }
	}
	else
	{
	    error("Unknown format (%d) or packing method (%d)",byType,byPacked);
	    break;
	}
	type|=8; /* contains hotspot info (set before accessing bmpext) */
	if(!checkexternal)
	{
	    if(!fTarget)
	    {
		strcat(szFilename,".");
		strcat(szFilename,bmpext[type&0x0F]);
		fTarget=my_fopen(szFilename,"wb");
		if(!fTarget) break;
		my_putw(wMagic,fTarget);
		my_putw(n,fTarget);
	    }
	    fseek(fTarget,4+4*j,SEEK_SET);
	    putdw(nextpict,fTarget);
	    fseek(fTarget,nextpict,SEEK_SET);
	    putc(byType,fTarget);
	    if(exportLZ77)
	    {
		putc(byPacked,fTarget);
	    }
	    else
	    {
		putc(byPacked&1,fTarget);
	    }
	    if(byType==8)
	    {
		putcw(mapmode,fTarget); /* mapping mode */
		my_putw(afh.rcBBox.right,fTarget); /* width of metafile-picture */
		my_putw(afh.rcBBox.bottom,fTarget); /* height of metafile-picture */
		putcdw(dwRawSize,fTarget);
	    }
	    else
	    {
		putcdw(xPels,fTarget);
		putcdw(yPels,fTarget);
		putcw(bmih.biPlanes,fTarget);
		putcw(bmih.biBitCount,fTarget);
		putcdw(bmih.biWidth,fTarget);
		putcdw(bmih.biHeight,fTarget);
		putcdw(bmih.biClrUsed,fTarget);
		putcdw(bmih.biClrImportant,fTarget);
	    }
	    pos=ftell(fTarget);
	    putdw(0,fTarget); /* changed later ! */
	    putdw(0,fTarget); /* changed later ! */
	    putdw(0,fTarget); /* changed later ! */
	    putdw(0,fTarget); /* changed later ! */
	    if(byType==6) CopyBytes(f,colors*4L,fTarget);
	    offset=ftell(fTarget);
	    f->seek(f,FileStart+dwOffsBitmap+dwPictureOffset);
	    if(exportLZ77)
	    {
		dwDataSize=CopyBytes(f,dwDataSize,fTarget);
	    }
	    else
	    {
		dwDataSize=DecompressIntoFile(byPacked&2,f,dwDataSize,fTarget);
	    }
	}
	if(dwHotspotSize)
	{
	    f->seek(f,FileStart+dwOffsBitmap+dwHotspotOffset);
	    if(f->get(f)!=1)
	    {
		fputs("No hotspots\n",stderr);
		dwHotspotSize=0L;
	    }
	    else
	    {
		unsigned int hotspots,n,j,l;
		unsigned long MacroDataSize;
		char *ptr;
		HOTSPOT *hotspot;

		hotspots=GetWord(f);
		MacroDataSize=GetDWord(f);
		hotspot=my_malloc(hotspots*sizeof(HOTSPOT));
		f->read(f,hotspot,sizeof(HOTSPOT)*hotspots);
		if(checkexternal)
		{
		    while(MacroDataSize-->0) f->get(f);
		}
		else
		{
		    putc(1,fTarget);
		    my_putw(hotspots,fTarget);
		    putdw(MacroDataSize,fTarget);
		    fwrite(hotspot,sizeof(HOTSPOT),hotspots,fTarget);
		    if(MacroDataSize) CopyBytes(f,MacroDataSize,fTarget);
		}
		for(n=0;n<hotspots;n++)
		{
		    j=StringRead(buffer,sizeof(buffer),f)+1;
		    l=j+StringRead(buffer+j,sizeof(buffer)-j,f)+1;
		    if(fTarget) fwrite(buffer,l,1,fTarget);
		    if(extractmacros) switch(hotspot[n].id0)
		    {
		    case 0xC8: /* macro (never seen) */
		    case 0xCC: /* macro without font change */
			CheckMacro(buffer+j);
			break;
		    case 0xE0: /* popup jump HC30 */
		    case 0xE1: /* topic jump HC30 */
		    case 0xE2: /* popup jump HC31 */
		    case 0xE3: /* topic jump HC31 */
		    case 0xE6: /* popup jump without font change */
		    case 0xE7: /* topic jump without font change */
			if(hash(buffer+j)!=hotspot[n].hash)
			{
			    fprintf(stderr,"Wrong hash %08lx instead %08lx for '%s'\n",hotspot[n].hash,hash(buffer+j),buffer+j);
			}
			AddTopic(buffer+j,FALSE);
			break;
		    case 0xEA: /* popup jump into external file */
		    case 0xEB: /* topic jump into external file / secondary window */
		    case 0xEE: /* popup jump into external file without font change */
		    case 0xEF: /* topic jump into external file / secondary window without font change */
			if(hotspot[n].id1!=0&&hotspot[n].id1!=1&&hotspot[n].id1!=4&&hotspot[n].id1!=6||hotspot[n].id2!=0)
			{
			}
			else
			{
			    filename=strchr(buffer+j,'@');
			    if(filename) *filename++='\0';
			    ptr=strchr(buffer+j,'>');
			    if(ptr) *ptr='\0';
			    if(filename)
			    {
				StoreReference(filename,TOPIC,buffer+j,hash(buffer+j));
			    }
			    else
			    {
				AddTopic(buffer+j,FALSE);
			    }
			    break;
			}
		    default:
			error("Unknown hotspot %02x %02x %02x X=%u Y=%u W=%u H=%u %08lx,%s,%s",hotspot[n].id0,hotspot[n].id1,hotspot[n].id2,hotspot[n].x,hotspot[n].y,hotspot[n].w,hotspot[n].h,hotspot[n].hash,buffer,buffer+j);
		    }
		}
		free(hotspot);
	    }
	}
	if(!checkexternal)
	{
	    dwPictureOffset=offset-nextpict;
	    nextpict=ftell(fTarget);
	    /* fix up some locations */
	    fseek(fTarget,pos,SEEK_SET);
	    putdw((dwDataSize<<1)+1,fTarget);
	    putdw((dwHotspotSize<<1)+1,fTarget);
	    putdw(dwPictureOffset,fTarget);
	    if(dwHotspotSize) putdw(dwPictureOffset+dwDataSize,fTarget);
	}
    }
    if(fTarget) my_fclose(fTarget);
    return type;
}
/****************************************************************************
// END OF GRAPHICS STUFF
//**************************************************************************/

char *getbitmapname(unsigned int n) /* retrieve extension of exported bitmap n */
{
    static char name[12];

    if(n<extensions&&extension[n])
    {
	sprintf(name,"bm%u.%s",n,bmpext[extension[n]&0x0F]);
    }
    else if(n==65535U)
    {
	missing=TRUE;
	fputs("There was a picture file rejected on creation of helpfile.\n",stderr);
	strcpy(name,"missing.bmp");
    }
    else /* should never happen */
    {
	warnings=TRUE;
	fprintf(stderr,"Bitmap bm%u not exported\n",n);
	sprintf(name,"bm%u.bmp",n);
    }
    return name;
}

void ListBitmaps(FILE *hpj) /* writes out [BITMAPS] section */
{
    int i;

    if(hpj&&extensions)
    {
	fputs("[BITMAPS]\n",hpj);
	for(i=0;i<extensions;i++) if(extension[i])
	{
	    fprintf(hpj,"bm%u.%s\n",i,bmpext[extension[i]&0x0F]);
	}
	putc('\n',hpj);
    }
}

void ExportBitmaps(FILE *HelpFile) /* export all bitmaps */
{
    BUFFER buf;
    MFILE *mf;
    char *leader;
    char FileName[20];
    long FileLength;
    int i,num,n,type;
    long savepos;

    leader="|bm"+before31;
    SearchFile(HelpFile,NULL,NULL);
    for(n=GetFirstPage(HelpFile,&buf,NULL);n;n=GetNextPage(HelpFile,&buf))
    {
	for(i=0;i<n;i++)
	{
	    my_gets(FileName,sizeof(FileName),HelpFile);
	    getdw(HelpFile);
	    if(memcmp(FileName,leader,strlen(leader))==0)
	    {
		savepos=ftell(HelpFile);
		if(SearchFile(HelpFile,FileName,&FileLength))
		{
		    mf=CreateVirtual(HelpFile);
		    type=ExtractBitmap(FileName+(FileName[0]=='|'),mf);
		    CloseMap(mf);
		    if(type)
		    {
			num=atoi(FileName+(FileName[0]=='|')+2);
			if(num>=extensions)
			{
			    extension=my_realloc(extension,(num+1)*sizeof(char));
			    while(extensions<=num) extension[extensions++]=0;
			}
			extension[num]=type;
		    }
		}
		fseek(HelpFile,savepos,SEEK_SET);
	    }
	}
    }
}

char *TopicName(long topic)
{
    static char name[20];
    int i;

    if(before31)
    {
	if(topic==0L) topic=Topic[0];
	for(i=16;i<Topics;i++) if(Topic[i]==topic)
	{
	    sprintf(name,"TOPIC%d",i);
	    return name;
	}
    }
    else
    {
	if(topic==-1L)
	{
	    NotInAnyTopic=TRUE;
	    return "21KSYK4"; /* evaluates to -1 without generating help compiler warning */
	}
	i=FindContext(topic);
	if(i!=-1)
	{
	    return unhash(ContextRec[i].HashValue);
	}
    }
    if(topic) fprintf(stderr,"Can not find topic offset %08lx\n",topic);
    return NULL;
}

char *GetWindowName(long n) /* secondary window name from window number */
{
    if(windowname==NULL||n<0||n>=windownames||windowname[n]==NULL) return "main";
    return windowname[n];
}

/* create HPJ file from contents of |SYSTEM internal file */
void SysList(FILE *HelpFile,FILE *hpj,char *IconFileName)
{
    long FileLength;
    SYSTEMHEADER SysHdr;
    SYSTEMRECORD *SysRec;
    STOPHEADER StopHdr;
    char name[51];
    char *ptr;
    long color;
    FILE *f;
    int fbreak,macro,windows,i,keywords,dllmaps,n;

    if(hpj&&SearchFile(HelpFile,"|SYSTEM",NULL))
    {
	my_fread(&SysHdr,sizeof(SysHdr),HelpFile);
	if(SysHdr.Minor==15)
	{
	    strcpy(helpcomp,"HC30");
	}
	else if(SysHdr.Minor==21)
	{
	    strcpy(helpcomp,"HC31/HCP");
	}
	else if(SysHdr.Minor==27)
	{
	    strcpy(helpcomp,"WMVC/MVCC");
	}
	else if(SysHdr.Minor==33)
	{
	    if(mvp)
	    {
		strcpy(helpcomp,"MVC");
	    }
	    else
	    {
		strcpy(helpcomp,"HCRTF");
	    }
	}
	fputs("[OPTIONS]\n",hpj);
	if(before31) /* If 3.0 get title */
	{
	    my_gets(HelpFileTitle,33,HelpFile);
	    if(HelpFileTitle[0]!='\0'&&HelpFileTitle[0]!='\n')
	    {
		fprintf(hpj,"TITLE=%s\n",HelpFileTitle);
	    }
	    fprintf(hpj,"INDEX=%s\n",TopicName(0L));
	    if(PhraseCount)
	    {
		fputs("COMPRESS=TRUE\n",hpj);
	    }
	    else
	    {
		fputs("COMPRESS=FALSE\n",hpj);
	    }
	    for(i='A';i<='z';i++) if(lists[i-'0']&&i!='K')
	    {
		fprintf(hpj,"MULTIKEY=%c\n",i);
	    }
	    putc('\n',hpj);
	}
	else  /* else get 3.1 System records */
	{
	    macro=0;
	    fbreak=0;
	    windows=0;
	    keywords=0;
	    dllmaps=0;
	    for(SysRec=GetFirstSystemRecord(HelpFile);SysRec;SysRec=GetNextSystemRecord(SysRec))
	    {
		switch(SysRec->RecordType)
		{
		case 0x0001:
		    if(SysRec->Data[0]) fprintf(hpj,"TITLE=%s\n",SysRec->Data);
		    break;
		case 0x0002:
		    ptr=strchr(SysRec->Data,'\r');
		    if(ptr) strcpy(ptr,"%date");
		    if(SysRec->Data[0]) fprintf(hpj,"COPYRIGHT=%s\n",SysRec->Data);
		    break;
		case 0x0003:
		    ptr=TopicName(*(long *)SysRec->Data);
		    if(ptr) fprintf(hpj,"CONTENTS=%s\n",ptr);
		    break;
		case 0x0004:
		    macro=1;
		    break;
		case 0x0005:
		    fprintf(hpj,"ICON=%s\n",IconFileName);
		    f=my_fopen(IconFileName,"wb");
		    if(f)
		    {
			fwrite(SysRec->Data,SysRec->DataSize,1,f);
			my_fclose(f);
		    }
		    break;
		case 0x0006:
		    windows++;
		    break;
		case 0x0008:
		    if(SysRec->Data[0]) fprintf(hpj,"CITATION=%s\n",SysRec->Data);
		    break;
		case 0x0009:
		    if(!mvp) fprintf(hpj,"LCID=0x%X 0x%X 0x%X\n",*(short *)(SysRec->Data+8),*(short *)SysRec->Data,*(short *)(SysRec->Data+2));
		    break;
		case 0x000A:
		    if(!mvp&&SysRec->Data[0]) fprintf(hpj,"CNT=%s\n",SysRec->Data);
		    break;
		case 0x000B:
//		    if(!mvp) fprintf(hpj,"CHARSET=%d\n",*(unsigned char *)(SysRec->Data+1));
		    break;
		case 0x000C:
		    if(mvp)
		    {
			fbreak=1;
		    }
		    else
		    {
			fprintf(hpj,"DEFFONT=%s,%d,%d\n",SysRec->Data+2,*(unsigned char *)SysRec->Data,*(unsigned char *)(SysRec->Data+1));
		    }
		    break;
		case 0x000D:
		    if(mvp) groups++;
		    break;
		case 0x000E:
		    if(mvp)
		    {
			keywords=1;
		    }
		    else
		    {
			fprintf(hpj,"INDEX_SEPARATORS=\"%s\"\n",SysRec->Data);
			strlcpy(index_separators,SysRec->Data,sizeof(index_separators));
		    }
		    break;
		case 0x0012:
		    if(SysRec->Data[0]) fprintf(hpj,"LANGUAGE=%s\n",SysRec->Data);
		    break;
		case 0x0013:
		    dllmaps=1;
		    break;
		}
	    }
	    if(win95)
	    {
		i=0;
		if(lzcompressed) i|=8;
		if(Hall) i|=4; else if(PhraseCount) i|=2;
		fprintf(hpj,"COMPRESS=%d\n",i);
	    }
	    else if(!lzcompressed)
	    {
		fputs("COMPRESS=OFF\n",hpj);
	    }
	    else if(PhraseCount)
	    {
		fputs("COMPRESS=HIGH\n",hpj);
	    }
	    else
	    {
		fputs("COMPRESS=MEDIUM\n",hpj);
	    }
	    if(SysHdr.Flags==8) fputs("CDROMOPT=TRUE\n",hpj);
	    for(i='A';i<='z';i++) if(lists[i-'0']&&i!='K'&&(i!='A'||!win95))
	    {
		fprintf(hpj,"MULTIKEY=%c\n",i);
	    }
	    putc('\n',hpj);
	    if(windows)
	    {
		fputs("[WINDOWS]\n",hpj);
		for(SysRec=GetFirstSystemRecord(HelpFile);SysRec;SysRec=GetNextSystemRecord(SysRec))
		{
		    if(SysRec->RecordType==0x0006)
		    {
			if(SysRec->DataSize==sizeof(SECWINDOW))
			{
			    PrintWindow(hpj,(SECWINDOW *)SysRec->Data);
			}
			else
			{
			    PrintMVBWindow(hpj,(MVBWINDOW *)SysRec->Data);
			}
		    }
		}
		putc('\n',hpj);
	    }
	    if(macro)
	    {
		fputs("[CONFIG]\n",hpj);
		for(SysRec=GetFirstSystemRecord(HelpFile);SysRec;SysRec=GetNextSystemRecord(SysRec))
		{
		    if(SysRec->RecordType==0x0004)
		    {
			if(sscanf(SysRec->Data,"SPC(%ld)%n",&color,&n)>0)
			{
			    fprintf(hpj,"SPC(%u,%u,%u)%s\n",(unsigned char)(color),(unsigned char)(color>>8),(unsigned char)(color>>16),SysRec->Data+n);
			}
			else
			{
			    fprintf(hpj,"%s\n",SysRec->Data);
			}
		    }
		}
		putc('\n',hpj);
	    }
	    if(fbreak)
	    {
		fputs("[FTINDEX]\n",hpj);
		for(SysRec=GetFirstSystemRecord(HelpFile);SysRec;SysRec=GetNextSystemRecord(SysRec))
		{
		    if(SysRec->RecordType==0x000C)
		    {
			ptr=strtok(SysRec->Data," ");
			if(ptr)
			{
			    fprintf(hpj,"dtype%s",ptr);
			    ptr=strtok(NULL," ");
			    if(ptr)
			    {
				fprintf(hpj,"=%s",ptr);
				ptr=strtok(NULL," ");
				if(ptr)
				{
				    fprintf(hpj,"!%s",ptr);
				    ptr=strtok(NULL," ");
				    if(ptr)
				    {
					fprintf(hpj,",%s",ptr+1);
					if(SearchFile(HelpFile,ptr,NULL))
					{
					    for(n=0;n<stopwordfiles;n++)
					    {
						if(strcmp(stopwordfilename[n],ptr)==0) break;
					    }
					    if(n==stopwordfiles)
					    {
						stopwordfilename=my_realloc(stopwordfilename,(stopwordfiles+1)*sizeof(char *));
						stopwordfilename[stopwordfiles++]=my_strdup(ptr);
						f=my_fopen(ptr+1,"wt");
						if(f)
						{
						    my_fread(&StopHdr,sizeof(StopHdr),HelpFile);
						    for(n=0;n<StopHdr.BytesUsed;n+=1+strlen(buffer))
						    {
							i=getc(HelpFile);
							my_fread(buffer,i,HelpFile);
							buffer[i]='\0';
							fprintf(f,"%s\n",buffer);
						    }
						    my_fclose(f);
						}
					    }
					}
					ptr=strtok(NULL," ");
					if(ptr) fprintf(hpj,",%s",ptr);
				    }
				}
			    }
			    putc('\n',hpj);
			}
		    }
		}
		putc('\n',hpj);
	    }
	    if(groups||multi&&browsenums>1)
	    {
		group=my_malloc(groups*sizeof(GROUP));
		fputs("[GROUPS]\n",hpj);
		i=0;
		for(SysRec=GetFirstSystemRecord(HelpFile);SysRec;SysRec=GetNextSystemRecord(SysRec))
		{
		    if(SysRec->RecordType==0x000D)
		    {
			ptr=strchr(SysRec->Data,' ');
			if(ptr) *ptr++='\0';
			groups=SearchFile(HelpFile,SysRec->Data,NULL);
			n=strcspn(SysRec->Data,".");
			SysRec->Data[n]='\0';
			if(ptr&&strcmp(ptr,"\"\" ")==0)
			{
			    fprintf(hpj,"group=%s\n",SysRec->Data);
			}
			else
			{
			    fprintf(hpj,"group=%s,%s\n",SysRec->Data,ptr);
			}
			group[i].Name=my_strdup(SysRec->Data);
			if(groups)
			{
			    my_fread(&group[i].GroupHeader,sizeof(group[i].GroupHeader),HelpFile);
			    if(group[i].GroupHeader.GroupType==2)
			    {
				group[i].Bitmap=my_malloc(group[i].GroupHeader.BitmapSize);
				my_fread(group[i].Bitmap,group[i].GroupHeader.BitmapSize,HelpFile);
			    }
			}
			else
			{
			    group[i].GroupHeader.GroupType=0;
			}
			i++;
		    }
		}
		if(multi) for(i=1;i<browsenums;i++) fprintf(hpj,"group=BROWSE%04x\n",i);
		if(SearchFile(HelpFile,"|GMACROS",&FileLength))
		{
		    long len;
		    long pos;
		    long off;

		    getdw(HelpFile); /* possible count or group number */
		    for(pos=4L;pos<FileLength;pos+=len)
		    {
			len=getdw(HelpFile); /* length of record containing two longs and two strings */
			off=getdw(HelpFile); /* offset of second string in record (first is at pos 8) */
			if(off<=0L) off=len;
			if(len<8) break;
			if(len<off) break;
			if(off>8)
			{
			    my_fread(buffer,off-8,HelpFile);
			    buffer[off-8]='\0';
			    fprintf(hpj,"entry=%s\n",buffer);
			}
			if(len>off)
			{
			    my_fread(buffer,len-off,HelpFile);
			    buffer[len-off]='\0';
			    fprintf(hpj,"exit=%s\n",buffer);
			}
		    }
		}
		putc('\n',hpj);
	    }
	    if(dllmaps)
	    {
		fputs("[DLLMAPS]\n",hpj);
		for(SysRec=GetFirstSystemRecord(HelpFile);SysRec;SysRec=GetNextSystemRecord(SysRec))
		{
		    if(SysRec->RecordType==0x0013)
		    {
			if(strcmp(SysRec->Data,"MVMCI")!=0&&strcmp(SysRec->Data,"MVIMAGE")!=0&&strcmp(SysRec->Data,"MVBRKR")!=0)
			{
			    ptr=SysRec->Data+strlen(SysRec->Data)+1;
			    fprintf(hpj,"%s=%s,",SysRec->Data,ptr);
			    ptr=ptr+strlen(ptr)+1;
			    fprintf(hpj,"%s,",ptr);
			    ptr=ptr+strlen(ptr)+1;
			    fprintf(hpj,"%s,",ptr);
			    ptr=ptr+strlen(ptr)+1;
			    fprintf(hpj,"%s\n",ptr);
			}
		    }
		}
		putc('\n',hpj);
	    }
	    if(keywords)
	    {
		fputs("[KEYINDEX]\n",hpj);
		for(SysRec=GetFirstSystemRecord(HelpFile);SysRec;SysRec=GetNextSystemRecord(SysRec))
		{
		    if(SysRec->RecordType==0x000E)
		    {
			fprintf(hpj,"keyword=%c,\"%s\"\n",SysRec->Data[1],SysRec->Data+30);
		    }
		}
		putc('\n',hpj);
	    }
	    for(i=0;i<windows;i++)
	    {
		sprintf(name,"|CF%d",i);
		if(SearchFile(HelpFile,name,&FileLength))
		{
		    fprintf(hpj,"[CONFIG:%d]\n",i);
		    /* may use [CONFIG-GetWindowName] instead, but WindowName need not be defined */
		    for(n=0;n<FileLength;n+=strlen(buffer)+1)
		    {
			my_gets(buffer,sizeof(buffer),HelpFile);
			fprintf(hpj,"%s\n",buffer);
		    }
		    putc('\n',hpj);
		}
	    }
	}
    }
}

/* load phrases for decompression from old Phrases file or new PhrIndex,
// PhrImage files of HCRTF */
BOOL PhraseLoad(FILE *HelpFile)
{
    long FileLength;
    char junk[30];
    BOOL newphrases;
    PHRINDEXHDR PhrIndexHdr;
    unsigned int n;
    long l,offset;
    long SavePos;

    if(SearchFile(HelpFile,"|PhrIndex",NULL))
    {
	my_fread(&PhrIndexHdr,sizeof(PhrIndexHdr),HelpFile);
	SavePos=ftell(HelpFile);
	if(SearchFile(HelpFile,"|PhrImage",&FileLength))
	{
	    if(FileLength!=PhrIndexHdr.phrimagecompressedsize)
	    {
		fprintf(stderr,"PhrImage FileSize %ld, in PhrIndex.FileHdr %ld\n",PhrIndexHdr.phrimagecompressedsize,FileLength);
	    }
	    PhraseCount=(unsigned int)PhrIndexHdr.entries;
	    PhraseOffsets=my_malloc(sizeof(unsigned int)*(PhraseCount+1));
	    Phrases=my_malloc(PhrIndexHdr.phrimagesize);
	    if(PhrIndexHdr.phrimagesize==PhrIndexHdr.phrimagecompressedsize)
	    {
		my_fread(Phrases,PhrIndexHdr.phrimagesize,HelpFile);
	    }
	    else
	    {
		DecompressIntoBuffer(2,HelpFile,FileLength,Phrases,PhrIndexHdr.phrimagesize);
	    }
	    fseek(HelpFile,SavePos,SEEK_SET);
	    GetBit(NULL);
	    offset=0;
	    PhraseOffsets[0]=offset;
	    for(l=0;l<PhrIndexHdr.entries;l++)
	    {
		for(n=1;GetBit(HelpFile);n+=1<<PhrIndexHdr.bits) ;
		if(GetBit(HelpFile)) n+=1;
		if(PhrIndexHdr.bits>1) if(GetBit(HelpFile)) n+=2;
		if(PhrIndexHdr.bits>2) if(GetBit(HelpFile)) n+=4;
		if(PhrIndexHdr.bits>3) if(GetBit(HelpFile)) n+=8;
		if(PhrIndexHdr.bits>4) if(GetBit(HelpFile)) n+=16;
		offset+=n;
		PhraseOffsets[(int)l+1]=offset;
	    }
	}
	Hall=TRUE;
	fprintf(stderr,"%u phrases loaded\n",PhraseCount);
    }
    else if(SearchFile(HelpFile,"|Phrases",&FileLength))
    {
	PhraseCount=my_getw(HelpFile);
	newphrases=PhraseCount==0x0800; /* VC4.0: MSDEV\HELP\MSDEV40.MVB */
	if(newphrases) PhraseCount=my_getw(HelpFile);
	if(my_getw(HelpFile)!=0x0100)
	{
	    error("Unknown |Phrases file structure");
	    return FALSE;
	}
	if(PhraseCount)
	{
	    if(before31)
	    {
		offset=(PhraseCount+1)*sizeof(short);
		FileLength-=(PhraseCount+1)*sizeof(short)+4;
		l=FileLength;
	    }
	    else
	    {
		l=getdw(HelpFile);
		if(newphrases)
		{
		    my_fread(&junk,sizeof(junk),HelpFile);
		    offset=(PhraseCount+1)*sizeof(short);
		    FileLength-=(PhraseCount+1)*sizeof(short)+sizeof(junk)+10;
		}
		else
		{
		    offset=(PhraseCount+1)*sizeof(short);
		    FileLength-=(PhraseCount+1)*sizeof(short)+8;
		}
	    }
	    PhraseOffsets=my_malloc(sizeof(unsigned int)*(PhraseCount+1));
	    for(n=0;n<=PhraseCount;n++) PhraseOffsets[n]=my_getw(HelpFile)-offset;
	    Phrases=my_malloc(l);
	    DecompressIntoBuffer((before31?0:2),HelpFile,FileLength,Phrases,l);
	    fprintf(stderr,"%u phrases loaded\n",PhraseCount);
	}
	Hall=FALSE;
    }
    return TRUE;
}

/* write phrase PhraseNum to out and returns advanced out
// or to f it out = NULL or uses PrintString if f = NULL, returns NULL then */
char *PrintPhrase(unsigned int PhraseNum,char *out,FILE *f)
{
    char *ptr;
    size_t len;

    if(PhraseNum>=PhraseCount)
    {
	error("Phrase %u does not exist",PhraseNum);
	return out;
    }
    ptr=Phrases+PhraseOffsets[PhraseNum];
    len=PhraseOffsets[PhraseNum+1]-PhraseOffsets[PhraseNum];
    if(out)
    {
	memcpy(out,ptr,len);
	return out+len;
    }
    if(f)
    {
	fwrite(ptr,len,1,f);
    }
    else
    {
	PrintString(ptr,len);
    }
    return NULL;
}

/* writeout .PH file from already loaded phrases */
void PhraseList(char *FileName)
{
    FILE *f;
    unsigned int n;

    if(PhraseCount)
    {
	f=my_fopen(FileName,"wt");
	if(f)
	{
	    for(n=0;n<PhraseCount;n++)
	    {
		PrintPhrase(n,NULL,f);
		putc('\n',f);
	    }
	    my_fclose(f);
	}
    }
}

char *FontFamily(unsigned int i)
{
    static char *familyname[]={"nil","roman","swiss","modern","script","decor","tech"};

    if(i>0&&i<7) return familyname[i];
    return familyname[0];
}

/* collect another color into color table */
unsigned char AddColor(unsigned char r,unsigned char g,unsigned char b)
{
    int n;

    for(n=0;n<colors;n++)
    {
	if(r==color[n].r&&g==color[n].g&&b==color[n].b) break;
    }
    if(n==colors)
    {
	color[colors].r=r;
	color[colors].g=g;
	color[colors].b=b;
	colors++;
    }
    return n;
}

/* load fonts from help file, filling up internal font structure,
// writing fonttbl, colortbl, and styletbl to rtf file */
void FontLoad(FILE *HelpFile,FILE *rtf,FILE *hpj)
{
    static char *BestFonts[]={"Arial","Times New Roman","MS Sans Serif","MS Serif","Helv","TmsRmn","MS Sans Serif","Helvetica","Times Roman","Times"};
    CHARMAPHEADER CharmapHeader;
    FONTHEADER FontHdr;
    FILE *f;
    char FontName[33];
    char CharMap[33];
    char *ptr;
    char *p;
    long FontStart;
    int i,j,k,l,len;
    unsigned char *family;
    BOOL charmap;
    OLDFONT oldfont;
    NEWFONT newfont;
    MVBFONT mvbfont;
    MVBSTYLE *mvbstyle;
    NEWSTYLE *newstyle;
    FONTDESCRIPTOR *fd;

    if(SearchFile(HelpFile,"|FONT",NULL))
    {
	FontStart=ftell(HelpFile);
	my_fread(&FontHdr,sizeof(FontHdr),HelpFile);
	fontnames=FontHdr.NumFacenames;
	len=(FontHdr.DescriptorsOffset-FontHdr.FacenamesOffset)/fontnames;
	fontname=my_malloc(fontnames*sizeof(char *));
	family=my_malloc(fontnames*sizeof(unsigned char));
	memset(family,0,fontnames*sizeof(unsigned char));
	charmap=FALSE;
	mvbstyle=NULL;
	newstyle=NULL;
	for(i=0;i<fontnames;i++)
	{
	    fseek(HelpFile,FontStart+FontHdr.FacenamesOffset+len*i,SEEK_SET);
	    my_fread(FontName,len,HelpFile);
	    FontName[len]='\0';
	    ptr=strchr(FontName,',');
	    if(ptr&&FontHdr.FacenamesOffset>=16)
	    {
		*ptr++='\0';
		fseek(HelpFile,FontStart+FontHdr.CharmapsOffset,SEEK_SET);
		for(j=0;hpj&&j<FontHdr.NumCharmaps;j++)
		{
		    my_fread(CharMap,32,HelpFile);
		    CharMap[32]='\0';
		    p=strchr(CharMap,',');
		    if(p&&strcmp(p+1,ptr)==0&&strcmp(CharMap,"|MVCHARTAB,0")!=0)
		    {
			if(!charmap)
			{
			    fputs("[CHARMAP]\n",hpj);
			    charmap=TRUE;
			}
			*p++='\0';
			if(strcmp(p,"0")==0)
			{
			    fprintf(hpj,"DEFAULT=%s\n",CharMap);
			}
			else
			{
			    fprintf(hpj,"%s=%s\n",FontName,CharMap);
			}
			break;
		    }
		}
	    }
	    fontname[i]=my_strdup(FontName);
	}
	if(charmap) putc('\n',hpj);
	if(hpj&&FontHdr.FacenamesOffset>=16) for(j=0;j<FontHdr.NumCharmaps;j++)
	{
	    fseek(HelpFile,FontStart+FontHdr.CharmapsOffset+j*32,SEEK_SET);
	    my_fread(CharMap,32,HelpFile);
	    CharMap[32]='\0';
	    p=strchr(CharMap,',');
	    if(p&&strcmp(CharMap,"|MVCHARTAB,0")!=0)
	    {
		*p++='\0';
		if(SearchFile(HelpFile,CharMap,NULL))
		{
		    my_fread(&CharmapHeader,sizeof(CHARMAPHEADER),HelpFile);
		    f=my_fopen(CharMap,"wt");
		    if(f)
		    {
			fprintf(f,"%d,\n",CharmapHeader.Entries);
			for(k=0;k<CharmapHeader.Entries;k++)
			{
			    fprintf(f,"%5u,",my_getw(HelpFile));
			    fprintf(f,"%5u,",my_getw(HelpFile));
			    fprintf(f,"%3u,",getc(HelpFile));
			    fprintf(f,"%3u,",getc(HelpFile));
			    fprintf(f,"%3u,",getc(HelpFile));
			    fprintf(f,"%3u,\n",getc(HelpFile));
			    my_getw(HelpFile);
			}
			fprintf(f,"%d,\n",CharmapHeader.Ligatures);
			for(k=0;k<CharmapHeader.Ligatures;k++)
			{
			    for(l=0;l<CharmapHeader.LigLen;l++)
			    {
				fprintf(f,"%3u,",getc(HelpFile));
			    }
			    putc('\n',f);
			}
			my_fclose(f);
		    }
		}
	    }
	}
	fseek(HelpFile,FontStart+FontHdr.DescriptorsOffset,SEEK_SET);
	colors=1;     /* auto */
	color[0].r=1;
	color[0].g=1;
	color[0].b=0;
	fonts=FontHdr.NumDescriptors;
	if(font) free(font);
	font=my_malloc(fonts*sizeof(FONTDESCRIPTOR));
	memset(font,0,fonts*sizeof(FONTDESCRIPTOR));
	if(FontHdr.FacenamesOffset>=16)
	{
	    scaling=1L;
	    rounderr=0;
	    for(i=0;i<FontHdr.NumDescriptors;i++)
	    {
		my_fread(&mvbfont,sizeof(mvbfont),HelpFile);
		fd=font+i;
		fd->FontName=mvbfont.FontName;
		fd->HalfPoints=-2L*mvbfont.Height;
		fd->Bold=mvbfont.Weight>500;
		fd->Italic=mvbfont.Italic!=0;
		fd->Underline=mvbfont.Underline!=0;
		fd->StrikeOut=mvbfont.StrikeOut!=0;
		fd->DoubleUnderline=mvbfont.DoubleUnderline!=0;
		fd->SmallCaps=mvbfont.SmallCaps!=0;
		fd->textcolor=AddColor(mvbfont.FGRGB[0],mvbfont.FGRGB[1],mvbfont.FGRGB[2]);
		fd->backcolor=AddColor(mvbfont.BGRGB[0],mvbfont.BGRGB[1],mvbfont.BGRGB[2]);
		fd->FontFamily=mvbfont.PitchAndFamily>>4;
		fd->style=mvbfont.style;
		fd->up=mvbfont.up;
		fd->expndtw=mvbfont.expndtw;
	    }
	    fseek(HelpFile,FontStart+FontHdr.FormatsOffset,SEEK_SET);
	    mvbstyle=my_malloc(FontHdr.NumFormats*sizeof(MVBSTYLE));
	    my_fread(mvbstyle,FontHdr.NumFormats*sizeof(MVBSTYLE),HelpFile);
	    for(i=0;i<FontHdr.NumFormats;i++)
	    {
		MVBSTYLE *m;

		m=mvbstyle+i;
		m->font.FGRGB[0]=AddColor(m->font.FGRGB[0],m->font.FGRGB[1],m->font.FGRGB[2]);
		m->font.BGRGB[0]=AddColor(m->font.BGRGB[0],m->font.BGRGB[1],m->font.BGRGB[2]);
	    }
	}
	else if(FontHdr.FacenamesOffset>=12)
	{
	    scaling=1L;
	    rounderr=0;
	    for(i=0;i<FontHdr.NumDescriptors;i++)
	    {
		my_fread(&newfont,sizeof(newfont),HelpFile);
		fd=font+i;
		fd->Bold=newfont.Weight>500;
		fd->Italic=newfont.Italic!=0;
		fd->Underline=newfont.Underline!=0;
		fd->StrikeOut=newfont.StrikeOut!=0;
		fd->DoubleUnderline=newfont.DoubleUnderline!=0;
		fd->SmallCaps=newfont.SmallCaps!=0;
		fd->FontName=newfont.FontName;
		fd->HalfPoints=-2L*newfont.Height;
		fd->textcolor=AddColor(newfont.FGRGB[0],newfont.FGRGB[1],newfont.FGRGB[2]);
		fd->backcolor=AddColor(newfont.BGRGB[0],newfont.BGRGB[1],newfont.BGRGB[2]);
		fd->FontFamily=newfont.PitchAndFamily>>4;
	    }
	    fseek(HelpFile,FontStart+FontHdr.FormatsOffset,SEEK_SET);
	    newstyle=my_malloc(FontHdr.NumFormats*sizeof(NEWSTYLE));
	    my_fread(newstyle,FontHdr.NumFormats*sizeof(NEWSTYLE),HelpFile);
	    for(i=0;i<FontHdr.NumFormats;i++)
	    {
		NEWSTYLE *m;

		m=newstyle+i;
		m->font.FGRGB[0]=AddColor(m->font.FGRGB[0],m->font.FGRGB[1],m->font.FGRGB[2]);
		m->font.BGRGB[0]=AddColor(m->font.BGRGB[0],m->font.BGRGB[1],m->font.BGRGB[2]);
	    }
	}
	else
	{
	    scaling=10L;
	    rounderr=5;
	    for(i=0;i<FontHdr.NumDescriptors;i++)
	    {
		my_fread(&oldfont,sizeof(oldfont),HelpFile);
		fd=font+i;
		fd->Bold=(oldfont.Attributes&FONT_BOLD)!=0;
		fd->Italic=(oldfont.Attributes&FONT_ITAL)!=0;
		fd->Underline=(oldfont.Attributes&FONT_UNDR)!=0;
		fd->StrikeOut=(oldfont.Attributes&FONT_STRK)!=0;
		fd->DoubleUnderline=(oldfont.Attributes&FONT_DBUN)!=0;
		fd->SmallCaps=(oldfont.Attributes&FONT_SMCP)!=0;
		fd->FontName=oldfont.FontName;
		fd->HalfPoints=oldfont.HalfPoints;
		fd->textcolor=AddColor(oldfont.FGRGB[0],oldfont.FGRGB[1],oldfont.FGRGB[2]);
		fd->backcolor=AddColor(oldfont.BGRGB[0],oldfont.BGRGB[1],oldfont.BGRGB[2]);
		if(oldfont.FontFamily<6)
		{
		    fd->FontFamily=lookup[oldfont.FontFamily];
		}
		else
		{
		    fd->FontFamily=oldfont.FontFamily;
		}
	    }
	}
	for(i=0;i<FontHdr.NumDescriptors;i++)
	{
	    if(font[i].FontName<fontnames)
	    {
		family[font[i].FontName]=font[i].FontFamily;
	    }
	}
	DefFont=0;
	l=sizeof(BestFonts)/sizeof(BestFonts[0]);
	if(fontname)
	{
	    for(i=0;i<fontnames;i++) if(family[i])
	    {
		for(j=0;j<l;j++)
		{
		    if (strcasecmp(fontname[i], BestFonts[j]) == 0)
		    {
			DefFont=i;
			l=j;
			break;
		    }
		}
	    }
	}
	fprintf(rtf,"{\\rtf1\\ansi\\deff%d\n{\\fonttbl",DefFont);
	for(i=0;i<fontnames;i++)
	{
	    fprintf(rtf,"{\\f%d\\f%s %s;}",i,FontFamily(family[i]),fontname[i]);
	    free(fontname[i]);
	}
	free(fontname);
	fputs("}\n",rtf);
	if(colors>1)
	{
	    fputs("{\\colortbl;",rtf);
	    for(i=1;i<colors;i++) fprintf(rtf,"\\red%d\\green%d\\blue%d;",color[i].r,color[i].g,color[i].b);
	    fputs("}\n",rtf);
	}
	fprintf(rtf,"{\\stylesheet{\\fs%d \\snext0 Normal;}\n",font[0].HalfPoints);
	if(mvbstyle)
	{
	    for(i=0;i<FontHdr.NumFormats;i++)
	    {
		MVBSTYLE *m,*n;

		m=mvbstyle+i;
		fprintf(rtf,"{\\*\\cs%u \\additive",m->StyleNum+9);
		if(m->BasedOn)
		{
		    n=mvbstyle+(m->BasedOn-1);
		    if(m->font.FontName!=n->font.FontName) fprintf(rtf,"\\f%d",m->font.FontName);
		    if(m->font.expndtw!=n->font.expndtw) fprintf(rtf,"\\expndtw%d",m->font.expndtw);
		    if(m->font.FGRGB[0]!=n->font.FGRGB[0]) fprintf(rtf,"\\cf%d",m->font.FGRGB[0]);
		    if(m->font.BGRGB[0]!=n->font.BGRGB[0]) fprintf(rtf,"\\cb%d",m->font.BGRGB[0]);
		    if(m->font.Height!=n->font.Height) fprintf(rtf,"\\fs%ld",-2L*m->font.Height);
		    if((m->font.Weight>500)!=(n->font.Weight>500)) fprintf(rtf,"\\b%d",m->font.Weight>500);
		    if(m->font.Italic!=n->font.Italic) fprintf(rtf,"\\i%d",m->font.Italic);
		    if(m->font.Underline!=n->font.Underline) fprintf(rtf,"\\ul%d",m->font.Underline);
		    if(m->font.StrikeOut!=n->font.StrikeOut) fprintf(rtf,"\\strike%d",m->font.StrikeOut);
		    if(m->font.DoubleUnderline!=n->font.DoubleUnderline) fprintf(rtf,"\\uldb%d",m->font.DoubleUnderline);
		    if(m->font.SmallCaps!=n->font.SmallCaps) fprintf(rtf,"\\scaps%d",m->font.SmallCaps);
		    if(m->font.up!=n->font.up) if(m->font.up>0) fprintf(rtf,"\\up%d",m->font.up); else fprintf(rtf,"\\dn%d",-m->font.up);
		    fprintf(rtf," \\sbasedon%u",m->BasedOn+9);
		}
		else
		{
		    fprintf(rtf,"\\f%d",m->font.FontName);
		    if(m->font.Italic) fputs("\\i",rtf);
		    if(m->font.Weight>500) fputs("\\b",rtf);
		    if(m->font.Underline) fputs("\\ul",rtf);
		    if(m->font.StrikeOut) fputs("\\strike",rtf);
		    if(m->font.DoubleUnderline) fputs("\\uldb",rtf);
		    if(m->font.SmallCaps) fputs("\\scaps",rtf);
		    if(m->font.expndtw) fprintf(rtf,"\\expndtw%d",m->font.expndtw);
		    if(m->font.up>0) fprintf(rtf,"\\up%d",m->font.up);
		    else if(m->font.up<0) fprintf(rtf,"\\dn%d",-m->font.up);
		    fprintf(rtf,"\\fs%ld",-2*m->font.Height);
		    if(m->font.FGRGB[0]) fprintf(rtf,"\\cf%d",m->font.FGRGB[0]);
		    if(m->font.BGRGB[0]) fprintf(rtf,"\\cb%d",m->font.BGRGB[0]);
		}
		fprintf(rtf," %s;}\n",m->StyleName);
	    }
	    free(mvbstyle);
	}
	else if(newstyle)
	{
	    for(i=0;i<FontHdr.NumFormats;i++)
	    {
		NEWSTYLE *m,*n;

		m=newstyle+i;
		fprintf(rtf,"{\\*\\cs%u \\additive",m->StyleNum+9);
		if(m->BasedOn)
		{
		    n=newstyle+(m->BasedOn-1);
		    if(m->font.FontName!=n->font.FontName) fprintf(rtf,"\\f%d",m->font.FontName);
		    if(m->font.FGRGB[0]!=n->font.FGRGB[0]) fprintf(rtf,"\\cf%d",m->font.FGRGB[0]);
		    if(m->font.BGRGB[0]!=n->font.BGRGB[0]) fprintf(rtf,"\\cb%d",m->font.BGRGB[0]);
		    if(m->font.Height!=n->font.Height) fprintf(rtf,"\\fs%ld",-2L*m->font.Height);
		    if((m->font.Weight>500)!=(n->font.Weight>500)) fprintf(rtf,"\\b%d",m->font.Weight>500);
		    if(m->font.Italic!=n->font.Italic) fprintf(rtf,"\\i%d",m->font.Italic);
		    if(m->font.Underline!=n->font.Underline) fprintf(rtf,"\\ul%d",m->font.Underline);
		    if(m->font.StrikeOut!=n->font.StrikeOut) fprintf(rtf,"\\strike%d",m->font.StrikeOut);
		    if(m->font.DoubleUnderline!=n->font.DoubleUnderline) fprintf(rtf,"\\uldb%d",m->font.DoubleUnderline);
		    if(m->font.SmallCaps!=n->font.SmallCaps) fprintf(rtf,"\\scaps%d",m->font.SmallCaps);
		    fprintf(rtf," \\sbasedon%u",m->BasedOn+9);
		}
		else
		{
		    fprintf(rtf,"\\f%d",m->font.FontName);
		    if(m->font.Italic) fputs("\\i",rtf);
		    if(m->font.Weight>500) fputs("\\b",rtf);
		    if(m->font.Underline) fputs("\\ul",rtf);
		    if(m->font.StrikeOut) fputs("\\strike",rtf);
		    if(m->font.DoubleUnderline) fputs("\\uldb",rtf);
		    if(m->font.SmallCaps) fputs("\\scaps",rtf);
		    fprintf(rtf,"\\fs%ld",-2*m->font.Height);
		    if(m->font.FGRGB[0]) fprintf(rtf,"\\cf%d",m->font.FGRGB[0]);
		    if(m->font.BGRGB[0]) fprintf(rtf,"\\cb%d",m->font.BGRGB[0]);
		}
		fprintf(rtf," %s;}\n",m->StyleName);
	    }
	    free(newstyle);
	}
	if(family) free(family);
	fputs("}\\pard\\plain\n",rtf);
	memset(&CurrentFont,0,sizeof(CurrentFont));
	CurrentFont.FontName=DefFont;
	if(hpj)
	{
	    fprintf(stderr,"%u font names, %u font descriptors",fontnames,FontHdr.NumDescriptors);
	    if(FontHdr.FacenamesOffset>=12) printf(", %u font styles",FontHdr.NumFormats);
	    fputs(" loaded\n",stderr);
	}
    }
}

/* read NumBytes from |TOPIC starting at TopicPos (or if TopicPos is 0
// where last left off) into dest, returning number of bytes read.
// TopicRead handles LZ77 decompression and the crossing of topic blocks */
long TopicRead(FILE *HelpFile,long TopicPos,void *dest,long NumBytes)
{
    static TOPICBLOCKHEADER TopicBlockHeader;
    static unsigned char TopicBuffer[0x4000];
    static long TopicFileStart;
    static long TopicBlockNum;
    unsigned int TopicBlockOffset;
    static unsigned int DecompSize;
    static long LastTopicPos;
    unsigned int n;

    if(!TopicFileStart) /* first call: HelpFile is at start of |TOPIC */
    {
	TopicFileStart=ftell(HelpFile);
	TopicBlockNum=-1L;
    }
    if(!TopicPos) TopicPos=LastTopicPos; /* continue where left off */
    if((TopicPos-sizeof(TOPICBLOCKHEADER))/DecompressSize!=TopicBlockNum) /* other topic block */
    {
	TopicBlockNum=(TopicPos-sizeof(TOPICBLOCKHEADER))/DecompressSize;
	if(TopicBlockNum*TopicBlockSize>=TopicFileLength) return 0;
	fseek(HelpFile,TopicFileStart+TopicBlockNum*TopicBlockSize,SEEK_SET);
	n=TopicBlockSize;
	if(n+TopicBlockNum*TopicBlockSize>TopicFileLength)
	{
	    n=(unsigned int)(TopicFileLength-TopicBlockNum*TopicBlockSize);
	}
	my_fread(&TopicBlockHeader,sizeof(TOPICBLOCKHEADER),HelpFile);
	n-=sizeof(TOPICBLOCKHEADER);
	if(lzcompressed)
	{
	    DecompSize=DecompressIntoBuffer(2,HelpFile,n,TopicBuffer,sizeof(TopicBuffer));
	}
	else
	{
	    DecompSize=my_fread(TopicBuffer,n,HelpFile);
	}
    }
    TopicBlockOffset=(TopicPos-sizeof(TOPICBLOCKHEADER))%DecompressSize;
    if(TopicBlockOffset+NumBytes>DecompSize) /* more than available in this block */
    {
	n=DecompSize-TopicBlockOffset;
	if(n) memcpy(dest,TopicBuffer+TopicBlockOffset,n);
	return n+TopicRead(HelpFile,(TopicBlockNum+1)*DecompressSize+sizeof(TOPICBLOCKHEADER),(char *)dest+n,NumBytes-n);
    }
    if(NumBytes) memcpy(dest,TopicBuffer+TopicBlockOffset,NumBytes);
    LastTopicPos=TopicPos+NumBytes;
    return NumBytes;
}

/* Hall or oldstyle Phrase replacement of str into out */
char *PhraseReplace(unsigned char *str,long len,char *out)
{
    int CurChar;

    if(Hall)
    {
	while(len)
	{
	    CurChar=*str++;
	    len--;
	    if((CurChar&1)==0) /* phrases 0..127 */
	    {
		out=PrintPhrase(CurChar/2,out,NULL);
	    }
	    else if((CurChar&3)==1) /* phrases 128..16511 */
	    {
		CurChar=128+(CurChar/4)*256+*str++;
		len--;
		out=PrintPhrase(CurChar,out,NULL);
	    }
	    else if((CurChar&7)==3) /* copy next n characters */
	    {
		while(CurChar>0)
		{
		    *out++=*str++;
		    len--;
		    CurChar-=8;
		}
	    }
	    else if((CurChar&0x0F)==0x07)
	    {
		while(CurChar>0)
		{
		    *out++=' ';
		    CurChar-=16;
		}
	    }
	    else /* if((CurChar&0x0F)==0x0F) */
	    {
		while(CurChar>0)
		{
		    *out++='\0';
		    CurChar-=16;
		}
	    }
	}
    }
    else
    {
	while(len)
	{
	    CurChar=*str++;
	    len--;
	    if(CurChar>0&&CurChar<16) /* phrase 0..1919 */
	    {
		CurChar=256*(CurChar-1)+*str++;
		len--;
		out=PrintPhrase(CurChar/2,out,NULL);
		if(CurChar&1) *out++=' ';
	    }
	    else
	    {
		*out++=CurChar;
	    }
	}
    }
    return out;
}

/* reads next chunk from |TOPIC like TopicRead, but does phrase decompression
// if Length > NumBytes, suitable to read LinkData2. If phrase decompression
// doesn't expands to Length bytes, buffer is padded using 0. TopicPhraseRead
// always NUL-terminates at dest[Length] just to be save */
long TopicPhraseRead(FILE *HelpFile,long TopicPos,char *dest,long NumBytes,long Length)
{
    char *buffer;
    long BytesRead;

    if(Length<=NumBytes) /* no phrase compression in this case */
    {
	BytesRead=TopicRead(HelpFile,TopicPos,dest,Length);
	if(BytesRead==Length&&Length<NumBytes) /* some trailing bytes are not used (bug in HCRTF ?) */
	{
	    buffer=my_malloc(NumBytes-Length);
	    BytesRead+=TopicRead(HelpFile,0L,buffer,NumBytes-Length);
	    free(buffer);
	}
    }
    else
    {
	buffer=my_malloc(NumBytes);
	BytesRead=TopicRead(HelpFile,TopicPos,buffer,NumBytes);
	NumBytes=PhraseReplace(buffer,NumBytes,dest)-dest;
	free(buffer);
	if(NumBytes>Length)
	{
	    error("Phrase replacement delivers %ld bytes instead of %ld",NumBytes,Length);
	    exit(1);
	}
    }
    while(NumBytes<=Length) dest[NumBytes++]='\0';
    return BytesRead;
}

void Annotate(long pos,FILE *rtf)
{
    long FileLength;
    char FileName[19];
    int i;
    long l;

    sprintf(FileName,"%ld!0",pos);
    if(SearchFile(AnnoFile,FileName,&FileLength))
    {
	fputs("{\\v {\\*\\atnid ANN}\\chatn {\\*\\annotation \\pard\\plain {\\chatn }",rtf);
	for(l=0;l<FileLength&&(i=getc(AnnoFile))!=-1;l++)
	{
	    if(i==0x0D)
	    {
		fputs("\\par\n",rtf);
	    }
	    else if(i!='{'&&i!='}'&&i!='\\'&&isprint(i))
	    {
		putc(i,rtf);
	    }
	    else if(i=='{')
	    {
		fputs("\\{\\-",rtf);
	    }
	    else if(i!='\0'&&i!=0x0A)
	    {
		//fprintf(rtf,"\\'%02x",i);
		fprintf(rtf, "{\\u%u}", cp1251_to_unicode(i));
	    }
	}
	fputs("}}",rtf);
    }
}

/* collect all keywords assigned to positions starting at NextKeywordOffset
// from all keyword lists, saving the first MAXKEYWORDS in KeywordRec
// (which is allocated to MAXKEYWORDS if NULL) for use in ListKeywords.
// updates NextKeywordOffset, clears NextKeywordRec, sets KeywordRecs. */
void CollectKeywords(FILE *HelpFile)
{
    unsigned short j,m;
    int i,n,k,l,map;
    long FileLength,savepos,KWDataOffset,from;
    long *keytopic;
    BUFFER buf;
    char kwdata[10];
    char kwbtree[10];

    fputs("Collecting keywords...",stderr);
    savepos=ftell(HelpFile);
    if(KeywordRec) /* free old keywords */
    {
	for(i=0;i<KeywordRecs;i++)
	{
	    if(KeywordRec[i].Keyword) free(KeywordRec[i].Keyword);
	}
    }
    else
    {
	KeywordRec=my_malloc(MAXKEYWORDS*sizeof(KEYWORDREC));
    }
    NextKeywordRec=KeywordRecs=0;
    from=NextKeywordOffset;
    NextKeywordOffset=0x7FFFFFFFL;
    for(k=0;k<2;k++) for(map='0';map<='z';map++)
    {
	if(k)
	{
	    if(!keyindex[map-'0']) continue;
	    sprintf(kwdata,"|%cKWDATA",map);
	    sprintf(kwbtree,"|%cKWBTREE",map);
	}
	else
	{
	    if(!lists[map-'0']) continue;
	    sprintf(kwdata,"|%cWDATA",map);
	    sprintf(kwbtree,"|%cWBTREE",map);
	}
	if(SearchFile(HelpFile,kwdata,&FileLength))
	{
	    keytopic=my_malloc(FileLength);
	    my_fread(keytopic,FileLength,HelpFile);
	    if(SearchFile(HelpFile,kwbtree,NULL))
	    {
		for(n=GetFirstPage(HelpFile,&buf,NULL);n;n=GetNextPage(HelpFile,&buf))
		{
		    for(i=0;i<n;i++)
		    {
			my_gets(keyword,sizeof(keyword),HelpFile);
			m=my_getw(HelpFile);
			KWDataOffset=getdw(HelpFile);
			for(j=0;j<m;j++)
			{
			    if(keytopic[KWDataOffset/4+j]>=from)
			    {
				if(KeywordRecs>=MAXKEYWORDS)
				{
				    NextKeywordOffset=KeywordRec[KeywordRecs-1].TopicOffset;
				    while(KeywordRec[KeywordRecs-1].TopicOffset==NextKeywordOffset)
				    {
					KeywordRecs--;
					if(KeywordRec[KeywordRecs].Keyword) free(KeywordRec[KeywordRecs].Keyword);
				    }
				}
				l=KeywordRecs;
				while(l>0&&KeywordRec[l-1].TopicOffset>keytopic[KWDataOffset/4+j])
				{
				    KeywordRec[l].KeyIndex=KeywordRec[l-1].KeyIndex;
				    KeywordRec[l].Footnote=KeywordRec[l-1].Footnote;
				    KeywordRec[l].Keyword=KeywordRec[l-1].Keyword;
				    KeywordRec[l].TopicOffset=KeywordRec[l-1].TopicOffset;
				    l--;
				}
				KeywordRec[l].KeyIndex=k>0;
				KeywordRec[l].Footnote=map;
				KeywordRec[l].Keyword=my_strdup(keyword);
				KeywordRec[l].TopicOffset=keytopic[KWDataOffset/4+j];
				KeywordRecs++;
			    }
			}
		    }
		}
		free(keytopic);
	    }
	}
    }
    fseek(HelpFile,savepos,SEEK_SET);
    for(i=0;i<22;i++) fputs("\b \b",stderr);
}

/* writes out all keywords appearing up to position TopicOffset and eats
// them up so they are not written out again. Merges keywords if possible */
void ListKeywords(FILE *HelpFile,FILE *rtf,long TopicOffset)
{
    int len,footnote,keyindex;

    if(NextKeywordRec>=KeywordRecs)
    {
	if(NextKeywordOffset==0x7FFFFFFFL) return;
	CollectKeywords(HelpFile);
    }
    footnote=keyindex=len=0;
    while(NextKeywordRec<KeywordRecs&&KeywordRec[NextKeywordRec].TopicOffset<=TopicOffset)
    {
	if(len>0&&(KeywordRec[NextKeywordRec].Footnote!=footnote||KeywordRec[NextKeywordRec].KeyIndex!=keyindex||len+strlen(KeywordRec[NextKeywordRec].Keyword)>(after31?1023:254)))
	{
	    fputs("}\n",rtf);
	    len=0;
	}
	if(len>0)
	{
	    putc(';',rtf);
	}
	else if(KeywordRec[NextKeywordRec].KeyIndex)
	{
	    fprintf(rtf,"{\\up K}{\\footnote\\pard\\plain{\\up K} %c:",KeywordRec[NextKeywordRec].Footnote);
	}
	else
	{
	    fprintf(rtf,"{\\up %c}{\\footnote\\pard\\plain{\\up %c} ",KeywordRec[NextKeywordRec].Footnote,KeywordRec[NextKeywordRec].Footnote);
	}
	len+=strlen(KeywordRec[NextKeywordRec].Keyword)+1;
	putrtf(rtf,KeywordRec[NextKeywordRec].Keyword);
	footnote=KeywordRec[NextKeywordRec].Footnote;
	keyindex=KeywordRec[NextKeywordRec].KeyIndex;
	NextKeywordRec++;
    }
    if(len) fputs("}\n",rtf);
}

/* create > footnote if topic at TopicOffset has a window assigned to
// using the |VIOLA internal file. Read VIOLA sequentially, reloading
// next page only if neccessary, because it is properly ordered. */
int ListWindows(FILE *HelpFile,long TopicOffset)
{
    long savepos;
    static int n,i;
    static BUFFER buf;
    static int VIOLAfound=-1;
    static VIOLAREC *Viola;
    int result;

    if(VIOLAfound==0) return -1;
    savepos=ftell(HelpFile);
    if(VIOLAfound==-1)
    {
	VIOLAfound=0;
	if(SearchFile(HelpFile,"|VIOLA",NULL))
	{
	    n=GetFirstPage(HelpFile,&buf,NULL);
	    if(n)
	    {
		Viola=my_malloc(n*sizeof(VIOLAREC));
		my_fread(Viola,n*sizeof(VIOLAREC),HelpFile);
		i=0;
		VIOLAfound=1;
	    }
	}
    }
    result=-1;
    if(VIOLAfound==1)
    {
	while(i>=n||TopicOffset>Viola[i].TopicOffset)
	{
	    if(i>=n)
	    {
		free(Viola);
		n=GetNextPage(HelpFile,&buf);
		if(n==0)
		{
		    VIOLAfound=0;
		    break;
		}
		Viola=my_malloc(n*sizeof(VIOLAREC));
		my_fread(Viola,n*sizeof(VIOLAREC),HelpFile);
		i=0;
	    }
	    else
	    {
		i++;
	    }
	}
	if(i<n&&Viola[i].TopicOffset==TopicOffset)
	{
	    result=Viola[i].WindowNumber;
	}
    }
    fseek(HelpFile,savepos,SEEK_SET);
    return result;
}

/* Browse sequence handling support functions. As an efficient means to
// resolve browse sequences, FirstPass numbers and saves all browse
// sequence start positions (that are topics where BrowseNext and
// BrowsePrev both point to some topic behind) and links them with the
// following topics. Whenever a topic is merged to the beginning of a
// browse sequence, it's start topics browse subnumber is incremented.
// The interesting part is a topic where BrowsePrev and BrowseNext both
// point to earlier topics. Two different browse sequences need to be
// merged at this point, that is will get the same browse start number
// and one start topics subnumber needs to accommodate the other browse
// sequence. Using the start records, TopicDump does know which browse
// sequence starts at which topic and knows the browse sequence number
// and subnumber assigned. */
void AddStart(long StartTopic,int BrowseNum,int Count)
{
    start=my_realloc(start,(starts+1)*sizeof(START));
    start[starts].StartTopic=StartTopic;
    start[starts].BrowseNum=BrowseNum;
    start[starts].Start=Count;
    starts++;
}

void FixStart(int BrowseNum,int NewBrowseNum,int AddCount)
{
    int i;

    for(i=0;i<starts;i++) if(start[i].BrowseNum==BrowseNum)
    {
	start[i].BrowseNum=NewBrowseNum;
	start[i].Start+=AddCount;
    }
}

void AddBrowse(long StartTopic,long NextTopic,long PrevTopic)
{
    int i;

    for(i=0;i<browses;i++) if(browse[i].StartTopic==-1L) break; /* empty space in array ? */
    if(i==browses) /* no empty space, add to array */
    {
	browse=my_realloc(browse,++browses*sizeof(BROWSE));
    }
    browse[i].StartTopic=StartTopic;
    browse[i].NextTopic=NextTopic;
    browse[i].PrevTopic=PrevTopic;
    browse[i].BrowseNum=browsenums++;
    browse[i].Start=1;
    browse[i].Count=1;
}

void MergeBrowse(long TopicOffset,long OtherTopicOffset,long NextTopic,long PrevTopic)
{
    int i,j;

    for(i=0;i<browses;i++) if(browse[i].StartTopic!=-1L)
    {
	if(browse[i].NextTopic==TopicOffset||browse[i].NextTopic==OtherTopicOffset) break;
    }
    for(j=0;j<browses;j++) if(browse[j].StartTopic!=-1L)
    {
	if(browse[j].PrevTopic==TopicOffset||browse[j].PrevTopic==OtherTopicOffset) break;
    }
    if(i<browses&&j<browses)
    {
	browse[i].Count++;
	browse[i].NextTopic=browse[j].NextTopic;
	FixStart(browse[j].BrowseNum,browse[i].BrowseNum,browse[i].Count);
	browse[j].Start+=browse[i].Count;
	AddStart(browse[j].StartTopic,browse[i].BrowseNum,browse[j].Start);
	browse[i].Count+=browse[j].Count;
	browse[j].StartTopic=-1L;
	if(browse[i].NextTopic==-1L&&browse[i].PrevTopic==-1L)
	{
	    AddStart(browse[i].StartTopic,browse[i].BrowseNum,browse[i].Start);
	    browse[i].StartTopic=-1L;
	}
    }
    else
    {
	warnings=TRUE;
	fprintf(stderr,"Can not merge %08lx %08lx %08lx\n",TopicOffset,NextTopic,PrevTopic);
    }
}

void LinkBrowse(long TopicOffset,long OtherTopicOffset,long NextTopic,long PrevTopic)
{
    int i;

    for(i=0;i<browses;i++) if(browse[i].StartTopic!=-1L)
    {
	if(browse[i].NextTopic==TopicOffset||browse[i].NextTopic==OtherTopicOffset) break;
    }
    if(i<browses)
    {
	browse[i].NextTopic=NextTopic;
	browse[i].Count++;
	if(browse[i].NextTopic==-1L&&browse[i].PrevTopic==-1L)
	{
	    AddStart(browse[i].StartTopic,browse[i].BrowseNum,browse[i].Start);
	    browse[i].StartTopic=-1L;
	}
    }
    else
    {
	warnings=TRUE;
	fprintf(stderr,"Can not link %08lx %08lx %08lx\n",TopicOffset,NextTopic,PrevTopic);
	for(i=0;i<browses;i++) if(browse[i].StartTopic!=-1L)
	{
	    fprintf(stderr,"Open browse %08lx %08lx\n",browse[i].PrevTopic,browse[i].NextTopic);
	}
    }
}

void BackLinkBrowse(long TopicOffset,long OtherTopicOffset,long NextTopic,long PrevTopic)
{
    int i;

    for(i=0;i<browses;i++) if(browse[i].StartTopic!=-1L)
    {
	if(browse[i].PrevTopic==TopicOffset||browse[i].PrevTopic==OtherTopicOffset) break;
    }
    if(i<browses)
    {
	browse[i].PrevTopic=PrevTopic;
	browse[i].Count++;
	browse[i].Start++;
	FixStart(browse[i].BrowseNum,browse[i].BrowseNum,1);
	if(browse[i].NextTopic==-1L&&browse[i].PrevTopic==-1L)
	{
	    AddStart(browse[i].StartTopic,browse[i].BrowseNum,browse[i].Start);
	    browse[i].StartTopic=-1L;
	}
    }
    else
    {
	warnings=TRUE;
	fprintf(stderr,"Can not backlink %08lx %08lx %08lx\n",TopicOffset,NextTopic,PrevTopic);
    }
}

unsigned long AddLink(long StartTopic,long NextTopic,long PrevTopic)
{
    int i,j;
    unsigned long result;

    result=0L;
    for(i=0;i<browses;i++) if(browse[i].StartTopic==-1L) break;
    if(i==browses) browse=my_realloc(browse,++browses*sizeof(BROWSE));
    for(j=0;j<starts;j++) if(start[j].StartTopic==StartTopic) break;
    if(j<starts)
    {
	browse[i].StartTopic=start[j].StartTopic;
	browse[i].BrowseNum=start[j].BrowseNum;
	browse[i].Start=start[j].Start;
	browse[i].Count=start[j].Start;
	browse[i].NextTopic=NextTopic;
	browse[i].PrevTopic=PrevTopic;
	result=browse[i].BrowseNum+((long)browse[i].Start<<16);
    }
    else
    {
	warnings=TRUE;
	fprintf(stderr,"Browse start %08lx not found\n",StartTopic);
    }
    return result;
}

unsigned long MergeLink(long TopicOffset,long OtherTopicOffset,long NextTopic,long PrevTopic)
{
    int i,j;
    unsigned long result;

    result=0L;
    for(i=0;i<browses;i++) if(browse[i].StartTopic!=-1L)
    {
	if(browse[i].NextTopic==TopicOffset||browse[i].NextTopic==OtherTopicOffset) break;
    }
    for(j=0;j<browses;j++) if(browse[j].StartTopic!=-1L)
    {
	if(browse[j].PrevTopic==TopicOffset||browse[j].PrevTopic==OtherTopicOffset) break;
    }
    if(i<browses&&j<browses)
    {
	browse[i].Count++;
	browse[j].Start--;
	if(browse[i].Count!=browse[j].Start)
	{
	    warnings=TRUE;
	    fprintf(stderr,"Prev browse end %d doen't match next browse start %d\n",browse[i].Count,browse[j].Start);
	}
	result=browse[i].BrowseNum+((long)browse[i].Count<<16);
	browse[i].NextTopic=browse[j].NextTopic;
	browse[i].Count=browse[j].Count;
	browse[j].StartTopic=-1L;
    }
    else
    {
	warnings=TRUE;
	fprintf(stderr,"Can not merge %08lx %08lx %08lx\n",TopicOffset,NextTopic,PrevTopic);
    }
    return result;
}

unsigned long LinkLink(long TopicOffset,long OtherTopicOffset,long NextTopic,long PrevTopic)
{
    int i;
    unsigned long result;

    result=0L;
    for(i=0;i<browses;i++) if(browse[i].StartTopic!=-1L)
    {
	if(browse[i].NextTopic==TopicOffset||browse[i].NextTopic==OtherTopicOffset) break;
    }
    if(i<browses)
    {
	browse[i].NextTopic=NextTopic;
	browse[i].Count++;
	result=browse[i].BrowseNum+((long)browse[i].Count<<16);
	if(browse[i].NextTopic==-1L&&browse[i].PrevTopic==-1L)
	{
	    browse[i].StartTopic=-1L;
	}
    }
    else
    {
	warnings=TRUE;
	fprintf(stderr,"Can not link %08lx %08lx %08lx\n",TopicOffset,NextTopic,PrevTopic);
    }
    return result;
}

unsigned long BackLinkLink(long TopicOffset,long OtherTopicOffset,long NextTopic,long PrevTopic)
{
    int i;
    unsigned long result;

    result=0L;
    for(i=0;i<browses;i++) if(browse[i].StartTopic!=-1L)
    {
	if(browse[i].PrevTopic==TopicOffset||browse[i].PrevTopic==OtherTopicOffset) break;
    }
    if(i<browses)
    {
	browse[i].PrevTopic=PrevTopic;
	browse[i].Start--;
	result=browse[i].BrowseNum+((long)browse[i].Start<<16);
	if(browse[i].NextTopic==-1L&&browse[i].PrevTopic==-1L)
	{
	    browse[i].StartTopic=-1L;
	}
    }
    else
    {
	warnings=TRUE;
	fprintf(stderr,"Can not backlink %08lx %08lx %08lx\n",TopicOffset,NextTopic,PrevTopic);
    }
    return result;
}

/* create numbered rtf file names, no numbering if i=0 */
void BuildName(char *buffer,int i)
{
    char num[7];

    strcpy(buffer,name);
    if(i)
    {
	sprintf(num, "%d", i);
	if(strlen(buffer)+strlen(num)>8)
	{
	    buffer[8-strlen(num)]='\0';
	}
	strcat(buffer,num);
    }
    strcat(buffer,".RTF");
}

/* emit rtf commands to change to font i.
// ul forces underline on, uldb forces doubleunderline on */
void ChangeFont(FILE *rtf,unsigned int i,BOOL ul,BOOL uldb)
{
    FONTDESCRIPTOR *f;
    long pos;

    if(i<fonts)
    {
	pos=ftell(rtf);
	f=font+i;
	if(f->style)
	{
	    fprintf(rtf,"\\plain\\cs%d",f->style+9);
	    if(uldb) fputs("\\uldb",rtf); else if(ul) fputs("\\ul",rtf);
	}
	else
	{
	    /* HC30 can't reset, so reset using \plain */
	    if(CurrentFont.Bold&&!f->Bold
	    || CurrentFont.Italic&&!f->Italic
	    || CurrentFont.Underline&&!(!uldb&&(ul||f->Underline))
	    || CurrentFont.StrikeOut&&!f->StrikeOut
	    || CurrentFont.DoubleUnderline&&!(uldb||f->DoubleUnderline)
	    || CurrentFont.SmallCaps&&!f->SmallCaps
	    || CurrentFont.FontName&&!f->FontName
	    || CurrentFont.textcolor&&!f->textcolor
	    || CurrentFont.backcolor&&!f->backcolor
	    || CurrentFont.up&&!f->up
	    || CurrentFont.style&&!f->style)
	    {
		fputs("\\plain",rtf);
		memset(&CurrentFont,0,sizeof(CurrentFont));
		CurrentFont.FontName=DefFont;
	    }
	    if(f->FontName!=CurrentFont.FontName) fprintf(rtf,"\\f%d",f->FontName);
	    if(f->Italic&&!CurrentFont.Italic) fputs("\\i",rtf);
	    if(f->Bold&&!CurrentFont.Bold) fputs("\\b",rtf);
	    if(!uldb&&(ul||f->Underline)&&!CurrentFont.Bold) fputs("\\ul",rtf);
	    if(f->StrikeOut&&!CurrentFont.StrikeOut) fputs("\\strike",rtf);
	    if((uldb||f->DoubleUnderline)&&!CurrentFont.DoubleUnderline) fputs("\\uldb",rtf);
	    if(f->SmallCaps&&!CurrentFont.SmallCaps) fputs("\\scaps",rtf);
	    if(f->expndtw!=CurrentFont.expndtw) fprintf(rtf,"\\expndtw%d",f->expndtw);
	    if(f->up!=CurrentFont.up)
	    {
		if(f->up>0) fprintf(rtf,"\\up%d",f->up);
		else if(f->up<0) fprintf(rtf,"\\dn%d",-f->up);
	    }
	    if(f->HalfPoints!=CurrentFont.HalfPoints) fprintf(rtf,"\\fs%d",f->HalfPoints);
	    if(f->textcolor!=CurrentFont.textcolor) fprintf(rtf,"\\cf%d",f->textcolor);
	    if(f->backcolor!=CurrentFont.backcolor) fprintf(rtf,"\\cb%d",f->backcolor);
	}
	memcpy(&CurrentFont,f,sizeof(CurrentFont));
	if(ul) CurrentFont.Underline=1;
	if(uldb)
	{
	    CurrentFont.Underline=0;
	    CurrentFont.DoubleUnderline=1;
	}
	if(ftell(rtf)!=pos) putc(' ',rtf);
    }
}

/* list all groups the topic TopicNum is assigned to and/or emit footnote
// for browse sequence of this topic as + footnote into rtf file */
void ListGroups(FILE *rtf,long TopicNum,unsigned long BrowseNum)
{
    int i;
    BOOL grouplisted;

    grouplisted=FALSE;
    for(i=0;i<groups;i++) if(group[i].GroupHeader.GroupType==1||group[i].GroupHeader.GroupType==2)
    {
	if(TopicNum>=group[i].GroupHeader.FirstTopic&&TopicNum<=group[i].GroupHeader.LastTopic&&(group[i].GroupHeader.GroupType==1||group[i].GroupHeader.GroupType==2&&(group[i].Bitmap[TopicNum>>3]&(1<<(TopicNum&7)))))
	{
	    if(!grouplisted)
	    {
		fputs("{\\up +}{\\footnote\\pard\\plain{\\up +} ",rtf);
		if(BrowseNum) fprintf(rtf,"BROWSE%04x:%04x",(unsigned short)BrowseNum,(unsigned short)(BrowseNum>>16));
		grouplisted=TRUE;
	    }
	    fprintf(rtf,";%s",group[i].Name);
	}
    }
    if(grouplisted)
    {
	fputs("}\n",rtf);
    }
    else if(BrowseNum)
    {
	fprintf(rtf,"{\\up +}{\\footnote\\pard\\plain{\\up +} BROWSE%04x:%04x}\n",(unsigned short)BrowseNum,(unsigned short)(BrowseNum>>16));
    }
}

/* advances TopicOffset to next block in |TOPIC if setting of TopicPos to
// NextBlock crosses TOPICBLOCKHEADER */
TOPICOFFSET NextTopicOffset(TOPICOFFSET TopicOffset,TOPICPOS NextBlock,TOPICPOS TopicPos)
{
    /* it should never be neccessary to subtract sizeof(TOPICBLOCKHEADER), as no
    // TOPICLINK may start in the last (12..21) bytes, but just to make shure... */
    if((NextBlock-sizeof(TOPICBLOCKHEADER))/DecompressSize!=(TopicPos-sizeof(TOPICBLOCKHEADER))/DecompressSize)
    {
	return ((NextBlock-sizeof(TOPICBLOCKHEADER))/DecompressSize)*0x8000L;
    }
    return TopicOffset;
}

/* TopicDump: converts the internal |TOPIC file to RTF format suitable for
// recompilation inserting footnotes with information from other internal
// files as required */
FILE *TopicDump(FILE *HelpFile,FILE *rtf,FILE *hpj,BOOL makertf)
{
    TOPICLINK TopicLink;
    char *LinkData1;  /* Data associated with this link */
    long nonscroll=-1L;
    char *LinkData2;  /* Second set of data */
    int fontset,i;
    int NextContextRec;
    unsigned long BrowseNum;
    char *hotspot;
    char *arg;
    BOOL firsttopic=TRUE;
    BOOL ul,uldb;
    int nextbitmap,TopicInRTF,NumberOfRTF;
    long TopicNum,TopicOffset,TopicPos;
    int col,cols,lastcol;
    short *iptr;
    unsigned short x1,x2,x3;
    short y1;
    long l1;
    char *ptr;
    char *cmd;
    char *str;
    long ActualTopicOffset,MaxTopicOffset;
    TOPICHEADER30 *TopicHdr30;
    TOPICHEADER *TopicHdr;
    long BogusTopicOffset;

    if(SearchFile(HelpFile,"|TOPIC",&TopicFileLength))
    {
	fontset=-1;
	nextbitmap=1;
	if(browse) free(browse);
	browse=NULL;
	browses=0;
	NextContextRec=0;
	ul=uldb=FALSE;
	hotspot=NULL;
	TopicOffset=0L;
	TopicPos=12L;
	TopicNum=16;
	TopicInRTF=0;
	NumberOfRTF=1;
	while(TopicRead(HelpFile,TopicPos,&TopicLink,sizeof(TopicLink))==sizeof(TOPICLINK))
	{
	    if(before31)
	    {
		if(TopicPos+TopicLink.NextBlock>=TopicFileLength) break;
	    }
	    else
	    {
		if(TopicLink.NextBlock<=0) break;
	    }
	    if(TopicLink.DataLen1>sizeof(TOPICLINK))
	    {
		LinkData1=my_malloc(TopicLink.DataLen1-sizeof(TOPICLINK)+1);
		if(TopicRead(HelpFile,0L,LinkData1,TopicLink.DataLen1-sizeof(TOPICLINK))!=TopicLink.DataLen1-sizeof(TOPICLINK)) break;
	    }
	    else LinkData1=NULL;
	    if(TopicLink.DataLen1<TopicLink.BlockSize) /* read LinkData2 using phrase replacement */
	    {
		LinkData2=my_malloc(TopicLink.DataLen2+1);
		if(TopicPhraseRead(HelpFile,0L,LinkData2,TopicLink.BlockSize-TopicLink.DataLen1,TopicLink.DataLen2)!=TopicLink.BlockSize-TopicLink.DataLen1) break;
	    }
	    else LinkData2=NULL;
	    if(LinkData1&&TopicLink.RecordType==TL_TOPICHDR) /* display a Topic Header record */
	    {
		if(TopicsPerRTF&&++TopicInRTF>=TopicsPerRTF)
		{
		    putc('}',rtf);
		    my_fclose(rtf);
		    BuildName(buffer,++NumberOfRTF);
		    if(hpj) fprintf(hpj,"%s\n",buffer);
		    rtf=my_fopen(buffer,"wt");
		    FontLoad(HelpFile,rtf,NULL);
		    TopicInRTF=0;
		}
		else if(!firsttopic)
		{
		     if(makertf&&nopagebreak)
		     {
			 fputs("\\par\n",rtf);
		     }
		     else
		     {
			 fputs("\\page\n",rtf);
		     }
		}
		firsttopic=FALSE;
		fprintf(stderr,"\rTopic %ld...",TopicNum-15);
		if(!makertf)
		{
		    BrowseNum=0L;
		    if(before31)
		    {
			TopicHdr30=(TOPICHEADER30 *)LinkData1;
			fprintf(rtf,"{\\up #}{\\footnote\\pard\\plain{\\up #} TOPIC%ld}\n",TopicNum);
			if(resolvebrowse)
			{
			    if(TopicHdr30->NextTopicNum>TopicNum&&TopicHdr30->PrevTopicNum>TopicNum
			    || TopicHdr30->NextTopicNum==-1&&TopicHdr30->PrevTopicNum>TopicNum
			    || TopicHdr30->NextTopicNum>TopicNum&&TopicHdr30->PrevTopicNum==-1)
			    {
				BrowseNum=AddLink(TopicNum,TopicHdr30->NextTopicNum,TopicHdr30->PrevTopicNum);
			    }
			    else if(TopicHdr30->NextTopicNum!=-1&&TopicHdr30->NextTopicNum<TopicNum&&TopicHdr30->PrevTopicNum!=-1&&TopicHdr30->PrevTopicNum<TopicNum)
			    {
				BrowseNum=MergeLink(TopicNum,TopicNum,TopicHdr30->NextTopicNum,TopicHdr30->PrevTopicNum);
			    }
			    else if(TopicHdr30->NextTopicNum!=-1&&TopicHdr30->NextTopicNum<TopicNum&&(TopicHdr30->PrevTopicNum==-1||TopicHdr30->PrevTopicNum>TopicNum))
			    {
				BrowseNum=BackLinkLink(TopicNum,TopicNum,TopicHdr30->NextTopicNum,TopicHdr30->PrevTopicNum);
			    }
			    else if(TopicHdr30->PrevTopicNum!=-1&&TopicHdr30->PrevTopicNum<TopicNum&&(TopicHdr30->NextTopicNum==-1||TopicHdr30->NextTopicNum>TopicNum))
			    {
				BrowseNum=LinkLink(TopicNum,TopicNum,TopicHdr30->NextTopicNum,TopicHdr30->PrevTopicNum);
			    }
			}
			ListKeywords(HelpFile,rtf,TopicPos);
		    }
		    else
		    {
			BogusTopicOffset=NextTopicOffset(TopicOffset,TopicLink.NextBlock,TopicPos);
			TopicHdr=(TOPICHEADER *)LinkData1;
			if(TopicHdr->Scroll!=-1L)
			{
			    nonscroll=TopicHdr->Scroll;
			}
			else
			{
			    nonscroll=TopicHdr->NextTopic;
			}
			if(resolvebrowse)
			{
			    if(TopicHdr->BrowseFor>TopicOffset&&TopicHdr->BrowseBck>TopicOffset
			    || TopicHdr->BrowseFor==-1L&&TopicHdr->BrowseBck>TopicOffset
			    || TopicHdr->BrowseFor>TopicOffset&&TopicHdr->BrowseBck==-1L)
			    {
				BrowseNum=AddLink(TopicOffset,TopicHdr->BrowseFor,TopicHdr->BrowseBck);
			    }
			    else if(TopicHdr->BrowseFor!=-1L&&TopicHdr->BrowseFor<TopicOffset&&TopicHdr->BrowseBck!=-1L&&TopicHdr->BrowseBck<TopicOffset)
			    {
				BrowseNum=MergeLink(TopicOffset,BogusTopicOffset,TopicHdr->BrowseFor,TopicHdr->BrowseBck);
			    }
			    else if(TopicHdr->BrowseFor!=-1L&&TopicHdr->BrowseFor<TopicOffset&&(TopicHdr->BrowseBck==-1L||TopicHdr->BrowseBck>TopicOffset))
			    {
				BrowseNum=BackLinkLink(TopicOffset,BogusTopicOffset,TopicHdr->BrowseFor,TopicHdr->BrowseBck);
			    }
			    else if(TopicHdr->BrowseBck!=-1L&&TopicHdr->BrowseBck<TopicOffset&&(TopicHdr->BrowseFor==-1L||TopicHdr->BrowseFor>TopicOffset))
			    {
				BrowseNum=LinkLink(TopicOffset,BogusTopicOffset,TopicHdr->BrowseFor,TopicHdr->BrowseBck);
			    }
			}
		    }
		    ListGroups(rtf,TopicNum-16,BrowseNum);
		    if(LinkData2&&TopicLink.DataLen2>0)
		    {
			if(*LinkData2)
			{
			    fputs("{\\up $}{\\footnote\\pard\\plain{\\up $} ",rtf);
			    putrtf(rtf,LinkData2);
			    fputs("}\n",rtf);
			}
			for(i=strlen(LinkData2)+1;i<TopicLink.DataLen2;i+=strlen(LinkData2+i)+1)
			{
			    fputs("{\\up !}{\\footnote\\pard\\plain{\\up !} ",rtf);
			    if(!after31&&strlen(LinkData2+i)>254)
			    {
				printf("Help compiler will issue Warning 3511: Macro '%s' exceeds limit of 254 characters\n",LinkData2+i);
			    }
			    putrtf(rtf,LinkData2+i);
			    fputs("}\n",rtf);
			}
		    }
		    while(NextContextRec<ContextRecs&&ContextRec[NextContextRec].TopicOffset<=TopicOffset)
		    {
			fputs("{\\up #}{\\footnote\\pard\\plain{\\up #} ",rtf);
                        putrtf(rtf,unhash(ContextRec[NextContextRec].HashValue));
                        fputs("}\n",rtf);
			if(!mvp) while(NextContextRec+1<ContextRecs&&ContextRec[NextContextRec].TopicOffset==ContextRec[NextContextRec+1].TopicOffset)
			{
			    NextContextRec++;
			}
			NextContextRec++;
		    }
		    i=ListWindows(HelpFile,TopicOffset);
		    if(i!=-1) fprintf(rtf,"{\\up >}{\\footnote\\pard\\plain{\\up >} %s}\n",GetWindowName(i));
		}
		TopicNum++;
	    }
	    else if(LinkData1&&LinkData2&&TopicLink.RecordType==TL_DISPLAY30||TopicLink.RecordType==TL_DISPLAY||TopicLink.RecordType==TL_TABLE)
	    {
		if(AnnoFile) Annotate(TopicPos,rtf);
		ptr=LinkData1;
		scanlong(&ptr);
		if(TopicLink.RecordType==TL_DISPLAY||TopicLink.RecordType==TL_TABLE)
		{
		    x1=scanword(&ptr);
		    ActualTopicOffset=TopicOffset;
		    MaxTopicOffset=ActualTopicOffset+x1;
		    TopicOffset+=x1;
		}
		if(TopicLink.RecordType==TL_TABLE)
		{
		    fputs("\\trowd",rtf);
		    cols=(unsigned char)*ptr++;
		    x1=(unsigned char)*ptr++;
		    switch(x1)
		    {
		    case 0:
		    case 2:
			l1=*(short *)ptr; /* min table width */
			ptr+=2;
			fputs("\\trqc",rtf);
			break;
		    case 1:
		    case 3:
			l1=32767L;
			break;
		    }
		    iptr=(short *)ptr;
		    if(cols>1)
		    {
			x1=iptr[0]+iptr[1]+iptr[3]/2;
			fprintf(rtf,"\\trgaph%ld\\trleft%ld \\cellx%ld\\cellx%ld",((iptr[3]*scaling-rounderr)*l1)/32767,(((iptr[1]-iptr[3])*scaling-rounderr)*l1-32767)/32767,((x1*scaling-rounderr)*l1)/32767,(((x1+iptr[2]+iptr[3])*scaling-rounderr)*l1)/32767);
			x1+=iptr[2]+iptr[3];
			for(col=2;col<cols;col++)
			{
			    x1+=iptr[2*col]+iptr[2*col+1];
			    fprintf(rtf,"\\cellx%ld",((x1*scaling-rounderr)*l1)/32767);
			}
		    }
		    else
		    {
			fprintf(rtf,"\\trleft%ld \\cellx%ld ",((iptr[1]*scaling-rounderr)*l1-32767)/32767,((iptr[0]*scaling-rounderr)*l1)/32767);
		    }
		    ptr=(char *)(iptr+2*cols);
		}
		lastcol=-1;
		str=LinkData2;
		for(col=0;(TopicLink.RecordType==TL_TABLE?*(short *)ptr!=-1:col==0)&&ptr<LinkData1+TopicLink.DataLen1-sizeof(TOPICLINK);col++)
		{
		    fputs("\\pard",rtf);
		    if(TopicPos<nonscroll) fputs("\\keepn",rtf);
		    if(TopicLink.RecordType==TL_TABLE)
		    {
			fputs("\\intbl",rtf);
			lastcol=*(short *)ptr;
			ptr+=5;
		    }
		    ptr += 4;
		    x2 = *(unsigned short *)ptr;
		    ptr += 2;
		    if(x2&0x1000) fputs("\\keep",rtf);
		    if(x2&0x0400) fputs("\\qr",rtf);
		    if(x2&0x0800) fputs("\\qc",rtf);
		    if(x2&0x0001) scanlong(&ptr);
		    if(x2&0x0002) fprintf(rtf,"\\sb%ld",scanint(&ptr)*scaling-rounderr);
		    if(x2&0x0004) fprintf(rtf,"\\sa%ld",scanint(&ptr)*scaling-rounderr);
		    if(x2&0x0008) fprintf(rtf,"\\sl%ld",scanint(&ptr)*scaling-rounderr);
		    if(x2&0x0010) fprintf(rtf,"\\li%ld",scanint(&ptr)*scaling-rounderr);
		    if(x2&0x0020) fprintf(rtf,"\\ri%ld",scanint(&ptr)*scaling-rounderr);
		    if(x2&0x0040) fprintf(rtf,"\\fi%ld",scanint(&ptr)*scaling-rounderr);
		    if(x2&0x0100)
		    {
			x1=(unsigned char)*ptr++;
			if(x1&1) fputs("\\box",rtf);
			if(x1&2) fputs("\\brdrt",rtf);
			if(x1&4) fputs("\\brdrl",rtf);
			if(x1&8) fputs("\\brdrb",rtf);
			if(x1&0x10) fputs("\\brdrr",rtf);
			if(x1&0x20) fputs("\\brdrth",rtf); else fputs("\\brdrs",rtf);
			if(x1&0x40) fputs("\\brdrdb",rtf);
			ptr+=2;
		    }
		    if(x2&0x0200)
		    {
			y1=scanint(&ptr);
			while(y1-->0)
			{
			    x1=scanword(&ptr);
			    if(x1&0x4000)
			    {
				switch(scanword(&ptr))
				{
				case 1:
				    fputs("\\tqr",rtf);
				    break;
				case 2:
				    fputs("\\tqc",rtf);
				    break;
				}
			    }
			    fprintf(rtf,"\\tx%ld",(x1&0x3FFF)*scaling-rounderr);
			}
		    }
		    putc(' ',rtf);
		    while(1) /* ptr<LinkData1+TopicLink.DataLen1-sizeof(TOPICLINK)&&str<end) */
		    {
			if(*str&&fontset>=0&&fontset<fonts&&font&&font[fontset].SmallCaps)
                            strlwr(str);
			do
			{
			    if(!makertf)
			    {
				while(NextContextRec<ContextRecs&&ContextRec[NextContextRec].TopicOffset<=ActualTopicOffset&&ContextRec[NextContextRec].TopicOffset<MaxTopicOffset)
				{
				    fputs("{\\up #}{\\footnote\\pard\\plain{\\up #} ",rtf);
                                    putrtf(rtf,unhash(ContextRec[NextContextRec].HashValue));
                                    fputs("}\n",rtf);
				    if(!mvp) while(NextContextRec+1<ContextRecs&&ContextRec[NextContextRec].TopicOffset==ContextRec[NextContextRec+1].TopicOffset)
				    {
					NextContextRec++;
				    }
				    NextContextRec++;
				}
				if(!before31) ListKeywords(HelpFile,rtf,ActualTopicOffset<MaxTopicOffset?ActualTopicOffset:MaxTopicOffset-1);
			    }
			    if(*str)
			    {
				if(*str!='{'&&*str!='}'&&*str!='\\'&&isprint((unsigned char)*str))
				{
				    putc(*str,rtf);
				}
				else if(!makertf&&*str=='{')
				{
				    fputs("\\{\\-",rtf); /* emit invisible dash after { brace */
				    /* because bmc or another legal command may follow, but this */
				    /* command was not parsed the help file was build, so it was */
				    /* used just as an example. The dash will be eaten up by the */
				    /* help compiler on recompile. */
				}
				else
				{
				    //fprintf(rtf,"\\'%02x",(unsigned char)*str);
				    fprintf(rtf, "{\\u%u}", cp1251_to_unicode(*str));
				}
			    }
			    if(ActualTopicOffset<MaxTopicOffset) ActualTopicOffset++;
			}
			while(*str++);
			if((unsigned char)ptr[0]==0xFF)
			{
			    ptr++;
			    break;
			}
			else switch((unsigned char)ptr[0])
			{
			case 0x20: /* vfld MVB */
			    if(*(long *)(ptr+1))
			    {
				fprintf(rtf,"\\{vfld%ld\\}",*(long *)(ptr+1));
			    }
			    else
			    {
				fputs("\\{vfld\\}",rtf);
			    }
			    ptr+=5;
			    break;
			case 0x21: /* dtype MVB */
			    if(*(short *)(ptr+1))
			    {
				fprintf(rtf,"\\{dtype%d\\}",*(short *)(ptr+1));
			    }
			    else
			    {
				fputs("\\{dtype\\}",rtf);
			    }
			    ptr+=3;
			    break;
			case 0x80: /* font change */
			    ChangeFont(rtf,fontset=*(short *)(ptr+1),ul,uldb);
			    ptr+=3;
			    break;
			case 0x81:
			    fputs("\\line\n",rtf);
			    ptr++;
			    break;
			case 0x82:
			    if(TopicLink.RecordType==TL_TABLE)
			    {
				if((unsigned char)ptr[1]!=0xFF)
				{
				    fputs("\n\\par\\intbl ",rtf);
				}
				else if(*(short *)(ptr+2)==-1)
				{
				    fputs("\\cell\\intbl\\row\n",rtf);
				}
				else if(*(short *)(ptr+2)==lastcol)
				{
				    fputs("\\par\\pard ",rtf);
				}
				else
				{
				    fputs("\\cell\\pard ",rtf);
				}
			    }
			    else
			    {
				fputs("\n\\par ",rtf);
			    }
			    ptr++;
			    break;
			case 0x83:
			    fputs("\\tab ",rtf);
			    ptr++;
			    break;
			case 0x86:
			    x3=(unsigned char)*ptr++;
			    x1=*ptr++;
			    if(x1==0x05) cmd="ewc"; else cmd="bmc";
			    goto picture;
			case 0x87:
			    x3=(unsigned char)*ptr++;
			    x1=*ptr++;
			    if(x1==0x05) cmd="ewl"; else cmd="bml";
			    goto picture;
			case 0x88:
			    x3=(unsigned char)*ptr++;
			    x1=*ptr++;
			    if(x1==0x05) cmd="ewr"; else cmd="bmr";
			    goto picture;
			picture:
			    l1=scanlong(&ptr);
			    switch(x1)
			    {
			    case 0x22: /* HC31 */
				ActualTopicOffset+=scanword(&ptr); /* number of hotspots in picture */
				if(ActualTopicOffset>MaxTopicOffset) ActualTopicOffset=MaxTopicOffset;
				/* fall thru */
			    case 0x03: /* HC30 */
				x1=((unsigned short *)ptr)[0];
				switch(x1)
				{
				case 1:
				    while(nextbitmap<extensions&&extension[nextbitmap]<0x10) nextbitmap++;
				    if(nextbitmap>=extensions)
				    {
					error("Bitmap never saved");
					break;
				    }
				    x2 = nextbitmap++;
				    goto other;
				case 0:
				    x2 = ((unsigned short *)ptr)[1];
				other:
				    if(makertf)
				    {
					switch(x3)
					{
					case 0x86:
					    fprintf(rtf,"{\\field {\\*\\fldinst import %s}}",getbitmapname(x2));
					    break;
					case 0x87:
					    fprintf(rtf,"{\\pvpara {\\field {\\*\\fldinst import %s}}\\par}\n",getbitmapname(x2));
					    break;
					case 0x88:
					    fprintf(rtf,"{\\pvpara\\posxr{\\field {\\*\\fldinst import %s}}\\par}\n",getbitmapname(x2));
					    break;
					}
				    }
				    else
				    {
					if(x2<extensions&&(extension[x2]&0x20))
					{
					    if(strcmp(cmd,"bmc")==0) cmd="bmct";
					    else if(strcmp(cmd,"bml")==0) cmd="bmlt";
					    else if(strcmp(cmd,"bmr")==0) cmd="bmrt";
					}
					fprintf(rtf,"\\{%s %s\\}",cmd,getbitmapname(x2));
				    }
				    break;
				}
				break;
			    case 0x05: /* ewc,ewl,ewr */
				if(ptr[6]=='!')
				{
				    fprintf(rtf,"\\{button %s\\}",ptr+7);
				}
				else if(ptr[6]=='*')
				{
				    char *plus;
				    int n,c1,c2;

				    sscanf(ptr+7,"%d,%d,%n",&c1,&c2,&n);
				    plus=strchr(ptr+7+n,'+');
				    if((c1&0xFFF5)!=0x8400) fprintf(stderr,"mci c1=%04x\n",c1);
				    fputs("\\{mci",rtf);
				    if(cmd[2]=='r') fputs("_right",rtf);
				    if(cmd[2]=='l') fputs("_left",rtf);
				    if(c2==1) fputs(" REPEAT",rtf);
				    if(c2==2) fputs(" PLAY",rtf);
				    if(!plus) fputs(" EXTERNAL",rtf);
				    if(c1&8) fputs(" NOMENU",rtf);
				    if(c1&2) fputs(" NOPLAYBAR",rtf);
				    fprintf(rtf,",%s\\}\n",plus?plus+1:ptr+7+n);
				}
				else
				{
				    fprintf(rtf,"\\{%s %s\\}",cmd,ptr+6);
				}
				break;
			    }
			    ptr+=l1;
			    break;
			case 0x89: /* end of hotspot */
			    if(!makertf)
			    {
				if(hotspot[0]=='%'&&fontset>=0&&fontset<fonts&&font[fontset].Underline)
				{
				    hotspot[0]='*';
				}
			    }
			    ChangeFont(rtf,fontset,ul=FALSE,uldb=FALSE);
			    if(!makertf)
			    {
				if(!after31&&strlen(hotspot)>255)
				{
				    puts("Help compiler will issue Warning 4072: Context string exceeds limit of 255 characters");
				}
				fputs("{\\v ",rtf);
                                putrtf(rtf,multi&&(hotspot[0]=='%'||hotspot[0]=='*')?hotspot+1:hotspot);
                                fputc('}',rtf);
			    }
			    ptr++;
			    break;
			case 0xC8: /* macro */
			    ChangeFont(rtf,fontset,FALSE,uldb=TRUE);
			    if(!makertf)
			    {
				hotspot=my_realloc(hotspot,strlen(ptr+3)+2);
				sprintf(hotspot,"!%s",ptr+3);
			    }
			    ptr+=*(short *)(ptr+1)+3;
			    break;
			case 0xCC: /* macro without font change */
			    ChangeFont(rtf,fontset,FALSE,uldb=TRUE);
			    if(!makertf)
			    {
				hotspot=my_realloc(hotspot,strlen(ptr+3)+3);
				sprintf(hotspot,"%%!%s",ptr+3);
			    }
			    ptr+=*(short *)(ptr+1)+3;
			    break;
			case 0xE0: /* popup jump HC30 */
			    ChangeFont(rtf,fontset,ul=TRUE,FALSE);
			    goto label0;
			case 0xE1: /* topic jump HC30 */
			    ChangeFont(rtf,fontset,FALSE,uldb=TRUE);
			label0:
			    if(!makertf)
			    {
				hotspot=my_realloc(hotspot,128);
				sprintf(hotspot,"TOPIC%ld",*(long *)(ptr+1));
			    }
			    ptr+=5;
			    break;
			case 0xE2: /* popup jump HC31 */
			    ChangeFont(rtf,fontset,ul=TRUE,FALSE);
			    goto label1;
			case 0xE3: /* topic jump HC31 */
			    ChangeFont(rtf,fontset,FALSE,uldb=TRUE);
			label1:
			    if(!makertf)
			    {
				arg=ContextId(*(long *)(ptr+1));
				hotspot=my_realloc(hotspot,strlen(arg)+1);
				sprintf(hotspot,"%s",arg);
			    }
			    ptr+=5;
			    break;
			case 0xE6: /* popup jump without font change */
			    ChangeFont(rtf,fontset,ul=TRUE,FALSE);
			    goto label2;
			case 0xE7: /* topic jump without font change */
			    ChangeFont(rtf,fontset,FALSE,uldb=TRUE);
			label2:
			    if(!makertf)
			    {
				arg=ContextId(*(long *)(ptr+1));
				hotspot=my_realloc(hotspot,strlen(arg)+2);
				sprintf(hotspot,"%%%s",arg);
			    }
			    ptr+=5;
			    break;
			case 0xEA: /* popup jump into external file */
			case 0xEE:
			    ChangeFont(rtf,fontset,ul=TRUE,FALSE);
			    goto label3;
			case 0xEB: /* topic jump into external file / secondary window */
			case 0xEF:
			    ChangeFont(rtf,fontset,FALSE,uldb=TRUE);
			label3:
			    if(!makertf)
			    {
				if((unsigned char)ptr[0]==0xEE||(unsigned char)ptr[0]==0xEF)
				{
				    cmd="%";
				}
				else
				{
				    cmd="";
				}
				arg=unhash(*(long *)(ptr+4)); // no ContextId, it may jump into external file
				switch((unsigned char)ptr[3])
				{
				case 0:
				    hotspot=my_realloc(hotspot,strlen(cmd)+strlen(arg)+1);
				    sprintf(hotspot,"%s%s",cmd,arg);
				    break;
				case 1:
				    hotspot=my_realloc(hotspot,strlen(cmd)+strlen(arg)+1+strlen(GetWindowName(ptr[8]))+1);
				    sprintf(hotspot,"%s%s>%s",cmd,arg,GetWindowName(ptr[8]));
				    break;
				case 4:
				    hotspot=my_realloc(hotspot,strlen(cmd)+strlen(arg)+1+strlen(ptr+8)+1);
				    sprintf(hotspot,"%s%s@%s",cmd,arg,ptr+8);
				    break;
				case 6:
				    hotspot=my_realloc(hotspot,strlen(cmd)+strlen(arg)+1+strlen(ptr+8)+1+strlen(strchr(ptr+8,'\0')+1)+1);
				    sprintf(hotspot,"%s%s>%s@%s",cmd,arg,ptr+8,strchr(ptr+8,'\0')+1);
				    break;
				}
			    }
			    ptr+=*(short *)(ptr+1)+3;
			    break;
			case 0x8B:
			    fputs("\\~",rtf);
			    ptr++;
			    break;
			case 0x8C:
			    fputs("\\-",rtf);
			    ptr++;
			    break;
			default:
			    ptr++;
			}
		    }
		}
	    }
	    if(LinkData1) free(LinkData1);
	    if(LinkData2) free(LinkData2);
	    if(before31)
	    {
		TopicPos+=TopicLink.NextBlock;
	    }
	    else
	    {
		TopicOffset=NextTopicOffset(TopicOffset,TopicLink.NextBlock,TopicPos);
		TopicPos=TopicLink.NextBlock;
	    }
	}
    }
    return rtf;
}

int _cdecl ContextRecCmp(const void *a,const void *b)
{
    if(((const CONTEXTREC *)a)->TopicOffset<((const CONTEXTREC *)b)->TopicOffset) return -1;
    if(((const CONTEXTREC *)a)->TopicOffset>((const CONTEXTREC *)b)->TopicOffset) return 1;
    return 0;
}

void ContextLoad(FILE *HelpFile)
{
    BUFFER buf;
    int n;
    long entries;

    if(SearchFile(HelpFile,"|CONTEXT",NULL))
    {
	n=GetFirstPage(HelpFile,&buf,&entries);
	if(entries)
	{
	    ContextRec=my_malloc(entries*sizeof(CONTEXTREC));
	    ContextRecs=0;
	    while(n)
	    {
		my_fread(ContextRec+ContextRecs,n*sizeof(CONTEXTREC),HelpFile);
		ContextRecs+=n;
		n=GetNextPage(HelpFile,&buf);
	    }
	    fprintf(stderr,"%d topic offsets and hash values loaded\n",ContextRecs);
	    qsort(ContextRec,ContextRecs,sizeof(CONTEXTREC),ContextRecCmp);
	}
    }
    else if(SearchFile(HelpFile,"|TOMAP",&entries))
    {
	Topic=my_malloc(entries);
	my_fread(Topic,entries,HelpFile);
	Topics=(int)(entries/sizeof(long));
    }
}

void GenerateContent(FILE *HelpFile,FILE *ContentFile) /* create a simple Win95 contents file */
{
    VIOLAREC *WindowRec;
    long FileLength,offset;
    int n,i,j,WindowRecs;
    BUFFER buf;
    char *ptr;

    fprintf(ContentFile,":Base %s%s>main\n",name,ext);
    if(HelpFileTitle[0]) fprintf(ContentFile,":Title %s\n",HelpFileTitle);
    WindowRecs=0;
    if(SearchFile(HelpFile,"|VIOLA",NULL))
    {
	n=GetFirstPage(HelpFile,&buf,&FileLength);
	if(FileLength)
	{
	    WindowRec=my_malloc(FileLength*sizeof(VIOLAREC));
	    while(n)
	    {
		my_fread(WindowRec+WindowRecs,n*sizeof(VIOLAREC),HelpFile);
		WindowRecs+=n;
		n=GetNextPage(HelpFile,&buf);
	    }
	}
    }
    if(SearchFile(HelpFile,"|TTLBTREE",NULL))
    {
	for(n=GetFirstPage(HelpFile,&buf,NULL);n;n=GetNextPage(HelpFile,&buf))
	{
	    for(i=0;i<n;i++)
	    {
		offset=getdw(HelpFile);
		if(my_gets(buffer,sizeof(buffer),HelpFile))
		{
		    ptr=TopicName(offset);
		    if(ptr)
		    {
			fprintf(ContentFile,"1 %s=%s",buffer,ptr);
			for(j=0;j<WindowRecs;j++)
			{
			    if(WindowRec[j].TopicOffset==offset)
			    {
				fprintf(ContentFile,">%s",GetWindowName(WindowRec[j].WindowNumber));
				break;
			    }
			}
			putc('\n',ContentFile);
		    }
		}
	    }
	}
    }
}

void ListRose(FILE *HelpFile,FILE *hpj)
{
    long FileLength,offset,hash,h,pos,savepos;
    unsigned char *ptr;
    long *keytopic;
    int n,i,l,e;
    unsigned short j,count;
    BUFFER buf,buf2;

    if(SearchFile(HelpFile,"|Rose",NULL))
    {
	savepos=ftell(HelpFile);
	if(SearchFile(HelpFile,"|KWDATA",&FileLength))
	{
	    keytopic=my_malloc(FileLength);
	    my_fread(keytopic,FileLength,HelpFile);
	    if(SearchFile(HelpFile,"|KWBTREE",NULL))
	    {
		fputs("[MACROS]\n",hpj);
		for(n=GetFirstPage(HelpFile,&buf,NULL);n;n=GetNextPage(HelpFile,&buf))
		{
		    for(i=0;i<n;i++)
		    {
			my_gets(keyword,sizeof(keyword),HelpFile);
			for(hash=0L,ptr=(unsigned char *)keyword;*ptr;ptr++)
			{
			    hash=hash*43L+table[*ptr];
			}
			count=my_getw(HelpFile);
			offset=getdw(HelpFile);
			for(j=0;j<count;j++)
			{
			    if(keytopic[offset/4+j]==-1L)
			    {
				pos=ftell(HelpFile);
				fseek(HelpFile,savepos,SEEK_SET);
				for(l=GetFirstPage(HelpFile,&buf2,NULL);l;l=GetNextPage(HelpFile,&buf2))
				{
				    for(e=0;e<l;e++)
				    {
					h=getdw(HelpFile);
					my_gets(buffer,sizeof(buffer),HelpFile);
					if(h==hash)
					{
					    fprintf(hpj,"%s\n%s\n",keyword,buffer);
					    my_gets(buffer,sizeof(buffer),HelpFile);
					    fprintf(hpj,"%s\n",buffer);
					}
					else
					{
					    my_gets(buffer,sizeof(buffer),HelpFile);
					}
				    }
				}
				fseek(HelpFile,pos,SEEK_SET);
				break;
			    }
			}
		    }
		}
		putc('\n',hpj);
	    }
	    free(keytopic);
	}
    }
}

/* dump section: all the dump-routines are used to display internal files
// of the help file with known format of contents for debugging reasons */
void PrintNewFont(int i,NEWFONT *newfont)
{
    printf("%3d: %-32.32s %6ld %-6s %02X%02X%02X %02X%02X%02X ",i,fontname[newfont->FontName],newfont->Height,FontFamily(newfont->PitchAndFamily>>4),newfont->FGRGB[2],newfont->FGRGB[1],newfont->FGRGB[0],newfont->BGRGB[2],newfont->BGRGB[1],newfont->BGRGB[0]);
    if(newfont->Weight>500) putchar('b');
    if(newfont->Italic) putchar('i');
    if(newfont->Underline) putchar('u');
    if(newfont->StrikeOut) putchar('s');
    if(newfont->DoubleUnderline) putchar('d');
    if(newfont->SmallCaps) putchar('c');
    putchar('\n');
}

void PrintMvbFont(int i,MVBFONT *mvbfont)
{
    printf("%3d: %-32.32s %6ld %-6s %02X%02X%02X %02X%02X%02X ",i,fontname[mvbfont->FontName],mvbfont->Height,FontFamily(mvbfont->PitchAndFamily>>4),mvbfont->FGRGB[2],mvbfont->FGRGB[1],mvbfont->FGRGB[0],mvbfont->BGRGB[2],mvbfont->BGRGB[1],mvbfont->BGRGB[0]);
    if(mvbfont->Weight>500) putchar('b');
    if(mvbfont->Italic) putchar('i');
    if(mvbfont->Underline) putchar('u');
    if(mvbfont->StrikeOut) putchar('s');
    if(mvbfont->DoubleUnderline) putchar('d');
    if(mvbfont->SmallCaps) putchar('c');
    putchar('\n');
}

void FontDump(FILE *HelpFile)
{
    FONTHEADER FontHdr;
    long FileStart;
    OLDFONT oldfont;
    NEWFONT newfont;
    NEWSTYLE newstyle;
    MVBFONT mvbfont;
    MVBSTYLE mvbstyle;
    int i,n;

    /* Go to the FONT file and get the headers */
    FileStart=ftell(HelpFile);
    my_fread(&FontHdr,sizeof(FontHdr),HelpFile);
    n=(FontHdr.DescriptorsOffset-FontHdr.FacenamesOffset)/FontHdr.NumFacenames;
    fontname=my_malloc(FontHdr.NumFacenames*sizeof(char *));
    fseek(HelpFile,FileStart+FontHdr.FacenamesOffset,SEEK_SET);
    for(i=0;i<FontHdr.NumFacenames;i++)
    {
	my_fread(buffer,n,HelpFile);
	buffer[n]='\0';
	printf("Font name %d: %s\n",i,buffer);
	fontname[i]=my_strdup(buffer);
    }
    puts("Font Facename 			Height Family Foregr Backgr Style");
    fseek(HelpFile,FileStart+FontHdr.DescriptorsOffset,SEEK_SET);
    if(FontHdr.FacenamesOffset>=16)
    {
	for(i=0;i<FontHdr.NumDescriptors;i++)
	{
	    my_fread(&mvbfont,sizeof(mvbfont),HelpFile);
	    PrintMvbFont(i,&mvbfont);
	}
	fseek(HelpFile,FileStart+FontHdr.FormatsOffset,SEEK_SET);
	for(i=0;i<FontHdr.NumFormats;i++)
	{
	    my_fread(&mvbstyle,sizeof(mvbstyle),HelpFile);
	    printf("Style %d",mvbstyle.StyleNum);
	    if(mvbstyle.BasedOn) printf(" based on %d",mvbstyle.BasedOn);
	    printf(" named '%s':\n",mvbstyle.StyleName);
	    PrintMvbFont(i,&mvbstyle.font);
	}
    }
    else if(FontHdr.FacenamesOffset>=12)
    {
	for(i=0;i<FontHdr.NumDescriptors;i++)
	{
	    my_fread(&newfont,sizeof(newfont),HelpFile);
	    PrintNewFont(i,&newfont);
	}
	fseek(HelpFile,FileStart+FontHdr.FormatsOffset,SEEK_SET);
	for(i=0;i<FontHdr.NumFormats;i++)
	{
	    my_fread(&newstyle,sizeof(newstyle),HelpFile);
	    printf("Style %d",newstyle.StyleNum);
	    if(newstyle.BasedOn) printf(" based on %d",newstyle.BasedOn);
	    printf(" named '%s':\n",newstyle.StyleName);
	    PrintNewFont(i,&newstyle.font);
	}
    }
    else
    {
	for(i=0;i<FontHdr.NumDescriptors;i++)
	{
	    my_fread(&oldfont,sizeof(oldfont),HelpFile);
	    printf("%3d: %-32.32s %4d.%d %-6s %02X%02X%02X %02X%02X%02X ",i,fontname[oldfont.FontName],oldfont.HalfPoints/2,(oldfont.HalfPoints&1)*5,FontFamily(oldfont.FontFamily<6?lookup[oldfont.FontFamily]:oldfont.FontFamily),oldfont.FGRGB[2],oldfont.FGRGB[1],oldfont.FGRGB[0],oldfont.BGRGB[2],oldfont.BGRGB[1],oldfont.BGRGB[0]);
	    if(oldfont.Attributes&FONT_BOLD) putchar('b');
	    if(oldfont.Attributes&FONT_ITAL) putchar('i');
	    if(oldfont.Attributes&FONT_UNDR) putchar('u');
	    if(oldfont.Attributes&FONT_STRK) putchar('s');
	    if(oldfont.Attributes&FONT_DBUN) putchar('d');
	    if(oldfont.Attributes&FONT_SMCP) putchar('c');
	    putchar('\n');
	}
    }
}

void PhrImageDump(FILE *HelpFile)
{
    long FileLength;
    unsigned int bytes;
    PHRINDEXHDR PhrIndexHdr;
    unsigned char *ptr;

    if(SearchFile(HelpFile,"|PhrIndex",NULL))
    {
	my_fread(&PhrIndexHdr,sizeof(PhrIndexHdr),HelpFile);
	if(SearchFile(HelpFile,"|PhrImage",&FileLength))
	{
	    if(PhrIndexHdr.phrimagesize==PhrIndexHdr.phrimagecompressedsize)
	    {
		HexDump(HelpFile,FileLength,0L);
	    }
	    else
	    {
		if(FileLength!=PhrIndexHdr.phrimagecompressedsize)
		{
		    fprintf(stderr,"PhrImage FileSize %ld, in PhrIndex.FileHdr %ld\n",PhrIndexHdr.phrimagecompressedsize,FileLength);
		}
		ptr=my_malloc(PhrIndexHdr.phrimagesize);
		bytes=DecompressIntoBuffer(2,HelpFile,FileLength,ptr,PhrIndexHdr.phrimagesize);
		HexDumpMemory(ptr,bytes);
		free(ptr);
	    }
	}
    }
}

void BTreeDump(FILE *HelpFile,char text[])
{
    int n,i,j;
    long count;
    BUFFER buf;
    char format[10];
    char *ptr;

    n=GetFirstPage(HelpFile,&buf,NULL);
    while(n)
    {
	for(i=0;i<n;i++)
	{
	    for(ptr=text;*ptr;ptr++)
	    {
		if(*ptr=='%')
		{
		    j=strcspn(ptr,"hsdiouxX!");
		    memcpy(format,ptr,j+1);
		    format[j+1]='\0';
		    if(format[j]=='!')
		    {
			count=getdw(HelpFile);
			while(count>=8)
			{
			    printf(" (%ld)",getdw(HelpFile));
			    printf("%08lx",getdw(HelpFile));
			    count-=8;
			}
		    }
		    else if(format[j]=='h')
		    {
			format[j]='s';
			printf(format,unhash(getdw(HelpFile)));
		    }
		    else if(format[j]=='s')
		    {
			my_gets(buffer,sizeof(buffer),HelpFile);
			printf(format,buffer);
		    }
		    else if(strchr(format,'l'))
		    {
			printf(format,getdw(HelpFile));
		    }
		    else
		    {
			printf(format,my_getw(HelpFile));
		    }
		    ptr+=j;
		}
		else
		{
		    putchar(*ptr);
		}
	    }
	}
	n=GetNextPage(HelpFile,&buf);
    }
}

void PhraseDump(void)
{
    unsigned int n;

    for(n=0;n<PhraseCount;n++)
    {
	printf("%-5d - ",n);
	PrintPhrase(n,NULL,NULL);
	putchar('\n');
    }
}

void SysDump(FILE *HelpFile)
{
    SYSTEMHEADER SysHdr;
    SYSTEMRECORD *SysRec;
    struct tm *TimeRec;
    char *ptr;

    my_fread(&SysHdr,sizeof(SysHdr),HelpFile);
    if(SysHdr.Minor==15)
    {
	ptr="HC30";
    }
    else if(SysHdr.Minor==21)
    {
	ptr="HC31/HCP";
    }
    else if(SysHdr.Minor==27)
    {
	ptr="WMVC/MVCC";
    }
    else if(SysHdr.Minor==33)
    {
	if(mvp)
	{
	    ptr="MVC";
	}
	else
	{
	    ptr="HCRTF";
	}
    }
    else ptr="Unknown";
    printf("%s Help Compiler used.\n",ptr);
    printf("System Flags & Compression Method=0x%04x\n",SysHdr.Flags);
    if(SysHdr.GenDate)
    {
	TimeRec=localtime(&SysHdr.GenDate);
	printf("Help File Generated: %s",asctime(TimeRec));
    }
    if(SysHdr.Minor<16)
    {
	my_gets(HelpFileTitle,33,HelpFile);
	printf("TITLE=%s\n",HelpFileTitle);
    }
    else for(SysRec=GetFirstSystemRecord(HelpFile);SysRec;SysRec=GetNextSystemRecord(SysRec))
    {
	switch(SysRec->RecordType)
	{
	case 0x0001:
	    printf("TITLE=%s\n",SysRec->Data);
	    break;
	case 0x0002:
	    printf("COPYRIGHT=%s\n",SysRec->Data);
	    break;
	case 0x0003:
	    printf("CONTENTS=0x%08lX\n",*(long *)SysRec->Data);
	    break;
	case 0x0004:
	    printf("[MACRO] %s\n",SysRec->Data);
	    break;
	case 0x0005:
	    puts("Icon in System record");
	    break;
	case 0x0006:
	    printf("[WINDOW] ");
	    if(SysRec->DataSize==sizeof(SECWINDOW))
	    {
		PrintWindow(stdout,(SECWINDOW *)SysRec->Data);
	    }
	    else if(SysRec->DataSize==sizeof(MVBWINDOW))
	    {
		PrintMVBWindow(stdout,(MVBWINDOW *)SysRec->Data);
	    }
	    else
	    {
		HexDumpMemory(SysRec->Data,SysRec->DataSize);
		error("[WINDOW] data size does not match");
	    }
	    break;
	case 0x0008:
	    printf("CITATION=%s\n",SysRec->Data);
	    break;
	case 0x0009:
	    if(!mvp) printf("LCID=0x%X 0x%X 0x%X\n",*(short *)(SysRec->Data+8),*(short *)SysRec->Data,*(short *)(SysRec->Data+2));
	    break;
	case 0x000A:
	    if(!mvp) printf("CNT=%s\n",SysRec->Data);
	    break;
	case 0x000B:
//	    if(!mvp) printf("CHARSET=%d\n",*(unsigned char *)(SysRec->Data+1));
	    break;
	case 0x000C:
	    if(mvp)
	    {
		printf("[FTINDEX] dtype %s\n",SysRec->Data);
	    }
	    else
	    {
		printf("DEFFONT=%s,%d,%d\n",SysRec->Data+2,*(unsigned char *)SysRec->Data,*(unsigned char *)(SysRec->Data+1));
	    }
	    break;
	case 0x000D:
	    if(mvp) printf("[GROUPS] %s\n",SysRec->Data);
	    break;
	case 0x000E:
	    if(mvp)
	    {
		printf("[KEYINDEX] keyword=%c, \"%s\"\n",SysRec->Data[1],SysRec->Data+30);
	    }
	    else
	    {
		printf("INDEX_SEPARATORS=\"%s\"\n",SysRec->Data);
	    }
	    break;
	case 0x0012:
	    if(SysRec->Data[0]) printf("LANGUAGE=%s\n",SysRec->Data);
	    break;
	case 0x0013:
	    ptr=SysRec->Data+strlen(SysRec->Data)+1;
	    printf("[DLLMAPS] %s=%s,",SysRec->Data,ptr);
	    ptr+=strlen(ptr)+1;
	    printf("%s,",ptr);
	    ptr+=strlen(ptr)+1;
	    printf("%s,",ptr);
	    ptr+=strlen(ptr)+1;
	    printf("%s\n",ptr);
	    break;
	default:
	    fprintf(stderr,"Unknown record type: 0x%04X\n",SysRec->RecordType);
	    HexDumpMemory(SysRec->Data,SysRec->DataSize);
	}
    }
}

/* dump the contents of |TOPIC for debugging */
void DumpTopic(FILE *HelpFile,long TopicPos)
{
    TOPICLINK TopicLink;
    TOPICHEADER30 *TopicHdr30;
    TOPICHEADER *TopicHdr;
    char *ptr;
    char *str;
    char *cmd;
    long l;
    unsigned short x1,x2;
    unsigned char b;
    int cols,col;
    short i;
    char *LinkData1;
    char *LinkData2;
    long TopicNum;
    long TopicOffset;

    if(!SearchFile(HelpFile,"|TOPIC",&TopicFileLength)) return;
    TopicOffset=0L;
    if(TopicPos<12) TopicPos=12L;
    TopicNum=16;
    while(TopicRead(HelpFile,TopicPos,&TopicLink,sizeof(TopicLink))==sizeof(TOPICLINK))
    {
	puts("----------------------------------------------------------------------------");
	printf("TopicLink Type %02x: BlockSize=%08lx DataLen1=%08lx DataLen2=%08lx\n",TopicLink.RecordType,TopicLink.BlockSize,TopicLink.DataLen1,TopicLink.DataLen2);
	printf("TopicPos=%08lx TopicOffset=%08lx PrevBlock=%08lx NextBlock=%08lx\n",TopicPos,TopicOffset,TopicLink.PrevBlock,TopicLink.NextBlock);
	if(TopicLink.DataLen1>sizeof(TOPICLINK))
	{
	    LinkData1=my_malloc(TopicLink.DataLen1-sizeof(TOPICLINK));
	    if(TopicRead(HelpFile,0L,LinkData1,TopicLink.DataLen1-sizeof(TOPICLINK))!=TopicLink.DataLen1-sizeof(TOPICLINK)) break;
	}
	else LinkData1=NULL;
	if(TopicLink.DataLen1<TopicLink.BlockSize) /* read LinkData2 using phrase replacement */
	{
	    LinkData2=my_malloc(TopicLink.DataLen2+1);
	    if(TopicPhraseRead(HelpFile,0L,LinkData2,TopicLink.BlockSize-TopicLink.DataLen1,TopicLink.DataLen2)!=TopicLink.BlockSize-TopicLink.DataLen1) break;
	}
	else LinkData2=NULL;
	if(LinkData1) HexDumpMemory(LinkData1,TopicLink.DataLen1-sizeof(TOPICLINK));
	if(TopicLink.RecordType==TL_TOPICHDR)
	{
	    if(before31)
	    {
		TopicHdr30=(TOPICHEADER30 *)LinkData1;
		puts("============================================================================");
		printf("TopicHeader TopicNum=%ld BlockSize=%ld PrevTopicNum=%d NextTopicNum=%d\n",TopicNum,TopicHdr30->BlockSize,TopicHdr30->PrevTopicNum,TopicHdr30->NextTopicNum);
		TopicNum++;
	    }
	    else
	    {
		TopicHdr=(TOPICHEADER *)LinkData1;
		puts("============================================================================");
		printf("TopicHeader TopicNum=%ld BlockSize=%ld NextTopicOffset=%08lx\n",TopicHdr->TopicNum,TopicHdr->BlockSize,TopicHdr->NextTopic);
		printf("NonScroll=%08lx Scroll=%08lx BrowseBck=%08lx BrowseFor=%08lx\n",TopicHdr->NonScroll,TopicHdr->Scroll,TopicHdr->BrowseBck,TopicHdr->BrowseFor);
	    }
	}
	else if(TopicLink.RecordType==TL_DISPLAY30||TopicLink.RecordType==TL_DISPLAY||TopicLink.RecordType==TL_TABLE)
	{
	    switch(TopicLink.RecordType)
	    {
	    case TL_DISPLAY30:
		fputs("Text ",stdout);
		break;
	    case TL_DISPLAY:
		fputs("Display ",stdout);
		break;
	    case TL_TABLE:
		fputs("Table ",stdout);
		break;
	    }
	    ptr=LinkData1;
	    printf("expandedsize=%ld ",scanlong(&ptr));
	    if(TopicLink.RecordType==TL_DISPLAY||TopicLink.RecordType==TL_TABLE)
	    {
		x1=scanword(&ptr);
		TopicOffset+=x1;
		printf("topicoffsetincrement=%u ",x1);
	    }
	    if(TopicLink.RecordType==TL_TABLE)
	    {
		cols=(unsigned char)*ptr++;
		x1=*ptr++;
		printf("columns=%d type=%d ",cols,x1);
		switch(x1)
		{
		case 0:
		case 2:
		    printf("minwidth=%d ",*(short *)ptr);
		    ptr+=2;
		case 1:
		case 3:
		    break;
		default:
		    error("Unknown TableType %d",x1);
		}
		for(col=0;col<cols;col++)
		{
		    printf("width=%d gap=%d ",*(short *)ptr,*(short *)(ptr+2));
		    ptr+=4;
		}
	    }
	    putchar('\n');
	    str=LinkData2;
	    for(col=0;(TopicLink.RecordType==TL_TABLE?*(short *)ptr!=-1:col==0)&&ptr<LinkData1+TopicLink.DataLen1-sizeof(TOPICLINK);col++)
	    {
		if(TopicLink.RecordType==TL_TABLE)
		{
		    printf("column=%d %04x %d ",*(short *)ptr,*(unsigned short *)(ptr+2),*(unsigned char *)(ptr+4)-0x80);
		    ptr+=5;
		}
		printf("%02x %d id=%04x ",*(unsigned char *)ptr,*(unsigned char *)(ptr+1)-0x80,*(unsigned short *)(ptr+2));
		ptr += 4;
		x2 = *(unsigned short*) ptr;
		ptr += 2;
		if(x2&0x0001) printf("unknownbit01=%ld ",scanlong(&ptr)); /* found in MVBs, purpose unknown, may mean that x2 is compressed long */
		if(x2&0x0002) printf("topspacing=%d ",scanint(&ptr));
		if(x2&0x0004) printf("bottomspacing=%d ",scanint(&ptr));
		if(x2&0x0008) printf("linespacing=%d ",scanint(&ptr));
		if(x2&0x0010) printf("leftindent=%d ",scanint(&ptr));
		if(x2&0x0020) printf("rightindent=%d ",scanint(&ptr));
		if(x2&0x0040) printf("firstlineindent=%d ",scanint(&ptr));
		if(x2&0x0080) fputs("unknownbit80set",stdout);
		if(x2&0x0100)
		{
		    x1=(unsigned char)*ptr++;
		    if(x1&1) fputs("box ",stdout);
		    if(x1&2) fputs("topborder ",stdout);
		    if(x1&4) fputs("leftborder ",stdout);
		    if(x1&8) fputs("bottomborder ",stdout);
		    if(x1&0x10) fputs("rightborder ",stdout);
		    if(x1&0x20) fputs("thickborder ",stdout);
		    if(x1&0x40) fputs("doubleborder ",stdout);
		    if(x1&0x80) fputs("unknownborder",stdout);
		    printf("%04x ", *(unsigned short*) ptr);
                    ptr += 2;
		}
		if(x2&0x0200)
		{
		    i=scanint(&ptr);
		    printf("tabs=%d ",i);
		    while(i-->0)
		    {
			x1=scanword(&ptr);
			printf("stop=%d ",x1&0x3FFF);
			if(x1&0x4000)
			{
			    x1=scanword(&ptr);
			    if(x1==1)
			    {
				fputs("right ",stdout);
			    }
			    else if(x1==2)
			    {
				fputs("center ",stdout);
			    }
			    else
			    {
				error("unknowntabmodifier=%02x",x1);
			    }
			}
		    }
		}
		if(x2&0x0400) fputs("rightalign ",stdout);
		if(x2&0x0800) fputs("centeralign ",stdout);
		if(x2&0x1000) fputs("keeplinestogether ",stdout);
		if(x2&0x2000) fputs("unknownbit2000set ",stdout); /* found in PRINTMAN.HLP */
		if(x2&0x4000) fputs("unknownbit4000set ",stdout); /* found in PRINTMAN.HLP, RATTLER.HLP */
		if(x2&0x8000) fputs("unknownbit8000set",stdout);
		putchar('\n');
		while(1)
		{
		    str=PrintString(str,strlen(str))+1;
		    if((unsigned char)ptr[0]==0xFF)
		    {
			ptr++;
			break;
		    }
		    else switch((unsigned char)ptr[0])
		    {
		    case 0x20:
			printf("{vfld%ld}",*(long *)(ptr+1));
			ptr+=5;
			break;
		    case 0x21:
			printf("{dtype%d}",*(short *)(ptr+1));
			ptr+=3;
			break;
		    case 0x80: /* font change */
			printf("[font=%u]",*(short *)(ptr+1));
			ptr+=3;
			break;
		    case 0x81:
			puts("[LF]");
			ptr++;
			break;
		    case 0x82:
			puts("[CR]");
			ptr++;
			break;
		    case 0x83:
			fputs("[TAB]",stdout);
			ptr++;
			break;
		    case 0x86:
			ptr++;
			b=*ptr++;
			if(b==0x05) cmd="ewc"; else cmd="bmc";
			goto picture;
		    case 0x87:
			ptr++;
			b=*ptr++;
			if(b==0x05) cmd="ewl"; else cmd="bml";
			goto picture;
		    case 0x88:
			ptr++;
			b=*ptr++;
			if(b==0x05) cmd="ewr"; else cmd="bmr";
		    picture:
			printf("[%s %02x ",cmd,b);
			l=scanlong(&ptr);
			switch(b)
			{
			case 0x22: /* HC31 */
			    x1=scanword(&ptr);
			    printf("hotspots=%u ",x1);
			case 0x03: /* HC30 */
			    switch(*(unsigned short *)ptr)
			    {
			    case 0:
				fputs("baggage ",stdout);
				break;
			    case 1:
				fputs("embedded ",stdout);
				break;
			    default:
				error("Unknown %04x",((unsigned short *)ptr)[0]);
			    }
			    printf("bm%u]",((unsigned short *)ptr)[1]);
			    break;
			case 0x05:
			    printf("%04x ",((unsigned short *)ptr)[0]);
			    printf("%04x ",((unsigned short *)ptr)[1]);
			    printf("%04x ",((unsigned short *)ptr)[2]);
			    printf("%s]",ptr+6);
			    break;
			default:
			    error("Unknown picture flag %02x",b);
			}
			ptr+=l;
			break;
		    case 0x89: /* end of hot spot */
			fputs("[U]",stdout);
			ptr++;
			break;
		    case 0x8B: /* non-break-space */
			fputs("[~]",stdout);
			ptr++;
			break;
		    case 0x8C: /* non-break-hyphen */
			fputs("[-]",stdout);
			ptr++;
			break;
		    case 0xC8: /* macro */
			printf("[!%s]",ptr+3);
			ptr+=*(short *)(ptr+1)+3;
			break;
		    case 0xCC: /* macro without font change */
			printf("[*!%s]",ptr+3);
			ptr+=*(short *)(ptr+1)+3;
			break;
		    case 0xE0: /* Popup HC30 */
			printf("[^TOPIC%ld]",*(long *)(ptr+1));
			ptr+=5;
			break;
		    case 0xE1: /* Jump HC30 */
			printf("[TOPIC%ld]",*(long *)(ptr+1));
			ptr+=5;
			break;
		    case 0xE2: /* Popup HC31 */
			printf("[^%08lx]",*(long *)(ptr+1));
			ptr+=5;
			break;
		    case 0xE3: /* Jump HC31 */
			printf("[%08lx]",*(long *)(ptr+1));
			ptr+=5;
			break;
		    case 0xE6: /* Popup without font change */
			printf("[*^%08lx]",*(long *)(ptr+1));
			ptr+=5;
			break;
		    case 0xE7: /* Jump without font change */
			printf("[*%08lx]",*(long *)(ptr+1));
			ptr+=5;
			break;
		    case 0xEA: /* Popup into external file / secondary window */
			cmd="^";
			goto jump;
		    case 0xEB: /* Jump into external file / secondary window */
			cmd="";
			goto jump;
		    case 0xEE: /* Popup into external file / secondary window without font change */
			cmd="^*";
			goto jump;
		    case 0xEF: /* Jump into external file / secondary window without font change */
			cmd="*";
		    jump:
			switch(ptr[3])
			{
			case 0:
			    printf("[%s%08lx] ",cmd,*(long *)(ptr+4));
			    break;
			case 1: /* Popup into secondary window (silly) */
			    printf("[%s%08lx>%d]",cmd,*(long *)(ptr+4),(unsigned char)ptr[8]);
			    break;
			case 4:
			    printf("[%s%08lx@%s] ",cmd,*(long *)(ptr+4),ptr+8);
			    break;
			case 6: /* Popup into external file / secondary window (silly) */
			    printf("[%s%08lx>%s@%s] ",cmd,*(long *)(ptr+4),ptr+8,strchr(ptr+8,'\0')+1);
			    break;
			default:
			    putchar('[');
			    for(i=0;i<*(short *)(ptr+1);i++) printf("%02x",(unsigned char)ptr[i]);
			    putchar(']');
			}
			ptr+=*(short *)(ptr+1)+3;
			break;
		    default:
			printf("[%02x]",(unsigned char)*ptr++);
		    }
		}
		putchar('\n');
	    }
	}
	if(LinkData2&&(TopicLink.RecordType!=TL_DISPLAY30&&TopicLink.RecordType!=TL_DISPLAY&&TopicLink.RecordType!=TL_TABLE))
	{
	    PrintString(LinkData2,TopicLink.DataLen2);
	    putchar('\n');
	}
	if(LinkData1) free(LinkData1);
	if(LinkData2) free(LinkData2);
	if(before31)
	{
	    TopicPos+=TopicLink.NextBlock;
	    if(TopicPos>=TopicFileLength) break;
	}
	else
	{
	    if(TopicLink.NextBlock<=0) break;
	    TopicOffset=NextTopicOffset(TopicOffset,TopicLink.NextBlock,TopicPos);
	    TopicPos=TopicLink.NextBlock;
	}
    }
}

void AliasList(FILE *hpj) /* write [ALIAS] section to HPJ file */
{
    int i,n;
    BOOL headerwritten;

    headerwritten=FALSE;
    for(i=0;i<ContextRecs;i=n)
    {
	for(n=i+1;n<ContextRecs&&ContextRec[i].TopicOffset==ContextRec[n].TopicOffset;n++)
	{
	    if(!headerwritten)
	    {
		fputs("Creating [ALIAS] section...\n",stderr);
		fputs("[ALIAS]\n",hpj);
		headerwritten=TRUE;
	    }
	    fprintf(hpj,"%s=",unhash(ContextRec[n].HashValue));
	    fprintf(hpj,"%s\n",unhash(ContextRec[i].HashValue));
	}
    }
    if(headerwritten) putc('\n',hpj);
}

void CTXOMAPList(FILE *HelpFile,FILE *hpj) /* write [MAP] section to HPJ file */
{
    CTXOMAPREC CTXORec;
    unsigned short n,i;
    char *ptr;

    if(SearchFile(HelpFile,"|CTXOMAP",NULL))
    {
	n=my_getw(HelpFile);
	if(n)
	{
	    fputs("Creating [MAP] section...\n",stderr);
	    fputs("[MAP]\n",hpj);
	    for(i=0;i<n;i++)
	    {
		my_fread(&CTXORec,sizeof(CTXORec),HelpFile);
		ptr=TopicName(CTXORec.TopicOffset);
		if(ptr)
		{
		    fprintf(hpj,"%s %ld\n",ptr,CTXORec.MapID);
		}
		else
		{
		    fprintf(hpj,"TOPIC%08lx %ld\n",CTXORec.TopicOffset,CTXORec.MapID);
		}
	    }
	    putc('\n',hpj);
	}
    }
}

void GuessFromKeywords(FILE *HelpFile)
{
    long *keytopic;
    char kwdata[10];
    char kwbtree[10];
    int m,i,n,k,l,j,map;
    long FileLength,KWDataOffset,TopicOffset;
    BUFFER buf;

    fputs("Guessing...",stderr);
    for(k=0;k<2;k++) for(map='0';map<='z';map++)
    {
	if(k)
	{
	    if(!keyindex[map-'0']) continue;
	    sprintf(kwdata,"|%cKWDATA",map);
	    sprintf(kwbtree,"|%cKWBTREE",map);
	}
	else
	{
	    if(!lists[map-'0']) continue;
	    sprintf(kwdata,"|%cWDATA",map);
	    sprintf(kwbtree,"|%cWBTREE",map);
	}
	if(SearchFile(HelpFile,kwdata,&FileLength))
	{
	    keytopic=my_malloc(FileLength);
	    my_fread(keytopic,FileLength,HelpFile);
	    if(SearchFile(HelpFile,kwbtree,NULL))
	    {
		for(n=GetFirstPage(HelpFile,&buf,NULL);n;n=GetNextPage(HelpFile,&buf))
		{
		    for(i=0;i<n;i++)
		    {
			my_gets(keyword,sizeof(keyword),HelpFile);
			m=my_getw(HelpFile);
			KWDataOffset=getdw(HelpFile);
			for(j=0;j<m;j++)
			{
			    TopicOffset=keytopic[KWDataOffset/4+j];
			    Guess(keyword,TopicOffset);
			    for(l=0;l<alternatives;l++)
			    {
				if(alternative[l].OtherTopicOffset==TopicOffset)
				{
				    Guess(keyword,alternative[l].TopicOffset);
				}
			    }
			}
		    }
		    fputc('.',stderr);
		}
		free(keytopic);
	    }
	}
    }
    if(guessed>0)
    {
	fprintf(stderr,"%ld context ids found\n",guessed);
    }
    else
    {
	fputs("no context ids found\n(you may use option /g to turn off guessing on this help file)\n",stderr);
    }
}

/* 1. extract topic names from topic macros, embedded pictures, and hotspot macros */
/* 2. build browse sequence start list */
/* 3. extract embedded pictures */
void FirstPass(FILE *HelpFile)
{
    SYSTEMRECORD *SysRec;
    TOPICLINK TopicLink;
    char *LinkData1;
    char *LinkData2;
    char *ptr;
    long l1,TopicNum,TopicPos,TopicOffset,BogusTopicOffset;
    int n,i,col,cols;
    BUFFER buf;
    TOPICHEADER30 *TopicHdr30;
    TOPICHEADER *TopicHdr;
    char filename[20];
    unsigned short x1,x2;
    short y1;
    MFILE *f;

    if(extractmacros)
    {
	for(SysRec=GetFirstSystemRecord(HelpFile);SysRec;SysRec=GetNextSystemRecord(SysRec))
	{
	    if(SysRec->RecordType==0x0004)
	    {
		strcpy(TopicTitle,"[CONFIG] section");
		CheckMacro(SysRec->Data);
	    }
	}
	if(SearchFile(HelpFile,"|TopicId",NULL))
	{
	    for(n=GetFirstPage(HelpFile,&buf,NULL);n;n=GetNextPage(HelpFile,&buf))
	    {
		for(i=0;i<n;i++)
		{
		    getdw(HelpFile);
		    my_gets(buffer,sizeof(buffer),HelpFile);
		    AddTopic(buffer,FALSE);
		}
	    }
	    guessing=FALSE; /* it's not neccessary to guess context ids if you know them */
	}
    }
    browses=0;
    browsenums=1;
    if(!SearchFile(HelpFile,"|TOPIC",&TopicFileLength)) return;
    TopicOffset=0L;
    TopicPos=12L;
    TopicNum=16;
    while(TopicRead(HelpFile,TopicPos,&TopicLink,sizeof(TopicLink))==sizeof(TOPICLINK))
    {
	if(before31)
	{
	    if(TopicPos+TopicLink.NextBlock>=TopicFileLength) break;
	}
	else
	{
	    if(TopicLink.NextBlock<=0) break;
	}
	if(TopicLink.DataLen1>sizeof(TOPICLINK))
	{
	    LinkData1=my_malloc(TopicLink.DataLen1-sizeof(TOPICLINK)+1);
	    if(TopicRead(HelpFile,0L,LinkData1,TopicLink.DataLen1-sizeof(TOPICLINK))!=TopicLink.DataLen1-sizeof(TOPICLINK)) break;
	}
	else LinkData1=NULL;
	if(TopicLink.DataLen1<TopicLink.BlockSize) /* read LinkData2 using phrase replacement */
	{
	    LinkData2=my_malloc(TopicLink.DataLen2+1);
	    if(TopicPhraseRead(HelpFile,0L,LinkData2,TopicLink.BlockSize-TopicLink.DataLen1,TopicLink.DataLen2)!=TopicLink.BlockSize-TopicLink.DataLen1) break;
	}
	else LinkData2=NULL;
	if(TopicLink.RecordType==TL_TOPICHDR) /* display a topic header record */
	{
	    fprintf(stderr,"\rTopic %ld...",TopicNum-15);
	    if(before31)
	    {
		TopicHdr30=(TOPICHEADER30 *)LinkData1;
		if(resolvebrowse)
		{
		    if(TopicHdr30->NextTopicNum>TopicNum&&TopicHdr30->PrevTopicNum>TopicNum
		    || TopicHdr30->NextTopicNum==-1&&TopicHdr30->PrevTopicNum>TopicNum
		    || TopicHdr30->NextTopicNum>TopicNum&&TopicHdr30->PrevTopicNum==-1)
		    {
			AddBrowse(TopicNum,TopicHdr30->NextTopicNum,TopicHdr30->PrevTopicNum);
		    }
		    else if(TopicHdr30->NextTopicNum!=-1&&TopicHdr30->NextTopicNum<TopicNum&&TopicHdr30->PrevTopicNum!=-1&&TopicHdr30->PrevTopicNum<TopicNum)
		    {
			MergeBrowse(TopicNum,TopicNum,TopicHdr30->NextTopicNum,TopicHdr30->PrevTopicNum);
		    }
		    else if(TopicHdr30->NextTopicNum!=-1&&TopicHdr30->NextTopicNum<TopicNum&&(TopicHdr30->PrevTopicNum==-1||TopicHdr30->PrevTopicNum>TopicNum))
		    {
			BackLinkBrowse(TopicNum,TopicNum,TopicHdr30->NextTopicNum,TopicHdr30->PrevTopicNum);
		    }
		    else if(TopicHdr30->PrevTopicNum!=-1&&TopicHdr30->PrevTopicNum<TopicNum&&(TopicHdr30->NextTopicNum==-1||TopicHdr30->NextTopicNum>TopicNum))
		    {
			LinkBrowse(TopicNum,TopicNum,TopicHdr30->NextTopicNum,TopicHdr30->PrevTopicNum);
		    }
		}
	    }
	    else
	    {
		BogusTopicOffset=NextTopicOffset(TopicOffset,TopicLink.NextBlock,TopicPos);
		if(BogusTopicOffset!=TopicOffset)
		{
		    alternative=my_realloc(alternative,(alternatives+1)*sizeof(ALTERNATIVE));
		    alternative[alternatives].TopicOffset=TopicOffset;
		    alternative[alternatives].OtherTopicOffset=BogusTopicOffset;
		    alternatives++;
		}
		TopicHdr=(TOPICHEADER *)LinkData1;
		if(resolvebrowse)
		{
		    if(TopicHdr->BrowseFor>TopicOffset&&TopicHdr->BrowseBck>TopicOffset
		    || TopicHdr->BrowseFor==-1L&&TopicHdr->BrowseBck>TopicOffset
		    || TopicHdr->BrowseFor>TopicOffset&&TopicHdr->BrowseBck==-1L)
		    {
			AddBrowse(TopicOffset,TopicHdr->BrowseFor,TopicHdr->BrowseBck);
		    }
		    else if(TopicHdr->BrowseFor!=-1L&&TopicHdr->BrowseFor<TopicOffset&&TopicHdr->BrowseBck!=-1L&&TopicHdr->BrowseBck<TopicOffset)
		    {
			MergeBrowse(TopicOffset,BogusTopicOffset,TopicHdr->BrowseFor,TopicHdr->BrowseBck);
		    }
		    else if(TopicHdr->BrowseFor!=-1L&&TopicHdr->BrowseFor<TopicOffset&&(TopicHdr->BrowseBck==-1L||TopicHdr->BrowseBck>TopicOffset))
		    {
			BackLinkBrowse(TopicOffset,BogusTopicOffset,TopicHdr->BrowseFor,TopicHdr->BrowseBck);
		    }
		    else if(TopicHdr->BrowseBck!=-1L&&TopicHdr->BrowseBck<TopicOffset&&(TopicHdr->BrowseFor==-1L||TopicHdr->BrowseFor>TopicOffset))
		    {
			LinkBrowse(TopicOffset,BogusTopicOffset,TopicHdr->BrowseFor,TopicHdr->BrowseBck);
		    }
		}
		if(extractmacros)
		{
		    if(TopicLink.DataLen2&&*LinkData2)
		    {
			strlcpy(TopicTitle,LinkData2,sizeof(TopicTitle));
			if(guessing)
			{
			    Guess(LinkData2,TopicOffset);
			    if(BogusTopicOffset!=TopicOffset)
			    {
				Guess(LinkData2,BogusTopicOffset);
			    }
			}
		    }
		    else
		    {
			strcpy(TopicTitle,"<< untitled topic >>");
		    }
		    if(TopicLink.DataLen2)
		    {
			for(i=strlen(LinkData2)+1;i<TopicLink.DataLen2;i+=strlen(LinkData2+i)+1)
			{
			    CheckMacro(LinkData2+i);
			}
		    }
		}
	    }
	    TopicNum++;
	}
	else if(TopicLink.RecordType==TL_DISPLAY30||TopicLink.RecordType==TL_DISPLAY||TopicLink.RecordType==TL_TABLE)
	{
	    ptr=LinkData1;
	    scanlong(&ptr);
	    if(TopicLink.RecordType==TL_DISPLAY||TopicLink.RecordType==TL_TABLE)
	    {
		TopicOffset+=scanword(&ptr);
	    }
	    if(TopicLink.RecordType==TL_TABLE)
	    {
		cols=(unsigned char)*ptr++;
		x1=(unsigned char)*ptr++;
		switch(x1)
		{
		case 0: /* found in CALC.HLP and TERMINAL.HLP */
		case 2:
		    ptr+=2;
		case 1:
		case 3:
		    break;
		default:
		    error("Unknown TableType %d",x1);
		}
		ptr+=4*cols;
	    }
	    for(col=0;(TopicLink.RecordType==TL_TABLE?*(short *)ptr!=-1:col==0)&&ptr<LinkData1+TopicLink.DataLen1-sizeof(TOPICLINK);col++)
	    {
		if(TopicLink.RecordType==TL_TABLE) ptr+=5;
		ptr += 4;
		x2 = *(unsigned short *)ptr;
		ptr += 2;
		if(x2&0x0001) scanlong(&ptr); /* found in MVBs, purpose */
		/* unknown, may mean that x2 is really compressed long */
		if(x2&0x0002) scanint(&ptr);
		if(x2&0x0004) scanint(&ptr);
		if(x2&0x0008) scanint(&ptr);
		if(x2&0x0010) scanint(&ptr);
		if(x2&0x0020) scanint(&ptr);
		if(x2&0x0040) scanint(&ptr);
		if(x2&0x0100) ptr+=3;
		if(x2&0x0200)
		{
		    y1=scanint(&ptr);
		    while(y1-->0) if(scanword(&ptr)&0x4000) scanword(&ptr);
		}
		while(ptr<LinkData1+TopicLink.DataLen1-sizeof(TOPICLINK))
		{
		    if((unsigned char)ptr[0]==0xFF)
		    {
			ptr++;
			break;
		    }
		    else switch((unsigned char)ptr[0])
		    {
		    case 0x21: /* dtype (MVB) */
		    case 0x80: /* font change */
			ptr+=3;
			break;
		    case 0x81:
		    case 0x82:
		    case 0x83:
		    case 0x89: /* end of hotspot */
		    case 0x8B: /* non-break-space */
		    case 0x8C: /* non-break-hyphen */
			ptr++;
			break;
		    case 0x86:
		    case 0x87:
		    case 0x88:
			ptr++;
			x1=*ptr++;
			l1=scanlong(&ptr);
			switch(x1)
			{
			case 0x22: /* HC31 */
			    x1=scanword(&ptr);
			    /* fall thru */
			case 0x03: /* HC30 */
			    switch(((unsigned short *)ptr)[0])
			    {
			    case 1:
				for (x2=1; x2<extensions; x2++) if (! extension[x2]) break;
				if (x2 >= extensions)
				{
				    extension=my_realloc(extension,(x2+1)*sizeof(char));
				    while(extensions<=x2) extension[extensions++]=0;
				}
				sprintf(filename,"bm%u",x2);
				f=CreateMap(ptr+2,l1-2);
				x1=ExtractBitmap(filename,f);
				CloseMap(f);
				extension[x2]=x1|0x10;
				break;
			    }
			    break;
			case 0x05:
			    if(ptr[6]=='!'&&strchr(ptr+7,','))
			    {
				CheckMacro(strchr(ptr+7,',')+1);
			    }
			    break;
			}
			ptr+=l1;
			break;
		    case 0xC8: /* macro */
		    case 0xCC: /* macro without font change */
			CheckMacro(ptr+3);
			ptr+=*(short *)(ptr+1)+3;
			break;
		    case 0x20: /* vfld (MVC) */
		    case 0xE0: /* popup jump HC30 */
		    case 0xE1: /* topic jump HC30 */
		    case 0xE2: /* popup jump HC31 */
		    case 0xE3: /* topic jump HC31 */
		    case 0xE6: /* popup jump without font change */
		    case 0xE7: /* topic jump without font change */
			ptr+=5;
			break;
		    case 0xEA: /* popup jump into external file */
		    case 0xEB: /* topic jump into external file / secondary window */
		    case 0xEE: /* popup jump into external file without font change */
		    case 0xEF: /* topic jump into external file / secondary window without font change */
			switch((unsigned char)ptr[3])
			{
			case 0:
			case 1:
			    break;
			case 4:
			    StoreReference(ptr+8,TOPIC,NULL,*(long *)(ptr+4));
			    break;
			case 6:
			    StoreReference(strchr(ptr+8,'\0')+1,TOPIC,NULL,*(long *)(ptr+4));
			    break;
			default:
			    error("Unknown modifier %02x in tag %02x",(unsigned char)ptr[3],(unsigned char)ptr[0]);
			}
			ptr+=*(short *)(ptr+1)+3;
			break;
		    default:
			error("Unknown %02x",*(unsigned char *)ptr);
			ptr++;
		    }
		}
	    }
	}
	if(LinkData1) free(LinkData1);
	if(LinkData2) free(LinkData2);
	if(before31)
	{
	    TopicPos+=TopicLink.NextBlock;
	}
	else
	{
	    TopicOffset=NextTopicOffset(TopicOffset,TopicLink.NextBlock,TopicPos);
	    TopicPos=TopicLink.NextBlock;
	}
    }
}

int _cdecl CTXOMAPRecCmp(const void *a,const void *b)
{
    if(((CTXOMAPREC *)a)->TopicOffset<((CTXOMAPREC *)b)->TopicOffset) return -1;
    if(((CTXOMAPREC *)a)->TopicOffset>((CTXOMAPREC *)b)->TopicOffset) return 1;
    return 0;
}

void ContextList(FILE *HelpFile)
{
    unsigned short maprecs,m;
    int j,window,len;
    BOOL morekeywords;
    CTXOMAPREC *map;
    char filename[13];
    TOPICLINK TopicLink;
    char *LinkData1;
    char *LinkData2;
    char *ptr;
    long TopicPos,TopicNum,TopicOffset;

    if(SearchFile(HelpFile,"|CTXOMAP",NULL))
    {
	maprecs=my_getw(HelpFile);
	if(maprecs)
	{
	    map=my_malloc((long)maprecs*sizeof(CTXOMAPREC));
	    my_fread(map,(long)maprecs*sizeof(CTXOMAPREC),HelpFile);
	    qsort(map,maprecs,sizeof(CTXOMAPREC),CTXOMAPRecCmp);
	}
    }
    else
    {
	 maprecs=0;
    }
    strcpy(filename,name);
    strcat(filename,ext);
    if(!SearchFile(HelpFile,"|TOPIC",&TopicFileLength)) return;
    TopicOffset=0L;
    TopicPos=12L;
    TopicNum=1;
    j=0;
    len=80;
    window=-1;
    morekeywords=TRUE;
    m=0;
    while(TopicRead(HelpFile,TopicPos,&TopicLink,sizeof(TopicLink))==sizeof(TOPICLINK))
    {
	if(TopicLink.DataLen1>sizeof(TOPICLINK))
	{
	    LinkData1=my_malloc(TopicLink.DataLen1-sizeof(TOPICLINK)+1);
	    if(TopicRead(HelpFile,0L,LinkData1,TopicLink.DataLen1-sizeof(TOPICLINK))!=TopicLink.DataLen1-sizeof(TOPICLINK)) break;
	}
	else LinkData1=NULL;
	if(TopicLink.DataLen1<TopicLink.BlockSize) /* read LinkData2 using phrase replacement */
	{
	    LinkData2=my_malloc(TopicLink.DataLen2+1);
	    if(TopicPhraseRead(HelpFile,0L,LinkData2,TopicLink.BlockSize-TopicLink.DataLen1,TopicLink.DataLen2)!=TopicLink.BlockSize-TopicLink.DataLen1) break;
	}
	else LinkData2=NULL;
	if(TopicLink.NextBlock<=0||TopicLink.RecordType==TL_TOPICHDR) /* display a topic header record */
	{
	    if(TopicNum>1) putchar('\n');
	    if(before31) TopicOffset=TopicPos;
	    while(m<maprecs&&map[m].TopicOffset<TopicOffset)
	    {
		printf("  WinHelp(wnd,\"%s\",HELP_CONTEXT,%lu)\n",filename,map[m].MapID);
		m++;
	    }
	    if(!before31)
	    {
		while(j<ContextRecs&&ContextRec[j].TopicOffset<TopicOffset)
		{
		    if(len==80)
		    {
			fputs("  Jump",stdout);
		    }
		    else
		    {
			fputs("  Popup",stdout);
		    }
		    printf("Id(`%s",filename);
		    if(window!=-1) printf(">%s",GetWindowName(window));
		    printf("',`%s')\n",unhash(ContextRec[j].HashValue));
		    j++;
		}
		if(morekeywords)
		{
		    if(NextKeywordRec>=KeywordRecs)
		    {
			if(NextKeywordOffset<0x7FFFFFFFL)
			{
			    CollectKeywords(HelpFile);
			}
			else
			{
			    morekeywords=FALSE;
			}
		    }
		    while(NextKeywordRec<KeywordRecs&&KeywordRec[NextKeywordRec].TopicOffset<TopicOffset)
		    {
			if(KeywordRec[NextKeywordRec].Footnote=='K')
			{
			    printf("  JumpKeyword(`%s',`%s')\n",filename,KeywordRec[NextKeywordRec].Keyword);
			}
			else if(KeywordRec[NextKeywordRec].Footnote=='A')
			{
			    printf("  ALink(`%s@%s')\n",KeywordRec[NextKeywordRec].Keyword,filename);
			}
			NextKeywordRec++;
		    }
		}
		window=ListWindows(HelpFile,TopicOffset);
	    }
	    if(TopicLink.NextBlock<=0) break;
	    if(LinkData2&&*LinkData2)
	    {
		printf("Topic %ld: %s",TopicNum,LinkData2);
		len=80;
	    }
	    else
	    {
		len=printf("Topic %ld: untitled: ",TopicNum);
	    }
	    TopicNum++;
	}
	else if(LinkData2&&len<79)
	{
	    for(ptr=LinkData2;len<79&&ptr<LinkData2+TopicLink.DataLen2;ptr++) if(*ptr)
	    {
		putchar(*ptr);
		len++;
	    }
	}
	if(TopicLink.RecordType==TL_DISPLAY||TopicLink.RecordType==TL_TABLE)
	{
	    ptr=LinkData1;
	    scanlong(&ptr);
	    TopicOffset+=scanword(&ptr);
	}
	if(LinkData1) free(LinkData1);
	if(LinkData2) free(LinkData2);
	if(before31)
	{
	    if(TopicPos+TopicLink.NextBlock>=TopicFileLength) break;
	    TopicPos+=TopicLink.NextBlock;
	}
	else
	{
	    if(TopicLink.NextBlock<=0) break;
	    TopicOffset=NextTopicOffset(TopicOffset,TopicLink.NextBlock,TopicPos);
	    TopicPos=TopicLink.NextBlock;
	}
    }
}

BOOL HelpDeCompile(FILE *HelpFile,char *dumpfile,int mode,char *exportname,long offset)
{
    char filename[81];
    char hpjfilename[81];
    long FileLength;
    FILE *rtf;
    FILE *hpj;
    int d;
    long topic;

    if(!SearchFile(HelpFile,NULL,&FileLength)) return FALSE;
    if(!dumpfile)
    {
	switch(mode)
	{
	case 0:
	    SysLoad(HelpFile);
	    fprintf(stderr,"Decompiling %s...\n",HelpFileTitle);
	    ContextLoad(HelpFile);
	    PhraseLoad(HelpFile);
	    ExportBitmaps(HelpFile);
	    fputs("Pass 1...\n",stderr);
	    FirstPass(HelpFile); /* valid only after ExportBitmaps */
	    putc('\n',stderr);
	    if(!before31&&guessing) GuessFromKeywords(HelpFile); /* after FirstPass, before SysList */
	    strcpy(hpjfilename,name);
	    if(mvp)
	    {
		strcat(hpjfilename,".MVP");
	    }
	    else
	    {
		strcat(hpjfilename,".HPJ");
	    }
	    hpj=my_fopen(hpjfilename,"wt");
	    if(hpj)
	    {
		strcpy(filename,name);
		strcat(filename,".ICO");
		SysList(HelpFile,hpj,filename); /* after ContextLoad */
		ListBaggage(HelpFile,hpj,before31);
		if(!mvp) AliasList(hpj); /* after ContextLoad, before TopicDump */
		strcpy(filename,name);
		strcat(filename,".PH");
		PhraseList(filename); /* after PhraseLoad */
		BuildName(filename,TopicsPerRTF>0);
		rtf=my_fopen(filename,"wt");
		if(rtf)
		{
		    FontLoad(HelpFile,rtf,hpj);
		    fputs("Pass 2...\n",stderr);
		    fprintf(hpj,"[FILES]\n%s\n\n",filename);
		    rtf=TopicDump(HelpFile,rtf,hpj,FALSE);
		    putc('}',rtf);
		    putc('\n',stderr);
		    my_fclose(rtf);
		}
		NotInAnyTopic=FALSE;
		CTXOMAPList(HelpFile,hpj);
		if(extensions&&before31) ListBitmaps(hpj);
		if(win95) ListRose(HelpFile,hpj);
		my_fclose(hpj);
	    }
	    if(PhraseOffsets)
	    {
		if(win95)
		{
		    puts("Help Compiler will issue Note HC1002: Using existing phrase table");
		}
		else
		{
		    puts("Help Compiler will issue Warning 5098: Using old key-phrase table");
		}
	    }
	    if(missing) puts("Help Compiler will issue Error 1230: File 'missing.bmp' not found");
	    if(NotInAnyTopic) puts("Help Compiler will issue Warning 4098: Context string(s) in [MAP] section not defined in any topic");
	    if(!extractmacros) puts("Help Compiler may issue Warning 4131: Hash conflict between 'x' and 'y'.");
	    if(warnings)
	    {
		printf("HELPDECO had problems with %s. Rebuilt helpfile may behave bad.\n",HelpFileName);
	    }
	    if(helpcomp[0])
	    {
		if(win95&&SearchFile(HelpFile,"|Petra",NULL)) strcat(helpcomp," /a");
		printf("Use %s %s to recompile ",helpcomp,hpjfilename);
		if(AnnoFile) fputs("annotated ",stdout);
		puts("helpfile.");
	    }
	    break;
	case 1:
	    HexDump(HelpFile,FileLength,offset);
	    break;
	case 2:
	    ListFiles(HelpFile);
	    break;
	case 3: /* create lookalike RTF */
	    SysLoad(HelpFile);
	    fprintf(stderr,"Writing %s...\n",HelpFileTitle);
	    exportplain=TRUE;
	    ExportBitmaps(HelpFile);
	    PhraseLoad(HelpFile);
	    BuildName(filename,TopicsPerRTF>0);
	    rtf=my_fopen(filename,"wt");
	    if(rtf)
	    {
		FontLoad(HelpFile,rtf,NULL);
		rtf=TopicDump(HelpFile,rtf,NULL,TRUE);
		putc('}',rtf);
		putc('\n',stderr);
		my_fclose(rtf);
	    }
	    break;
	case 4: /* generate contents file */
	    SysLoad(HelpFile);
	    fprintf(stderr,"Scanning %s...\n",HelpFileTitle);
	    ContextLoad(HelpFile);
	    PhraseLoad(HelpFile);
	    checkexternal=TRUE;
	    ExportBitmaps(HelpFile);
	    FirstPass(HelpFile);
	    putc('\n',stderr);
	    if(!before31&&guessing) GuessFromKeywords(HelpFile); /* after FirstPass, before SysList */
	    strcpy(filename,name);
	    strcat(filename,".CNT");
	    rtf=my_fopen(filename,"wt");
	    if(rtf)
	    {
		GenerateContent(HelpFile,rtf);
		my_fclose(rtf);
	    }
	    break;
	case 5: /* create entry point list  */
	    resolvebrowse=FALSE;
	    checkexternal=TRUE;
	    SysLoad(HelpFile);
	    fprintf(stderr,"Parsing %s...\n",HelpFileTitle);
	    ContextLoad(HelpFile);
	    PhraseLoad(HelpFile);
	    ExportBitmaps(HelpFile);
	    FirstPass(HelpFile);
	    putc('\n',stderr);
	    if(!before31&&guessing) GuessFromKeywords(HelpFile); /* after FirstPass, before SysList */
	    ContextList(HelpFile);
	    break;
	case 6: /* check external references */
	case 7:
	    resolvebrowse=FALSE;
	    checkexternal=TRUE;
	    SysLoad(HelpFile);
	    fprintf(stderr,"Checking %s...\n",HelpFileTitle);
	    PhraseLoad(HelpFile);
	    FirstPass(HelpFile);
	    putc('\n',stderr);
	    if(!external)
	    {
		printf("No references to external files found in %s.\n",HelpFileName);
	    }
	    else if(mode==6)
	    {
		CheckReferences();
	    }
	    else
	    {
		ListReferences();
	    }
	    break;
	}
    }
    else
    {
	if(!SearchFile(HelpFile,dumpfile,&FileLength))
	{
	    filename[0]='|';
	    strlcpy(filename+1,dumpfile,sizeof(filename)-1);
	    if(!SearchFile(HelpFile,filename,&FileLength))
	    {
		fprintf(stderr,"Internal file %s not found.\n",dumpfile);
		return TRUE;
	    }
	    dumpfile=filename;
	}
	printf("FileName: %s FileSize: %ld\n",dumpfile,FileLength);
	if(exportname) /* export internal file */
	{
	    FILE *f;

	    f=my_fopen(exportname,"wb");
	    if(f)
	    {
		copy(HelpFile,FileLength,f);
		my_fclose(f);
	    }
	}
	else if(mode==1)
	{
	    HexDump(HelpFile,FileLength,offset);
	}
	else if(strcmp(dumpfile,"|TOPIC")==0)
	{
	    SysLoad(HelpFile);
	    PhraseLoad(HelpFile);
	    DumpTopic(HelpFile,offset);
	}
	else if(strcmp(dumpfile+strlen(dumpfile)-4,".GRP")==0)
	{
	    GroupDump(HelpFile);
	}
	else if(strcmp(dumpfile,"@LINK")==0)
	{
	    LinkDump(HelpFile);
	}
	else if(sscanf(dumpfile,"%ld!%d",&topic,&d)==2&&topic!=0L&&d==0)
	{
	    AnnotationDump(HelpFile,FileLength,dumpfile);
	}
	else if(strcmp(dumpfile,"|Phrases")==0||strcmp(dumpfile,"|PhrIndex")==0)
	{
	    SysLoad(HelpFile);
	    PhraseLoad(HelpFile);
	    PhraseDump();
	}
	else if(strcmp(dumpfile,"|SYSTEM")==0)
	{
	    SysDump(HelpFile);
	}
	else if(strcmp(dumpfile,"|TOMAP")==0)
	{
	    ToMapDump(HelpFile,FileLength);
	}
	else if(strcmp(dumpfile,"|CONTEXT")==0)
	{
	    BTreeDump(HelpFile,"ContextId: %h TopicOffset: 0x%08lx\n");
	}
	else if(dumpfile[0]=='|'&&(strcmp(dumpfile+2,"WBTREE")==0||strcmp(dumpfile+2,"KWBTREE")==0))
	{
	    fseek(HelpFile,7,SEEK_CUR);
	    d=getc(HelpFile);
	    fseek(HelpFile,-8,SEEK_CUR);
	    if(d=='!')
	    {
		BTreeDump(HelpFile,"Keyword: '%s' Occurrance: %!\n");
	    }
	    else
	    {
		BTreeDump(HelpFile,"Keyword: '%s' Count: %u KWDataAddress: 0x%08lx\n");
	    }
	}
	else if(dumpfile[0]=='|'&&(strcmp(dumpfile+2,"WMAP")==0||strcmp(dumpfile+2,"KWMAP")==0))
	{
	    KWMapDump(HelpFile);
	}
	else if(dumpfile[0]=='|'&&(strcmp(dumpfile+2,"WDATA")==0||strcmp(dumpfile+2,"KWDATA")==0))
	{
	    KWDataDump(HelpFile,FileLength);
	}
	else if(strcmp(dumpfile,"|VIOLA")==0)
	{
	    BTreeDump(HelpFile,"TopicOffset: 0x%08lx WindowNumber: %ld\n");
	}
	else if(strcmp(dumpfile,"|CTXOMAP")==0)
	{
	    CTXOMAPDump(HelpFile);
	}
	else if(strcmp(dumpfile,"|CATALOG")==0)
	{
	    CatalogDump(HelpFile);
	}
	else if(strcmp(dumpfile,"|Petra")==0)
	{
	    BTreeDump(HelpFile,"TopicOffset: 0x%08lx SourceFileName: %s\n");
	}
	else if(strcmp(dumpfile,"|TopicId")==0)
	{
	    BTreeDump(HelpFile,"TopicOffset: 0x%08lx ContextId: %s\n");
	}
	else if(strcmp(dumpfile,"|Rose")==0)
	{
	    BTreeDump(HelpFile,"KeywordHashValue: 0x%08lx\nMacro: %s\nTitle: %s\n");
	}
	else if(strcmp(dumpfile,"|TTLBTREE")==0)
	{
	    BTreeDump(HelpFile,"TopicOffset: 0x%08lx TopicTitle: %s\n");
	}
	else if(strcmp(dumpfile,"|FONT")==0)
	{
	    FontDump(HelpFile);
	}
	else /* generic  */
	{
	    topic=ftell(HelpFile);
	    if(my_getw(HelpFile)==0x293B) /* if it's a B+ tree */
	    {
		my_getw(HelpFile);
		my_getw(HelpFile);
		filename[0]='\0';
		while((d=getc(HelpFile))>0) /* format according to Structure */
		{
		    switch(d)
		    {
		    case 'L':
		    case '4':
		    case 'a':
			strcat(filename,"0x%08lx ");
			break;
		    case '2':
			strcat(filename,"%5u ");
			break;
		    case 'F':
		    case 'i':
		    case 'z':
			strcat(filename,"'%s' ");
			break;
		    case '!':
			strcat(filename,"%!");
			break;
		    default:
			error("Unknown Btree field type '%c'",d);
		    }
		}
		strcat(filename,"\n");
		fseek(HelpFile,topic,SEEK_SET);
		BTreeDump(HelpFile,filename);
	    }
	    else
	    {
		fseek(HelpFile,topic,SEEK_SET);
		HexDump(HelpFile,FileLength,offset);
	    }
	}
    }
    return TRUE;
}

int _cdecl main(int argc,char *argv[])
{
    char AnnoFileName[81];
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    FILE *f;
    int mode;
    BOOL annotate;
    char *filename;
    char *dumpfile;
    char *exportname;
    long offset;
    int i,j;

    /* initialize hash value coding oldtable */
    memset(oldtable,0,sizeof(oldtable));
    for(i=0;i<9;i++) oldtable['1'+i]=i+1;
    oldtable['0']=10;
    oldtable['.']=12;
    oldtable['_']=13;
    for(i=0;i<26;i++) oldtable['A'+i]=oldtable['a'+i]=17+i;
    exportname=dumpfile=filename=NULL;
    AnnoFileName[0]='\0';
    mode=0;
    offset=0;
    annotate=FALSE;
    /* scan arguments */
    for(i=1;i<argc;i++)
    {
	if(argv[i][0]=='/'||argv[i][0]=='-')
	{
	    switch(tolower((unsigned char)argv[i][1]))
	    {
	    case 'a':
		if(argv[i][2])
		{
		    strlcpy(AnnoFileName,argv[i]+2,sizeof(AnnoFileName));
		}
		else if(argv[i+1]&&argv[i+1][0]!='/'&&argv[i+1][0]!='-')
		{
		    strlcpy(AnnoFileName,argv[i+1],sizeof(AnnoFileName));
		    i++;
		}
		annotate=TRUE;
		break;
	    case 'b':
		resolvebrowse=FALSE;
		break;
	    case 'c':
		mode=4;
		break;
	    case 'd':
		mode=2;
		break;
	    case 'e':
		mode=7;
		break;
	    case 'f':
		listtopic=TRUE;
		break;
	    case 'g':
		guessing=FALSE;
		break;
	    case 'h': // add entry to prefix table
		for(j=0;j<sizeof(prefix)/sizeof(prefix[0])&&prefix[j];j++) ;
		if(j<sizeof(prefix)/sizeof(prefix[0]))
		{
		    if(argv[i][2])
		    {
			prefix[j]=argv[i]+2;
		    }
		    else if(argv[i+1]&&argv[i+1][0]!='/'&&argv[i+1][0]!='-')
		    {
			prefix[j]=argv[i+1];
			i++;
		    }
		}
		else
		{
		    fprintf(stderr,"Prefix table full.\n");
		}
		break;
	    case 'i':
		reportderived=TRUE;
		break;
	    case 'l':
		mode=5;
		break;
	    case 'm':
		extractmacros=FALSE;
		break;
	    case 'n':
		nopagebreak=TRUE;
		break;
	    case 'p':
		mode=6;
		break;
	    case 'r':
		mode=3;
		break;
	    case 's':
		if(argv[i][2])
		{
		    TopicsPerRTF=atoi(argv[i]+2);
		}
		else if(argv[i+1]&&argv[i+1][0]!='/'&&argv[i+1][0]!='-')
		{
		    TopicsPerRTF=atoi(argv[i+1]);
		    i++;
		}
		break;
	    case 't':
		if(argv[i][2])
		{
		    offset=strtoul(argv[i]+2,NULL,0);
		}
		else if(argv[i+1]&&argv[i+1][0]!='/'&&argv[i+1][0]!='-')
		{
		    offset=strtoul(argv[i+1],NULL,0);
		    i++;
		}
		break;
	    case 'x':
		mode=1;
		break;
	    case 'y':
		overwrite=TRUE;
		break;
	    case 'z':
		exportLZ77=TRUE;
		break;
	    default:
		fprintf(stderr,"unknown option '%s' ignored\n",argv[i]);
	    }
	}
	else if(exportname)
	{
	    fprintf(stderr,"additional parameter '%s' ignored\n",argv[i]);
	}
	else if(dumpfile)
	{
	    exportname=argv[i];
	}
	else if(filename)
	{
	    dumpfile=argv[i];
	}
	else
	{
	    filename=argv[i];
	}
    }
    if(filename)
    {
	strupr(filename);
	_splitpath(filename, drive, dir, name, ext);
	if (ext[0] == '\0')
            strcpy(ext, ".HLP");
	mvp = ext[1] == 'M';
	_makepath(HelpFileName, drive, dir, name, ext);
	f = fopen(HelpFileName,"rb");
	if(f)
	{
	    if(annotate)
	    {
		if(AnnoFileName[0]=='\0') _makepath(AnnoFileName,drive,dir,name,".ANN");
		AnnoFile=fopen(AnnoFileName,"rb");
		if(!AnnoFile)
		{
		    fprintf(stderr,"Couldn't find annotation file '%s'\n",AnnoFileName);
		}
	    }
	    prefixhash[0]=0L;
	    for(i=1;prefix[i];i++)
	    {
		prefixhash[i]=hash(prefix[i]);
	    }
	    if(!HelpDeCompile(f,dumpfile,mode,exportname,offset))
	    {
		fprintf(stderr,"%s isn't a valid WinHelp file !\n",HelpFileName);
	    }
	    if(annotate&&AnnoFile) fclose(AnnoFile);
	    my_fclose(f);
	}
	else
	{
	    fprintf(stderr,"Can not open '%s'\n",HelpFileName);
	}
    }
    else
    {
	fprintf(stderr,"HELPDECO - decompile *.HLP/*.MVB files of Windows 3.x / 95 - %ld bit Version 2.1\n"
		       "M.Winterhoff, Geschw.-Scholl-Ring 17, 38444 Wolfsburg, Germany, CIS 100326,2776\n"
		       "\n"
		       "usage:   HELPDECO helpfile[.hlp]    [/y]  - decompile helpfile into all sources\n"
		       "         HELPDECO helpfile[.hlp]    [/y] /a[annfile.ANN]  - and add annotations\n"
		       "         HELPDECO helpfile[.hlp] /r [/y] [/n]    - decompile into lookalike RTF\n"
		       "         HELPDECO helpfile[.hlp] /c [/y]  - generate Win95 .CNT content file\n"
		       "         HELPDECO helpfile[.hlp] /l       - list entry points of this helpfile\n"
		       "         HELPDECO helpfile[.hlp] /e [/f]  - list references to other helpfiles\n"
		       "         HELPDECO helpfile[.hlp] /p [/f]  - check references to other helpfiles\n"
		       "         HELPDECO helpfile[.hlp] /d [/x]  - display internal directory\n"
		       "         HELPDECO helpfile[.hlp] \"internalfile\" [/x]    - display internal file\n"
		       "         HELPDECO helpfile[.hlp] \"internalfile\" filename - export internal file\n"
		       "options: /y overwrite without warning, /f list referencing topics, /x hex dump\n"
		       "         /g no guessing, /hprefix add known contextid prefix, /n no page breaks\n"
		       "To recreate all source files neccessary to rebuild a Windows helpfile, create\n"
		       "a directory, change to this directory and call HELPDECO with the path and name\n"
		       "of the helpfile to dissect. HELPDECO will extract all files contained in the\n"
		       "helpfile in two passes and deposit them in the current directory. You may then\n"
		       "rebuild the helpfile using the appropriate help compiler HC30, HC31, HCP, HCW,\n"
		       "HCRTF, MVC, WMVC or MVCC. The file will not be identical, but should look and\n"
		       "work like the original. This program is Freeware. Use at your own risk. No\n"
		       "part of it may be used commercially. No fees may be charged on distributing.\n"
#ifndef _WIN32
		       "Launch from Windows 95/Windows NT command line to handle larger helpfiles.\n"
#endif
		       ,sizeof(int)*8);
    }
    return 0;
}
