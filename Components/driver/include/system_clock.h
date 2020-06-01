#ifndef  SYSTEM_CLOCK_H
#define SYSTEM_CLOCK_H

#include "hal_types.h"


#define  IRC24M_CLOCK          0x00   //内部高频RC振荡器
#define  HXCO_CLOCK            0x01   //外部高频晶振
#define  EXTERLAN_INPUT_CLOCK  0x10   //外部时钟输入
#define  IRC32K_CLOCK          0x11   //内部低频RC振荡器

#define  CLOCK_OUTPUT_P5_4     0x54  //系统时钟输出P5.4
#define  CLOCK_OUTPUT_P1_6     0x16  //系统时钟输出P1.6

#define ORIGINAL_ICR_FREQ_ADJUST    0xaa   //原先ICR频率

typedef struct 
{
    uint8 lirtrim;   //IRC频率微调参数
    uint8 irtrim;    //IRC频率调整参数
    uint8 get_status;    //状态
}icr_freq;

typedef struct 
{
    uint8 clock_source;  //时钟源
    uint8 clock_div;     //分频系数
}clock_info;

extern void system_clock_init(void);
static void get_original_irc_freq_adjust(void);
extern void set_irc_freq_adjust(uint8 coarse_tuning, uint8 fine_tuning,uint8 mode);
extern clock_info get_system_clock_source(void);
extern void set_system_clock_source(uint8 clock_source, uint8 clock_div);
extern void set_system_clock_output(uint8 clock_div,uint8 clock_pin,uint8 output_enable);



#endif