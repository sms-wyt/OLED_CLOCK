#include "ds3231.h"
#include "i2c_hard.h"

#define BCD_TO_DEC(bcd)   (bcd/16*10 + bcd % 16)  //BCD转DEC
#define DEC_TO_BAC(dec)   (dec/10*16 + dec%10)    //DEC转BCD

#define DS3231_ADDR  0xd0  //DS3231从机地址

/***********************DS3231相关寄存器***************************/
#define SECOND_REGISTER      0x00  //秒寄存器
//#define MINUTE_REGISTER      0x01  //分钟寄存器
//#define HOUR_REGISTER        0x02  //小时寄存器
//#define WEEK_REGISTER        0x03  //星期寄存器
//#define DAY_REGISTER         0x04  //日期寄存器
//#define MONTH_REGISTER       0x05  //日期寄存器
//#define YEAR_REGISTER        0x06  //年份寄存器
#define CONTROL_REGISTER     0x0e  //控制寄存器
#define STATUS_REGISTER      0x0f  //状态寄存器
#define TMEP_H_REGISTER      0x11  //温度高位寄存器
#define TMEP_L_REGISTER      0x12  //温度低寄存器

/*******************************************使用示例***********************************************
ds3231_time set_time,get_time;
uint8 temp,sign;
set_time.seconds = 10
set_time.minutes = 10
set_time.hours = 10
set_time.day =   1
set_time.week =   1
set_time.month =  1
set_time.year =  19
ds3231_init();//初始化ds3231
ds3231_set_time(&set_time);//设置时间
ds3231_get_time(&get_time);//获取时间
ds3231_start_temp_convert();//启动温度转化
ds3231_get_temp(&temp,&sign);//获取温度数据
**************************************************************************************************/
/**************************************************************************************************
 *@函数            ds3231_init
 *
 *@简述            ds3231初始化
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
void ds3231_init(void)
{ 
    uint8 value[2] ={CONTROL_REGISTER,0x00};
    hard_i2c_transaction(DS3231_ADDR, value, 2, NULL, 0);
}
/**************************************************************************************************
 *@函数            ds3231_get_time
 *
 *@简述            获取DS3231时间(24小时制)
 *
 *@输入参数  
 *
 *@参数            time - 时间结构体 十进制（秒分时周日月年）
 *
 *输出参数
 *
 *无
 * 
 *@返回     0：成功   1：失败
 **************************************************************************************************
 */
uint8 ds3231_get_time(ds3231_time *time)
{
    uint8 *get_time;
    uint8 value,reg = SECOND_REGISTER; 
    get_time = (uint8*)time;
    
    for(uint8 i = 0; i < 7; i++,reg++)
    {
        if(hard_i2c_transaction(DS3231_ADDR, &reg, 1, &value, 1))
        {
            return 1;
        }       
        get_time[i] = BCD_TO_DEC(value);//BCD码转为十进制
    }
    
    return 0;
}
/**************************************************************************************************
 *@函数            ds3231_set_time
 *
 *@简述            设置DS3231时间（24小时制）
 *
 *@输入参数  
 *
 *@参数            time - 时间结构体 十进制（秒分时周日月年）
 *
 *输出参数
 *
 *无
 * 
 *@返回     0：成功   1：失败
 **************************************************************************************************
 */
uint8 ds3231_set_time(ds3231_time *time)
{ 
    uint8 set_time[8] = {SECOND_REGISTER};
    uint8 *data;
    data = (uint8*)time;
    
    if(data[2] >= 64)//禁止设定12小时制
    {
        return 1;
    }
    
    for(uint8 i = 0; i < 8; i++)
    {
        set_time[i+1] = DEC_TO_BAC(data[i]);//十进制转为BCD码
    }
    
    return(hard_i2c_transaction(DS3231_ADDR, set_time, 8, NULL, 0));
}
/**************************************************************************************************
 *@函数            ds3231_start_temp_convert
 *
 *@简述            启动DS3231温度测量转化（温度准确度：±3℃）
 *
 *@输入参数  
 *
 *@参数            无
 *
 *输出参数
 *
 *无
 * 
 *@返回     0：成功   1：失败
 *
 *说明：如果在纽扣电池供电情况下，DS3231温度测量转化的是一次性64秒。
 **************************************************************************************************
 */
uint8 ds3231_start_temp_convert(void)
{
    uint8 value[2] = {CONTROL_REGISTER};
    //读取control寄存器原先的数据
    if(hard_i2c_transaction(DS3231_ADDR, value, 1, value+1, 1))
    {
        return 1;
    }
    value[1] |= 0x20;//加上温度转化启动位
    //写入control寄存器数据
    if(hard_i2c_transaction(DS3231_ADDR, value, 2, NULL, 0))
    {
        return 1;
    }
    return 0;
}
/**************************************************************************************************
 *@函数            ds3231_get_temp
 *
 *@简述            获取DS3231温度
 *
 *@输入参数  
 *
 *@参数            *temp - 温度数据指针（温度放大了100倍）
 *                 *sign - 符号数据指针（0 - 0℃以上  1 - 0℃以下）
 *
 *输出参数
 *
 *无
 * 
 *@返回     0：读取成功   1：读取失败
 **************************************************************************************************
 */
uint8 ds3231_get_temp(uint16 *temp, uint8 *sign)
{
    uint8 temp_h,temp_l,reg_temp_h,reg_temp_l,reg_status,status;
    
    reg_temp_h = TMEP_H_REGISTER;
    reg_temp_l = TMEP_L_REGISTER;
    reg_status = STATUS_REGISTER;
    
    if(hard_i2c_transaction(DS3231_ADDR, &reg_status, 1, &status, 1))
    {
        return 1;
    }
    
    if(status & 0x04 == 1)//判断温度是否转化完成
    {
        return 1;
    }
    
    if(hard_i2c_transaction(DS3231_ADDR, &reg_temp_h, 1, &temp_h, 1))
    {
        return 1;
    }
    
    if(hard_i2c_transaction(DS3231_ADDR, &reg_temp_l, 1, &temp_l, 1))
    {
        return 1;
    }
    
    *temp = (((uint16)temp_h <<2)  + ((uint16)temp_l >> 6)) * 25;
    
    if(temp_h & 0x80 == 1)//温度是否为0℃以下
    {
        *sign = 1;
    }
    
    return 0;
}