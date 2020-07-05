/**************************************************************************************************
 *                                        - ioSTC8.h -
 *                                   STC8系列单片机IAR平台头文件
 *                            参考datasheet：STC8系列单片机技术参考手册
 *                                      编写者：WYT
 *                                      测试者：WYT
 *                                     QQ：936308890
 **************************************************************************************************
 */

#ifndef IOSTC8_H
#define IOSTC8_H

/* ------------------------------------------------------------------------------------------------
 *                                      编译器抽象
 * ------------------------------------------------------------------------------------------------
 */
#ifdef __IAR_SYSTEMS_ICC__  /*IAR平台 C语言 编译器*/
#pragma language=extended  /*支持语言扩展*/
#define SFR(name,addr)   __sfr   __no_init  volatile  unsigned char  name @ addr;
#define SFRBIT(name, addr, bit7, bit6, bit5, bit4, bit3, bit2, bit1, bit0) \
__sfr __no_init volatile union \
{                              \
  unsigned char name;          \
  struct {                     \
    unsigned char bit0 : 1;    \
    unsigned char bit1 : 1;    \
    unsigned char bit2 : 1;    \
    unsigned char bit3 : 1;    \
    unsigned char bit4 : 1;    \
    unsigned char bit5 : 1;    \
    unsigned char bit6 : 1;    \
    unsigned char bit7 : 1;    \
  };                           \
} @ addr;

#define XDATABIT(name, addr, bit7, bit6, bit5, bit4, bit3, bit2, bit1, bit0) \
__xdata __no_init volatile union  \
{                               \
  unsigned char name;          \
  struct {                      \
    unsigned char bit0 : 1;    \
    unsigned char bit1 : 1;    \
    unsigned char bit2 : 1;    \
    unsigned char bit3 : 1;    \
    unsigned char bit4 : 1;    \
    unsigned char bit5 : 1;    \
    unsigned char bit6 : 1;    \
    unsigned char bit7 : 1;    \
  } ;                           \
} @ addr;
#define SBIT(name,addr) /* 未在IAR C语言编译器中使用 */
#define XREG(addr)       ((unsigned char volatile __xdata *) 0)[addr]
#define XREG_INT(addr)   ((unsigned int volatile __xdata *) 0)[addr]
#define PXREG(addr)      (*(unsigned char volatile __xdata *) addr)
#define VECT(num,addr)   addr


#elif defined __IAR_SYSTEMS_ASM__  /*IAR平台 8051汇编 编译器*/
#define SFR(name,addr)   name  DEFINE  addr
SFRBITMACRO MACRO t, addr, bit7 , bit6, bit5, bit4, bit3, bit2, bit1, bit0
t    DEFINE addr
bit7 DEFINE addr.7
bit6 DEFINE addr.6
bit5 DEFINE addr.5
bit4 DEFINE addr.4    
bit3 DEFINE addr.3
bit2 DEFINE addr.2
bit1 DEFINE addr.1
bit0 DEFINE addr.0
            ENDM
#define SFRBIT(name, addr, bit7, bit6, bit5, bit4, bit3, bit2, bit1, bit0) \
    SFRBITMACRO <name>, <addr>, <bit7>, <bit6>, <bit5>, <bit4>, <bit3>, <bit2>, <bit1>, <bit0>
#define XDATABIT(name, addr, bit7, bit6, bit5, bit4, bit3, bit2, bit1, bit0) \
    SFRBITMACRO <name>, <addr>, <bit7>, <bit6>, <bit5>, <bit4>, <bit3>, <bit2>, <bit1>, <bit0>
#define SBIT(name,addr)  name  DEFINE  addr
#define XREG(addr)       addr
#define PXREG(addr)      addr
#define VECT(num,addr)   addr


#else
#error "Unrecognized compiler."
#endif


/*******************STC8系列单片机全部中断向量及其入口地址********************/
#define  INT0_VECTOR     VECT(  0, 0x03 )
#define  Timer0_VECTOR   VECT(  1, 0x0b )
#define  INT1_VECTOR     VECT(  2, 0x13 )
#define  Timer1_VECTOR   VECT(  3, 0x1b )
#define  UART1_VECTOR    VECT(  4, 0x23 )
#define  ADC_VECTOR      VECT(  5, 0x2b )
#define  LVD_VECTOR      VECT(  6, 0x33 )
#define  PCA_VECTOR      VECT(  7, 0x3b )     
#define  UART2_VECTOR    VECT(  8, 0x43 )
#define  SPI_VECTOR      VECT(  9, 0x4b )
#define  INT2_VECTOR     VECT(  10, 0x53 )
#define  INT3_VECTOR     VECT(  11, 0x5b )
#define  Timer2_VECTOR   VECT(  12, 0x63 )   
#define  INT4_VECTOR     VECT(  16, 0x83 )
#define  UART3_VECTOR    VECT(  17, 0x8b )
#define  UART4_VECTOR    VECT(  18, 0x93 )
#define  Timer3_VECTOR   VECT(  19, 0x9b )
#define  Timer4_VECTOR   VECT(  20, 0xa3 )
#define  CMP_VECTOR      VECT(  21, 0xab )
#define  PWM_VECTOR      VECT(  22, 0xb3 )
#define  PWMFD_VECTOR    VECT(  23, 0xbb )
#define  I2C_VECTOR      VECT(  24, 0xc3 )
#define  CAN_VECTOR      VECT(  25, 0xcb )     
/*****************************51内核特殊功能寄存器**************************/                    
SFR(  SP        ,  0x81  )  
SFR(  DPL       ,  0x82  )   
SFR(  DPH       ,  0x83  )  
SFR(  DPL1      ,  0xe4  )  
SFR(  DPH1      ,  0xe5  )  
SFR(  ACC       ,  0xe0  ) 
SFR(  B         ,  0xf0  )  
SFR(  DPS       ,  0xe3  ) 
SFR(  TA        ,  0xae  )
SFRBIT( PSW     ,  0xd0, CY, AC, F0, RS1, RS0, OV, , P )  
/*****************************P0端口特殊功能寄存器**************************/
SFRBIT( P0      ,  0x80, P0_7, P0_6, P0_5, P0_4, P0_3, P0_2, P0_1, P0_0 )
SFR(  P0M0        ,  0x94  )
SFR(  P0M1        ,  0x93  )
/*****************************P1端口特殊功能寄存器**************************/
SFRBIT( P1      ,  0x90, P1_7, P1_6, P1_5, P1_4, P1_3, P1_2, P1_1, P1_0 )
SFR(  P1M0        ,  0x92  )
SFR(  P1M1        ,  0x91  )
/*****************************P2端口特殊功能寄存器**************************/
SFRBIT( P2      ,  0xa0, P2_7, P2_6, P2_5, P2_4, P2_3, P2_2, P2_1, P2_0 )
SFR(  P2M0        ,  0x96  )
SFR(  P2M1        ,  0x95  )
/*****************************P3端口特殊功能寄存器**************************/
SFRBIT( P3      ,  0xb0, P3_7, P3_6, P3_5, P3_4, P3_3, P3_2, P3_1, P3_0 )
SFR(  P3M0        ,  0xb2  )
SFR(  P3M1        ,  0xb1  )
/*****************************P4端口特殊功能寄存器**************************/
SFRBIT( P4      ,  0xc0, P4_7, P4_6, P4_5, P4_4, P4_3, P4_2, P4_1, P4_0 )
SFR(  P4M0       ,  0xb4  )
SFR(  P4M1        ,  0xb3  )
/*****************************P5端口特殊功能寄存器**************************/
SFRBIT( P5      ,  0xc8, P5_7, P5_6, P5_5, P5_4, P5_3, P5_2, P5_1, P5_0 )
SFR(  P5M0        ,  0xca  )
SFR(  P5M1        ,  0xc9  )
/*****************************P6端口特殊功能寄存器**************************/
SFRBIT( P6      ,  0xe8, P6_7, P6_6, P6_5, P6_4, P6_3, P6_2, P6_1, P6_0 )
SFR(  P6M0        ,  0xcc  )
SFR(  P6M1        ,  0xcb  )
/*****************************P7端口特殊功能寄存器**************************/
SFRBIT( P7      ,  0xc8, P7_7, P7_6, P7_5, P7_4, P7_3, P7_2, P7_1, P7_0 )
SFR(  P7M0        ,  0xe2  )
SFR(  P7M1        ,  0xe1  )

/**********************************说明*************************************
*I/O端口特殊功能寄存器位于扩展RAM区域
*先将P_SW2的BIT7置为1,才能正常操作这些寄存器
****************************************************************************/
/*****************************I/O端口特殊功能寄存器*************************/
XDATABIT( P0PU  , 0xfe10, , , , , , , , )
XDATABIT( P1PU  , 0xfe11, , , , , , , , )
XDATABIT( P2PU  , 0xfe12, , , , , , , , )
XDATABIT( P3PU  , 0xfe13, , , , , , , , )
XDATABIT( P4PU  , 0xfe14, , , , , , , , )
XDATABIT( P5PU  , 0xfe15, , , , , , , , )
XDATABIT( P6PU  , 0xfe16, , , , , , , , )
XDATABIT( P7PU  , 0xfe17, , , , , , , , )
XDATABIT( P0NCS , 0xfe18, , , , , , , , )
XDATABIT( P1NCS , 0xfe19, , , , , , , , )
XDATABIT( P2NCS , 0xfe1a, , , , , , , , )
XDATABIT( P3NCS , 0xfe1b, , , , , , , , )
XDATABIT( P4NCS , 0xfe1c, , , , , , , , )
XDATABIT( P5NCS , 0xfe1d, , , , , , , , )
XDATABIT( P6NCS , 0xfe1e, , , , , , , , )
XDATABIT( P7NCS , 0xfe1f, , , , , , , , )
/*****************************系统管理特殊功能寄存器**************************/
SFRBIT( PCON  , 0x87, SMOD, SMOD0, LVDF, POF, GF1, GF0, PD, IDL )

SFRBIT( AUXR  , 0x8e, T0x12, T1x12, UART_M0x6, T2R, T2_CT, T2x12, EXTRAM, S1ST2 )

SFR(  BUS_SPEED  ,  0xa1  )
SFR(  P_SW1      ,  0xa2  )
SFRBIT( P_SW2  , 0xba, EAXFR, , , , , , , )
SFR(  VOCTRL     ,  0xbb  )
SFR(  RSTCFG     ,  0xff  )

/**********************************说明************************************
*时钟系统特殊功能寄存器位于扩展RAM区域
*先将P_SW2的BIT7置为1,才能正常操作这些寄存器
***************************************************************************/
/*****************************时钟系统特殊功能*****************************/
XDATABIT( CKSEL  , 0xfe00, , , , , MCLKO_S, , , )
XDATABIT( CLKDIV  , 0xfe01, , , , , , , , )
XDATABIT( IRC24MCR  , 0xfe02, ENIRC24M, , , , , , , IRC24MST)
XDATABIT( XOSCCR  , 0xfe03, ENXOSC, X1TYPE, , , , , , XOSCST)
XDATABIT( IRC32KCR  , 0xfe04, ENIRC32K, , , , , , , IRC32KST)
SFR(  LIRTRIM    ,  0x9e  )
SFR(  IRTRIM     ,  0x9f  )
/*****************************中断特殊功能寄存器****************************/
SFRBIT( IE    ,  0xa8, EA, ELVD, EADC, ES, ET1, EX1, ET0, EX0 )
SFRBIT( IE2  , 0xaf, , ET4, ET3, ES4, ES3, ET2, ESPI, ES2 )

SFRBIT( IP    ,  0xb8, PPCA, PLVD, PADC, PS, PT1, PX1, PT0, PX0 )
SFRBIT( IP2  , 0xb5, , PI2C, PCMP, PX4, PPWMFD, PPWM, PSPI, PS2)

SFRBIT( IPH  , 0xb7, PPCAH, PLVDH, PADCH, PSH, PT1H, PX1H, PT0H, PX0H )

SFRBIT( IP2H  , 0xb6, , PI2CH, PCMPH, PX4H, PPWMFDH, PPWMH, PSPIH, PS2H )

SFRBIT( INTCLKO  , 0x8f, , EX4, EX3, EX2, , T2CLKO, T1CLKO, T0CLKO )

SFRBIT( AUXINTIF  , 0xef, , INT4IF, INT3IF, INT2IF, , T4IF, T3IF, T2IF )
/*****************************定时器特殊功能寄存器**************************/
SFRBIT( TCON    ,  0x88, TF1, TR1, TF0, TR0, IE1, IT1, IE0, IT0 )
SFRBIT( TMOD  , 0x89, T1_GATE, T1_CT, T1_M1, T1_M0, T0_GATE, T0_CT, T0_M1, T0_M0 )
SFR(  TL0       ,  0x8a  )
SFR(  TL1       ,  0x8b  )
SFR(  TH0       ,  0x8c  )
SFR(  TH1       ,  0x8d  )
SFRBIT( T4T3M  , 0xd1, T4R, T4_CT, T4x12, T4CLKO, T3R, T3_CT, T3x12, T3CLKO )
SFR(  T4H       ,  0xd2  )
SFR(  T4L       ,  0xd3  )
SFR(  T3H       ,  0xd4  )
SFR(  T3L       ,  0xd5  )
SFR(  T2H       ,  0xd6  )
SFR(  T2L       ,  0xd7  )
SFR(  TH4       ,  0xd2  )
SFR(  TL4       ,  0xd3  )
SFR(  TH3       ,  0xd4  )
SFR(  TL3       ,  0xd5  )
SFR(  TH2       ,  0xd6  )
SFR(  TL2       ,  0xd7  )
SFR(  WKTCL     ,  0xaa  )
SFRBIT( WKTCH  , 0xab, WKTEN, , , , , , ,  )
SFRBIT( WDT_CONTR  , 0xc1, WDT_FLAG, , EN_WDT, CLR_WDT, IDL_WDT, , ,  )
/*****************************串行口特殊功能寄存器**************************/
SFRBIT( SCON    ,  0x98, SM0, SM1, SM2, REN, TB8, RB8, TI, RI )
SFR(  SBUF      ,  0x99  )
SFRBIT( S2CON  , 0x9a, S2SM0, S2ST4, S2SM2, S2REN, S2TB8, S2RB8, S2TI, S2RI )
SFR(  S2BUF     ,  0x9b  )
SFRBIT( S3CON  , 0xac, S3SM0, S3ST4, S3SM2, S3REN, S3TB8, S3RB8, S3TI, S3RI )
SFR(  S3BUF     ,  0xad  )
SFRBIT( S4CON  , 0x84, S4SM0, S4ST4, S4SM2, S4REN, S4TB8, S4RB8, S4TI, S4RI )
SFR(  S4BUF     ,  0x85  )
SFR(  SADDR     ,  0xa9  )
SFR(  SADEN     ,  0xb9  )
/*****************************ADC特殊功能寄存器****************************/
SFRBIT( ADC_CONTR  , 0xbc, ADC_POWER, ADC_START, ADC_FLAG, , , , ,  )
SFR(  ADC_RES       ,  0xbd  )
SFR(  ADC_RESL      ,  0xbe  )
SFRBIT( ADCCFG  , 0xde, , , , , , , ADC_RESFMT,  )
/*****************************SPI特殊功能寄存器****************************/
SFRBIT( SPSTAT  , 0xcd, SPIF, WCOL, , , , , ,  )
SFRBIT( SPCTL  , 0xce, SSIG, SPEN, DORD, MSTR, CPOL, CPHA, ,  )
SFR(  SPDAT    ,  0xcf  )
/****************************IAP/ISP特殊功能寄存器*************************/
SFR(  IAP_DATA       ,  0xc2  )
SFR(  IAP_ADDRH      ,  0xc3  )
SFR(  IAP_ADDRL      ,  0xc4  )
SFR(  IAP_CMD        ,  0xc5  )
SFR(  IAP_TRIG       ,  0xc6  )
SFRBIT( IAP_CONTR  , 0xc7, IAPEN, SWBS, SWRST, CMD_FAIL, , , ,  )
/****************************比较器特殊功能寄存器*************************/
SFRBIT( CMPCR1  , 0xe6, CMPEN, CMPIF, PIE, NIE, PIS, NIS, CMPOE, CMPRES )
SFRBIT( CMPCR2  , 0xe7, INVCMPO, DISFLT, , , , , ,  )
/**************************PCA/PWM 特殊功能寄存器*************************/
SFRBIT( CCON  , 0xd8, CF, CR, , , CCF3, CCF2, CCF1, CCF0 )
SFRBIT( CMOD  , 0xd9, CIDL, , , , , , , ECF )
SFR(  CL   ,  0xe9  )
SFR(  CH   ,  0xf9  )
SFRBIT( CCAPM0  , 0xda, , ECOM0, CCAPP0, CCAPN0, MAT0, TOG0, PWM0, ECCF0 )
SFRBIT( CCAPM1  , 0xdb, , ECOM1, CCAPP1, CCAPN1, MAT1, TOG1, PWM1, ECCF1 )
SFRBIT( CCAPM2  , 0xdc, , ECOM2, CCAPP2, CCAPN2, MAT2, TOG2, PWM2, ECCF2 )
SFRBIT( CCAPM3  , 0xdd, , ECOM3, CCAPP3, CCAPN3, MAT3, TOG3, PWM3, ECCF3 )
SFR(  CCAP0L      ,  0xea  )
SFR(  CCAP1L      ,  0xeb  )
SFR(  CCAP2L      ,  0xec  )
SFR(  CCAP3L      ,  0xed  )
SFR(  CCAP0H      ,  0xfa  )
SFR(  CCAP1H      ,  0xfb  )
SFR(  CCAP2H      ,  0xfc  )
SFR(  CCAP3H      ,  0xfd  )
SFR(  PCA_PWM0    ,  0xf2  )
SFR(  PCA_PWM1    ,  0xf3  )
SFR(  PCA_PWM2    ,  0xf4  )
SFR(  PCA_PWM3    ,  0xf5  )
/*****************增强型PWM波形发生器特殊功能寄存器**********************/
SFRBIT( PWMCFG  , 0xf1, CBIF, ETADC, , , , , ,  )
SFRBIT( PWMIF  , 0xf6, C7IF, C6IF, C5IF, C4IF, C3IF, C2IF, C1IF, C0IF )
SFRBIT( PWMFDCR  , 0xf7, INVCMP, INVIO, ENFD, FLTFLIO, EFDI, FDCMP, FDIO, FDIF )
SFRBIT( PWMCR  , 0xfe, ENPWM, ECBI, , , , , ,  )


/**********************************说明************************************
*其他PWM特殊功能寄存器位于扩展RAM区域
*先将P_SW2的BIT7置为1,才能正常操作这些寄存器
***************************************************************************/
/**************************增强型PWM特殊功能寄存器***************************/
XDATABIT( PWMCH  , 0xfff0, , , , , , , ,  )
XDATABIT( PWMCL  , 0xfff1, , , , , , , ,  )
XDATABIT( PWMCKS  , 0xfff2, , , , SELT2, , , ,  )
XDATABIT( TADCPH  , 0xfff3, , , , , , , ,  )
XDATABIT( TADCPL  , 0xfff4, , , , , , , ,  )
XDATABIT( PWM0T1H  , 0xff00, , , , , , , ,  )
XDATABIT( PWM0T1L  , 0xff01, , , , , , , ,  )
XDATABIT( PWM0T2H  , 0xff02, , , , , , , ,  )
XDATABIT( PWM0T2L  , 0xff03, , , , , , , ,  )
XDATABIT( PWM0CR  , 0xff04, ENC0O, C0INI, , , , EC0I, EC0T2SI, EC0T1SI )
XDATABIT( PWM0HLD  , 0xff05, , , , , , , HC0H, HC0L )
XDATABIT( PWM1T1H  , 0xff10, , , , , , , ,  )
XDATABIT( PWM1T1L  , 0xff11, , , , , , , ,  )
XDATABIT( PWM1T2H  , 0xff12, , , , , , , ,  )
XDATABIT( PWM1T2L  , 0xff13, , , , , , , ,  )
XDATABIT( PWM1CR  , 0xff14, ENC1O, C1INI, , , , EC1I, EC1T2SI, EC1T1SI )
XDATABIT( PWM1HLD  , 0xff15, , , , , , , HC1H, HC1L )
XDATABIT( PWM2T1H  , 0xff20, , , , , , , ,  )
XDATABIT( PWM2T1L  , 0xff21, , , , , , , ,  )
XDATABIT( PWM2T2H  , 0xff22, , , , , , , ,  )
XDATABIT( PWM2T2L  , 0xff23, , , , , , , ,  )
XDATABIT( PWM2CR  , 0xff24, ENC2O, C2INI, , , , EC2I, EC2T2SI, EC2T1SI )
XDATABIT( PWM2HLD  , 0xff25, , , , , , , HC2H, HC2L )
XDATABIT( PWM3T1H  , 0xff30, , , , , , , ,  )
XDATABIT( PWM3T1L  , 0xff31, , , , , , , ,  )
XDATABIT( PWM3T2H  , 0xff32, , , , , , , ,  )
XDATABIT( PWM3T2L  , 0xff33, , , , , , , ,  )
XDATABIT( PWM3CR  , 0xff34, ENC3O, C3INI, , , , EC3I, EC3T2SI, EC3T1SI )
XDATABIT( PWM3HLD  , 0xff35, , , , , , , HC3H, HC3L )
XDATABIT( PWM4T1H  , 0xff40, , , , , , , ,  )
XDATABIT( PWM4T1L  , 0xff41, , , , , , , ,  )
XDATABIT( PWM4T2H  , 0xff42, , , , , , , ,  )
XDATABIT( PWM4T2L  , 0xff43, , , , , , , ,  )
XDATABIT( PWM4CR  , 0xff44, ENC4O, C4INI, , , , EC4I, EC4T2SI, EC4T1SI )
XDATABIT( PWM4HLD  , 0xff45, , , , , , , HC4H, HC4L )
XDATABIT( PWM5T1H  , 0xff50, , , , , , , ,  )
XDATABIT( PWM5T1L  , 0xff51, , , , , , , ,  )
XDATABIT( PWM5T2H  , 0xff52, , , , , , , ,  )
XDATABIT( PWM5T2L  , 0xff53, , , , , , , ,  )
XDATABIT( PWM5CR  , 0xff54, ENC5O, C5INI, , , , EC5I, EC5T2SI, EC5T1SI )
XDATABIT( PWM5HLD  , 0xff55, , , , , , , HC5H, HC5L )
XDATABIT( PWM6T1H  , 0xff60, , , , , , , ,  )
XDATABIT( PWM6T1L  , 0xff61, , , , , , , ,  )
XDATABIT( PWM6T2H  , 0xff62, , , , , , , ,  )
XDATABIT( PWM6T2L  , 0xff63, , , , , , , ,  )
XDATABIT( PWM6CR  , 0xff64, ENC6O, C6INI, , , , EC6I, EC6T2SI, EC6T1SI )
XDATABIT( PWM6HLD  , 0xff65, , , , , , , HC6H, HC6L )
XDATABIT( PWM7T1H  , 0xff70, , , , , , , ,  )
XDATABIT( PWM7T1L  , 0xff71, , , , , , , ,  )
XDATABIT( PWM7T2H  , 0xff72, , , , , , , ,  )
XDATABIT( PWM7T2L  , 0xff73, , , , , , , ,  )
XDATABIT( PWM7CR  , 0xff74, ENC7O, C7INI, , , , EC7I, EC7T2SI, EC7T1SI )
XDATABIT( PWM7HLD  , 0xff75, , , , , , , HC7H, HC7L )

/**********************************说明************************************
*I2C特殊功能寄存器位于扩展RAM区域
*先将P_SW2的BIT7置为1,才能正常操作这些寄存器
***************************************************************************/
/**************************I2C特殊功能寄存器*******************************/
XDATABIT( I2CCFG  , 0xfe80, ENI2C, MSSL, , , , , ,  )
XDATABIT( I2CMSCR  , 0xfe81, EMSI, , , , , , , )
XDATABIT( I2CMSST  , 0xfe82, MSBUSY, MSIF, , , , , MSACKI, MSACKO)
XDATABIT( I2CSLCR  , 0xfe83, , ESTAI, ERXI, ETXI, ESTOI, , , SLRST)
XDATABIT( I2CSLST  , 0xfe84, SLBUSY, STAIF, RXIF, TXIF, STOIF, , SLACKI, SLACKO)
XDATABIT( I2CSLADR  , 0xfe85, , , , , , , , MA)
XDATABIT( I2CTXD  , 0xfe86, , , , , , , , )
XDATABIT( I2CRXD  , 0xfe87, , , , , , , , )
XDATABIT( I2CMSAUX  , 0xfe88, , , , , , , , WDTA)


#ifdef __IAR_SYSTEMS_ICC__
#pragma language=default
#endif

/**************************************************************************************************
 */
#endif
