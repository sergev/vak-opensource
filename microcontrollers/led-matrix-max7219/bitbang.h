/*
 * Generic interface to FT232R chip in bitbang mode.
 */
#define MASK_TXD    0x01
#define MASK_RXD    0x02
#define MASK_RTS    0x04
#define MASK_CTS    0x08
#define MASK_DTR    0x10
#define MASK_DSR    0x20
#define MASK_DCD    0x40
#define MASK_RI     0x80

/*
 * Connect to FT232RL adapter by name.
 * Configure needed pins as outputs by mask.
 */
int bitbang_open(char *device_desc, int output_mask);

/*
 * Close adapter.
 */
void bitbang_close(void);

/*
* Perform sync bitbang output/input transaction.
* Array output[] contains data to send.
* Counter nbytes gives a length.
* On return, received data are put back to array reply[64].
*/
int bitbang_io(unsigned char *output, int nbytes, unsigned char *reply);
