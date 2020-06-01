/*---------------------------------------------------------------------------/
/  FatFs 功能配置
/---------------------------------------------------------------------------*/

#define FFCONF_DEF	86606	/* 版本 ID */

/*---------------------------------------------------------------------------/
/ 功能配置
/---------------------------------------------------------------------------*/

#define FF_FS_READONLY	0
/* 这个选择开关“只读”配置 (0:读/写 or 1:只读)
/  如果选择了1配置，则删除了 写API函数,如f_write(), f_sync(),
/  f_unlink(), f_mkdir(), f_chmod(), f_rename(), f_truncate(), f_getfree()
/和可选择的写函数. */

#define FF_FS_MINIMIZE	0
/* 这个选择开关定义了最小级别，而且会移除一些基本API函数 
/
/   0: 基本函数完全使能.
/   1: f_stat(), f_getfree(), f_unlink(), f_mkdir(), f_truncate() and f_rename()
/      被移除.
/   2: 和1选项对比，增加了对f_opendir(), f_readdir() and f_closedir() 的移除
/   3: 和1选项对比，增加了对f_lseek()的移除*/


#define FF_USE_STRFUNC	2
/* 这个选择开关了字符串函数, f_gets(), f_putc(), f_puts() and f_printf().
/
/  0: 失能字符串函数
/  1: 不带有 LF-CRLF conversion.
/  2: 带有 with LF-CRLF conversion. */


#define FF_USE_FIND		1
/* 这个选择开关过滤目录读取函数, f_findfirst() and
/  f_findnext(). (0:失能, 1:使能 2:使能，也匹配altname[] t) */


#define FF_USE_MKFS		0
/* 这个选择开关 f_mkfs() 函数. (0:失能 or 1:使能) */


#define FF_USE_FASTSEEK	1
/* 这个选择开关快速寻找函数. (0:失能 or 1:使能) */


#define FF_USE_EXPAND	0
/* 这个选择开关 f_expand 函数. (0:失能 or 1:使能) */


#define FF_USE_CHMOD	0
/* 这个选择开关 属性处理函数, f_chmod() and f_utime().
/  (0:失能 or 1:使能) 同时 FF_FS_READONLY 需要为0才能使能这个选择. */


#define FF_USE_LABEL	1
/* 这个选择开关 卷标函数, f_getlabel() and f_setlabel().
/  (0:失能 or 1:使能) */


#define FF_USE_FORWARD	0
/* 这个选择开关 f_forward() 函数. (0:失能 or 1:使能) */


/*---------------------------------------------------------------------------/
/ 区域设置和命名空间配置
/---------------------------------------------------------------------------*/

#define FF_CODE_PAGE	936
/* 这个选择指定在目标系统被使用的OEM代码页
/  不正确的代码页设置可能导致打开文件失败
/
/   437 - 美国英文
/   720 - 阿拉伯文
/   737 - 希腊文
/   771 - KBL
/   775 - Baltic
/   850 - 拉丁文 1
/   852 - 拉丁文 2
/   855 - 西里尔文
/   857 - 土耳其文
/   860 - 葡萄牙文
/   861 - 冰岛文
/   862 - 希伯来文
/   863 - 加拿大法文
/   864 - 阿拉伯文
/   865 - 北欧
/   866 - 俄文
/   869 - 希腊文 2
/   932 - 日文 (DBCS)
/   936 - 简体中文(DBCS)
/   949 - 韩文(DBCS)
/   950 - 繁体中文(DBCS)
/     0 - 包含上述所有代码页，可由 f_setcp()进行配置
*/


#define FF_USE_LFN		0
#define FF_MAX_LFN		255
/*  FF_USE_LFN 开关 支持LFN (长文件名).
/
/   0: 失能 LFN. FF_MAX_LFN 起不到效果
/   1: 使能 LFN 在BSS有静态工作缓冲区。总是不是线程安全级。
/   2: 使能 LFN 在stack（栈）有动态工作缓冲区。
/   3: 使能 LFN 在heap（堆）有动态工作缓冲区。
/
/  使能LFN，ffunicode.c文件需要添加到工程中。LFN功能需要占用某些特定的工作缓冲区，
/  缓冲区大小为(FF_MAX_LFN + 1) * 2 bytes。如果使能exFAT，还需要额外(FF_MAX_LFN + 44) / 15 * 32 bytes
/  FF_MAX_LFN定义了在UTF-16 代码单元的工作缓冲区大小，范围为12-255。推荐设置为255来完全支持LFN特性
/ 当使用stack（栈）作为工作缓冲区，注意可能stack（栈）溢出的情况。当使用heap（堆）空间作为工作缓冲区，
/ 相关空间的管理函数，如ff_memalloc()和ff_memfree()包含于ffsystem.c需要添加到工程中。*/



#define FF_LFN_UNICODE	0
/* 这个选择开关 当LFN使能时的字符编码API
/
/   0: ANSI/OEM in current CP (TCHAR = char)
/   1: Unicode in UTF-16 (TCHAR = WCHAR)
/   2: Unicode in UTF-8 (TCHAR = char)
/   3: Unicode in UTF-32 (TCHAR = DWORD)
/
/ 同时字符串I/O函数的行为也将受此选项的影响*/

#define FF_LFN_BUF		255
#define FF_SFN_BUF		12
/* 这个开关设置定义了在FILINFO结构体的名字成员大小
/  这用于用于读取目录项。 这些值应该足够让文件名读取
/  最大的读取文件名长度取决于字符编码。 depends
/  当LFN失能，这些选择没起作用。*/

#define FF_STRF_ENCODE	0
/* 当 FF_LFN_UNICODE >= 1 并且LFN 使能, 字符串I/O函数, f_gets(),
/  f_putc(), f_puts and f_printf() 可以在里面转化字符编码.
/  这个选择假定字符编码 ON THE FILE 可以通过这些函数读/写
/
/   0: ANSI/OEM in current CP
/   1: Unicode in UTF-16LE
/   2: Unicode in UTF-16BE
/   3: Unicode in UTF-8
*/

#define FF_FS_RPATH		0
/* 此选项配置支持相对路径
/
/   0: 失能相对路径并且移除相关函数
/   1: 使能相对路径 f_chdir() 和 f_chdrive() 可以使用
/   2: 在选项1的基础上，增加可以使用f_getcwd()函数
*/

/*---------------------------------------------------------------------------/
/ 驱动器/卷标配置
/---------------------------------------------------------------------------*/

#define FF_VOLUMES		1
/* 可以使用的卷标 (逻辑驱动)范围1-10 */


#define FF_STR_VOLUME_ID	0
#define FF_VOLUME_STRS		"RAM","NAND","CF","SD","SD2","USB","USB2","USB3"
/* FF_STR_VOLUME_ID 开关支持任意字符串volume ID
/  FF_STR_VOLUME_ID可被设置为1或者2, 任意字符串可以用作路径名中的驱动器号
/  FF_VOLUME_STRS 定义volume ID字符串作为逻辑驱动器
/  项目的数量必须少于FF_VOLUMES
/  合法的volume ID strings字符有A-Z，a-z，和0-9 however, 区分大小写
/  如果FF_STR_VOLUME_ID >= 1 并且 FF_VOLUME_STRS 没有定义，用户需要定义volume字符串表，即
/  const char* VolumeStr[FF_VOLUMES] = {"ram","flash","sd","usb",...
*/

#define FF_MULTI_PARTITION	0
/* 这个选择开关支持在逻辑驱动器中多卷标
/  默认为 (0), 每个逻辑驱动器码一定对应相同的物理驱动器号并且只有在物理驱动器的会被安装
/  当这个函数使能（1）了，每个每个逻辑驱动器码一定对应相同的物理驱动器码，在VolToPart[]中列出分区
/  同时，f_fdisk()函数可以使用*/
//  funciton will be available. 


#define FF_MIN_SS		512
#define FF_MAX_SS		512
/* 这个选项配置支持扇区空间的范围（512,1024, 2048 or 4096）
/  对于大多数系统、通用存储器、硬盘，通常设置512
/  更大的值需要flash空间和一些选择媒体类型。
/  当FF_MAX_SS 大于 FF_MIN_SS, FatFs可以配置不同的扇区大小模式
/  使用disk_ioctl()函数需要用到GET_SECTOR_SIZE命令*/


#define FF_LBA64		0
/*这个选择开关支持64位LBA(0:失能 or 1:使能)
/  使能64位LBA，同时exFAT需要使能(FF_FS_EXFAT == 1) */


#define FF_MIN_GPT		0x100000000
/* 开关GPT格式用f_mkfs和f_fdisk函数创建分区的最小扇区数，0x100000000 为最大
/  这个选择在FF_LBA64 == 0时无效*/


#define FF_USE_TRIM		0
/* 这个选择开关支持(0:失能 or 1:使能)
/  使能Trim 功能, 使用disk_ioctl()函数需要用到CTRL_TRIM命令 */



/*---------------------------------------------------------------------------/
/ 系统配置
/---------------------------------------------------------------------------*/

#define FF_FS_TINY		0
/* 这个选择开关tiny缓冲配置(0:正常 或者 1:Tiny)
/  处于tiny 配置, 文件对象大小被缩小到 FF_MAX_SS bytes
/  不同于私有扇区缓冲区删除文件对象，在文件系统的普通扇区缓冲区被用于文件数据传输*/


#define FF_FS_EXFAT		0
/* 这个选择开关支持exFAT文件系统(0:失能 or 1:使能)
/  使能 exFAT,LFN也要使能. (FF_USE_LFN >= 1)
/  请注意，使能exFAT会失去ANSI C (C89)兼容. */


#define FF_FS_NORTC	1
#define FF_NORTC_MON	1
#define FF_NORTC_MDAY	1
#define FF_NORTC_YEAR	2019
/* FF_FS_NORTC 开关时间戳功能。如果系统没有RTC功能或者不需要时间标记设置FF_FS_NORTC = 1来失能时间戳功能
/  每个在FatFs修改的对象都要一个固定的时间戳，定义在FF_NORTC_MON, FF_NORTC_MDAY and FF_NORTC_YEAR的当前时间
/  使能时间戳功能FF_FS_NORTC = 0), get_fattime() 函数 需要添加到工程，以便读取当前时间。
/  此时FF_NORTC_MON,FF_NORTC_MDAY 和 FF_NORTC_YEAR起不到作用。
/  这个选择在只读配置(FF_FS_READONLY = 1)也不起作用 */


#define FF_FS_NOFSINFO	0
/* 如果需要知道当前FAT32卷标空闲空间，设置这个选择为bit 0，f_getfree() 函数在卷标挂载后的第一时间会对FAT完全扫描
/  Bit 1 控制使用最后分配的簇号
/
/  bit0=0: 如果在FSINFO可用，使用空闲簇计数
/  bit0=1: 不使用在FSINFO的空闲簇计数
/  bit1=0: 如果在FSINFO可用，使用最后分配的簇号
/  bit1=1: 不使用在FSINFO的最后分配的簇号
*/


#define FF_FS_LOCK		0
/* 这个选择 FF_FS_LOCK 开关 文件锁功能来控制打开重复文件和对象的非法操作，
/  这个选择必须FF_FS_READONLY为1
/  0:  失能文件锁功能。为了避免卷标损坏，应用程序要避免非法打开、移除、重命名操作对象
/  >0: 使能文件锁功能，数值定义多少个文件/子目录可以在文件锁控制下同时打开。
/  请注意文件锁控制是独立于re-entrancy（重新输入）的*/


/* #include <somertos.h>	// O/S definitions */
#define FF_FS_REENTRANT	0
#define FF_FS_TIMEOUT	1000
#define FF_SYNC_t		HANDLE
/* TFF_FS_REENTRANT 开关FatFs模块本身的re-entrancy（重新输入线程安全）
/  请注意，不管这个选择，进入不同卷标的文件通常是re-entrant和卷标控制函数，
/  f_mount(), f_mkfs()和f_fdisk() 函数, 通常不是re-entrant的。
/  仅仅只有文件/目录进入相同的卷标才受此控制 
/
/   0: 失能 re-entrancy. FF_FS_TIMEOUT 和 FF_SYNC_t 不起作用.
/   1: 使能 re-entrancy. 用户提供的同步处理程序,
/      ff_req_grant(), ff_rel_grant(), ff_del_syncobj() and ff_cre_syncobj()
/      函数必须添加到工程中，历程在syscall.c中
/
/  FF_FS_TIMEOUT 定义超时时间（以时间tick为单位）
/  FF_SYNC_t 定义 O/S 依赖sync对象类型，比如HANDLE, ID, OS_EVENT*,
/  SemaphoreHandle_t and etc. A header file for O/S 定义的头文件需要包含在ff.h里面 */



/*--- 结束配置选择 ---*/
