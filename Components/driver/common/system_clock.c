#include "system_clock.h"
#include "ioSTC8.h"


icr_freq original_icr_freq; //初始IRC频率调整参数

/**************************************************************************************************
 *@函数            system_clock_init
 *
 *@简述            系统时钟初始化
 *
 *@输入参数  
 *
 *@参数            无
 *
 *输出参数
 *
 *无
 * 
 *@返回     无
 **************************************************************************************************
 */
void system_clock_init(void)
{
    get_original_irc_freq_adjust();//获取初始IRC频率调整参数
    set_system_clock_source(IRC24M_CLOCK, 1);//设置内部高频RC振荡器、不分频
}
/**************************************************************************************************
 *@函数            get_original_irc_freq_adjust
 *
 *@简述            获取初始IRC频率调整参数
 *
 *@输入参数  
 *
 *@参数            无
 *
 *输出参数
 *
 *无
 * 
 *@返回     无
 **************************************************************************************************
 */
void get_original_irc_freq_adjust(void)
{
    original_icr_freq.irtrim = IRTRIM;
    original_icr_freq.lirtrim = LIRTRIM;
    original_icr_freq.get_status = 0xaa;//标记已经获取参数
}
/**************************************************************************************************
 *@函数            set_irc_freq_adjust
 *
 *@简述            设置IRC频率调整参数
 *
 *@输入参数       coarse_tuning - IRC频率调整参数
 *                fine_tuning - IRC频率微调参数
 *
 *@参数            无
 *
 *输出参数
 *
 *无
 * 
 *@返回     无
 **************************************************************************************************
 */
void set_irc_freq_adjust(uint8 coarse_tuning, uint8 fine_tuning,uint8 mode)
{
    //判断是否设回初始化参数
    if((mode == ORIGINAL_ICR_FREQ_ADJUST) && (original_icr_freq.get_status == 0xaa))
    {
        IRTRIM = original_icr_freq.irtrim;
        LIRTRIM = original_icr_freq.lirtrim;
    }
    else
    {
        IRTRIM = coarse_tuning;
        LIRTRIM = fine_tuning;
    }
}
/**************************************************************************************************
 *@函数            get_system_clock_source
 *
 *@简述            获取当前系统时钟源
 *
 *@输入参数       
 *
 *@参数            无
 *
 *输出参数
 *
 *无
 * 
 *@返回     source - 时钟源
 **************************************************************************************************
 */
clock_info get_system_clock_source(void)
{
    clock_info source;
    P_SW2 |= 0x80;
    
    switch(CKSEL&0x03)
    {
      case IRC24M_CLOCK:
        source.clock_source = IRC24M_CLOCK;
        break;
      case HXCO_CLOCK:
        source.clock_source = IRC24M_CLOCK;
        break;
      case IRC32K_CLOCK:
        source.clock_source = IRC32K_CLOCK;
        break;
      default:
        source.clock_source = EXTERLAN_INPUT_CLOCK;
        break;
    }
    source.clock_div = CLKDIV;
    P_SW2 &= 0x7f;
    return source;      
}
/**************************************************************************************************
 *@函数            set_system_clock_source
 *
 *@简述            系统时钟源选择
 *
 *@输入参数       clock_source - 时钟源
 *                clock_div - 分频系数（1-255）
 *
 *@参数            无
 *
 *输出参数
 *
 *无
 * 
 *@返回     无
 **************************************************************************************************
 */
void set_system_clock_source(uint8 clock_source, uint8 clock_div)
{
    clock_info old_clock_source;
    old_clock_source = get_system_clock_source();//获取之前系统时钟源
    P_SW2 |= 0x80;
    if(clock_source != old_clock_source.clock_source)
    {   
        switch(clock_source) //判断系统时钟源的类型
        {
            case IRC24M_CLOCK:
                IRC24MCR |= 0x01;//使能内部高频RC振荡器
                while(IRC24MCR&0x8f == 0);//等待高频RC振荡器稳定
                break;
            case HXCO_CLOCK:
                XOSCCR |= 0x01;//使能外部晶振
                while(XOSCCR&0x8f == 0);//等待晶振稳定
                break;
            case IRC32K_CLOCK:
                IRC32KCR |= 0x01;//使能内部低频RC振荡器
                while(IRC32KCR&0x8f == 0);//等待低频RC振荡器稳定
                break;
            default:
                break;
        }
        CKSEL &= 0xfc; //清除时钟源
        CKSEL |= clock_source; //设置新系统时钟源
        switch(old_clock_source.clock_source)//判断之前时钟源的类型
        {
            case IRC24M_CLOCK:
                IRC24MCR &= 0xfe;//失能内部高频RC振荡器
                break;
            case HXCO_CLOCK:
                XOSCCR &= 0xfe;//失能外部晶振
                break;
            case IRC32K_CLOCK:
                IRC32KCR &= 0xfe;//失能内部低频RC振荡器
                break;
            default:
                break;
        }
    }
    
    CLKDIV = clock_div;
    P_SW2 &= 0x7f;
}
/**************************************************************************************************
 *@函数            set_system_clock_output
 *
 *@简述            设置系统时钟对外输出
 *
 *@输入参数       clock_div - 输出分频(0-7),对应2的(0-7幂次方)
 *                clock_pin - 时钟输出管脚
 *                output_enable - 使能输出（0：失能, !0:使能）
 *         
 *@参数            无
 *
 *输出参数
 *
 *无
 * 
 *@返回     无
 **************************************************************************************************
 */
void set_system_clock_output(uint8 clock_div,uint8 clock_pin,uint8 output_enable)
{
    P_SW2 |= 0x80;
    
    CKSEL &= 0xf0;
    if(output_enable != 0)//时钟输出使能
    {
        if(clock_div == 0)
        {
            CKSEL |= 0x01;
        }
        CKSEL |= clock_div<<1;
        CKSEL&= 0xf7;
        if(clock_pin == CLOCK_OUTPUT_P5_4)
        {
            CKSEL|= 0x08;
        }
    }
    
    P_SW2 &= 0x7f;
}