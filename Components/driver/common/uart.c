
/********************include******************/
#include "uart.h"
#include "stdarg.h"  //由于使用这个库，会额外占1820字节的RAM
#include "stdio.h"
#include "stdlib.h"
#include "ioSTC8.h"
#include "heap_manage.h"
/**********************************************/

/**********************************************
名称：uart_init
功能：串口初始化。内部24MHz，波特率9600
参数：void
返回：void
**********************************************/
void uart_init(void)
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设定定时器1为16位自动重装方式
	TL1 = 0xCC;		//设定定时初值
	TH1 = 0xFF;		//设定定时初值
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1
}

/**********************************************
名称：Uart_write
功能：串口发送函数
参数：*pBuf--> 待发送数据指针  	len-->数据长度
返回：void
**********************************************/
void Uart_write(uint8 *pBuf,int ulLen)
{
 	int i;
    for(i = 0; i < ulLen; i++)
    {
       SBUF=*pBuf;  //数据加载到发送缓冲区
       while(!TI);  //等待发送完成         
	   TI=0;		//发送标志清零
	   pBuf++;     //数据指针指向下一组数据 
    }
}

/*********************************************
名称：printfs
功能：类似printf串口打印函数(采用数组缓存数据)
参数：fmt-->不定参数标志位。如%d，%s等
返回：发送数据长度
注意：在IAR平台正常使用；
但在keil平台，如果输出为以下类型，需要改为动，否则数据不正常。
显示long类型 %d ===> %ld
显示char类型 %d ===> %bd

*********************************************/

int printfs(char*fmt,...)
{  
    char  pBuf[50];
    int ulen;
    va_list ap;
    va_start(ap, fmt);
    ulen = vsprintf(pBuf, fmt, ap);
	va_end(ap);
	Uart_write((uint8*)pBuf, ulen);
    return ulen;
}	 


/*********************************************
名称：printfd
功能：类似printf串口打印函数(采用动态申请空间缓存数据)
参数：fmt-->不定参数标志位。如%d，%s等
返回：发送数据长度
注意：在IAR平台正常使用；
但在keil平台，如果输出为以下类型，需要改为动，否则数据不正常。
显示long类型 %d ===> %ld
显示char类型 %d ===> %bd
*********************************************/ 

int printfd(char*fmt,...)
{  
    int ulen;
    va_list  ap;
    char *pBuf;
    pBuf = (char*)heap_alloc(60);
    if(pBuf==NULL)
      return 0;
    va_start(ap, fmt);
    ulen = vsprintf(pBuf, fmt, ap);
    va_end(ap);
    Uart_write((uint8*)pBuf, ulen);
    heap_free(pBuf);
    pBuf = NULL;
    return ulen;

}  	  
