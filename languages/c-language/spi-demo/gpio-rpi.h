
#define MODE_INPUT      0
#define MODE_OUTPUT     1

int gpio_export(int pin);
int gpio_unexport(int pin);
int gpio_set_mode(int pin, int mode);
int gpio_read(int pin);
int gpio_write(int pin, int value);
