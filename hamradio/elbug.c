/* serial elbug = electronic morse key. */
/* by Günther Montag dl4mge */

/* elbug is distributed alone and as part of the hf package. */
/* I plan too edit it also separate. */

/* Just compile me by 'gcc elbug.c -o elbug' */
/* and copy elbug to user/local/bin. */
/* Call me as root. */
/* How to cable your homebrew elbug? See below! */

/*
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 */

#include <stdio.h>
#include <sys/types.h>
#ifdef __linux__
#include <sys/io.h>
#define IOPERM ioperm
#endif
#ifdef __FreeBSD__ 
#include <machine/cpufunc.h>
#include <machine/sysarch.h>
#define IOPERM i386_set_ioperm
#endif
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/kd.h>		/* Linux, UnixWare */
/*
if kd.h not found, maybe you have to take one of these 2:
<sys/vtkd.h> for OpenServer
<sys/kbio.h> for FreeBSD
*/

#define PAUSE 0
#define DIT 1
#define DAH 2
#define WORDPAUSE 4
/* from cwdaemon/lib.c */
typedef struct {
const unsigned char character;	/* The character represented */
const char *representation;     /* Dot-dash shape of the character */
} cw_entry_t;

static const cw_entry_t cw_table[] = {
			/* ASCII 7bit letters */
	{ 'A', ".-"     }, { 'B', "-..."   }, { 'C', "-.-."   },
	{ 'D', "-.."    }, { 'E', "."      }, { 'F', "..-."   },
	{ 'G', "--."    }, { 'H', "...."   }, { 'I', ".."     },
	{ 'J', ".---"   }, { 'K', "-.-"    }, { 'L', ".-.."   },
	{ 'M', "--"     }, { 'N', "-."     }, { 'O', "---"    },
	{ 'P', ".--."   }, { 'Q', "--.-"   }, { 'R', ".-."    },
	{ 'S', "..."    }, { 'T', "-"      }, { 'U', "..-"    },
	{ 'V', "...-"   }, { 'W', ".--"    }, { 'X', "-..-"   },
	{ 'Y', "-.--"   }, { 'Z', "--.."   },
			/* Numerals */
	{ '0', "-----"  }, { '1', ".----"  }, { '2', "..---"  },
	{ '3', "...--"  }, { '4', "....-"  }, { '5', "....."  },
	{ '6', "-...."  }, { '7', "--..."  }, { '8', "---.."  },
	{ '9', "----."  },
			/* Punctuation */
	{ '"', ".-..-." }, { '\'',".----." }, { '$', "...-..-"},
	{ '(', "-.--."  }, { ')', "-.--.-" }, { '+', ".-.-."  },
	{ ',', "--..--" }, { '-', "-....-" }, { '.', ".-.-.-" },
	{ '/', "-..-."  }, { ':', "---..." }, { ';', "-.-.-." },
	{ '=', "-...-"  }, { '?', "..--.." }, { '_', "..--.-" },
	{ '@', ".--.-." },
			/* Cwdaemon special characters */
	{ '<', "...-.-" }, { '>', "-...-.-"}, { '!', "...-." }, 
	{ '&', ".-..."  }, { '*', ".-.-."  },
			/* ISO 8859-1 accented characters */
	{ 0334,"..--"   },	/* U with diaresis */
	{ 0304,".-.-"   },	/* A with diaeresis */
	{ 0307,"-.-.."  },	/* C with cedilla */
	{ 0326,"---."   },	/* O with diaresis */
	{ 0311,"..-.."  },	/* E with acute */
	{ 0310,".-..-"  },	/* E with grave */
	{ 0300,".--.-"  },	/* A with grave */
	{ 0321,"--.--"  },	/* N with tilde */
			/* ISO 8859-2 accented characters */
	{ 0252,"----"   },	/* S with cedilla */
	{ 0256,"--..-"  },	/* Z with dot above */
			/* Sentinel end of table value */
	{ '\0', NULL } };

char *cable = "\n"
"* * * elbug - electronic morse key by dl4mge. How to cable: * * *\n"
	"*  Middle pad: +9V battery via Resistor 2 k                     *\n"                           
	"*  Left  contact: -> DCD (9-pin plug: 1) (25-pin plug: 8)       *\n"
	"*  Right contact: -> CTS (9-pin plug: 8) (25-pin plug: 5)       *\n"
	"*  Ground: -pole battery (9-pin plug: 5) (25-pin plug: 7)       *\n"
	"* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n" ;

/* default parameters */
int pin = TIOCM_RTS;
char* name_ptt = "/dev/ttyS0";
char* name_spkr = "/dev/tty1";
int port = 0x3f8;
int invert_ptt = 0;
int spkr = 0;
unsigned int wpm = 12, tone = 440;
int argp;
int fd_ptt, fd_spkr;
int dotus;

void decode(int sign) 
{
	static char morse[16];
	int i;
	static int loop = 0, pause = 0, pausecount = 0, bit = 0;
	unsigned char element;

	if(!loop) {
		loop = sizeof(cw_table) / sizeof(cw_entry_t);
		printf("decode function: %d signs in table.\n", loop);
	}
	
	if (sign == DIT) element = '.';
	if (sign == DAH) element = '-';

	if (sign) {
	    pause = 0;
	    pausecount = 0;
	    morse[bit] = element;
	    bit++;
	} else {
	    if (pause) return;
	    if (!pausecount) { /* compare with morse table */
	    	morse[bit] = '\0';
	    	bit = 0;
	    	for (i = 0; i < loop - 1; i++) {
	            //printf ("comparing sign %d...\n", i);
	            if(!strcmp (cw_table[i].representation, 	morse)) {
		    	printf("%c", cw_table[i].character);
		    	fflush(stdout);
	       	    	break;
		    }	       	
	   	    if (i == loop - 2) {
	       	    	printf(" ?? ");
		    	fflush(stdout);
	       	    }
	       	}
	    } 
	    pausecount++;
	    if (pausecount > WORDPAUSE) {
	        printf(" ");
	        fflush(stdout);
	        pause = 1;
	    }
	}
}

void wait(int us) {
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = us;
    select (0, NULL, NULL, NULL, &tv);
}

#ifdef KIOCSOUND
/* from cwlib, simplified */
console_open ()
{
	/* Open the console device file, for write only. */
	fd_spkr = open (name_spkr, O_WRONLY);
	if (fd_spkr == -1)
	    {
		printf ("console speaker can not be opened\n");
		return 0;
	    }
	
	/* Check to see if the file can do console tones. */
	if (ioctl (fd_spkr, KIOCSOUND, 0) == -1)
	    {
		close (fd_spkr);
		printf ("console speaker can not speak\n");
		return 0;
	    }

	printf ("console speaker opened.\n");
	return 1;
}
#endif

void output_elbug_serial(int ptt)
{
	int status;
	
	if (fd_ptt < 0) {
	    printf("no serial port open for output.\n");
	    exit (1);
	}

	/* tone */
	if (ptt) {
		if (spkr) ioctl(fd_spkr, KIOCSOUND, argp);
	} else {
		if (spkr) ioctl(fd_spkr, KIOCSOUND, 0);
	}
	
	/* serial output */
	
	/* seems this worked for intel only. */
	/*
	if (ioctl(fd_ptt, 
	    ((ptt && !invert_ptt) || (!ptt && invert_ptt)) ? 
	    TIOCMBIS : TIOCMBIC, &pin)) {
	    printf ("ioctl: TIOCMBI[CS]\n");
	    printf ( "serial port can not be accessed!\n");
	    exit (1);
	}
	*/
	
	if (invert_ptt) ptt = !ptt;
	if (ioctl(fd_ptt, TIOCMGET, &status)) {
	    printf ("ioctl: TIOCMGET, cannot read serial port.\n");
	    if (spkr) ioctl(fd_spkr, KIOCSOUND, 0);
	    exit (1);
	}
	if (ptt) {
		status &= pin;
	} else {
		status &= ~pin;
	}
	if (ioctl(fd_ptt, TIOCMSET, &status)) {
	    printf ("ioctl: TIOCMSET, cannot write to serial port.\n");
	    if (spkr) ioctl(fd_spkr, KIOCSOUND, 0);
	    exit (1);
	}
}

void elbug_send_dit() {
    output_elbug_serial(1);
    //printf(".");
    wait(dotus);
    output_elbug_serial(0);
    wait(dotus);
}

void elbug_send_dah() {
    output_elbug_serial(1);
    //printf("_");    
    wait(dotus * 3);
    output_elbug_serial(0);
    wait(dotus);
}

int main(int argc, char *argv[]) {
    int c, status, err = 0;
    static int idlewait = 0;
    unsigned short int dcd, cts;
    
    while ((c = getopt(argc, argv, "f:w:s:di")) != -1) {
	switch (c) {
	    case 'f':
		tone = strtoul(optarg, NULL, 0);
		if (tone && (tone < 50 || tone > 5000)) {
		    printf("tone %d out of range 0 or 50...5000 Hz\n", tone);
    		    err++;
		}
		break;
	    case 's':
		name_ptt = optarg;
		if (! strcmp (name_ptt, "/dev/ttyS0")) {
		    port = 0x03F8;
		    break;
		}
		if (! strcmp (name_ptt, "/dev/ttyS1")) {
		    port = 0x02F8;
		    break;
		}
		if (! strcmp (name_ptt, "/dev/ttyS2")) {
		    port = 0x03E8;
		    break;
		}
		if (! strcmp (name_ptt, "/dev/ttyS3")) {
		    port = 0x02E8;
		    break;
		}
		break;
    	    case 'd':
		pin = TIOCM_DTR;
    		break;
            case 'i':
	        invert_ptt = 1;
    	        break;
    	    case 'w':
		wpm = strtoul(optarg, NULL, 0);
		if (wpm < 3 || wpm > 90) {
		    printf("Speed %d out of range 3...90 wpm\n", wpm);
    		    err++;
		}
		break;
	    default:
        	err++;
        	break;
    	}
    	if (err) {
    	  printf("/* dl4mge's elbug.*/\n"
		"usage: elbug [-w <speed wpm>] [-s <ptt comm port>] [-d] [-i] \n"
			"  -w: speed in wpm              (default: 12 words per minute)\n"
			"  -f: tone frequency in Hz      (default: 440 Hz, enter 0 for silent mode)\n"
			"  -s: serial port for in/output (default: /dev/ttyS0)\n"
			"  -d: output through DTR pin    (default is RTS)\n"
			"  -i: invert PTT                (default: PTT = positive)\n"
			"%s", cable);
	exit (0);
	}
    }
    if ((err = IOPERM(port, 8, 1))) {
	printf("permission problem for serial port %04x: ioperm = %d\n", port, err);
	printf("This program has to be called with root permissions.\n");
    }
    if ((fd_ptt = open(name_ptt, O_RDWR, 0)) < 0) {
	printf("error in opening ptt device %s - maybe try another one?\n", 
	    name_ptt);
    }
    
    dotus = 1000000 / (wpm * 50 / 60);
    if (tone) argp = 1193180/tone; else argp = 0;
    /* from man console_ioctl */
    		
    printf("%s",cable);
    printf("\nelbug: %d wpm at %s, address %04x, %s %s\n",
	wpm, name_ptt, port,
	invert_ptt ? "inverted ptt output," : "",
	pin == TIOCM_DTR ? "DTR output" : "RTS output");
    printf("A dit will last %d ms.\n", dotus/1000);
    printf("See options by elbug -h. Stop me by <strg>c.\n");
    
    output_elbug_serial(0);
#ifdef KIOCSOUND
    if (argp) spkr = console_open();
#endif        
    
    /* main loop */
    for (;;) {
    	status = 0;
        ioctl(fd_ptt, TIOCMGET, &status);
	cts = status & TIOCM_CTS;
	dcd = status & TIOCM_CAR;
//	printf("cts: %d, dcd: %d\n", cts, dcd);
 	if (cts) {
		elbug_send_dit();
		decode(DIT);
	}
	if (dcd) {
		elbug_send_dah();
		decode(DAH);
	}
	wait(3000);
	idlewait += 3000;
	if (idlewait > dotus) {
		idlewait = 0;
		decode(PAUSE);
	}
    }
}
