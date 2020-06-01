#ifndef  VS1053_H
#define  VS1053_H

#include "hal_types.h"

static void vs1053_SCI(uint8 instruction, uint8 addr, uint8 *data, uint32 len);
extern void vs1053_SDI(uint8 *data, uint32 len);
extern void vs1053_init(void);
extern void vs1053_soft_reset(void);
extern void vs1053_hard_reset(void);
extern void vs1053_sine_test(void);
extern void vs1053_volume_ctrl(uint8 volume);
extern void vs1053_reset_decode_time(void);
extern void vs1053_play_music(void);
extern void vs1053_load_music(uint8 *pname);




#endif