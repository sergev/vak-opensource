/* ********************************************************************
 Program	: yack.h
 Author		: Jan Lategahn DK3LJ
 Purpose	: definition of keyer hardware
 Created	: 15.10.2010
 Update		: 16.11.2011
 Version	: 0.7
 
 Changelog
 ---------
 Version		Date		Change
 ----------------------------------------------------------------------
 
 
 Todo
 ----
 
 
 *********************************************************************/

// User configurable settings

// The following settings define the hardware connections to the keyer chip

// Definition of where the keyer itself is connected 
#define		KEYDDR			DDRB
#define		KEYPORT			PORTB
#define		KEYINP			PINB
#define		DITPIN			3
#define		DAHPIN			4

// Definition of where the transceiver keyer line is connected
#define		OUTDDR			DDRB
#define		OUTPORT			PORTB
#define		OUTPIN			0

// Definition of where the sidetone output is connected (beware,
// this is chip dependent and can not just be changed at will)
#define		STDDR			DDRB
#define		STPORT			PORTB
#define		STPIN			1

// Definition of where the control button is connected
#define		BTNDDR			DDRB
#define		BTNPORT			PORTB
#define		BTNINP			PINB
#define		BTNPIN			2


// The following defines the meaning of status bits in the yackflags and volflags 
// global variables

// Definition of the yackflags variable. These settings get stored in EEPROM when changed.
#define		NOTUSED1		0b00000001  // Available
#define     CONFLOCK		0b00000010  // Configuration locked down
#define		MODE			0b00001100  // 2 bits to define keyer mode (see next section)
#define		SIDETONE		0b00010000  // Set if the chip must produce a sidetone
#define		TXKEY			0b00100000  // Set if the chip keys the transmitter
#define		TXINV			0b01000000  // Set if TX key line is active low
#define		PDLSWAP			0b10000000  // Set if DIT and DAH are swapped

#define		IAMBICA			0b00000000  // IAMBIC A mode
#define		IAMBICB			0b00000100  // IAMBIC B mode (default)
#define		ULTIMATIC		0b00001000  // Ultimatic Mode
#define		DAHPRIO			0b00001100  // Always give DAH priority

#define		FLAGDEFAULT		IAMBICB | TXKEY | SIDETONE

// Definition of volflags variable. These flags do not get stored in EEPROM.
#define		DITLATCH		0b00000001  // Set if DIT contact was closed
#define     DAHLATCH		0b00000010  // Set if DAH contact was closed
#define     SQUEEZED        0b00000011  // DIT and DAH = squeezed
#define		DIRTYFLAG		0b00000100  // Set if cfg data was changed and needs storing
#define     CKLATCH         0b00001000  // Set if the command key was pressed at some point
#define		VSCOPY          0b00110000  // Copies of Sidetone and TX flags from yackflags


// The following defines timing constants. In the default version the keyer is set to operate in
// 10ms heartbeat intervals. If a higher resolution is required, this can be changed to a faster
// beat

// YACK heartbeat frequency (in ms)
#define		YACKBEAT		5
#define		YACKSECS(n)		(n*(1000/YACKBEAT)) // Beats in n seconds
#define		YACKMS(n)		(n/YACKBEAT) // Beats in n milliseconds

// Power save mode
#define     POWERSAVE       // Comment this line if no power save mode required
#define     PSTIME          30 // 30 seconds until automatic powerdown
#define     PWRWAKE         ((1<<PCINT3) | (1<<PCINT4) | (1<<PCINT2)) // Dit, Dah or Command wakes us up..

// These values limit the speed that the keyer can be set to
#define		MAXWPM			50  
#define		MINWPM			5
#define		DEFWPM			12

// Farnsworth parameters
#define     FARNSWORTH      1
#define     WPMSPEED        0
#define     MAXFARN         255

#define		WPMCALC(n)		((1200/YACKBEAT)/n) // Calculates number of beats in a dot 

#define		DITLEN			1	// Length of a dot
#define		DAHLEN			3	// Length of a dash
#define		IEGLEN			1	// Length of inter-element gap
#define		ICGLEN			3	// Length of inter-character gap
#define		IWGLEN			7	// Length of inter-word gap

// Duration of various internal timings in seconds
#define		TUNEDURATION	20  // Duration of tuning keydown (in seconds)
#define     DEFTIMEOUT      5   // Default timeout 5 seconds
#define     MACTIMEOUT      15  // Timeout after playing back a macro

// The following defines various parameters in relation to the pitch of the sidetone

// CTC mode prescaler.. If changing this, ensure that ctc config
// is adapted accordingly
#define		PRESCALE		8
#define		CTCVAL(n)		((F_CPU/n/2/PRESCALE)-1) // Defines how to compute CTC setting for
                                                     // a given frequency

// Default sidetone frequency
#define		DEFFREQ			800     // Default sidetone frequency
#define		MAXFREQ			1500    // Maximum frequency
#define		MINFREQ			400     // Minimum frequenc

#define		MAXCTC			CTCVAL(MAXFREQ) // CTC values for the above three values
#define		MINCTC			CTCVAL(MINFREQ) 
#define		DEFCTC			CTCVAL(DEFFREQ)

// The following are various definitions in use throughout the program
#define		RBSIZE			100     // Size of each of the two EEPROM buffers

#define		MAGPAT			0xA5    // If this number is found in EEPROM, content assumed valid

#define		DIT				1
#define		DAH             2

#define		UP				1
#define		DOWN			2

#define		ON				1
#define		OFF				0

#define		RECORD			1
#define		PLAY			2

#define		READ			1
#define		WRITE			2

#define		TRUE            1
#define		FALSE           0


// Generic functionality
#define 	SETBIT(ADDRESS,BIT)     (ADDRESS |= (1<<BIT))
#define 	CLEARBIT(ADDRESS,BIT)   (ADDRESS &= ~(1<<BIT))

typedef		uint8_t         byte;
typedef		uint16_t        word;


// Forward declarations of public functions
void        yackinit (void);
void        yackchar(char c);
void        yackstring(const char *p);
char        yackiambic(byte ctrl);
void        yackpitch (uint8_t dir);
void        yacktune (void);
void        yackmode (uint8_t mode);
void        yackinhibit (uint8_t mode);
void        yackerror (void);
void        yacktoggle(byte flag);
byte        yackflag(byte flag);
void        yackbeat(void);
void        yackmessage(byte function, byte msgnr);
void        yacksave (void);
byte        yackctrlkey(byte mode);
void        yackreset (void);
word        yackuser (byte func, byte nr, word content);
void        yacknumber(word n);
word        yackwpm(void);
void        yackplay(byte i);
void        yackdelay(byte n);
void        yackfarns(void);
void        yackspeed (byte dir, byte mode);

#ifdef POWERSAVE
void        yackpower(byte n);
#endif














