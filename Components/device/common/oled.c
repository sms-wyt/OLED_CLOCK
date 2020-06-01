#include "oled.h"
#include "spi_hard.h"
#include "ioSTC8.h"
#include "delay.h"
#include <math.h>
#include "uart.h"
#include "font.h"

#define  OLED_DC     P1_6    //OLED命令/数据选择引脚
#define  OLED_RES    P1_7    //OLED复位引脚
#define  OLED_CS     P1_5    //OLED片选引脚

#define  OLED_CMD    0
#define  OLED_DATA   1

#define PI 3.14159

uint8 OLED_DISPLAY[8][128]; //显示缓冲数组（总共可以表示8192个像素点）

#define     OLED_GET_PAGE_BIT(PAGE, Bit)	((((PAGE) & (0x80 >> (Bit)))>0)?1:0) //顺向（高位在前）
//#define     OLED_GET_PAGE_BIT(PAGE, Bit)	((((PAGE) & (0x01 << (Bit)))>0)?1:0) //逆向（低位在前）

/**************************************************************************************************
 *@函数            OLED_WR_Byte
 *
 *@简述            OLED写一个字节
 *
 *@输入参数        dat - 数据
 *                cmd - 指令类型（命令/数据）
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
void OLED_WR_Byte(uint8 dat, uint8 cmd)
{
    uint8 value = dat;
    if(cmd)
    {
        OLED_DC = 1; //数据模式
    }   
    else
    {
        OLED_DC = 0; //命令模式
    }    
    OLED_CS = 0; //使能片选
    hard_spi_transaction(&value, 1, NULL, 0); //数据传输
    OLED_CS = 1; //失能片选
    OLED_DC = 1; //失能命令
}
/**************************************************************************************************
 *@函数            OLED_WR_Bytes
 *
 *@简述            OLED写多个字节
 *
 *@输入参数        dat - 数据（数据命令）
 *                len - 数据长度
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
void OLED_WR_Bytes(uint8 *dat, uint32 len)
{
    OLED_DC = 1; //数据模式 
    OLED_CS = 0; //使能片选
    hard_spi_transaction(dat, len, NULL, 0); //数据传输
    OLED_CS = 1; //失能片选
    OLED_DC = 1; //失能命令
}

/**************************************************************************************************
 *@函数            OLED_Init
 *
 *@简述            初始化OLED
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
void OLED_Init(void)
{ 	
    set_hard_spi_speed(0x03);
    OLED_DC = 1;
    OLED_CS = 1;
    
    OLED_RES=0;
    Delay_ms(100);
    OLED_RES=1;
    Delay_ms(100);
					  
    OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
    OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
    OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
    OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
    OLED_WR_Byte(0xCF,OLED_CMD); // Set SEG Output Current Brightness
    OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
    OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
    OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
    OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
    OLED_WR_Byte(0x00,OLED_CMD);//-not offset
    OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
    OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
    OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
    OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
    OLED_WR_Byte(0x12,OLED_CMD);
    OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
    OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
    OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)  //页寻址模式
    OLED_WR_Byte(0x00,OLED_CMD);//
    OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
    OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
    OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
    OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
    OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
	
    OLED_WR_Byte(0xAF,OLED_CMD); /*display ON*/ 
    OLED_Clear();
    set_hard_spi_speed(0x00);
} 
/**************************************************************************************************
 *@函数            OLED_Set_Pos
 *
 *@简述            设置OLED起始页和起始列
 *
 *@输入参数         x:0-127
 *                  y:0-7
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
void OLED_Set_Pos(uint8 x, uint8 y) 
{ 
    OLED_WR_Byte(0xb0+y,OLED_CMD);//起始页地址
    OLED_WR_Byte((x&0x0f),OLED_CMD); //起始列低地址
    OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD); //起始列高地址 
}
/**************************************************************************************************
 *@函数            OLED_Clear
 *
 *@简述            OLED清屏
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
void OLED_Clear(void)  
{  	    
    for(uint8 i = 0; i < 8; i++)  
    {  
        OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
	OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置列低地址
	OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置列高地址   
	for(uint8 n = 0; n < 128; n++)
        {
            OLED_WR_Byte(0,OLED_DATA); 
        }
    } 
}

/**************************************************************************************************
 *@函数            OLED_Display_On
 *
 *@简述            开启OLED显示
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
void OLED_Display_On(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
    OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
    OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
/**************************************************************************************************
 *@函数            OLED_Display_Off
 *
 *@简述            关闭OLED显示
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
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
    OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
    OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}	
/**************************************************************************************************
 *@函数            OLED_SetPixel
 *
 *@简述            OLED设置坐标像素点状态
 *
 *@输入参数        x - x坐标
 *                y - y坐标
 *                PixelValue - 像素点状态（1：填充，0：清空）
 *
 *@参数            无
 *
 *输出参数
 *
 *无
 * 
 *@返回     无
 *说明：规定OLED显示区域左上角顶点处为坐标原点(0,0),
 *      x坐标增长方向：向右→
 *      y坐标增长方向：向下↓
 *      坐标原点(0,0)对应OLED_DISPLAY[0][0],即第零页第一个像素点
 *      坐标(127,63)为OLED屏幕右下角顶点
 **************************************************************************************************
 */
void OLED_SetPixel(int32 x, int32 y, int32 PixelValue)
{
    int32 pos,bx,temp=0;
    
    pos = y/8;//计算y坐标所在页
    bx = y%8;//计算位偏移
    temp = 1<<bx;
    if(PixelValue)
    {
        OLED_DISPLAY[pos][x]|=temp;
    }
    else 
    {
        OLED_DISPLAY[pos][x]&=~temp;	  
    }
}

/**************************************************************************************************
 *@函数            OLED_GetPixel
 *
 *@简述            获取坐标像素点状态
 *
 *@输入参数        x - x坐标
 *                y - y坐标
 *
 *@参数            无
 *
 *输出参数
 *
 *无
 * 
 *@返回          PixelValue - 像素点状态
 **************************************************************************************************
 */
int32 OLED_GetPixel(int32 x, int32 y)
{
    int32 pos,bx,temp=0;

    pos = y/8;//计算y坐标所在页
    bx = y%8;//计算位偏移
    temp = 1<<bx;

    if(OLED_DISPLAY[pos][x] & temp)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/**************************************************************************************************
 *@函数            OLED_ReverseAllPixel
 *
 *@简述            OLED所有像素点取反
 *
 *@输入参数        
 *
 *@参数            无
 *
 *输出参数
 *
 *无
 * 
 *@返回         
 **************************************************************************************************
 */
void OLED_ReverseAllPixel(void)
{
    uint8 *value;   
    value = (uint8*)OLED_DISPLAY;//二级指针转为一级指针
    for(uint16 i = 0; i < 1024; i++,value++)
    {
        *value = ~*value;      
    }
}
/**************************************************************************************************
 *@函数            OLED_Refresh_Display
 *
 *@简述            OLED更新显示
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
void OLED_Refresh_Display(void)
{
    uint8 *value;
    value = (uint8*)OLED_DISPLAY;//二级指针转为一级指针
    OLED_WR_Byte (0xb0,OLED_CMD);//开始页：0
    OLED_WR_Byte (0x00,OLED_CMD); //开始列低地址为0
    OLED_WR_Byte (0x10,OLED_CMD); //开始列高地址为0
    OLED_WR_Bytes(value,1024);
}
/**************************************************************************************************
 *@函数            OLED_Clear_Display
 *
 *@简述            OLED清屏
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
void OLED_Clear_Display(void)  
{  
    uint8 *value;
    value = (uint8*)OLED_DISPLAY;//二级指针转为一级指针
    
    for(uint32 i = 0; i < 1024; i++)
    {
      value[i] = 0x00;
    } 
    OLED_Refresh_Display();
}
/**************************************************************************************************
 *@函数            OLED_Clear_Ram
 *
 *@简述            OLED清显存
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
void OLED_Clear_Ram(void)  
{  
    uint8 *value;
    value = (uint8*)OLED_DISPLAY;//二级指针转为一级指针
    
    for(uint32 i = 0; i < 1024; i++)
    {
      value[i] = 0x00;
    } 
}
/**************************************************************************************************
 *@函数            OLED_ShowChinese
 *
 *@简述            OLED显示汉字（纵向取模方式二）
 *
 *@输入参数        iStartX - 起点x坐标
 *                iStartY - 起点y坐标
 *                code - GBK编码数组
 *                size - 字体规格（等宽等高）
 *
 *@参数            无
 *
 *输出参数
 *
 *无
 * 
 *@返回     无
说明： 纵向取模方式二
                 列iStartY → 列iStartY+1  →  .......
行iStartX      ↓ H(字节1)      ↓ H          .
              ↓ L(字节1)      ↓ L          .
行iStartX+1  ↓ H(字节2)       ↓ H         .
   .        ↓ L(字节2)       ↓ L         .
   .        .                 .           .
   .       .                 .           .
   .      .                 .           .
 **************************************************************************************************
 */
void OLED_ShowChinese(uint8 iStartX, uint8 iStartY, uint8 *code,uint16 size)
{
    uint8 uiPixIndex = 0;
    uint8 y0 = iStartY;
    for(uint8 j = 0; j < size; j++,code++,uiPixIndex = 0)
    {
        for(uint8 k = 0; k < size; k++)
        {
            if(uiPixIndex == 8)
            {
                uiPixIndex = 0;
                code++;
            }
            OLED_SetPixel(iStartX,iStartY, OLED_GET_PAGE_BIT(*code, uiPixIndex));            
            uiPixIndex++;
            iStartY++;
        }        
        iStartY = y0;
        iStartX++;
    }
}
/**************************************************************************************************
 *@函数            OLED_ShowChar
 *
 *@简述            OLED显示一个字符
 *
*@输入参数        iStartX - 起点x坐标
 *                iStartY - 起点y坐标
 *                ascii_code - 字符的ASCII码
 *                size - 字体规格（最小1206）
 *
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
void OLED_ShowChar(uint8 iStartX, uint8 iStartY,uint8 ascii_code,uint16 size)
{
    uint8 uiPixIndex = 0;
    uint8 y0 = iStartY;
    uint8 __code const *code; //这是一个指针，指向uint8常量（const）

    if(size == 12) //如果为12号字体
    {
        code = asc2_1206[ascii_code - ' '];
    }
    else if(size == 16) //如果是16号字体
    {
        code = asc2_1608[ascii_code - ' '];
    }
   
    
    for(uint8 j = 0; j < size/2; j++,code++,uiPixIndex = 0)//列循环（字宽）（size/2表示半角）
    {
        for(uint8 k = 0; k < size; k++) //行循环（字高）
        {
            if(uiPixIndex == 8)//判断是否写完一个字节
            {
                uiPixIndex = 0;
                code++;
            }
            OLED_SetPixel(iStartX,iStartY, OLED_GET_PAGE_BIT(*code, uiPixIndex));//根据字节位判断像素填充还是清空            
            uiPixIndex++;//字节位自增
            iStartY++;//下一行
        }        
        iStartY = y0;//开始行
        iStartX++;//下一列
    }
}
/**************************************************************************************************
 *@函数            OLED_ShowNum
 *
 *@简述            OLED显示一串数字
 *
*@输入参数        iStartX - 起点x坐标
 *                iStartY - 起点y坐标
 *                num - 数字
 *                size - 字体规格（最小1206）
 *
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
void OLED_ShowNum(uint8 iStartX, uint8 iStartY,uint32 num ,uint16 size)
{
    uint16 len_str = 0;
    uint16 x;
    uint8 len = 0;
    uint8 temp = 0;
    uint32 num_len = num;
    do
    {
        num_len /= 10;
        len++;
    }while(num_len!=0);//计算数字的位数
      
    for(uint8 i = 0; i <len ; i++)
    {
        temp = (num/OLED_Pow(10,len-i-1))%10; //取出数位
        
        x = i*size/2 - len_str ;
        if(x+iStartX+size >= 127) //如果无法完整显示一个字符，移到下一行
        {
            len_str = i*size/2;
            iStartY += size; //下一行
            iStartX = 0; //初始偏移清零
            x = 0;//从第零列开始
            if(iStartY > 63) //如果下一行无法完整显示一个字符，break
            {
                break;
            }
        }
        
        OLED_ShowChar(x+iStartX,iStartY,temp+'0',size);//新字符位置    
    }
}
/**************************************************************************************************
 *@函数            OLED_Pow
 *
 *@简述            OLED幂函数
 *
*@输入参数        base - 底数
 *                index - 指数
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
uint32 OLED_Pow(uint8 base,uint8 index)
{
    uint32 num = 1;
    while(index--)
    {
        num *= base;
    }
    return num;
}
/**************************************************************************************************
 *@函数            OLED_ShowString
 *
 *@简述            OLED显示字符串
 *
*@输入参数        iStartX - 起点x坐标
 *                iStartY - 起点y坐标
 *                str - 字符串
 *                size - 字体规格（最小1206）
 *
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
void OLED_ShowString(uint8 iStartX, uint8 iStartY,uint8 *str ,uint16 size)
{
  uint8 i = 0;;
  uint16 len = 0;
  uint16 x = 0;
  while(*str!= '\0')//如果到达字符串末尾，退出循环
  {    
       x = i*size/2 - len ;
        if(x+iStartX+size >= 127) //如果无法完整显示一个字符，移到下一行
        {
            len = i*size/2;
            iStartY += size;
            iStartX = 0;
            x = 0;
            if(iStartY > 63) //如果下一行无法完整显示一个字符，break
            {
                break;
            }
        }
        OLED_ShowChar(x+iStartX,iStartY,*str,size);//新字符位置
        str++;//下一个字符串
        i++;
  }
}
/**************************************************************************************************
 *@函数            OLED_DrawLine
 *
 *@简述            OLED画一条线
 *
 *@输入参数        iStartX - 起点x坐标
 *                iStartY - 起点y坐标
 *                iEndX - 终点x坐标
 *                iEndY - 终点y坐标
 *                fill - 填充(0：不填充，1：填充)
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
void OLED_DrawLine(int16 iStartX, int16 iStartY, int16 iEndX, int16 iEndY, int16 fill)
{
    /*----------------------------------*/
    /* Variable Declaration				*/
    /*----------------------------------*/
    int16  iDx, iDy;
    int16  iIncX, iIncY;
    int16  iErrX = 0, iErrY = 0;
    int16  iDs;
    int16  iCurrentPosX, iCurrentPosY;

    /*----------------------------------*/
    /* Initialize						*/
    /*----------------------------------*/
    iErrX = 0;
    iErrY = 0;
    iDx = iEndX - iStartX; //X轴差值
    iDy = iEndY - iStartY; //Y轴差值
    iCurrentPosX = iStartX;
    iCurrentPosY = iStartY;

    if(iDx > 0) //X轴差值大于0
    {
        iIncX = 1;
    }
    else
    {
        if(iDx == 0) //X轴差值等于0
        {
            iIncX = 0;
        }
        else   //X轴差值小于0
        {
            iIncX = -1;
            iDx = -iDx; //iDx取反
        }
    }

    if(iDy > 0) //Y轴差值大于0
    {
        iIncY = 1;
    }
    else
    {
        if(iDy == 0) //Y轴差值等于0
        {
            iIncY = 0;
        }
        else    //Y轴差值小于0
        {
            iIncY = -1;
            iDy = -iDy;
        }
    }

    if(iDx > iDy) //斜率小于45°
    {
        iDs = iDx;
    }
    else     //斜率大于等于45°
    {
        iDs = iDy;
    }

    /*----------------------------------*/
    /* Process							*/
    /*----------------------------------*/
    for(uint8 i = 0; i <= iDs+1; i++)
    {
        OLED_SetPixel(iCurrentPosX,iCurrentPosY, fill);//当前位置画点
        iErrX += iDx; //X轴偏移
        if(iErrX > iDs)
        {
            iErrX -= iDs;
            iCurrentPosX += iIncX;
        }
        iErrY += iDy; //Y轴偏移
        if(iErrY > iDs)
        {
            iErrY -= iDs;
            iCurrentPosY += iIncY;
        }
    }
}
/**************************************************************************************************
 *@函数            OLED_DrawDottedLine
 *
 *@简述            OLED画虚线
 *
 *@输入参数        iStartX - 起点x坐标
 *                iStartY - 起点y坐标
 *                iEndX - 终点x坐标
 *                iEndY - 终点y坐标
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
void OLED_DrawDottedLine(int16 iStartX, int16 iStartY, int16 iEndX, int16 iEndY)
{
    /*----------------------------------*/
    /* Variable Declaration				*/
    /*----------------------------------*/
    int16  iDx, iDy;
    int16  iIncX, iIncY;
    int16  iErrX = 0, iErrY = 0;
    int16  iDs;
    int16  iCurrentPosX, iCurrentPosY;
    uint16 Dotted_piont = 1;
    bool point = 1;
    /*----------------------------------*/
    /* Initialize						*/
    /*----------------------------------*/
    iErrX = 0;
    iErrY = 0;
    iDx = iEndX - iStartX; //X轴差值
    iDy = iEndY - iStartY; //Y轴差值
    iCurrentPosX = iStartX;
    iCurrentPosY = iStartY;

    if(iDx > 0) //X轴差值大于0
    {
        iIncX = 1;
    }
    else
    {
        if(iDx == 0) //X轴差值等于0
        {
            iIncX = 0;
        }
        else   //X轴差值小于0
        {
            iIncX = -1;
            iDx = -iDx; //iDx取反
        }
    }

    if(iDy > 0) //Y轴差值大于0
    {
        iIncY = 1;
    }
    else
    {
        if(iDy == 0) //Y轴差值等于0
        {
            iIncY = 0;
        }
        else    //Y轴差值小于0
        {
            iIncY = -1;
            iDy = -iDy;
        }
    }

    if(iDx > iDy) //斜率小于45°
    {
        iDs = iDx;
    }
    else     //斜率大于等于45°
    {
        iDs = iDy;
    }

    /*----------------------------------*/
    /* Process							*/
    /*----------------------------------*/
    for(uint8 i = 0; i <= iDs+1; i++)
    {
        OLED_SetPixel(iCurrentPosX,iCurrentPosY, point);//当前位置画点
        iErrX += iDx; //X轴偏移
        if(iErrX > iDs)
        {
            iErrX -= iDs;
            iCurrentPosX += iIncX;
        }
        iErrY += iDy; //Y轴偏移
        if(iErrY > iDs)
        {
            iErrY -= iDs;
            iCurrentPosY += iIncY;
        }
        
        if(++Dotted_piont && Dotted_piont%3 == 0)//计算虚线间隔
        {
            point = !point;
        }
    }
}
/**************************************************************************************************
 *@函数            OLED_DrawCircle
 *
 *@简述            OLED画圆
 *
 *@输入参数        uiCx - 圆心x坐标
 *                uiCy - 圆心y坐标
 *                uiRadius - 半径
 *                eEdgeColor - 边缘填充(0：不填充，1：填充)
 *                eFillColor - 圆内区域填充(0：不填充，1：填充)
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
void OLED_DrawCircle(uint8 uiCx, uint8 uiCy, uint8 uiRadius, uint8 eEdgeColor, uint8 eFillColor)
{
     /*----------------------------------*/
    /* Variable Declaration				*/
    /*----------------------------------*/
    uint8 uiPosXOffset, uiPosYOffset;
    int16 uiPosXOffset_Old, uiPosYOffset_Old;
    int16 iXChange, iYChange, iRadiusError;

    /*----------------------------------*/
    /* Initialize						*/
    /*----------------------------------*/
    uiPosXOffset				= uiRadius;
    uiPosYOffset				= 0;
    uiPosXOffset_Old			= 0xFFFF;
    uiPosYOffset_Old			= 0xFFFF;
    iXChange					= 1 - 2 * uiRadius;
    iYChange					= 1;
    iRadiusError				= 0;

    /*----------------------------------*/
    /* Process							*/
    /*----------------------------------*/
    if(uiRadius < 1) //半径小于1
    {
        OLED_SetPixel(uiCx, uiCy, eEdgeColor);
    }
    else
    {
        while(uiPosXOffset >= uiPosYOffset)
        {
            if((uiPosXOffset_Old != uiPosXOffset) || (uiPosYOffset_Old != uiPosYOffset) )
            {
                // Fill the circle 填充圈圈
                if((uiRadius > 1) && (eFillColor != 2) && (uiPosXOffset_Old != uiPosXOffset))
                {

                    OLED_DrawLine(uiCx-uiPosXOffset, uiCy-uiPosYOffset+1, uiCx-uiPosXOffset, uiCy+uiPosYOffset-1, eFillColor);
                    OLED_DrawLine(uiCx+uiPosXOffset, uiCy-uiPosYOffset+1, uiCx+uiPosXOffset, uiCy+uiPosYOffset-1, eFillColor);
                    uiPosXOffset_Old = uiPosXOffset;
                }
                OLED_DrawLine(uiCx-uiPosYOffset, uiCy-uiPosXOffset+1, uiCx-uiPosYOffset, uiCy+uiPosXOffset-1, eFillColor);
                OLED_DrawLine(uiCx+uiPosYOffset, uiCy-uiPosXOffset+1, uiCx+uiPosYOffset, uiCy+uiPosXOffset-1, eFillColor);
                uiPosYOffset_Old = uiPosYOffset;

                // Draw edge.
                OLED_SetPixel(uiCx+uiPosXOffset, uiCy+uiPosYOffset, eEdgeColor);
                OLED_SetPixel(uiCx-uiPosXOffset, uiCy+uiPosYOffset, eEdgeColor);
                OLED_SetPixel(uiCx-uiPosXOffset, uiCy-uiPosYOffset, eEdgeColor);
                OLED_SetPixel(uiCx+uiPosXOffset, uiCy-uiPosYOffset, eEdgeColor);
                OLED_SetPixel(uiCx+uiPosYOffset, uiCy+uiPosXOffset, eEdgeColor);
                OLED_SetPixel(uiCx-uiPosYOffset, uiCy+uiPosXOffset, eEdgeColor);
                OLED_SetPixel(uiCx-uiPosYOffset, uiCy-uiPosXOffset, eEdgeColor);
                OLED_SetPixel(uiCx+uiPosYOffset, uiCy-uiPosXOffset, eEdgeColor);
            }
            uiPosYOffset++;
            iRadiusError += iYChange;
            iYChange += 2;
            if ((2 * iRadiusError + iXChange) > 0)
            {
                uiPosXOffset--;
                iRadiusError += iXChange;
                iXChange += 2;
            }
        }
    }
}
/**************************************************************************************************
 *@函数            OLED_DrawRectangle
 *
 *@简述            OLED画矩形
 *
 *@输入参数        uiStartX - 起点x坐标
 *                uiStartY - 起点y坐标
 *                uiWidth - 宽度
 *                uiHeight - 高度
 *                eEdgeColor - 边缘填充(0：不填充，1：填充)
 *                eFillColor - 圆内区域填充(0：不填充，1：填充)
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
void OLED_DrawRectangle(uint16 uiStartX, uint16 uiStartY, uint16 uiWidth, uint16 uiHeight, uint8 eEdgeColor, uint8 eFillColor)
{
    uint16 uiColumnIndex;

    if((uiWidth > 0) && (uiHeight > 0))
    {
        if((uiWidth == 1) && (uiHeight == 1))
        {
            OLED_SetPixel(uiStartX, uiStartY, eEdgeColor);
        }
        else if(uiWidth == 1)
        {
            OLED_DrawLine(uiStartX, uiStartY, uiStartX, uiStartY+uiHeight-1, eEdgeColor);
        }
        else if(uiHeight == 1)
        {
            OLED_DrawLine(uiStartX, uiStartY, uiStartX+uiWidth-1, uiStartY, eEdgeColor);
        }
        else
        {
            // Draw edge.
            // Check and set changed page and column index is in edge display action.画四条线构成矩形
            OLED_DrawLine(uiStartX, uiStartY, uiStartX, uiStartY+uiHeight-1, eEdgeColor);
            OLED_DrawLine(uiStartX+uiWidth-1, uiStartY, uiStartX+uiWidth-1, uiStartY+uiHeight-1, eEdgeColor);
            OLED_DrawLine(uiStartX, uiStartY, uiStartX+uiWidth-1, uiStartY, eEdgeColor);
            OLED_DrawLine(uiStartX, uiStartY+uiHeight-1, uiStartX+uiWidth-1, uiStartY+uiHeight-1, eEdgeColor);
            // Fill area.
            if((eFillColor != 2) && (uiWidth > 2) && (uiHeight > 2))
            {
                for(uiColumnIndex=(uiStartX+1); uiColumnIndex<(uiStartX+uiWidth-1); uiColumnIndex++)
                {
                    OLED_DrawLine(uiColumnIndex, uiStartY+1, uiColumnIndex, uiStartY+uiHeight-2, eFillColor);
                }
            }
        }
    }
}
/**************************************************************************************************
 *@函数            OLED_DrawEllipse
 *
 *@简述            OLED画椭圆
 *
 *@输入参数        uiCx - 圆心x坐标
 *                uiCy - 圆心y坐标
 *                a - 横长
 *                b - 纵长
 *                fill - 边缘填充(0：不填充，1：填充)
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
void OLED_DrawEllipse(uint8 uiCx,uint8 uiCy,int a,int b,uint8 fill)    //中点Bresenham算法画椭圆
{
    int x,y;
    float d1,d2;
    x = 0;y = b;
    d1=b*b+a*a*(-b+0.25);

    OLED_SetPixel(uiCx+x,uiCy+y,fill);
    OLED_SetPixel(uiCx-x,uiCy-y,fill);
    OLED_SetPixel(uiCx-x,uiCy+y,fill);
    OLED_SetPixel(uiCx+x,uiCy-y,fill);
 
    while(b*b*(x+1) < a*a*(y-0.5)){
        if(d1<=0){
            d1+=b*b*(2*x+3);
            x++;
        }
        else{
            d1+=b*b*(2*x+3)+a*a*(-2*y+2);
            x++;
            y--;
        }
        OLED_SetPixel(uiCx+x,uiCy+y,fill);
        OLED_SetPixel(uiCx-x,uiCy-y,fill);
        OLED_SetPixel(uiCx-x,uiCy+y,fill);
        OLED_SetPixel(uiCx+x,uiCy-y,fill);
    }
    d2=b*b*(x+0.5)*(x+0.5)+a*a*(y-1)*(y-1)-a*a*b*b;
    while(y>0){
        if(d2<=0){
            d2+=b*b*(2*x+2)+a*a*(-2*y+3);
            x++,y--;
        }
        else{
            d2+=a*a*(-2*y+3);
            y--;
        }
        OLED_SetPixel(uiCx+x,uiCy+y,fill);
        OLED_SetPixel(uiCx-x,uiCy-y,fill);
        OLED_SetPixel(uiCx-x,uiCy+y,fill);
        OLED_SetPixel(uiCx+x,uiCy-y,fill);
    }
}
/**************************************************************************************************
 *@函数            OLED_Drawcos
 *
 *@简述            OLED画cos图(一个周期)
 *
 *@输入参数        uiStartX - 起点x坐标
 *                uiStartY - 起点y坐标
 *                EnlargeX - x放大系数
 *                EnlargeY - y放大系数
 *                fill - 边缘填充(0：不填充，1：填充)
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
void OLED_Drawcos(uint8 uiStartX,uint8 uiStartY,uint8 EnlargeX,uint8 EnlargeY,uint8 fill)
{
    double y;                   
    int x;                   
    for(y = 1;y >= -1;y -= 0.05) 
    {
        x=(int)(acos(y)*EnlargeX);       
        OLED_SetPixel(x+uiStartX,(int)((1-y)*EnlargeY)+uiStartY,fill);  //0-π的值
        OLED_SetPixel((int)(2*PI*EnlargeX)-x+uiStartX,(int)((1-y)*EnlargeY)+uiStartY,fill);  
    }
    OLED_SetPixel((int)(PI*EnlargeX)+uiStartX,2*EnlargeY+uiStartY,1);
}
/**************************************************************************************************
 *@函数            OLED_DrawSin
 *
 *@简述            OLED画sin图(一个周期)
 *
 *@输入参数        uiStartX - 起点x坐标
 *                uiStartY - 起点y坐标
 *                EnlargeX - x放大系数
 *                EnlargeY - y放大系数
 *                fill - 边缘填充(0：不填充，1：填充)
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
void OLED_DrawSin(uint8 uiStartX,uint8 uiStartY,uint8 EnlargeX,uint8 EnlargeY,uint8 fill)
{   
    double y;                  
    int x;                    
    for(y = 1;y >= 0;y -= 0.05)  
    {
        x=(int)(asin(y)*EnlargeX);    
        OLED_SetPixel(x+uiStartX,(int)((1-y)*EnlargeY)+uiStartY,fill);
        OLED_SetPixel((int)(PI*EnlargeX-x)+uiStartX,(int)((1-y)*EnlargeY)+uiStartY,fill);
    }
    for(y = 0;y >= -1;y -= 0.05)
    {  
        x=(int)(asin(-y)*EnlargeX);
        OLED_SetPixel((int)(PI*EnlargeX+x)+uiStartX,(int)((-y+1)*EnlargeY)+uiStartY,fill);
        OLED_SetPixel((int)(2*PI*EnlargeX-x)+uiStartX,(int)((-y+1)*EnlargeY)+uiStartY,fill);
    }
    OLED_SetPixel((int)(PI*EnlargeX+(double)1/2*PI*EnlargeX)+uiStartX,2*EnlargeY+uiStartY,fill);
}

//画长方体
void drawCuboid(uint8 x,uint8 y,uint8 width,uint8 height,uint8 depth)
{
    OLED_DrawRectangle(x,y,width,height,1,0);//前面矩形
    OLED_DrawRectangle(x-depth/2,y+depth/2,width,height,1,0);//后面矩形
  
    OLED_DrawDottedLine(x,y,x,y+height-1);//虚线(高)
    OLED_DrawDottedLine(x,y+height-1,x-depth/2,y+depth/2+height-1);//虚线(深)
    OLED_DrawDottedLine(x,y+height-1,x+width-1,y+height-1);//虚线(宽)
    
    OLED_DrawLine(x,y,x-depth/2,y+depth/2,1);//实线(深)
    OLED_DrawLine(x+width-1,y,x-depth/2+width-1,y+depth/2,1);//实线(深)
    OLED_DrawLine(x+width-1,y+height-1,x-depth/2+width-1,y+depth/2+height-1,1);//实线(深)
}