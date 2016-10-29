/*
 * Library: MSP430.dll
 */
#ifndef MINGW32
#define __stdcall /*empty*/
#endif

/*
 * Initialize the interface.
 *
 * port    - interface port reference (application specific).
 * version - the version number of the MSP430 DLL is returned.
 */
int __stdcall (*MSP430_Initialize) (const char *port, long *version);

/*
 * Close the interface.
 * Parameters: vccOff - turn off the device Vcc (0 volts) if TRUE.
 * Returns 0 on success.
 */
int __stdcall (*MSP430_Close) (int vccOff);

/*
 * Configure the device interface.
 */
int __stdcall (*MSP430_Configure) (int op, int arg);

/*
 * Set the device Vcc pin to voltage/1000 volts.
 * A "voltage" of zero (0) turns off voltage to the device.
 * The FET interface does not support setting the voltage.
 * for nonzero values the power on the FET kits is enabled,
 * disables otherwise.
 */
int __stdcall (*MSP430_VCC) (int mvolt);

/*
 * Reset the device using the specified method(s). Optionally start device execution,
 * and release the JTAG control signals.
 *
 * method:      The bit mask specifying the method(s) to use to reset the device:
 *                PUC:     The device is reset using PUC (i.e., a "soft" reset).
 *                RST_NMI: The device is reset using the RST/NMI pin (i.e., a "hard" reset).
 *                VCC:     The device is reset by cycling power to the device.
 * execute:     Start device execution (when TRUE).
 * releaseJTAG: Release the JTAG control signals (when TRUE). execute must be TRUE.
 *
 * It is possible to combine reset methods. The methods are applied in the following order:
 * PUC then RST_NMI then VCC. If a reset operation fails, the next reset method (if any) is applied.
 *
 * Following reset by RST/NMI and/or VCC, a PUC is automatically executed to reset the device in
 * such a way that it does not begin to execute a resident program (or garbage).
 */
int __stdcall (*MSP430_Reset) (int method, int execute, int releaseJTAG);

/*
 * Get the device identification data.
 * Bytes 4..35 contain a text string.
 */
int __stdcall (*MSP430_Identify) (char *buf, int len, int arg);

/*
 * Erase the device FLASH memory. MSP430_Read_Memory() can be used to search the
 * address of a failed erase operation.
 * If address+length extends beyond the segment containing address, intermediate
 * segments are erased (and checked).
 *
 * type: ERASE_SEGMENT: Erase the segment containing 'address'.
 *       ERASE_MAIN:    Erase the Main memory.
 *       ERASE_ALL:     Erase the Main and Information memories.
 * address:             Starting address of erase check operation. Must be word aligned.
 * length:              Length of erase check operation (even number of bytes).
 */
int __stdcall (*MSP430_Erase) (int type, int address, int length);

/*
 * Read and write the device memory. "Device memory" includes the Special Function
 * Registers (i.e., peripheral registers), RAM, Information (FLASH) memory, and
 * Main (FLASH) memory.
 * The write to FLASH memory operation DOES NOT erase the FLASH, use MSP430_Erase
 * in advance to do so.
 *
 * address: The starting address of the device memory to be read or written.
 * buffer:  The buffer into which device memory is read, or from which device memory is written.
 * count:   The number of bytes of device memory read or written.
 * rw:      Specify a read (READ) or write (WRITE) operation.
 */
int __stdcall (*MSP430_Memory) (int, unsigned char *buffer, int count, int rw);

#if 0
int MSP430_ProgramFile (string, int, int);
int MSP430_Error_Number (void);
string MSP430_Error_String (int);
#endif

/*
 * Configurations of the MSP430 driver
 */
#define CONFIGURE_VERIFICATION_MODE	0 /* Verify data downloaded to FLASH memories */
#define CONFIGURE_EMULATION_MODE	1
#define CONFIGURE_CLK_CNTRL_MODE	2
#define CONFIGURE_MCLK_CNTRL_MODE	3
#define CONFIGURE_FLASH_TEST_MODE	4
#define CONFIGURE_LOCKED_FLASH_ACCESS	5 /* Allows Locked Info Mem Segment A access (if set to '1') */
#define CONFIGURE_FLASH_SWOP		6
#define CONFIGURE_EDT_TRACE_MODE	7
#define CONFIGURE_INTERFACE_MODE	8 /* see INTERFACE_TYPE below */
#define CONFIGURE_SET_MDB_BEFORE_RUN	9
#define CONFIGURE_RAM_PRESERVE_MODE	10 /* Configure whether RAM content should be preserved/restored */

/* Interface type */
#define INTERFACE_JTAG_IF		0
#define INTERFACE_SPYBIWIRE_IF		1
#define INTERFACE_SPYBIWIREJTAG_IF	2

/* Device reset methods */
#define RESET_PUC			1 /* Power up clear (soft reset) */
#define RESET_RST			2 /* RST/NMI (hard reset) */
#define RESET_VCC			4 /* Cycle Vcc (power on reset) */

/* FLASH erase type */
#define ERASE_SEGMENT			0 /* Erase a segment */
#define ERASE_MAIN			1 /* Erase all MAIN memory */
#define ERASE_ALL			2 /* Erase all MAIN and INFORMATION memory */

#define MEMORY_WRITE			0
#define MEMORY_READ			1
