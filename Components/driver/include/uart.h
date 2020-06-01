#ifndef UART_H
#define UART_H

/*************INCLUDE*****************/
#include "type.h"
#include "hal_types.h"
/*************************************/

/***********头文件声明***************/
extern void Uart_write(uint8 *pBuf,int ulLen);
extern void uart_init(void);
extern int printfs(char*fmt,...);
extern int printfd(char*fmt,...);
/************************************/

#endif