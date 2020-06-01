#include "SD.h"
#include "spi_hard.h"
#include "ioSTC8.h"
#include "diskio.h"
#include "uart.h"
#include "system_clock.h"

#define SD_CS  P2_1 //SD卡片选
 

uint8 Addr_Mode=0;        // 寻址方式，1为块寻址，0为字节寻址（使用时需将地址左移9位）
uint8 SD_Ver=SD_VER_ERR;  // SD卡的版本

/***********************SD卡读写测试********************************
uint8 badapple[1024];
uint8 *point_data;
point_data = badapple;
disk_initialize(0);//初始化SD卡
//512字节 多块写
SD_Write_nSector(point_data,1136976,2);
//512字节单块写
SD_Write_Sector(point_data, 1136976);
point_data += 512;
SD_Write_Sector(point_data, 1136977);
//512字节 多块读
SD_Read_nSector(point_data,1136976,2);
//512字节单块读
SD_Read_Sector(point_data,1136976);
point_data += 512;
SD_Read_Sector(point_data,1136977);
******************************************************************/
/**************************************************************************************************
 *@函数            SD_Select
 *
 *@简述            使能SD卡片选
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
static void SD_Select(void)
{
  SD_CS = 0;
}
/**************************************************************************************************
 *@函数            SD_Deselect
 *
 *@简述            失能SD卡片选
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
static void SD_Deselect(void)
{
  SD_CS = 1;
  SD_add_sclk();//8个空时钟
}
/**************************************************************************************************
 *@函数            SD_add_sclk
 *
 *@简述            硬件SPI产生8个空时钟
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
static void SD_add_sclk(void)
{
  uint8 temp = 0xff;
  hard_spi_transaction(&temp, 1, NULL, 0);
}
/**************************************************************************************************
 *@函数            SD_Init
 *
 *@简述            SD卡初始化
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

uint8 SD_Init(void)
{
  uint8 CMD0[6] = {0x40,0x00,0x00,0x00,0x00,0x95};//CMD0,复位SD卡，并且切换为SPI模式(0x95为CRC校验）
  uint8 CMD1[6] ={0x41,0x00,0x00,0x00,0x00,0x01}; //CMD1，MMC卡使用CMD1命令进行初始化
  uint8 CMD8[6] ={0x48,0x00,0x00,0x01,0xAA,0x87}; //CMD8，用于鉴别SD卡的版本，并可从应答得知SD卡的工作电压
  uint8 CMD16[6]={0x50,0x00,0x00,0x02,0x00,0x01}; //CMD16，设置扇区大小为512字节，此命令用于在初始化完成之后进行试探性的操作，
  uint8 CMD55[6]={0x77,0x00,0x00,0x00,0x00,0x01}; //CMD55，用于告知SD卡后面是ACMD，即应用层命令 CMD55+ACMD41配合使用
  uint8 ACMD41H[6]={0x69,0x40,0x00,0x00,0x00,0x01}; //ACMD41,此命令用于检测SD卡是否初始化完成，MMC卡，不适用此命令，针对2.0的SD卡
  uint8 ACMD41S[6]={0x69,0x00,0x00,0x00,0x00,0x01}; //ACMD41,此命令用于检测SD卡是否初始化完成，MMC卡，不适用此命令，针对1.0的SD卡
  uint8 CMD58[6]={0x7A,0x00,0x00,0x00,0x00,0x01}; //CMD58，用于鉴别SD2.0到底是SDHC，还是普通的SD卡，二者对扇区地址的寻址方式不同

  uint8 value[4],temp;
  uint16 times = 5000;//最大发送次数

  clock_info pre_clock;
  pre_clock = get_system_clock_source();//记录当前的时钟源和分频系数
  set_system_clock_source(IRC24M_CLOCK,6);//选择内部RC高频振荡器作为主时钟源、6分频
  set_irc_freq_adjust(0,0,0);//主时钟频率设为16M


  SPCTL |= 0x03;//硬件SPI时钟为主时钟32分频
  SD_Select();//使能SD卡片选
/************************复位***********************************/  
  for(uint8 i = 0; i < 10; i++) // 首先要发送最少74个时钟信号，这是必须的！激活SD卡
  {
    SD_add_sclk();
  }
  
  do
  {
    temp = SD_Write_Cmd(CMD0);//发送CMD0指令，让SD卡切换为SPI模式
    if(--times == 0)//发送次数达到最大值
    {
      SD_Deselect();//失能SD卡片选
      return INIT_CMD0_ERROR;//返回CMD0指令无回应
    }
  }while(temp != 0x01);//判断SD卡回应是否为0x01，否则重复发送
/*************************************************************/  
  
  temp = SD_Write_Cmd(CMD8);//写入CMD8，读取SD卡回应

  if(temp ==0x01) //判断CMD8回应是否为0x01，SD卡版本为2.0
  {
    hard_spi_transaction(NULL, 0, value, 4);//读取4个字节的R7回应，通过它可知此SD卡是否支持2.7~3.6V的工作电压
    if(value[2] == 0x01 && value[3] == 0xaa)//SD卡是否支持2.7~3.6V,条件成立则支持-------------------
    {
          times = 39000;
      do
      {
        //这里需要反复发送CMD55和ADCMD41，大概0.12S-0.3s后SD卡才回应0x00
          SD_Write_Cmd(CMD55);// 写入CMD55
          temp = SD_Write_Cmd(ACMD41H);// 写入ACMD41，针对SD2.0
          if(--times == 0)//发送次数达到最大值
          {
            SD_Deselect();//失能SD卡片选 
            return(INIT_SDV2_ACMD41_ERROR);//对SD2.0使用ACMD41进行初始化时产生错误
          }
      }while(temp!=0);//判断SD卡回应是否为0x00，否则重复发送
     
        
      if(0==SD_Write_Cmd(CMD58))//写入CMD58，开始鉴别SD2.0
      {  
        //SDHC存储卡容量为“4GB-32GB”。
        hard_spi_transaction(NULL, 0, value, 4);//读取4个字节的OCR，其中CCS指明了是SDHC还是普通的SD
        if(value[0] & 0x40)
        {
           SD_Ver=SD_VER_V2HC; //SDHC卡	
	   Addr_Mode=1; //SDHC卡为扇区寻址方式
        }
        else
        {
          SD_Ver=SD_VER_V2; //普通的SD卡，2.0的卡包含SDHC和一些普通的卡，字节寻址方式	
        }
      }
    }
  }
    else // SD V1.0或MMC
    {
      // SD卡使用ACMD41进行初始化，而MMC使用CMD1来进行初始化，依此来进一步判断是SD还是MMC
       SD_Write_Cmd(CMD55);            // 写入CMD55
       temp=SD_Write_Cmd(ACMD41S);     // 写入ACMD41，针对SD1.0
       if(temp <= 1)//检查返回值是否正确，如果正确，说明ACMD41命令被接受，即为SD卡
       {
         SD_Ver=SD_VER_V1; //普通的SD1.0卡，一般来说容量不会超过2G
         times = 500;
         do
         {
           SD_Write_Cmd(CMD55);//写入CMD55
           temp=SD_Write_Cmd(ACMD41S);//写入ACMD41，针对SD1.0
           if(--times == 0)
           {
             SD_Deselect();//失能SD卡片选 
             return(INIT_SDV1_ACMD41_ERROR);//对SD1.0使用ACMD41进行初始化时产生错误
           }
         }while(temp!=0);
       }
       else//否则为MMC
       {
         SD_Ver=SD_VER_MMC; //MMC卡，它不支持ACMD41命令，而是使用CMD1进行初始化
         times=500;
         do
         {
           temp=SD_Write_Cmd(CMD1);//写入CMD1
           if(--times == 0)
           {
             SD_Deselect();//失能SD卡片选
             return(INIT_CMD1_ERROR);//MMC卡使用CMD1命令进行初始化中产生错误
           }
         }while(temp!=0);
       }
    }
    if(0 != SD_Write_Cmd(CMD16))//SD卡的块大小必须为512字节
    {
      SD_Ver=SD_VER_ERR; //如果不成功，则此卡为无法识别的卡
      SD_Deselect();//失能SD卡片选
      return INIT_ERROR;
    }
       
  SD_Deselect();//失能SD卡片选
  SPCTL &= 0xfc;//硬件SPI时钟为主时钟4分频
  
  set_system_clock_source(pre_clock.clock_source,pre_clock.clock_div);//恢复之前的时钟源和分频系数
  set_irc_freq_adjust(0,0,ORIGINAL_ICR_FREQ_ADJUST);//恢复之前的频率
  return 0;
}
/**************************************************************************************************
 *@函数            SD_Write_Cmd
 *
 *@简述            SD卡写命令
 *
 *@输入参数  
 *
 *@参数            *cmd - 命令
 *                       
 *输出参数
 *
 *无
 * 
 *@返回     0xff - 不成功，其他则为成功
 **************************************************************************************************
 */

static uint8 SD_Write_Cmd(uint8 *cmd)
{
  uint8 temp;
  uint16 times;
  temp = 0xff;
  times = 500;
  
 // SD_Deselect();//提高兼容性
  SD_Select();
 do
 {
   hard_spi_transaction(NULL, 0, &temp, 1);
 }while(temp != 0xff);//等待SD卡准备好，再向其发送命令
 
   hard_spi_transaction(cmd, 6, NULL, 0);//发送6个字节的命令数据

  do
  {
    hard_spi_transaction(NULL, 0, &temp, 1);
    times--;
  }while(temp == 0xff && times > 0);

  return temp;
}

/**************************************************************************************************
 *@函数            SD_Read_Sector
 *
 *@简述            SD卡读取一个扇区
 *
 *@输入参数  
 *
 *@参数            *data - 扇区数据
 *                 addr - 地址
 *                        
 *输出参数
 *
 *无
 * 
 *@返回     无
 **************************************************************************************************
*/ 
uint8 SD_Read_Sector(uint8 *data, uint32 addr)
{
  uint8 temp;
  uint16 times = 500;
  uint8 crc[2];
  uint8 cmd17[] = {0x51,0x00,0x00,0x00,0x00,0xff};

 if(!Addr_Mode)
 {
    addr <<= 9;//将块地址（扇区地址）转为字节地址，乘以512
 }
 cmd17[1]=addr>>24; //将字节地址写入到CMD17字节序列中
 cmd17[2]=addr>>16;
 cmd17[3]=addr>>8;
 cmd17[4]=addr;	
  SD_Select();
  do
  {
    temp = SD_Write_Cmd(cmd17);    
      if(--times == 0)
      { 
         SD_Deselect();//失能SD卡片选
        return(READ_BLOCK_ERROR);//读块失败
      }
  }while(temp != 0);
  do
  {
    hard_spi_transaction(NULL, 0, &temp, 1);
  }while(temp != 0xfe);//当读到0xfe时，表示后面是扇区数据
  hard_spi_transaction(NULL, 0, data, 512);//读取扇区数据
  hard_spi_transaction(NULL, 0, crc, 2);//读取2位CRC校验码
  SD_Deselect();
  return 0;
}
/**************************************************************************************************
 *@函数            SD_Read_nSector
 *
 *@简述            SD卡读取多个扇区
 *
 *@输入参数  
 *
 *@参数            *data - 扇区数据
 *                 addr - 地址
 *                        
 *输出参数
 *
 *无
 * 
 *@返回     无
 **************************************************************************************************
*/ 
uint8 SD_Read_nSector(uint8 *data, uint32 addr, uint32 nsec)
{
  uint8 temp;	
  uint8 crc[2];
  uint16 times = 5000;
  uint8 CMD18[6]={0x52,0x00,0x00,0x00,0x00,0x01}; //CMD18的字节序列
  uint8 CMD12[6]={0x4C,0x00,0x00,0x00,0x00,0x01}; //CMD12，强制停止命令	  
  if(!Addr_Mode) 
  {
    addr<<=9;
  }
  
  CMD18[1]=addr>>24; //将字节地址写入到CMD17字节序列中
  CMD18[2]=addr>>16;
  CMD18[3]=addr>>8;
  CMD18[4]=addr;
  
  SD_Select();
  do
 {  
    temp=SD_Write_Cmd(CMD18);
    if(--times == 0) 
    { 
       SD_Deselect();//失能SD卡片选
	return(READ_CMD18_ERROR); //命令写入失败
    }
  }while(temp!=0);
  for(uint32 i = 0; i < nsec; i++)
  {
     do
    {
      hard_spi_transaction(NULL, 0, &temp, 1);
    }while(temp != 0xfe);//当读到0xfe时，表示后面是扇区数据
    hard_spi_transaction(NULL, 0, data, 512);//读取扇区数据
    hard_spi_transaction(NULL, 0, crc, 2);//读取2位CRC校验码
    data += 512;
  }
  SD_Write_Cmd(CMD12);//写入CMD12命令，停止数据读取
 
  SD_Deselect();//失能SD卡 
  return 0;
}
/**************************************************************************************************
 *@函数            SD_Write_Sector
 *
 *@简述            SD卡写入一个扇区
 *
 *@输入参数  
 *
 *@参数            *data - 扇区数据
 *                 addr - 地址
 *                        
 *输出参数
 *
 *无
 * 
 *@返回     0 - 成功    WRITE_BLOCK_ERROR - 失败
 **************************************************************************************************
*/
uint8 SD_Write_Sector(const uint8 *data, uint32 addr)
{
  uint8 temp = 0xfe;
  uint8 crc[2]={0xff,0xff};
  uint16 times = 500;
  uint8 cmd24[] = {0x58,0x00,0x00,0x00,0x00,0xff};
  if(!Addr_Mode)
    addr <<= 9;//将块地址（扇区地址）转为字节地址，乘以512
  cmd24[1]=addr>>24; //将字节地址写入到CMD17字节序列中
  cmd24[2]=addr>>16;
  cmd24[3]=addr>>8;
  cmd24[4]=addr;	
  SD_Select();
  do
  {
    temp = SD_Write_Cmd(cmd24);    
      if(--times == 0)
      {
         SD_Deselect();//失能SD卡片选
        return(WRITE_BLOCK_ERROR);//写块失败
      }
  }while(temp != 0); 
 
  temp = 0xfe;
  hard_spi_transaction(&temp, 1, NULL, 0);//写0xfe，表示后面是扇区数据
  hard_spi_transaction(data, 512, NULL, 0);////写入扇区数据
  hard_spi_transaction(crc, 2, NULL, 0);////写入crc校验
  hard_spi_transaction(NULL, 0, &temp, 1);//读取数据
  if((temp & 0x1F)!=0x05) //判断SD卡是否接受数据
  {
    return(WRITE_BLOCK_ERROR); //写块数据失败
  }
  do
  {
    hard_spi_transaction(NULL, 0, &temp, 1);//读取数据
  }while(temp != 0xff);//等待SD卡存储数据
  SD_Deselect();
  return 0;
}
/**************************************************************************************************
 *@函数            SD_Write_nSector
 *
 *@简述            连续写入多个扇区
 *
 *@输入参数  
 *
 *@参数            无
 *                        
 *输出参数
 *
 *无
 * 
 *@返回     Capacity - 扇区数量 
 **************************************************************************************************
*/
uint8 SD_Write_nSector(const uint8 *data, uint32 addr, uint32 nsec)
{
  uint8 temp;
  uint8 crc[2] = {0xff,0xff};
  uint16 times = 500;
  uint8 CMD25[6]={0x59,0x00,0x00,0x00,0x00,0x01}; //CMD25用于完成多块连续写
  uint8 CMD55[6]={0x77,0x00,0x00,0x00,0x00,0x01}; //CMD55，用于告知SD卡后面是ACMD,CMD55+ACMD23
  uint8 ACMD23[6]={0x57,0x00,0x00,0x00,0x00,0x01};//CMD23，多块连续预擦除
  if(!Addr_Mode) 
  {
    addr<<=9;
  }
  CMD25[1]=addr>>24;
  CMD25[2]=addr>>16;
  CMD25[3]=addr>>8;
  CMD25[4]=addr;
  
  ACMD23[1]=nsec>>24;
  ACMD23[2]=nsec>>16;
  ACMD23[3]=nsec>>8;
  ACMD23[4]=nsec;
  
  SD_Select();
  if(SD_Ver!=SD_VER_MMC) //如果不是MMC卡，则首先写入预擦除命令，CMD55+ACMD23，这样后面的连续块写的速度会更快
  {
     SD_Write_Cmd(CMD55);
     SD_Write_Cmd(ACMD23);
  }
  do
 {  
    temp=SD_Write_Cmd(CMD25);
    if(--times == 0) 
    { 
       SD_Deselect();//失能SD卡片选
       return(WRITE_CMD25_ERROR); //命令写入失败
    }
  }while(temp!=0);
   do
  {
    hard_spi_transaction(NULL, 0, &temp, 1);//读取数据
  }while(temp != 0xff);//等待SD卡存储数据
  for(uint32 i = 0; i < nsec; i++)
  {
     temp = 0xfc;
     hard_spi_transaction(&temp, 1, NULL, 0);//写0xfc，表示后面是扇区数据
     hard_spi_transaction(data, 512, NULL, 0);////写入扇区数据
     hard_spi_transaction(crc, 2, NULL, 0);////写入crc校验
     hard_spi_transaction(NULL, 0, &temp, 1);//读取数据
     if((temp & 0x1F)!=0x05) //如果返回值是 XXX00DELAY_TIME1 说明数据已经被SD卡接受了
    {
       return(WRITE_NBLOCK_ERROR); //写块数据失败
    }
    data += 512;
    do
    {
       hard_spi_transaction(NULL, 0, &temp, 1);//读取数据
    }while(temp != 0xff);//等待SD卡存储数据   
  }
  temp = 0xfd;
  hard_spi_transaction(&temp, 1, NULL, 0);//写0xfd,表示停止多字节写
  do
    {
       hard_spi_transaction(NULL, 0, &temp, 1);//读取数据
    }while(temp != 0xff);//等待SD卡存储数据  
  SD_Deselect();//失能SD卡 
  return 0;
}
/**************************************************************************************************
 *@函数            SD_GetTotalSec
 *
 *@简述            获取全部扇区数量
 *
 *@输入参数  
 *
 *@参数            无
 *                        
 *输出参数
 *
 *无
 * 
 *@返回     Capacity - 扇区数量 
 **************************************************************************************************
*/
uint32 SD_GetTotalSec(void)
{
  uint8 n,temp;
  uint8 CSD[16],crc[2];
  uint32 Capacity;  
  uint16 csize; 	
  uint8 CMD9[6]={0x49,0x00,0x00,0x00,0x00,0x01}; //CMD9
  SD_Select();
  if(SD_Write_Cmd(CMD9)!=0) //写入CMD9命令
	{
		return GET_CSD_ERROR; //获取CSD时产生错误
	} 
  do
  {
     hard_spi_transaction(NULL, 0, &temp, 1);//读取数据
  }while(temp != 0xFE); //当读到0xfe时，说明后面的是16字节的CSD数据
  hard_spi_transaction(NULL, 0, CSD, 16);//读取数据 
  hard_spi_transaction(NULL, 0, crc, 2);//读取2个CRC校验
  SD_Deselect();//失能SD卡
  //如果为SDHC卡，即大容量卡，按照下面方式计算
	if((CSD[0]&0xC0)==0x40)	 //SD2.0的卡
	{	
		csize=CSD[9]+(((uint16)(CSD[8]))<<8)+1;
		Capacity=((uint32)csize)<<10;          // 得到扇区数	
                		   
	}
	else //SD1.0的卡
	{	
		n=(CSD[5]&0x0F)+((CSD[10]&0x80)>>7)+((CSD[9]&0x03)<<1)+2;
		csize=(CSD[8]>>6)+((uint16)CSD[7]<<2)+((uint16)(CSD[6]&0x03)<<0x0A)+1;
		Capacity=(uint32)csize<<(n-9);         // 得到扇区数   
	}
	return Capacity;
	
}