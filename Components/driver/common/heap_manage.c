#include "heap_manage.h"
#include "string.h"
#include "uart.h"

//堆总空间(数组all_heap[ HEAP_SIZE] ，！！！！实际占 HEAP_SIZE×2 字节！！！！)
#define HEAP_SIZE  1500    
#define HEAPMEM_IN_USE             0x8000

//堆空间结束位置
#define HEAP_LASTBLK_IDX      ((HEAP_SIZE / HEAPMEM_HDRSZ) - 1) 
//区分块位置
#define HEAPMEM_SMALLBLK_HDRCNT   (HEAPMEM_SMALLBLK_BUCKET / HEAPMEM_HDRSZ)
//大块起始位置
#define HEAPMEM_BIGBLK_IDX        (HEAPMEM_SMALLBLK_HDRCNT + 1)

//首部大小
#define HEAPMEM_HDRSZ              sizeof(heapMemHdr_t)  
//最小块大小
#define HEAPMEM_MIN_BLKSZ         (HEAPMEM_ROUND((HEAPMEM_HDRSZ * 2)))
//小块大小
#define HEAPMEM_SMALL_BLKSZ       (HEAPMEM_ROUND(16))
//默认长块大小
#define HEAPMEM_LL_BLKSZ          (HEAPMEM_ROUND(417) + (19 * HEAPMEM_HDRSZ))

//小块总空间
#define HEAPMEM_SMALLBLK_BUCKET  ((HEAPMEM_SMALL_BLKSZ * HEAPMEM_SMALL_BLKCNT) + HEAPMEM_LL_BLKSZ)
//大块总空间
#define HEAPMEM_BIGBLK_SZ         (HEAP_SIZE - HEAPMEM_SMALLBLK_BUCKET - HEAPMEM_HDRSZ*2)

//默认小块数量
#define HEAPMEM_SMALL_BLKCNT       8
// 调整申请内存大小宏操作（如申请17字节空间，则调整为18字节）
#define HEAPMEM_ROUND(X)       ((((X) + HEAPMEM_HDRSZ - 1) / HEAPMEM_HDRSZ) * HEAPMEM_HDRSZ)

typedef struct {
  unsigned len : 15;//本快的长度最大为2^16-1个字节，且申请空间的最小粒度为2个字节
  unsigned inUse : 1;//标志位表示本快是否已经被使用
} heapMemHdrHdr_t;

typedef union {
  //因此，当halDataAlign\u t小于UINT16时，编译器强制结构对齐最大的元素，而不会在目标上浪费空间。
  uint8 alignDummy;
  uint16 val;//存储上一块长度，in use信息
  heapMemHdrHdr_t hdr;//快头指针
} heapMemHdr_t;

static __no_init heapMemHdr_t all_heap[HEAP_SIZE];//定义堆空间数组
static __no_init heapMemHdr_t *ff1;  //第一个空块

static uint8 heapMemStat;            // 离散状态标志 0x01：踢出

/**************************************************************************************************
 *@函数            heap_init
 *
 *@简述            堆空间初始化
 *
 *@输入参数  
 *
 *@参数            无
 *
 *输出参数
 *
 *无
 * 
 *@返回           无
 **************************************************************************************************
 */
void heap_init(void)
{
  all_heap[HEAP_LASTBLK_IDX].val = 0;//在堆的末尾设置一个空块，以便与零进行快速比较
  ff1 = all_heap;//设置管理小块空间的首部
  ff1->val = HEAPMEM_SMALLBLK_BUCKET;
  //设置划分小块空间与大块空间的首部
  all_heap[HEAPMEM_SMALLBLK_HDRCNT].val = (HEAPMEM_HDRSZ | HEAPMEM_IN_USE);;
  // 设置管理大块空间首部
  all_heap[HEAPMEM_BIGBLK_IDX].val = HEAPMEM_BIGBLK_SZ;  // Set 'len' & clear 'inUse' field.
}
/**************************************************************************************************
 *@函数            heap_kick
 *
 *@简述            将长存块空间从小块空间去除
 *
 *@输入参数  
 *
 *@参数            无
 *
 *输出参数
 *
 *无
 * 
 *@返回           无
 **************************************************************************************************
 */
void heap_kick(void)
{
//  uint8 intState;
  //申请一个单位内存空间，这样这个空间就会紧挨长存块空间后
  heapMemHdr_t *tmp = heap_alloc(1);
  //进入临界区
  //ff1指向刚申请内存空间首部，即ff1获取到了第一块小块空闲地址
  ff1 = tmp - 1;   //偏移到空间首部
  heap_free(tmp); //释放刚申请的内存空间
  heapMemStat = 0x01; //设置长存块剔除标志

  //退出临界区
}
/**************************************************************************************************
 *@函数            heap_alloc
 *
 *@简述            堆空间分配函数
 *
 *@输入参数  
 *
 *@参数            size - 字节长度
 *
 *输出参数
 *
 *无
 * 
 *@返回           void*
 **************************************************************************************************
 */
void *heap_alloc(uint16 size)
{
  heapMemHdr_t *prev = NULL;
  heapMemHdr_t *hdr;
//  uint8 intState;
  uint8 coal = 0;
  size += HEAPMEM_HDRSZ; //给需要申请的空间分配一个管理首部
 //进入临界区
 
  //调整size大小，是空间对齐（与处理器和编译器相关）
  if ( sizeof( uint8 ) == 2 )//假设uint8占用2个字节
  {
    size += (size & 0x01);//假设为196个，则size为196；假设为197个，则size要198才满足
  }
  else if ( sizeof( uint8 ) != 1 )
  {
    const uint8 mod = size % sizeof( uint8 );

    if ( mod != 0 )
    {
      size += (sizeof( uint8 ) - mod);
    }
  }
  //判断小块内存空间是否足够分配，否则向大块内存空间申请
  if ((heapMemStat == 0) || (size <= HEAPMEM_SMALL_BLKSZ))
  {
    hdr = ff1;//小块内存，从ff1开始查找
  }
  else
  {
    hdr = (all_heap + HEAPMEM_BIGBLK_IDX);//从大块开始查找
  }
  
  //开始迭代的寻找适合的内存空间
  do
  {
    if ( hdr->hdr.inUse )//遇到非free块
    {
      coal = 0;//告诉下一块，本块非free
    }
    else //遇到free块
    {
      if ( coal != 0 )//上一块是free块
      {
        prev->hdr.len += hdr->hdr.len;//两个free块合并相邻内存空间

        if ( prev->hdr.len >= size )  //合并后的大小满足size
        {
          hdr = prev;  //得到块的地址
          break;
        }
      }
      else //上一块是非free块
      {
        if ( hdr->hdr.len >= size )//一个快的大小就可以满足情况，分配，跳出循环返回
        {
          break;
        }

        coal = 1;//否则，标记coal为1，告诉下一块，本快是free的
        prev = hdr; //保存当前内存地址
      }
    }
    //(uint8 *)hdr这个操作使本来2个字节，强制转换成1个字节
    hdr = (heapMemHdr_t *)((uint8 *)hdr + hdr->hdr.len);//经典malloc实现方式，迭代下一块

    if ( hdr->val == 0 )//已经到达堆底部（初始化时，已经让堆底为零，方便识别）
    {
      hdr = NULL;//空指针，表示找不到合适size块
      break;
    }
  }while(1);
  
  if ( hdr != NULL )//已经找到合适size块
  {
    uint16 tmp = hdr->hdr.len - size;//表示块的大小大于请求的大小时，为了不浪费空间，还要把块切开
    //确定是否满足拆分阈值
    if ( tmp >= HEAPMEM_MIN_BLKSZ )//剩下的大小可以单独成为一个free块
    {
      // Split the block before allocating it.
      heapMemHdr_t *next = (heapMemHdr_t *)((uint8 *)hdr + size);
      next->val = tmp;                     // 告诉后一个块自己的信息
      hdr->val = (size | HEAPMEM_IN_USE);  // value代表前一块的大小和使用情况，这样相当于双向链表
    }
    else
    {
      hdr->hdr.inUse = TRUE; //标记本块已经被使用
    }
    
  //  if ((heapMemStat != 0) && (ff1 == hdr))
  //  {
   //   ff1 = (heapMemHdr_t *)((uint8 *)hdr + hdr->hdr.len);
   // }
    hdr++;   
  }
  //退出临界区
  return (void *)hdr;
}
/**************************************************************************************************
 *@函数            heap_free
 *
 *@简述            堆释放函数
 *
 *@输入参数  
 *
 *@参数            *ptr - 内存地址
 *
 *输出参数
 *
 *无
 * 
 *@返回           无
 **************************************************************************************************
 */
void heap_free(void *ptr)
{
  //如果heapMemHdr_t为2个字节，则下面指针减去1，物理地址会改变2
  heapMemHdr_t *hdr = (heapMemHdr_t *)ptr - 1;//获取该内存空间首部
 // uint8 intState;
  //进入中断临界
  hdr->hdr.inUse = FALSE; //标记使用状态为：未使用
  
  ptr = NULL;
  
 // if (ff1 > hdr)
 // {
 //   ff1 = hdr;//调整ff1位置
 // }
  //退出中断临界
}