/*----------------------------------------------------------------------------/
/  FatFs - Generic FAT Filesystem module  R0.14                               /
/-----------------------------------------------------------------------------/
/
/ Copyright (C) 2019, ChaN, all right reserved.
/
/ FatFs module is an open source software. Redistribution and use of FatFs in
/ source and binary forms, with or without modification, are permitted provided
/ that the following condition is met:

/ 1. Redistributions of source code must retain the above copyright notice,
/    this condition and the following disclaimer.
/
/ This software is provided by the copyright holder and contributors "AS IS"
/ and any warranties related to this software are DISCLAIMED.
/ The copyright owner or contributors be NOT LIABLE for any damages caused
/ by use of this software.
/
/----------------------------------------------------------------------------*/

#ifndef FF_DEFINED
#define FF_DEFINED	86606	/* Revision ID */

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_types.h"
#include "ffconf.h"		/* FatFs configuration options */





/* Definitions of volume management */

#if FF_MULTI_PARTITION		/* Multiple partition configuration */
typedef struct {
	BYTE pd;	/* Physical drive number */
	BYTE pt;	/* Partition: 0:Auto detect, 1-4:Forced partition) */
} PARTITION;
extern PARTITION VolToPart[];	/* Volume - Partition mapping table */
#endif

#if FF_STR_VOLUME_ID
#ifndef FF_VOLUME_STRS
extern const char* VolumeStr[FF_VOLUMES];	/* User defied volume ID */
#endif
#endif


/* 有关FatFs API的路径名称字符串类型 */

#ifndef _INC_TCHAR
#define _INC_TCHAR

#if FF_USE_LFN && FF_LFN_UNICODE == 1 	/* Unicode in UTF-16 encoding */
typedef WCHAR TCHAR;
#define _T(x) L ## x
#define _TEXT(x) L ## x
#elif FF_USE_LFN && FF_LFN_UNICODE == 2	/* Unicode in UTF-8 encoding */
typedef char TCHAR;
#define _T(x) u8 ## x
#define _TEXT(x) u8 ## x
#elif FF_USE_LFN && FF_LFN_UNICODE == 3	/* Unicode in UTF-32 encoding */
typedef DWORD TCHAR;
#define _T(x) U ## x
#define _TEXT(x) U ## x
#elif FF_USE_LFN && (FF_LFN_UNICODE < 0 || FF_LFN_UNICODE > 3)
#error Wrong FF_LFN_UNICODE setting
#else									/* ANSI/OEM code in SBCS/DBCS */
typedef char TCHAR;
#define _T(x) x
#define _TEXT(x) x
#endif

#endif





/* 文件系统对象结构体 (FATFS) */

typedef struct {
	BYTE	fs_type;		/* 文件系统类型 (0:没挂载) */
	BYTE	pdrv;			/* 相关物理驱动 */
	BYTE	n_fats;			/* FATs号(1 or 2) */
	BYTE	wflag;			/* win[] 标志 (b0:dirty) */
	BYTE	fsi_flag;		/* FSINFO 标志 (b7:disabled, b0:dirty) */
	WORD	id;				/* Volume mount ID */
	WORD	n_rootdir;		/* 根目录条目数 (FAT12/16) */
	WORD	csize;			/* 簇大小*/
#if FF_MAX_SS != FF_MIN_SS
	WORD	ssize;			/* 扇区大小 (512, 1024, 2048 or 4096) */
#endif
#if FF_USE_LFN
	WCHAR*	lfnbuf;			/* LFN 工作缓冲区 */
#endif
#if FF_FS_EXFAT
	BYTE*	dirbuf;			/* Directory entry block scratchpad buffer for exFAT */
#endif
#if FF_FS_REENTRANT
	FF_SYNC_t	sobj;		/* Identifier of sync object */
#endif
#if !FF_FS_READONLY
	DWORD	last_clst;		/* 上次分配的簇 */
	DWORD	free_clst;		/* 空闲簇 */
#endif
#if FF_FS_RPATH
	DWORD	cdir;			/* Current directory start cluster (0:root) */
#if FF_FS_EXFAT
	DWORD	cdc_scl;		/* Containing directory start cluster (invalid when cdir is 0) */
	DWORD	cdc_size;		/* b31-b8:Size of containing directory, b7-b0: Chain status */
	DWORD	cdc_ofs;		/* Offset in the containing directory (invalid when cdir is 0) */
#endif
#endif
	DWORD	n_fatent;		/* Number of FAT entries (number of clusters + 2) */
	DWORD	fsize;			/* Size of an FAT [sectors] */
	LBA_t	volbase;		/* Volume base sector */
	LBA_t	fatbase;		/* FAT 基扇区 */
	LBA_t	dirbase;		/* Root directory base sector/cluster */
	LBA_t	database;		/* Data base sector */
#if FF_FS_EXFAT
	LBA_t	bitbase;		/* Allocation bitmap base sector */
#endif
	LBA_t	winsect;		/* Current sector appearing in the win[] */
	BYTE	win[FF_MAX_SS];	/* Disk access window for Directory, FAT (and file data at tiny cfg) */
} FATFS;



/* 对象ID和分配信息 (FFOBJID) */

typedef struct {
	FATFS*	fs;				/* Pointer to the hosting volume of this object */
	WORD	id;				/* Hosting volume mount ID */
	BYTE	attr;			/* Object attribute */
	BYTE	stat;			/* Object chain status (b1-0: =0:not contiguous, =2:contiguous, =3:fragmented in this session, b2:sub-directory stretched) */
	DWORD	sclust;			/* Object data start cluster (0:no cluster or root directory) */
	FSIZE_t	objsize;		/* Object size (valid when sclust != 0) */
#if FF_FS_EXFAT
	DWORD	n_cont;			/* Size of first fragment - 1 (valid when stat == 3) */
	DWORD	n_frag;			/* Size of last fragment needs to be written to FAT (valid when not zero) */
	DWORD	c_scl;			/* Containing directory start cluster (valid when sclust != 0) */
	DWORD	c_size;			/* b31-b8:Size of containing directory, b7-b0: Chain status (valid when c_scl != 0) */
	DWORD	c_ofs;			/* Offset in the containing directory (valid when file object and sclust != 0) */
#endif
#if FF_FS_LOCK
	UINT	lockid;			/* File lock ID origin from 1 (index of file semaphore table Files[]) */
#endif
} FFOBJID;



/* File object structure (FIL) */

typedef struct {
	FFOBJID	obj;			/* Object identifier (must be the 1st member to detect invalid object pointer) */
	BYTE	flag;			/* File status flags */
	BYTE	err;			/* Abort flag (error code) */
	FSIZE_t	fptr;			/* File read/write pointer (Zeroed on file open) */
	DWORD	clust;			/* Current cluster of fpter (invalid when fptr is 0) */
	LBA_t	sect;			/* Sector number appearing in buf[] (0:invalid) */
#if !FF_FS_READONLY
	LBA_t	dir_sect;		/* Sector number containing the directory entry (not used at exFAT) */
	BYTE*	dir_ptr;		/* Pointer to the directory entry in the win[] (not used at exFAT) */
#endif
#if FF_USE_FASTSEEK
	DWORD*	cltbl;			/* Pointer to the cluster link map table (nulled on open, set by application) */
#endif
#if !FF_FS_TINY
	BYTE	buf[FF_MAX_SS];	/* File private data read/write window */
#endif
} FIL;



/* 目录对象结构体 (DIR) */

typedef struct {
	FFOBJID	obj;			/* 对象标识符 */
	DWORD	dptr;			/* 当前读/写偏移量 */
	DWORD	clust;			/* 当前簇 */
	LBA_t	sect;			/* 当前扇区 (0:读操作已终止) */
	BYTE*	dir;			/* Pointer to the directory item in the win[] */
	BYTE	fn[12];			/* SFN (in/out) {body[8],ext[3],status[1]} */
#if FF_USE_LFN
	DWORD	blk_ofs;		/* Offset of current entry block being processed (0xFFFFFFFF:Invalid) */
#endif
#if FF_USE_FIND
	const TCHAR* pat;		/* Pointer to the name matching pattern */
#endif
} DIR;



/* 文件信息结构体 (FILINFO) */

typedef struct {
	FSIZE_t	fsize;			/* 文件大小 */
	WORD	fdate;			/* 已修改的数据 */
	WORD	ftime;			/* 已修改的时基 */
	BYTE	fattrib;		/* 文件属性 */
#if FF_USE_LFN
	TCHAR	altname[FF_SFN_BUF + 1];/* 可选文件名 */
	TCHAR	fname[FF_LFN_BUF + 1];	/* 主文件名 */
#else
	TCHAR	fname[12 + 1];	/* 文件名 */
#endif
} FILINFO;



/* 格式参数结构体 (MKFS_PARM) */

typedef struct {
	BYTE fmt;			/* 格式选择 (FM_FAT, FM_FAT32, FM_EXFAT and FM_SFD) */
	BYTE n_fat;			/* Number of FATs */
	UINT align;			/* 数据域对齐 (扇区) */
	UINT n_root;		/* 根目录条目数 */
	DWORD au_size;		/* 簇大小 (字节) */
} MKFS_PARM;



/* 文件函数返回值 (FRESULT) */

typedef enum {
	FR_OK = 0,				/* (0) 成功 */
	FR_DISK_ERR,			/* (1) 硬件底层磁盘I/O出错*/
	FR_INT_ERR,				/* (2) 断言失败 */
	FR_NOT_READY,			/* (3) 物理驱动器不工作 */
	FR_NO_FILE,				/* (4) 无法找到文件 */
	FR_NO_PATH,				/* (5) 无法找到路径 */
	FR_INVALID_NAME,		/* (6) 路径名格式无效 */
	FR_DENIED,				/* (7) 目录已满，访问被拒绝 */
	FR_EXIST,				/* (8) 访问被拒绝 */
	FR_INVALID_OBJECT,		/* (9) 文件/目录对象无效*/
	FR_WRITE_PROTECTED,		/* (10) 物理驱动器写保护了 */
	FR_INVALID_DRIVE,		/* (11) 逻辑驱动器号无效 */
	FR_NOT_ENABLED,			/* (12) 卷标没有工作区域 */
	FR_NO_FILESYSTEM,		/* (13) 没有有效的FAT卷标 */
	FR_MKFS_ABORTED,		/* (14) The f_mkfs()由于任何问题而中止 */
	FR_TIMEOUT,				/* (15) 无法获得在定义的时间段内访问卷标的授权 */
	FR_LOCKED,				/* (16) 根据文件共享策略，操作被拒绝 */
	FR_NOT_ENOUGH_CORE,		/* (17) LFN 工作缓冲区无法分配 */
	FR_TOO_MANY_OPEN_FILES,	/* (18) 打开的文件数量 > FF_FS_LOCK */
	FR_INVALID_PARAMETER	/* (19) 参数无效 */
} FRESULT;



/*--------------------------------------------------------------*/
/* FatFs 模块应用接口API                           */

FRESULT f_open (FIL* fp, const TCHAR* path, BYTE mode);			/* 打开/创建一个文件 */
FRESULT f_close (FIL* fp);						/* 关闭一个打开的文件对象 */
FRESULT f_read (FIL* fp, void* buff, UINT btr, UINT* br);		/* 读取文件数据 */
FRESULT f_write (FIL* fp, const void* buff, UINT btw, UINT* bw);	/* 写入数据到文件 */
FRESULT f_lseek (FIL* fp, FSIZE_t ofs);					/* 移动文件对象的文件指针 */
FRESULT f_truncate (FIL* fp);						/* 截断文件大小 */
FRESULT f_sync (FIL* fp);						/* 刷新写入文件的缓存数据 */
FRESULT f_opendir (DIR* dp, const TCHAR* path);				/* 打开一个目录 */
FRESULT f_closedir (DIR* dp);						/* 关闭一个打开的目录 */
FRESULT f_readdir (DIR* dp, FILINFO* fno);				/* 读取目录项 */
FRESULT f_findfirst (DIR* dp, FILINFO* fno, const TCHAR* path, const TCHAR* pattern);	/* 打开目录并读取第一个匹配项 */
FRESULT f_findnext (DIR* dp, FILINFO* fno);				/* 读取匹配的下一项 */
FRESULT f_mkdir (const TCHAR* path);					/* 创建一个子目录 */
FRESULT f_unlink (const TCHAR* path);					/* 删除一个已存在的文件或者目录 */
FRESULT f_rename (const TCHAR* path_old, const TCHAR* path_new);	/* 重命名/移动文件或者目录 */
FRESULT f_stat (const TCHAR* path, FILINFO* fno);			/* 检查文件或子目录的存在性 */
FRESULT f_chmod (const TCHAR* path, BYTE attr, BYTE mask);		/* 更改文件或子目录的属性 */
FRESULT f_utime (const TCHAR* path, const FILINFO* fno);		/* 更改文件或子目录的时间戳 */
FRESULT f_chdir (const TCHAR* path);					/* 改变当前目录 */
FRESULT f_chdrive (const TCHAR* path);					/* 改变当前驱动器 */
FRESULT f_getcwd (TCHAR* buff, UINT len);				/* 获取当前目录 */
FRESULT f_getfree (const TCHAR* path, DWORD* nclst, FATFS** fatfs);	/* 获取驱动器的空闲簇数量 */
FRESULT f_getlabel (const TCHAR* path, TCHAR* label, DWORD* vsn);	/* 获取卷标 */
FRESULT f_setlabel (const TCHAR* label);				/* 设置卷标 */
FRESULT f_forward (FIL* fp, UINT(*func)(const BYTE*,UINT), UINT btf, UINT* bf);	/* 读取文件数据并将其转发到数据流设备 */
FRESULT f_expand (FIL* fp, FSIZE_t fsz, BYTE opt);			/* 为文件分配一个连续的块 */
FRESULT f_mount (FATFS* fs, const TCHAR* path, BYTE opt);			/* 挂载/卸载一个逻辑驱动器 */
FRESULT f_mkfs (const TCHAR* path, const MKFS_PARM* opt, void* work, UINT len);	/* 创建一个FAT卷标 */
FRESULT f_fdisk (BYTE pdrv, const LBA_t ptbl[], void* work);		/* 把一个物理驱动器分成几个分区 */
FRESULT f_setcp (WORD cp);						/* 设置当前代码页 */
int f_putc (TCHAR c, FIL* fp);						/* 在文件中添加字符 */
int f_puts (const TCHAR* str, FIL* cp);					/* 在文件中添加字符串 */
int f_printf (FIL* fp, const TCHAR* str, ...);				/* 在文件中添加格式化字符串 */
TCHAR* f_gets (TCHAR* buff, int len, FIL* fp);				/* 从文件获取字符串 */

#define f_eof(fp) ((int)((fp)->fptr == (fp)->obj.objsize))
#define f_error(fp) ((fp)->err)
#define f_tell(fp) ((fp)->fptr)
#define f_size(fp) ((fp)->obj.objsize)
#define f_rewind(fp) f_lseek((fp), 0)
#define f_rewinddir(dp) f_readdir((dp), 0)
#define f_rmdir(path) f_unlink(path)
#define f_unmount(path) f_mount(0, path, 0)

#ifndef EOF
#define EOF (-1)
#endif




/*--------------------------------------------------------------*/
/* Additional user defined functions                            */

/* RTC function */
#if !FF_FS_READONLY && !FF_FS_NORTC
DWORD get_fattime (void);
#endif

/* LFN support functions */
#if FF_USE_LFN >= 1						/* Code conversion (defined in unicode.c) */
WCHAR ff_oem2uni (WCHAR oem, WORD cp);	/* OEM code to Unicode conversion */
WCHAR ff_uni2oem (DWORD uni, WORD cp);	/* Unicode to OEM code conversion */
DWORD ff_wtoupper (DWORD uni);			/* Unicode upper-case conversion */
#endif
#if FF_USE_LFN == 3						/* Dynamic memory allocation */
void* ff_memalloc (UINT msize);			/* Allocate memory block */
void ff_memfree (void* mblock);			/* Free memory block */
#endif

/* Sync functions */
#if FF_FS_REENTRANT
int ff_cre_syncobj (BYTE vol, FF_SYNC_t* sobj);	/* Create a sync object */
int ff_req_grant (FF_SYNC_t sobj);		/* Lock sync object */
void ff_rel_grant (FF_SYNC_t sobj);		/* Unlock sync object */
int ff_del_syncobj (FF_SYNC_t sobj);	/* Delete a sync object */
#endif




/*--------------------------------------------------------------*/
/* Flags and offset address                                     */


/* 文件进入模式及打开方法标志(3rd argument of f_open) */
#define	FA_READ			0x01    //允许读取数据
#define	FA_WRITE		0x02    //允许写入数据
#define	FA_OPEN_EXISTING	0x00    //打开文件。如果文件不存在，则打开失败。(默认)
#define	FA_CREATE_NEW		0x04    //创建一个新文件。如果文件已存在，则创建失败。
#define	FA_CREATE_ALWAYS	0x08    //创建一个新文件。如果文件已存在，则它将被截断并覆盖
#define	FA_OPEN_ALWAYS		0x10   //如果文件存在，则打开；否则，创建一个新文件。
#define	FA_OPEN_APPEND		0x30    //在文件尾部追加写入内容

/* 快速寻找控制 (2nd argument of f_lseek) */
#define CREATE_LINKMAP	((FSIZE_t)0 - 1)

/* 选择格式（格式化用） (2nd argument of f_mkfs) */
#define FM_FAT		0x01
#define FM_FAT32	0x02
#define FM_EXFAT	0x04
#define FM_ANY		0x07
#define FM_SFD		0x08

/* 文件系统类型 (FATFS.fs_type) */
#define FS_FAT12	1
#define FS_FAT16	2
#define FS_FAT32	3
#define FS_EXFAT	4

/* 进入目录的文件属性位 (FILINFO.fattrib) */
#define	AM_RDO	0x01	/* Read only */
#define	AM_HID	0x02	/* Hidden */
#define	AM_SYS	0x04	/* System */
#define AM_DIR	0x10	/* Directory */
#define AM_ARC	0x20	/* Archive */


#ifdef __cplusplus
}
#endif

#endif /* FF_DEFINED */
