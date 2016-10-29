/*
 * MRF24WG WiFi interrupt and SPI configuration file.
 * Specific to chipKIT WiFire board.
 */
#include "pic32mz.h"

#define BUS_FREQ            100000000   /* Peripheral clock is 100 MHz */
#define WF_SPI_FREQ         10000000    /* SPI clock 10 MHz */

/*
 * Use port SPI4.
 */
#define WF_SPI              4
#define WIFI_SPI_IRQ_E      PIC32_IRQ_SPI4E
#define WIFI_SPI_IRQ_TX     PIC32_IRQ_SPI4TX
#define WIFI_SPI_IRQ_RX     PIC32_IRQ_SPI4RX
#define WIFI_SPICON         SPI4CON
#define WIFI_SPICONSET      SPI4CONSET
#define WIFI_SPICONCLR      SPI4CONCLR
#define WIFI_SPISTAT        SPI4STAT
#define WIFI_SPIBUF         SPI4BUF
#define WIFI_SPIBRG         SPI4BRG

/*
 * WiFi interrupt pin INT4/RA15
 */
#define WF_INT              4           /* INT4 */
#define WIFI_INT_EDGE(v)    if (v) INTCONSET = 1 << WF_INT; \
                            else   INTCONCLR = 1 << WF_INT

#define WF_INT_PIN          15          /* RA15 */
#define WF_INT_INPUT(v)     if (v) TRISASET = 1 << WF_INT_PIN; \
                            else   TRISACLR = 1 << WF_INT_PIN
#define WF_INT_SET(v)       if (v) PORTASET = 1 << WF_INT_PIN; \
                            else   PORTACLR = 1 << WF_INT_PIN
#define WF_INT_VAL          ((PORTA >> WF_INT_PIN) & 1)

#define WIFI_INT_MASK       (1 << WF_INT)
#define WIFI_INT_IE         (IEC(0) & WIFI_INT_MASK)
#define WIFI_INT_IF         (IFS(0) & WIFI_INT_MASK)
#define WIFI_IECxCLR        IECCLR(0)
#define WIFI_IFSxCLR        IFSCLR(0)
#define WIFI_IECxSET        IECSET(0)
#define WIFI_IFSxSET        IFSSET(0)

/*
 * WiFi hibernate signal at pin RG1
 */
#define WF_HIBERNATE_PIN        1
#define WF_HIBERNATE_INPUT(v)   if (v) TRISGSET = 1 << WF_HIBERNATE_PIN; \
                                else   TRISGCLR = 1 << WF_HIBERNATE_PIN
#define WF_HIBERNATE_SET(v)     if (v) PORTGSET = 1 << WF_HIBERNATE_PIN; \
                                else   PORTGCLR = 1 << WF_HIBERNATE_PIN

/*
 * WiFi reset signal at pin RF4
 */
#define WF_RESET_PIN        4
#define WF_RESET_INPUT(v)   if (v) TRISFSET = 1 << WF_RESET_PIN; \
                            else   TRISFCLR = 1 << WF_RESET_PIN
#define WF_RESET_SET(v)     if (v) PORTFSET = 1 << WF_RESET_PIN; \
                            else   PORTFCLR = 1 << WF_RESET_PIN

/*
 * WiFi chip select signal at pin RD9
 */
#define WF_CS_PIN           9
#define WF_CS_INPUT(v)      if (v) TRISDSET = 1 << WF_RESET_PIN; \
                            else   TRISDCLR = 1 << WF_RESET_PIN
#define WF_CS_SET(v)        if (v) PORTDSET = 1 << WF_RESET_PIN; \
                            else   PORTDCLR = 1 << WF_RESET_PIN

/*
 * Peripheral pin select.
 */
#define WF_INT_PPS()        INT4R   = 0b1101    // INT4     RA15
#define WF_HIBERNATE_PPS()  RPG1R   = 0b0000    // HIB      RG1
#define WF_RESET_PPS()      RPF4R   = 0b0000    // RESET    RF4
#define WF_CS_PPS()         RPD9R   = 0b0000    // CS       RD9
#define WF_SCK_PPS()        RPD10R  = 0b0000    // SCK4     RD10
#define WF_SDI_PPS()        SDI4R   = 0b0010    // SDI4     RF5
#define WF_SDO_PPS()        RPG0R   = 0b1000    // SDO4     RG0
