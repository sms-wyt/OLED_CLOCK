#ifndef SD_H
#define SD_H

#include "hal_types.h"


//相关宏定义
//-------------------------------------------------------------
#define SD_VER_ERR     0X00		 // 无法识别的版本
#define SD_VER_MMC     0X01		 // 版本号：MMC
#define SD_VER_V1      0X02		 // 版本号：SD1.0
#define SD_VER_V2      0X03	         //版本号：SD2.0(普通SD)
#define SD_VER_V2HC    0X04		 //版本号：SD2.0(高容量SDHC)

#define INIT_ERROR                  0x01 //初始化错误
#define INIT_CMD0_ERROR             0x02 //CMD0错误
#define INIT_CMD1_ERROR             0x03 //CMD1错误
#define INIT_SDV2_ACMD41_ERROR	    0x04 //ACMD41错误
#define INIT_SDV1_ACMD41_ERROR	    0x05 //ACMD41错误

#define WRITE_CMD24_ERROR           0x06 //写块时产生CMD24错误
#define WRITE_BLOCK_ERROR           0x07 //写块错误

#define READ_BLOCK_ERROR            0x08 //读块错误

#define WRITE_CMD25_ERROR           0x09 //在连续多块写时产生CMD25错误
#define WRITE_NBLOCK_ERROR          0x0A //连续多块写失败

#define READ_CMD18_ERROR            0x0B //在连续多块读时产生CMD18错误
 
#define GET_CSD_ERROR               0x0C //读CSD失败



static void SD_Select(void);
static void SD_Deselect(void);
extern uint8 SD_Init(void);
static uint8 SD_Write_Cmd(uint8 *cmd);
static void SD_add_sclk(void);
extern uint8 SD_Read_Sector(uint8 *data, uint32 addr);
extern uint8 SD_Read_nSector(uint8 *data, uint32 addr, uint32 nsec);
extern uint8 SD_Write_Sector(const uint8 *data, uint32 addr);
extern uint8 SD_Write_nSector(const uint8 *data, uint32 addr, uint32 nsec);
extern uint32 SD_GetTotalSec(void);
#endif