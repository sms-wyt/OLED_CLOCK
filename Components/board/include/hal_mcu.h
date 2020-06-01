
#ifndef _HAL_MCU_H
#define _HAL_MCU_H


/* ------------------------------------------------------------------------------------------------
 *                                           Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "hal_types.h"
#include "ioSTC8.h"


#define HAL_ASSERT(expr)    st( if (!( expr )) while(1); )
#define st(x)      do { x } while (__LINE__ == -1)
//根据STC8官方手册附录A2，开启/关闭中断存在滞后现象，需要等待1-2个指令周期系统才能彻底开启/关闭中断
#define HAL_ENABLE_INTERRUPTS()         st(EA = 1;  asm("nop"); asm("nop"); )
#define HAL_DISABLE_INTERRUPTS()        st(EA = 0;  asm("nop"); asm("nop"); )
#define HAL_INTERRUPTS_ARE_ENABLED()    (EA)

typedef unsigned char halIntState_t;
#define HAL_ENTER_CRITICAL_SECTION(x)   st( x = EA;  HAL_DISABLE_INTERRUPTS(); )
#define HAL_EXIT_CRITICAL_SECTION(x)    st( EA = x;)
#define HAL_CRITICAL_STATEMENT(x)       st( halIntState_t _s; HAL_ENTER_CRITICAL_SECTION(_s); x; HAL_EXIT_CRITICAL_SECTION(_s); )

/* ------------------------------------------------------------------------------------------------
 *                               Memory Attributes and Compiler Macros
 * ------------------------------------------------------------------------------------------------
 */

/* ----------- IAR Compiler ----------- */
#ifdef __IAR_SYSTEMS_ICC__
#define CODE      __code
#define XDATA     __xdata
#define DATA      __data
#define NEAR_FUNC __near_func
#define ASM_NOP    asm("NOP")

/* ----------- KEIL Compiler ----------- */
#elif defined __KEIL__
#define  CODE   code
#define  XDATA  xdata
#define ASM_NOP   __nop()

/* ----------- GNU Compiler ----------- */
#elif defined __GNUC__
#define ASM_NOP __asm__ __volatile__ ("nop")

/* ---------- MSVC compiler ---------- */
#elif _MSC_VER
#define ASM_NOP __asm NOP

/* ----------- CCS Compiler ----------- */
#elif defined __TI_COMPILER_VERSION
#define ASM_NOP    asm(" NOP")

/* ----------- Unrecognized Compiler ----------- */
#else
#error "ERROR: Unknown compiler."
#endif


/* ------------------------------------------------------------------------------------------------
 *                                        Standard Defines
 * ------------------------------------------------------------------------------------------------
 */
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif


#endif
