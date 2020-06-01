#ifndef _HAL_TYPES_H
#define _HAL_TYPES_H

#include <stdbool.h>
/* ------------------------------------------------------------------------------------------------
 *                                               Types
 * ------------------------------------------------------------------------------------------------
 */
/** @defgroup HAL_TYPES HAL Types
 * @{
 */
typedef signed   char   int8;     //!< Signed 8 bit integer
typedef unsigned char   uint8;    //!< Unsigned 8 bit integer

typedef signed   short  int16;    //!< Signed 16 bit integer
typedef unsigned short  uint16;   //!< Unsigned 16 bit integer

typedef signed   long   int32;    //!< Signed 32 bit integer
typedef unsigned long   uint32;   //!< Unsigned 32 bit integer

typedef float fp32;
typedef uint8           halDataAlign_t; //!< Used for byte alignment

//Fatfs数据类型
typedef uint32	UINT;	/* int must be 16-bit or 32-bit */
typedef uint8	BYTE;	/* char must be 8-bit */
typedef uint16	WORD;	/* 16-bit unsigned integer */
typedef uint32	DWORD;	/* 32-bit unsigned integer */
typedef WORD			WCHAR;	/* UTF-16 character type */

typedef DWORD FSIZE_t;
typedef DWORD LBA_t;
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


/**************************************************************************************************
 */
#endif
