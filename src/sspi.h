#ifndef _VVC_SSPI_H
#define _VVC_SSPI_H

#include "global.h"

// Write a byte of data using software SPI.
inline void sspi_w(uint8_t dat);
// Write a 'command' byte for 4-wire SPI interfaces.
inline void sspi_cmd(uint8_t cdat);

#endif
