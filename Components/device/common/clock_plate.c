#include "clock_plate.h"
#include "oled.h"
#include "uart.h"
#include "delay.h"


#define SECOND_HAND    1     //秒针
#define MINUTE_HAND    2     //分针
#define HOUR_HAND      3     //时针

#define CLOCK_RADIUS   31    //钟表外围半径
#define CIRCLE_X       34    //钟表轴心坐标x
#define CIRCLE_Y       31    //钟表轴心坐标y

#define CLOCK_SCALE_RADIUS   CLOCK_RADIUS - 2    //刻度点位置半径

#define SECOND_HAND_LENGTH   28    //秒针长度
#define MINUTE_HAND_LENGTH   27    //分针长度
#define HOUR_HAND_LENGTH     18    //时针长度

uint32 angle_tan[] = {
//0°  6°  12°  18°  24°  30°  36°  42°  48°   54°    60°  66°   72°   78°   84°    90°
  0,1051,2126,3249,4452,5773,7265,9004,11106,13763,17320,22460,30776,47046,95143,0xffffffff
};//tanθ扩大1000倍

//为了避免每次都重新计算位置，用数组将位置保存下来。
//每个数组元素占16个位，高8位存放y坐标，低8位存放x坐标
uint16 second_coordinate[16];  //秒针坐标
uint16 minute_coordinate[16];  //分针坐标
uint16 hour_coordinate[16];    //小时坐标
uint16 scale_coordinate[12];   //刻度点坐标

clock_time pre_time; //前一次时间

/**************************************************************************************************
 *@函数            clock_plate_init
 *
 *@简述            初始化钟表盘
 *
*@输入参数       
 *                
 *@参数            无
 *
 *输出参数         无
 * 
 *@返回     
 **************************************************************************************************
 */
void clock_plate_init(void)
{
    uint8 x,y;
    
    clock_calculate_coordinate(SECOND_HAND,CLOCK_SCALE_RADIUS);//计算生成刻度坐标
    for(uint8 i = 0; i < 60; i++)
    {
      if(i%5 == 0)//保存12个刻度点坐标
      {
          clock_get_coordinate(i,SECOND_HAND,&x,&y);
          scale_coordinate[i/5] = y;  //记录y坐标
          scale_coordinate[i/5] <<= 8;
          scale_coordinate[i/5] |= x; //记录x坐标
      }
    }
    
    clock_calculate_coordinate(SECOND_HAND,SECOND_HAND_LENGTH); //计算生成秒针坐标
    clock_calculate_coordinate(MINUTE_HAND,MINUTE_HAND_LENGTH); //计算生成分针坐标
    clock_calculate_coordinate(HOUR_HAND,HOUR_HAND_LENGTH); //计算生成时针坐标

    //OLED_DrawCircle(CIRCLE_X,CIRCLE_Y,CLOCK_RADIUS,1,0);//绘制钟表外围
    for(uint8 i = 0; i < 12; i++) //绘制12个刻度
    {
      x = (uint8)scale_coordinate[i];
      y = (uint8)(scale_coordinate[i]>>8);
      OLED_SetPixel(x,y,1);
    }
}
/**************************************************************************************************
 *@函数            clock_run
 *
 *@简述            运行表盘函数
 *
*@输入参数         time - 时间结构体
 *                
 *@参数            无
 *
 *输出参数         无
 * 
 *@返回     
 **************************************************************************************************
 */
void clock_run(clock_time time)
{ 
    clock_show_time(pre_time,0);//擦除前一次时间轨迹
    clock_show_time(time,1);//显示当前时间
    pre_time = time;//记录最新时间
}
/**************************************************************************************************
 *@函数            clock_show_time
 *
 *@简述            表盘显示时间函数
 *
*@输入参数         time - 时间结构体
 *                 state - 状态
 *               
 *@参数            无
 *
 *输出参数         无
 * 
 *@返回     
 **************************************************************************************************
 */
void clock_show_time(clock_time time,uint8 state)
{
    uint8 x,y;
    
    if(time.hours > 12) //如果为24小时
    {
      time.hours -= 12;//转为12小时制
    }
    
    if(state)
    {
        OLED_ShowChar(CIRCLE_X-5,2,'1',12); //数字1
        OLED_ShowChar(CIRCLE_X,2,'2',12); //数字2
        OLED_ShowChar(CIRCLE_X-2,50,'6',12);//数字6
        OLED_ShowChar(CIRCLE_X+24,25,'3',12);//数字3
        OLED_ShowChar(CIRCLE_X-29,25,'9',12);//数字9

        OLED_DrawCircle(CIRCLE_X,CIRCLE_Y,2,1,1);//绘制转轴
    }

    clock_get_coordinate(time.hours*5+time.minutes/12,HOUR_HAND,&x,&y);
    OLED_DrawLine(CIRCLE_X,CIRCLE_Y,x,y,state); //绘制时针
    
    clock_get_coordinate(time.minutes,MINUTE_HAND,&x,&y);
    OLED_DrawLine(CIRCLE_X,CIRCLE_Y,x,y,state); //绘制分针
    
    clock_get_coordinate(time.seconds,SECOND_HAND,&x,&y);
    OLED_DrawLine(CIRCLE_X,CIRCLE_Y,x,y,state); //绘制秒针
}
/**************************************************************************************************
 *@函数            clock_calculate_coordinate
 *
 *@简述            计算针轨迹右下部数字15-30的坐标
 *
*@输入参数        clock_hand - 类别：时针、分针、秒针
 *                length - 指针长度
 *
 *@参数            无
 * 
 *@返回     
 **************************************************************************************************
 */
static void clock_calculate_coordinate(uint8 clock_hand,uint8 length)
{
    uint8 x,y;
    uint8 rad = 0;
    uint16 *coordinate_array;
    double value = 0;
    double angel = 0;
    double cal_x,cal_y;
    double all_value;
    
    OLED_Clear_Ram();//清显存
    
    OLED_DrawCircle(CIRCLE_X,CIRCLE_Y,length,1,0);//绘制轨迹
    rad = length;
    
    switch(clock_hand) //判断指针类型
    {
        case SECOND_HAND:  //秒针
          coordinate_array = second_coordinate;
          break;
        case MINUTE_HAND: //分针
          coordinate_array = minute_coordinate;
          break;
        case HOUR_HAND:  //时针
          coordinate_array = hour_coordinate;
          break;
        default:
          break;
    }
    
    for(uint8 k = 0; k < 16; k++) //计算轨迹右下部15-30的坐标
    {
       angel = angle_tan[k];
       all_value = 977889999;
       for(uint8 i = CIRCLE_X - 1; i <= CIRCLE_X + rad; i++)
       {
          for(uint8 j = CIRCLE_Y - 1; j <= CIRCLE_Y + rad; j++)
          {
              if(OLED_GetPixel(i,j)) //扫描针的轨迹
              {              
                  cal_x = i-CIRCLE_X;
                  cal_y = j-CIRCLE_Y;
                  value =cal_y*10000 - angel*cal_x;
                  if(value < 0)//负数处理
                  {
                      value = -value;
                  }
                  if(all_value - value> 0) //寻找最合适的坐标
                  {
                      all_value = value;
                      x = i;
                      y = j;                           
                  }                                                 
              }
          }
       }
      coordinate_array[k] = y;  //记录y坐标
      coordinate_array[k] <<= 8;
      coordinate_array[k] |= x; //记录x坐标
    }
    OLED_Clear_Ram();//清显存
}   
/**************************************************************************************************
 *@函数            clock_get_coordinate
 *
 *@简述            获取坐标
 *
*@输入参数        clock_num - 时钟数字（0-59）
 *                clock_hand - 类别：时针、分针、秒针
 *
 *@参数            无
 *
 *输出参数       x - x坐标
 * 
 *               y - y坐标
 *无
 * 
 *@返回     无
 **************************************************************************************************
 */
void clock_get_coordinate(uint8 clock_num,uint8 clock_hand,uint8 *x,uint8 *y)
{
    uint8 num = 0;
    uint8 area_type;
    uint8 rad = 0;
    uint16 *coordinate_array;

    switch(clock_hand) //判断指针类型
    {
        case SECOND_HAND:  //秒针
          coordinate_array = second_coordinate;
          rad = SECOND_HAND_LENGTH;  //秒针长度
          break;
        case MINUTE_HAND: //分针
          coordinate_array = minute_coordinate;
          rad = MINUTE_HAND_LENGTH;  //分针长度
          break;
        case HOUR_HAND:  //时针
          coordinate_array = hour_coordinate;
          rad = HOUR_HAND_LENGTH;  //时针长度
          break;
        default:
          break;
    }
    
    num = clock_convert_num(clock_num,&area_type);//转换时钟数字为15-30和区域类型
    *x = (uint8)coordinate_array[num-15];
    *y = (uint8)(coordinate_array[num-15]>>8);
    
     switch(area_type) //判断数字所在区域类型，以便换算得出对应坐标
    {
        case 0:  //0-15
          *y = CIRCLE_Y + CIRCLE_Y - *y;
          break;
        case 1:  //15-30 
          break;
        case 2:    //30-45
          *x = CIRCLE_X + CIRCLE_X - *x;
          break;
        case 3:   //45-60   
          *x = CIRCLE_X + CIRCLE_X - *x;
          *y = CIRCLE_Y + CIRCLE_Y - *y;   
          break;
        case 4:   //数字0的处理
          *y -= rad + rad;//y坐标减去2倍rad
          break;
        case 5:   //数字45的处理
          *x -= rad + rad;//x坐标减去2倍rad
          break;
        default:
          break;
    }            
}
/**************************************************************************************************
 *@函数            clock_convert_num
 *
 *@简述            转换时钟数字为15-30之间的数字、标记区域类型
 *
*@输入参数        num - 时钟数字（0-59）
 *
 *@参数            无
 *
 *输出参数       area_type - 区域类型
 * 
 *无
 * 
 *@返回     result - 数字（15-30）
 **************************************************************************************************
 */
//将数字转换成15-30的之间的数，标记数字的区域类型
static uint8 clock_convert_num(uint8 num,uint8 *area_type)
{
    uint8 result = 0;
    if(num < 15) //0-15
    {
        result = 30 - num;
        *area_type = 0;
        if(num == 0) //数字0特殊处理
        {
            *area_type = 4;
        }
    }
    else if(num < 30) //15-30
    {
        result = num;
        *area_type = 1;
    }
    else if(num < 45) //30-45
    {
        result = 60 - num;
        *area_type = 2;
    }
    else if(num < 60) //45-60
    {
        result = num - 30;
        *area_type = 3;
        if(num == 45) //时钟45特殊处理
        {
            *area_type = 5; 
        }
    }
    return result;
}
