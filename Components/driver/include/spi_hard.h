#ifndef SPI_HARD_H
#define SPI_HARD_H

#include "hal_types.h"

extern void hard_spi_init(void);
extern void hard_spi_open(void);
extern void hard_spi_close(void);
static uint8 hard_spi_transaction_byte(uint8 data);
extern void hard_spi_transaction(const uint8 *tx, uint32 tx_len, uint8 *rx, uint32 rx_len);
extern void set_hard_spi_speed(uint8 speed);




#endif