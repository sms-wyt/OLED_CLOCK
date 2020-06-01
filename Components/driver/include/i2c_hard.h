#ifndef I2C_HARD_H
#define I2C_HARD_H

#include "hal_types.h"

static void hard_i2c_delay(void);
extern void hard_i2c_ulock(void);
extern void hard_i2c_init(void);
extern void hard_i2c_open(void);
extern void hard_i2c_close(void);
static void hard_i2c_wait(void);
static void hard_i2c_start(void);
static void hard_i2c_stop(void);
static uint8 hard_i2c_write_byte(uint8 data);
static uint8 hard_i2c_read_byte(uint8 ack);
extern uint8 hard_i2c_transaction(uint8 slave_addr, uint8* tx, uint32 tx_len, uint8* rx, uint32 rx_len);

#endif