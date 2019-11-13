#include <stdint.h>

int spi_init(char *spidev_name, unsigned bits_per_sec);
void spi_close(void);
int spi_set_mode(int mode);

int spi_transfer(uint8_t buf);
int spi_bulk_rw(uint8_t *buf, unsigned len);
