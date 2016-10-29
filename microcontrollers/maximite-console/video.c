// Based on Maximite code.
//
// These files are distributed on condition that the following copyright
// notice and credits are retained in any subsequent versions:
//
// Copyright 2011, 2012 Geoff Graham.
//
// Based on code by Lucio Di Jasio in his book "Programming 32-bit
// Microcontrollers in C - Exploring the PIC32".
//
// Non US keyboard support by Fabrice Muller.

#include <p32xxxx.h>
#include <plib.h>
#include <string.h>

#include "maximite.h"
#include "video.h"

// Video defines PINs.
#define P_VGA_COMP          PORTCbits.RC14          // VGA/Composite jumper
#define P_VGA_SELECT        1                       // State when VGA selected
#define P_VGA_COMP_PULLUP   CNPUEbits.CNPUE0

#define P_VIDEO_SPI         2                       // The SPI peripheral used for video
                                                    // Note: PIN G9 is automatically set as the framing input
#define P_SPI_INPUT         SPI2ABUF                // Input buffer for the SPI peripheral
#define P_SPI_INTERRUPT     _SPI2A_TX_IRQ           // Interrupt used by the video DMA
#define P_VID_OC_OPEN       OpenOC3                 // The function used to open the output compare
#define P_VID_OC_REG        OC3R                    // The output compare register

#define P_VIDEO             PORTGbits.RG8           // Video
#define P_VIDEO_TRIS        TRISGbits.TRISG8

#define P_HORIZ             PORTDbits.RD2           // Horizontal sync
#define P_HORIZ_TRIS        TRISDbits.TRISD2

#define P_VERT_SET_HI       LATFSET = (1 << 1)      // Set vert sync hi
#define P_VERT_SET_LO       LATFCLR = (1 << 1)      // Set vert sync lo
#define P_VERT_TRIS         TRISFbits.TRISF1

// Parameters for VGA video with 31.5KHz horizontal scanning and 60Hz vertical refresh.
// Graphics is 480x432 pixels. This gives us 80 chars per line and 36 lines per screen.
#define VGA_VRES        432                                         // Vert graphics resolution (pixels)
#define VGA_HRES        480                                         // Horiz graphics resolution (pixels)
#define VGA_LINE_N      525                                         // number of lines in VGA frame
#define VGA_LINE_T      2540                                        // Tpb clock in a line (31.777us)
#define VGA_VSYNC_N     2                                           // V sync lines
#define VGA_VBLANK_N    (VGA_LINE_N - VGA_VRES - VGA_VSYNC_N)       // Nbr of blank lines
#define VGA_PREEQ_N     ((VGA_VBLANK_N/2) - 12)                     // Nbr blank lines at the bottom of the screen
#define VGA_POSTEQ_N    VGA_VBLANK_N - VGA_PREEQ_N                  // Nbr blank lines at the top of the screen
#define VGA_PIX_T       4                                           // Tpb clock per pixel
#define VGA_HSYNC_T     300                                         // Tpb clock width horizontal pulse
#define VGA_BLANKPAD    5                                           // number of zero bytes before sending data

// Common paramaters for Composite video.
#define C_PIX_T         10                                          // Tpb clock per pixel
#define C_HSYNC_T       374                                         // Tpb clock width horizontal pulse
#define C_BLANKPAD      8                                           // number of zero words (4 bytes each) before sending data
#define C_VSYNC_N       3                                           // V sync lines

// Parameters for PAL composite video.
// Graphics is 304x216 pixels. This gives us 50 chars per line and 18 lines per screen.
#define PAL_VRES        216                                         // Vert graphics resolution (pixels)
#define PAL_HRES        304                                         // Horiz graphics resolution (pixels)
#define PAL_LINE_N      312                                         // Number of lines in PAL frames
#define PAL_LINE_T      5120                                        // Tpb clock in a line (64us) 5115 //
#define PAL_VBLANK_N    (PAL_LINE_N - PAL_VRES - C_VSYNC_N)         // Nbr of blank lines
#define PAL_PREEQ_N     ((PAL_VBLANK_N/2) - 8)                      // Nbr blank lines at the bottom of the screen
#define PAL_POSTEQ_N    (PAL_VBLANK_N - PAL_PREEQ_N)                // Nbr blank lines at the top of the screen

// Parameters for NTSC composite video.
// Graphics is 304x180 pixels. This gives us 50 chars per line and 15 lines per screen.
#define NTSC_VRES       180                                         // Vert graphics resolution (pixels)
#define NTSC_HRES       304                                         // Horiz graphics resolution (pixels)
#define NTSC_LINE_N     262                                         // Number of lines in NTSC frames
#define NTSC_LINE_T     5080                                        // Tpb clock in a line (63.55us)
#define NTSC_VBLANK_N   (NTSC_LINE_N - NTSC_VRES - C_VSYNC_N)       // Nbr of blank lines
#define NTSC_PREEQ_N    ((NTSC_VBLANK_N/2) - 8)                     // Nbr blank lines at the bottom of the screen
#define NTSC_POSTEQ_N   (NTSC_VBLANK_N - NTSC_PREEQ_N)              // Nbr blank lines at the top of the screen

// States of the vertical sync state machine.
#define SV_PREEQ    0                                               // Generating blank lines before the vert sync
#define SV_SYNC     1                                               // Generating the vert sync
#define SV_POSTEQ   2                                               // Generating blank lines after the vert sync
#define SV_LINE     3                                               // Visible lines, send the video data out

static int vert_resolution, horiz_resolution;                       // Global vert and horiz resolution in pixels on the screen

// Define the video buffer.
// Note that this can differ from the pixel resolution, for example for
// composite horiz_resolution is not an even multiple of 32 where
// horiz_buf is.
static int vert_buf, horiz_buf;                                     // Global vert and horiz resolution of the video buffer

static int *video_memory = NULL;                                    // Image buffer. Contains the bitmap video image.

static int vga;                                                     // True if we are using the internal VGA video
static unsigned int SvSyncT;                                        // Used to determine the timing for the SV_SYNC state.

volatile static int vcount;                                         // Counter for the number of lines in a frame
volatile static int vstate;                                         // The state of the state machine

static int VS[4] = { SV_SYNC, SV_POSTEQ, SV_LINE, SV_PREEQ };       // The next state table
static int VC[4];                                                   // The next counter table (initialise in video_init() below)

const unsigned int pal_vga_option = 0x00000000;                     // Use to hold the PAL/VGA setting.

// Definitions related to setting NTSC and PAL options.
#define CONFIG_PAL        0b111
#define CONFIG_NTSC       0b010

// Define the blink rate for the cursor.
#define CURSOR_OFF       350
#define CURSOR_ON        650

volatile static unsigned int cursor_timer;

// Round up to the nearest page size.
#define PAGESIZE          256
#define PAGE_ROUND_UP(a)  (((a) + (PAGESIZE - 1)) & (~(PAGESIZE - 1)))

extern unsigned int _splim;

// These keep track of where the next char will be written.
static int pos_x, pos_y;               // Position in pixels.

// Cursor control variables.
static int cursor_shape = CURSOR_STANDARD;
static int auto_line_wrap = 1;         // Used to track if we want automatic line wrap.
static int print_pixel_mode = 0;       // Used to track the pixel mode when we are printing.

struct video_font_t {
    void *p;
    unsigned char width, height;
    unsigned char start, end;
};

#define NBR_FONTS    10

// The font table and the current height and width of the font.
static struct video_font_t font_table[NBR_FONTS];
static int font_nbr;
static int font_width;
static int font_height;

// Used to save the font drawing commands from having to constantly look up the font table.
static unsigned short *font_ptr_16;    // Pointer to a font using 16 bit words.
static int font_start;
static int font_end;
static int font_scale;
static int font_reverse;

// Dimensions of this font is 5 bits wide and 10 bits high with one blank
// line above, below and to the right. So overall its dimensions are
// 6 bits wide x 12 bits high.
static const unsigned short font_zero[95][6] ={
    { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },     // SPACE
    { 0x0000, 0x0000, 0x00BE, 0x0000, 0x0000, 0x0000 },     // !
    { 0x0000, 0x0006, 0x0000, 0x0006, 0x0000, 0x0000 },     // "
    { 0x0028, 0x007C, 0x0028, 0x007C, 0x0028, 0x0000 },     // #
    { 0x0048, 0x0054, 0x00FE, 0x0054, 0x0024, 0x0000 },     // $
    { 0x0086, 0x0066, 0x0010, 0x00CC, 0x00C2, 0x0000 },     // %
    { 0x006C, 0x0092, 0x00AA, 0x0044, 0x00A0, 0x0000 },     // &
    { 0x0000, 0x000A, 0x0006, 0x0000, 0x0000, 0x0000 },     // '
    { 0x0000, 0x0038, 0x0044, 0x0082, 0x0000, 0x0000 },     // (
    { 0x0000, 0x0082, 0x0044, 0x0038, 0x0000, 0x0000 },     // )
    { 0x0028, 0x0010, 0x007C, 0x0010, 0x0028, 0x0000 },     // *
    { 0x0010, 0x0010, 0x007C, 0x0010, 0x0010, 0x0000 },     // +
    { 0x0000, 0x0280, 0x0180, 0x0000, 0x0000, 0x0000 },     // ,
    { 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0000 },     // -
    { 0x0000, 0x00C0, 0x00C0, 0x0000, 0x0000, 0x0000 },     // .
    { 0x0040, 0x0020, 0x0010, 0x0008, 0x0004, 0x0000 },     // /
    { 0x007C, 0x00A2, 0x0092, 0x008A, 0x007C, 0x0000 },     // 0
    { 0x0008, 0x0004, 0x00FE, 0x0000, 0x0000, 0x0000 },     // 1
    { 0x0084, 0x00C2, 0x00A2, 0x0092, 0x008C, 0x0000 },     // 2
    { 0x0044, 0x0082, 0x0092, 0x0092, 0x006C, 0x0000 },     // 3
    { 0x0030, 0x0028, 0x0024, 0x00FE, 0x0020, 0x0000 },     // 4
    { 0x004E, 0x008A, 0x008A, 0x008A, 0x0072, 0x0000 },     // 5
    { 0x007C, 0x0092, 0x0092, 0x0092, 0x0064, 0x0000 },     // 6
    { 0x0002, 0x0002, 0x00E2, 0x0012, 0x000E, 0x0000 },     // 7
    { 0x006C, 0x0092, 0x0092, 0x0092, 0x006C, 0x0000 },     // 8
    { 0x004C, 0x0092, 0x0092, 0x0092, 0x007C, 0x0000 },     // 9
    { 0x0000, 0x006C, 0x006C, 0x0000, 0x0000, 0x0000 },     // :
    { 0x0000, 0x00AC, 0x006C, 0x0000, 0x0000, 0x0000 },     // ;
    { 0x0010, 0x0028, 0x0044, 0x0082, 0x0000, 0x0000 },     // <
    { 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0000 },     // =
    { 0x0000, 0x0082, 0x0044, 0x0028, 0x0010, 0x0000 },     // >
    { 0x0004, 0x0002, 0x00A2, 0x0012, 0x000C, 0x0000 },     // ?
    { 0x007C, 0x0082, 0x00B2, 0x00AA, 0x00BC, 0x0000 },     // @
    { 0x00FC, 0x0012, 0x0012, 0x0012, 0x00FC, 0x0000 },     // A
    { 0x00FE, 0x0092, 0x0092, 0x0092, 0x006C, 0x0000 },     // B
    { 0x007C, 0x0082, 0x0082, 0x0082, 0x0044, 0x0000 },     // C
    { 0x00FE, 0x0082, 0x0082, 0x0082, 0x007C, 0x0000 },     // D
    { 0x00FE, 0x0092, 0x0092, 0x0092, 0x0082, 0x0000 },     // E
    { 0x00FE, 0x0012, 0x0012, 0x0012, 0x0002, 0x0000 },     // F
    { 0x007C, 0x0082, 0x0082, 0x0092, 0x0074, 0x0000 },     // G
    { 0x00FE, 0x0010, 0x0010, 0x0010, 0x00FE, 0x0000 },     // H
    { 0x0000, 0x0082, 0x00FE, 0x0082, 0x0000, 0x0000 },     // I
    { 0x0060, 0x0080, 0x0080, 0x0080, 0x007E, 0x0000 },     // J
    { 0x00FE, 0x0010, 0x0028, 0x0044, 0x0082, 0x0000 },     // K
    { 0x00FE, 0x0080, 0x0080, 0x0080, 0x0080, 0x0000 },     // L
    { 0x00FE, 0x0004, 0x0018, 0x0004, 0x00FE, 0x0000 },     // M
    { 0x00FE, 0x0004, 0x0008, 0x0010, 0x00FE, 0x0000 },     // N
    { 0x007C, 0x0082, 0x0082, 0x0082, 0x007C, 0x0000 },     // O
    { 0x00FE, 0x0012, 0x0012, 0x0012, 0x000C, 0x0000 },     // P
    { 0x003C, 0x0042, 0x0042, 0x0042, 0x00BC, 0x0000 },     // Q
    { 0x00FE, 0x0012, 0x0012, 0x0012, 0x00EC, 0x0000 },     // R
    { 0x004C, 0x0092, 0x0092, 0x0092, 0x0064, 0x0000 },     // S
    { 0x0002, 0x0002, 0x00FE, 0x0002, 0x0002, 0x0000 },     // T
    { 0x007E, 0x0080, 0x0080, 0x0080, 0x007E, 0x0000 },     // U
    { 0x003E, 0x0040, 0x0080, 0x0040, 0x003E, 0x0000 },     // V
    { 0x00FE, 0x0040, 0x0020, 0x0040, 0x00FE, 0x0000 },     // W
    { 0x0082, 0x0044, 0x0038, 0x0044, 0x0082, 0x0000 },     // X
    { 0x000E, 0x0010, 0x00E0, 0x0010, 0x000E, 0x0000 },     // Y
    { 0x00C2, 0x00A2, 0x0092, 0x008A, 0x0086, 0x0000 },     // Z
    { 0x0000, 0x00FE, 0x0082, 0x0000, 0x0000, 0x0000 },     // [
    { 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0000 },     // backslash
    { 0x0000, 0x0000, 0x0082, 0x00FE, 0x0000, 0x0000 },     // ]
    { 0x0008, 0x0004, 0x0002, 0x0004, 0x0008, 0x0000 },     // ^
    { 0x0080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0000 },     // _
    { 0x0000, 0x0002, 0x0004, 0x0008, 0x0000, 0x0000 },     // `
    { 0x0040, 0x00A8, 0x00A8, 0x00A8, 0x00F0, 0x0000 },     // a
    { 0x00FE, 0x0088, 0x0088, 0x0088, 0x0070, 0x0000 },     // b
    { 0x0070, 0x0088, 0x0088, 0x0088, 0x0088, 0x0000 },     // c
    { 0x0070, 0x0088, 0x0088, 0x0088, 0x00FE, 0x0000 },     // d
    { 0x0070, 0x00A8, 0x00A8, 0x00A8, 0x0030, 0x0000 },     // e
    { 0x0008, 0x0008, 0x00FC, 0x000A, 0x000A, 0x0000 },     // f
    { 0x0070, 0x0488, 0x0488, 0x0488, 0x03F0, 0x0000 },     // g
    { 0x00FE, 0x0010, 0x0008, 0x0008, 0x00F0, 0x0000 },     // h
    { 0x0000, 0x0088, 0x00FA, 0x0080, 0x0000, 0x0000 },     // i
    { 0x0200, 0x0400, 0x0408, 0x03FA, 0x0000, 0x0000 },     // j
    { 0x00FE, 0x0020, 0x0050, 0x0088, 0x0000, 0x0000 },     // k
    { 0x0000, 0x0082, 0x00FE, 0x0080, 0x0000, 0x0000 },     // l
    { 0x00F8, 0x0008, 0x00F0, 0x0008, 0x00F0, 0x0000 },     // m
    { 0x00F8, 0x0010, 0x0008, 0x0008, 0x00F0, 0x0000 },     // n
    { 0x0070, 0x0088, 0x0088, 0x0088, 0x0070, 0x0000 },     // o
    { 0x07F0, 0x0088, 0x0088, 0x0088, 0x0070, 0x0000 },     // p
    { 0x0070, 0x0088, 0x0088, 0x0088, 0x07F0, 0x0000 },     // q
    { 0x0000, 0x00F8, 0x0010, 0x0008, 0x0008, 0x0000 },     // r
    { 0x0090, 0x00A8, 0x00A8, 0x00A8, 0x0040, 0x0000 },     // s
    { 0x0008, 0x0008, 0x007E, 0x0088, 0x0088, 0x0000 },     // t
    { 0x0078, 0x0080, 0x0080, 0x0040, 0x00F8, 0x0000 },     // u
    { 0x0038, 0x0040, 0x0080, 0x0040, 0x0038, 0x0000 },     // v
    { 0x0078, 0x0080, 0x0060, 0x0080, 0x0078, 0x0000 },     // w
    { 0x0088, 0x0050, 0x0020, 0x0050, 0x0088, 0x0000 },     // x
    { 0x0078, 0x0480, 0x0480, 0x0480, 0x03F8, 0x0000 },     // y
    { 0x0088, 0x00C8, 0x00A8, 0x0098, 0x0088, 0x0000 },     // z
    { 0x0000, 0x0010, 0x006C, 0x0082, 0x0082, 0x0000 },     // {
    { 0x0000, 0x0000, 0x00FE, 0x0000, 0x0000, 0x0000 },     // |
    { 0x0082, 0x0082, 0x006C, 0x0010, 0x0000, 0x0000 },     // }
    { 0x0004, 0x0002, 0x0004, 0x0008, 0x0004, 0x0000 }      // ~
};

static void init_cursor_timer(void);
static void video_font_init(void);

// Initialise the video components.
void video_init(void) {
    video_font_init();     // Initialise the font table.

    // Setup the I/O pins used by the video.
#if !defined(DUINOMITE)                          // Maximite & UBW32
    CNCONbits.ON = 1;                            // Turn on Change Notification module.
    P_VGA_COMP_PULLUP = 1;                       // Turn on the pullup for pin C14 also called CN0.
#endif

    P_VIDEO_TRIS = 0;                            // Video output
    P_HORIZ_TRIS = 0;                            // Horiz sync output
    P_VERT_TRIS = 0;                             // Vert sync output used by VGA

    vga = (P_VGA_COMP == P_VGA_SELECT);          // VGA will be true if the jumper is NOT there.

    if (vga) {
        vert_buf = vert_resolution = VGA_VRES;
        horiz_resolution = VGA_HRES;
        horiz_buf = ((VGA_HRES + 31) / 32) * 32;
        VC[0] = VGA_VSYNC_N;                     // Setup the table used to count lines.
        VC[1] = VGA_POSTEQ_N;
        VC[2] = VGA_VRES;
        VC[3] = VGA_PREEQ_N;
        // Enable the SPI channel which will clock the video data out.
        // Set master and framing mode. The last argument sets the speed.
        SpiChnOpen(P_VIDEO_SPI, SPICON_ON | SPICON_MSTEN | SPICON_MODE32 |
                                SPICON_FRMEN | SPICON_FRMSYNC | SPICON_FRMPOL,
                   VGA_PIX_T);
        // Enable the output compare which is used to time the width of the
        // horiz sync pulse.
        P_VID_OC_OPEN(OC_ON | OC_TIMER3_SRC | OC_CONTINUE_PULSE, 0, VGA_HSYNC_T);
        // Enable timer 3 and set to the horizontal scanning frequency.
        OpenTimer3(T3_ON | T3_PS_1_1 | T3_SOURCE_INT, VGA_LINE_T-1);
    } else if (pal_vga_option == CONFIG_PAL) {
        // This is for the PAL composite output and is the same as VGA with
        // timing differences.
        vert_buf = vert_resolution = PAL_VRES;
        horiz_resolution = PAL_HRES;
        horiz_buf = (((PAL_HRES + 31) / 32) * 32);
        SvSyncT = PAL_LINE_T - C_HSYNC_T;
        VC[0] = C_VSYNC_N;
        VC[1] = PAL_POSTEQ_N;
        VC[2] = PAL_VRES;
        VC[3] = PAL_PREEQ_N;
        SpiChnOpen(P_VIDEO_SPI, SPICON_ON | SPICON_MSTEN | SPICON_MODE32 |
                                SPICON_FRMEN | SPICON_FRMSYNC | SPICON_FRMPOL,
                   C_PIX_T);
        P_VID_OC_OPEN(OC_ON | OC_TIMER3_SRC | OC_CONTINUE_PULSE, 0, C_HSYNC_T);
        OpenTimer3(T3_ON | T3_PS_1_1 | T3_SOURCE_INT, PAL_LINE_T-1);
    } else {
        // This is for the NTSC composite output and is similar again.
        vert_buf = vert_resolution = NTSC_VRES;
        horiz_resolution = NTSC_HRES;
        horiz_buf = ((PAL_HRES + 31) / 32) * 32;
        SvSyncT = NTSC_LINE_T - C_HSYNC_T;
        VC[0] = C_VSYNC_N;
        VC[1] = NTSC_POSTEQ_N;
        VC[2] = NTSC_VRES;
        VC[3] = NTSC_PREEQ_N;
        SpiChnOpen(P_VIDEO_SPI, SPICON_ON | SPICON_MSTEN | SPICON_MODE32 |
                                SPICON_FRMEN | SPICON_FRMSYNC | SPICON_FRMPOL,
                   C_PIX_T);
        P_VID_OC_OPEN(OC_ON | OC_TIMER3_SRC | OC_CONTINUE_PULSE, 0, C_HSYNC_T);
        OpenTimer3(T3_ON | T3_PS_1_1 | T3_SOURCE_INT, NTSC_LINE_T-1);
    }

    video_memory = (int *)(PAGE_ROUND_UP((unsigned int)&_splim));

    vstate = SV_PREEQ;     // Initialise the state machine.
    vcount = 1;            // Set the count so that the first interrupt will increment the state.

    // Setup DMA 1 to send data to SPI channel 2.
    DmaChnOpen(1, 1, DMA_OPEN_DEFAULT);
    DmaChnSetEventControl(1, DMA_EV_START_IRQ_EN | DMA_EV_START_IRQ(P_SPI_INTERRUPT));

    if (vga)
        DmaChnSetTxfer(1, (void *)video_memory, (void *)&P_SPI_INPUT, horiz_buf/8, 4, 4);
    else {
        const static int zero[] = {0, 0, 0};
        DmaChnSetTxfer(1, (void *)zero, (void *)&P_SPI_INPUT, C_BLANKPAD, 4, 4);
        // Setup DMA 0 to pump the data from the graphics buffer to the SPI peripheral.
        DmaChnOpen( 0, 0, DMA_OPEN_DEFAULT);
        DmaChnSetEventControl(0, DMA_EV_START_IRQ_EN | DMA_EV_START_IRQ(P_SPI_INTERRUPT));
        DmaChnSetTxfer(0, (void *)video_memory, (void *)&P_SPI_INPUT, horiz_buf/8 + 6, 4, 4);
        // Chain DMA 0 so that it will start on completion of the DMA 1 transfer.
        DmaChnSetControlFlags(0, DMA_CTL_CHAIN_EN | DMA_CTL_CHAIN_DIR);
    }

    mT3SetIntPriority(7);   // Set priority level 7 for the timer 3 interrupt to use shadow register set.
    mT3IntEnable(1);        // Enable Timer3 Interrupt.

    init_cursor_timer();
}

// Timer 3 interrupt.
// Used to generate the horiz and vert sync pulse under control of the state machine.
void __ISR(_TIMER_3_VECTOR, ipl7) T3Interrupt(void) {
    static int *vptr;

    switch (vstate) {                                               // Vertical state machine.
        case SV_PREEQ:  // 0
            vptr = video_memory;                                    // Prepare for the new frame.
            break;

        case SV_SYNC:   // 1
            if (!vga) P_VID_OC_REG = SvSyncT;                       // Start the vertical sync pulse for composite.
            P_VERT_SET_LO;                                          // Start the vertical sync pulse for VGA.
            break;

        case SV_POSTEQ: // 2
            if (!vga) P_VID_OC_REG = C_HSYNC_T;                     // End of the vertical sync pulse for composite.
            P_VERT_SET_HI;                                          // End of the vertical sync pulse for VGA.
            break;

        case SV_LINE:   // 3
            P_SPI_INPUT = 0;                                        // Preload the SPI with 4 zero bytes to pad the start of the video.
            if (vga)
                DCH1SSA = KVA_TO_PA((void*) (vptr));                // Update the DMA1 source address (DMA1 is used for VGA data).
            else
                DCH0SSA = KVA_TO_PA((void*) (vptr - 1));            // Update the DMA0 source address (DMA0 is used for composite data).
            vptr += horiz_buf/32;                                   // Move the pointer to the start of the next line.
            DmaChnEnable(1);                                        // Arm the DMA transfer.
            break;
   }

    if (--vcount == 0) {                                            // Count down the number of lines.
        vcount = VC[vstate & 3];                                    // Set the new count.
        vstate = VS[vstate & 3];                                    // And advance the state machine.
    }

    mT3ClearIntFlag();                                              // Clear the interrupt flag
}

void init_cursor_timer(void) {
    // Setup timer 4.
    PR4 = 1 * ((BUSFREQ/2)/1000) - 1;
    T4CON = 0x8010;
    mT4SetIntPriority(1);
    mT4ClearIntFlag();
    mT4IntEnable(1);
}

// Timer 4 interrupt processor. This fires every millisecond.
void __ISR( _TIMER_4_VECTOR, ipl1) T4Interrupt(void) {
    if(++cursor_timer > CURSOR_OFF + CURSOR_ON) cursor_timer = 0;
    // Clear the interrupt flag.
    mT4ClearIntFlag();
}

// Clear the screen.
void video_clear_screen(void) {
    memset(video_memory, 0, vert_buf*(horiz_buf/8));

    pos_x = 0;
    pos_y = 0;
}

// Turn on or off a single pixel in the graphics buffer.
static void video_draw_pixel(int x, int y, int b) {
    int offset, value;
    if (x < 0 || x >= horiz_resolution || y < 0 || y >= vert_resolution || !horiz_buf)
        return;

    offset = y * (horiz_buf/32) + x/32;
    value = 0x80000000 >> (x & 0x1f);

    if (b == 0)
        video_memory[offset] &= ~value;        // Turn off the pixel
    else if(b == -1)
        video_memory[offset] ^= value;         // Invert the pixel. Thanks to Alan Williams for the contribution.
    else
        video_memory[offset] |= value;         // Turn on the pixel.
}

// Write a char on the video using the graphic bit font.
// x and y are in pixels
static void video_draw_char(int x, int y, int c, int mode) {
    const unsigned short* offset = font_ptr_16 + (c - font_start) * font_width;
    int j;

    // This loop only supports fonts with the height <= 16.
    for (j = 0; j < font_width; j++) {                            // Loop through each word (ie, horizontal scan line).
        int k;
        const int mask = *(offset + j);
        for (k = 0; k < font_height; k++) {                       // Loop through the bits in the word (ie, vertical pixels).
            int h;
            int t = mask & (1 << k);
            if (font_reverse) t = !t;
            if (mode) {
                if (mode & 0b010) t = !t;
                if ((mode & 0b001) && t == 0) continue;
                if (mode & 0b100) t = -1;
            }
            for (h = 0; h < font_scale; h++) {
                int w;
                for (w = 0; w < font_scale; w++)
                    video_draw_pixel(x + (j * font_scale) + w, y + (k * font_scale) + h, t);  // Draw the pixel.
            }
        }
    }
}

// Draw a line on a the video output
//    (x1, y1) - the start coordinate
//    (x2, y2) - the end coordinate
//    colour - zero for erase, non zero to draw

#define abs(a) (((a) > 0) ? (a) : -(a))

static void video_draw_line(int x1, int y1, int x2, int y2, int colour) {
   const int addx = x1 > x2 ? -1 : 1;
   const int addy = y1 > y2 ? -1 : 1;
   const int dx = abs(x2 - x1);
   const int dy = abs(y2 - y1);
   int x = x1, y = y1;

   if (dx >= dy) {
      int p = 2*dy - dx;
      int i;
      for (i = 0; i <= dx; ++i) {
         video_draw_pixel(x, y, colour);
         if (p < 0) {
            p += 2*dy;
            x += addx;
         } else {
            p += 2*dy - 2*dx;
            x += addx;
            y += addy;
         }
      }
   } else {
      int p = 2*dx - dy;
      int i;
      for (i = 0; i <= dy; ++i) {
         video_draw_pixel(x, y, colour);
         if (p < 0) {
            p += 2*dx;
            y += addy;
         } else {
            p += 2*dx - 2*dy;
            x += addx;
            y += addy;
         }
      }
   }
}

// Used to define the current font.
static void video_set_font(int font, int scale, int reverse) {
    font_ptr_16 = (unsigned short *)font_table[font].p;
    font_height = font_table[font].height;
    font_width = font_table[font].width;
    font_start = font_table[font].start;
    font_end = font_table[font].end;
    font_scale = scale;
    font_reverse = reverse;
    font_nbr = font;
}

// Setup the font table.
void video_font_init(void) {
    font_table[0].p = font_zero;
    font_table[0].width = 6;
    font_table[0].height = 12;
    font_table[0].start = ' ';
    font_table[0].end = '~';
    video_set_font(0, 1, 0);
}

// Put a char onto the video screen.
void video_put_char(char c) {
    if (auto_line_wrap) {
        if (pos_x < 0) pos_x = 0;
        if (pos_y < 0) pos_y = 0;
        if (pos_y > vert_resolution) pos_y = vert_resolution;
    }

    if(c >= font_start && c <= font_end) {
        // If it is a printable character check if we need to scroll at the end
        // of the line.
        if (pos_x + (font_width * font_scale) > horiz_resolution && auto_line_wrap) {
            // If we are at the end of a line wrap to the next.
            pos_x = 0;
            pos_y += (font_height * font_scale);
        }
    } else {
        // If it is not a printable char we need to check for control characters.

        // For a return set the horizontal position to zero
        if (c == '\r') pos_x = 0;

        // For a line feed add the char height to the vert position.
        if (c == '\n') pos_y += (font_height * font_scale);

        // For a tab move the horiz position to the next tab spot.
        if (c == '\t')
            pos_x = ((pos_x + ((font_width * font_scale) << 3)) >> 3) << 3;

        // For a backspace move the horiz position back by a char width.
        if (c == '\b') {
            pos_x -= (font_width * font_scale);
            if (pos_x < 0) pos_x = 0;
        }
    }

    // if we are beyond the bottom of the screen scroll up the previous lines
    if (pos_y + (font_height * font_scale) > vert_resolution + 1 && auto_line_wrap) {
        int *to = video_memory;
        const int *from = to + (horiz_buf/32) * (font_height * font_scale);
        int i;

        // Scroll up.
        for (i = 0; i <(horiz_buf/32) * (vert_buf - (font_height * font_scale)); i++)
            *to++ = *from++;

        // Clear the new line.
        for (i = 0; i < (horiz_buf/32) * (font_height * font_scale); i++)
            *to++ = 0;
        pos_y -= (font_height * font_scale);
    }

    // Now we are ready to draw the character.
    if (c >= font_start && c <= font_end) {
        video_draw_char(pos_x, pos_y, c, print_pixel_mode);
        pos_x += (font_width * font_scale);                          // And update our position.
    }
}

// Control the cursor.
void video_show_cursor(int show) {
    static int visible = 0;
    static int last_shape = CURSOR_HIDDEN;
    int shape = CURSOR_HIDDEN;
    int x;

    if (cursor_timer > CURSOR_ON) show = 0;
    if (!visible && !show) return;                                   // Not showing and not required so skip the rest.
    if (visible && show && last_shape == cursor_shape) return;       // Cursor is on and the correct type so skip the rest.

    if (visible && !show)
        shape = last_shape;                   // This will turn the cursor off.
    else if(!visible && show)
        shape = last_shape = cursor_shape;    // This will turn it on with the current cursor.
    else if (last_shape != cursor_shape)
        shape = last_shape;                   // This will turn it off ready for the next entry where it will turn on with the new cursor.

    x = (pos_x != 0);

    if (shape == CURSOR_STANDARD)
        video_draw_line(pos_x - 1, pos_y + 9, pos_x + 5, pos_y + 9, -1); // Invert the underline.
    else
        video_draw_line(pos_x - x, pos_y + 10, pos_x - x, pos_y, -1);    // Invert the vert bar.

    visible = !visible;
}

void video_display_string(const char *p) {
    while (*p) video_put_char(*p++);
}
