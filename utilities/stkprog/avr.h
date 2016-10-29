typedef struct _avr_t avr_t;

avr_t *avr_open (char *devname);
void avr_close (avr_t *avr);
void avr_erase (avr_t *avr);
void avr_lock (avr_t *avr);
void avr_write_byte (avr_t *avr, u_int32_t addr, unsigned char byte);
void avr_write_word (avr_t *avr, u_int32_t addr, unsigned char bytelow, unsigned char bytehigh);
void avr_flush_write_buffer (avr_t *avr);
unsigned char avr_read_byte (avr_t *avr, u_int32_t addr);
void avr_read_block (avr_t *avr, u_int32_t addr, unsigned char *buf,
	u_int32_t bytes);
void avr_write_block (avr_t *avr, u_int32_t addr,
	unsigned char *buf, u_int32_t bytes);
char *avr_name (avr_t *avr);
u_int32_t avr_flash_size (avr_t *avr);
unsigned char avr_have_fuse (avr_t *avr);
unsigned char avr_read_fuse (avr_t *avr);
unsigned char avr_read_fuse_high (avr_t *avr);
unsigned char avr_read_fuse_extended (avr_t *avr);
void avr_write_fuse (avr_t *avr, unsigned char byte);
void avr_write_fuse_high (avr_t *avr, unsigned char byte);
void avr_write_fuse_extended (avr_t *avr, unsigned char byte);

void avr_identify (avr_t *avr);

int avr_check_block (avr_t *avr, u_int32_t addr,
	unsigned char *buf, u_int32_t bytes);
