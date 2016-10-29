#ifndef ethernet_h
#define ethernet_h

#include <stdint.h>
#include "client.h"
#include "server.h"

#define MAX_SOCK_NUM 4

void ethernet_init (uint8_t *mac, uint8_t *ip, uint8_t *gateway, uint8_t *subnet);

#endif
