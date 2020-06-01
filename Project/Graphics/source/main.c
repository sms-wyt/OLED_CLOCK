
/*************INCLUDE*****************/
#include "ioSTC8.h"
#include "spi_hard.h"
#include "oled.h"
#include "uart.h"
#include "clock_plate.h"
#include "i2c_hard.h"
#include "ds3231.h"
#include "delay.h"
#include "stdlib.h"


void main(void) 
{
    ds3231_time get_time;
    uint8 second;
    clock_time current_time;
    SP = 0x80;
    hard_spi_init();//初始化硬件SPI
    hard_i2c_init();//初始时硬件IIC
    OLED_Init();//OLED初始化
    OLED_Clear_Display();//清除显示缓存
    clock_plate_init();//表盘初始化
    ds3231_init();//
    
    while(1)
    {
        Delay_ms(200);
    
        ds3231_get_time(&get_time);
        if(second != get_time.seconds)
        {
            current_time.hours = get_time.hours;
            current_time.minutes = get_time.minutes;
            current_time.seconds = get_time.seconds;
            clock_run(current_time);
            OLED_Refresh_Display();
            second = get_time.seconds;
        }
  
    }
}
  
  


