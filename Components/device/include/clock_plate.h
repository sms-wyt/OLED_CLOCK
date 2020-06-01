#ifndef CLOCK_PLATE
#define CLOCK_PLATE

#include "hal_types.h"

typedef struct 
{
  uint8 seconds;  //√Î£®0-59£©
  uint8 minutes;  //∑÷£®0-59£©
  uint8 hours;    // ±£®0-23£©
}clock_time;

extern void clock_plate_init(void);
static void clock_calculate_coordinate(uint8 clock_hand,uint8 length);
static uint8 clock_convert_num(uint8 num,uint8 *area_type);
static void clock_get_coordinate(uint8 clock_num,uint8 clock_hand,uint8 *x,uint8 *y);
static void clock_show_time(clock_time time,uint8 state);
extern void clock_run(clock_time time);

#endif
