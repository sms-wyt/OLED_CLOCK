/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "SD.h"

/******************************使用示例**********************************/

/************************获取扇区数和扇区大小*****************************
  uint32 SumSec;
  uint32 SecSiZe;
  disk_initialize(0);//初始化SD卡
  disk_ioctl(0, GET_SECTOR_COUNT, &SumSec);//获取SD卡全部扇区数
  disk_ioctl(0, GET_SECTOR_SIZE, &SecSiZe);//获取SD卡扇区大小
  printfs("SumSec=%ld\r\n",SumSec);
  printfs("SecSiZe=%ld\r\n",SecSiZe);

**************************创建 测试文件.txt*******************************
 FIL  filescr1;
 UINT bw; 
 FATFS  fatfs;
 disk_initialize(0);//初始化SD卡
 f_mount(&fatfs,"0:",1);//挂载SD卡作为磁盘
 f_open(&filescr1, "0:/测试文件.txt", FA_CREATE_NEW|FA_WRITE|FA_READ);//创建打开测试文件.txt
 f_write(&filescr1, "Hello,World!", 12, &bw);//往测试文件.txt写入Hello,World!
 f_close(&filescr1);//关闭文件
**************************************************************************/
/**************************************************************************************************
 *@函数            disk_status
 *
 *@简述            获取磁盘状态
 *
 *@输入参数  
 *
 *@参数            pdrv - 磁盘序号
 *                       
 *输出参数
 *
 *无
 * 
 *@返回     初始化状态
 **************************************************************************************************
 */
DSTATUS disk_status (BYTE pdrv)
{
   return 0;
}
/**************************************************************************************************
 *@函数            disk_initialize
 *
 *@简述            初始化磁盘驱动器
 *
 *@输入参数  
 *
 *@参数            pdrv - 磁盘序号
 *                       
 *输出参数
 *
 *无
 * 
 *@返回     初始化状态
 **************************************************************************************************
 */
DSTATUS disk_initialize (BYTE pdrv)
{
  DSTATUS stat;
  stat = SD_Init();
  if(stat == 0)
     return RES_OK;
  else
     return RES_NOTRDY;
}
/**************************************************************************************************
 *@函数            disk_read
 *
 *@简述            磁盘读取
 *
 *@输入参数  
 *
 *@参数            pdrv - 磁盘序号
 *                 buff - 数据指针
 *                 sector - 起始扇区
 *                 count - 读取数量
 *                     
 *输出参数
 *
 *无
 * 
 *@返回     状态
 **************************************************************************************************
 */
DRESULT disk_read (BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{	
        uint8 result;
        if(!count)
          return RES_PARERR;
        if(count == 1)
        {
         result = SD_Read_Sector(buff, sector);
        }
        else
        {
          //for(uint32 i = 0; i < count; i++)
          //{
          //  SD_Read_Sector(buff, sector+i);
          //  buff += 512;
          //}
          //result = 0;
           result = SD_Read_nSector(buff, sector, count);
        }
        if(result == 0)
          return RES_OK;
        else
          return RES_ERROR;
}
/**************************************************************************************************
 *@函数            disk_write
 *
 *@简述            磁盘写入
 *
 *@输入参数  
 *
 *@参数            pdrv - 磁盘序号
 *                 buff - 数据指针
 *                 sector - 起始扇区
 *                 count - 读取数量
 *                     
 *输出参数
 *
 *无
 * 
 *@返回     状态
 **************************************************************************************************
 */
#if FF_FS_READONLY == 0  //只读宏开关 0：读/写，1：只读
DRESULT disk_write (BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
	uint8 result;
        if(count == 1)
        {
          result = SD_Write_Sector(buff, sector);
        }
        else
        {
           result = SD_Write_nSector(buff, sector, count);
           //for(uint32 i = 0; i < count; i++)
           //{
           //   SD_Write_Sector(buff+i, sector);
            //  buff += 512;
           //}
           //result = 0;
        }
        if(result == 0)
          return RES_OK;
        else
          return RES_ERROR;
}

#endif
/**************************************************************************************************
 *@函数            disk_ioctl
 *
 *@简述            磁盘相关控制属性
 *
 *@输入参数  
 *
 *@参数            pdrv - 磁盘序号
 *                 cmd - 命令
 *                 buff - 命令返回数据
 *                     
 *输出参数
 *
 *无
 * 
 *@返回     状态
 **************************************************************************************************
 */
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void *buff)
{
        DRESULT res;
        switch(cmd)
        { 
          case CTRL_SYNC:
		res = RES_OK;
             break;          
          case GET_SECTOR_SIZE://获取扇区大小,一般有512、1024、2048、4096
              *(WORD*)buff = 512;
              res = RES_OK;
            break;
            
          case GET_BLOCK_SIZE://获取总大小
            // *(WORD*)buff = 512;
             *(DWORD*)buff = SD_GetTotalSec();
             res = RES_OK;
            break;
          case GET_SECTOR_COUNT:
             *(DWORD*)buff = SD_GetTotalSec();
             res = RES_OK;
            break;
          default:
            res = RES_PARERR;
            break;
        }
        return res;
}