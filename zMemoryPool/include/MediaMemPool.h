/**
 *	@date		2016:11:1
 *	@name	 	MediaMemPool.h
 *	@author		zhuqingquan	
 *	@brief		A memory pool implement for media application base on FragmentBlockPool
 *				the init param for FragmentBlockPool is fix to media application situation.
 **/
#ifndef _MEDIA_MEMORY_POOL_H_
#define _MEDIA_MEMORY_POOL_H_

#include "MemoryPoolHeader.h"

typedef struct _MediaMempoolInitParam
{
	unsigned long* blockSizeArray;
	unsigned long  sizeArrayCount;
}MediaMempoolInitParam;

/**
 *	@name			MediaMempool_init
 *	@brief			初始化内存池
 *	@param[in]		MediaMempoolInitParam * initparam 初始化参数
 *	@return			MEMORYPOOL_EXPORT_IMPORT bool true--成功 false--失败
 **/
MEMORYPOOL_EXPORT_IMPORT bool MediaMempool_init(MediaMempoolInitParam* initparam);

/**
 *	@name	MediaMempool_deinit
 *	@brief	deinit memory pool. after deinit, all allocate will failed.
 **/
MEMORYPOOL_EXPORT_IMPORT void MediaMempool_deinit();

/**
 * Method		MediaMempool_malloc
 * @brief		在MemoryPool中申请内存空间
 * @param[in]	unsigned long nSize 申请的内存空间的大小
 * @return		void* 申请到的内存空间的首地址，NULL--失败
 */
MEMORYPOOL_EXPORT_IMPORT void* MediaMempool_malloc(unsigned long nSize);

/**
 * Method		MediaMempool_free
 * @brief		将从MemoryPool中申请的内存空间归还到MemoryPool中
 * @param[in]	void * memory 申请的内存空间首地址
 */
MEMORYPOOL_EXPORT_IMPORT void  MediaMempool_free(void* memory);

/**
 * Method		MediaMempool_purgePool
 * @brief		将从MemoryPool中的空闲内存归还到系统中
 *				这个归还的策略未必是全部空闲内存都归还，也可能保留部分
 */
MEMORYPOOL_EXPORT_IMPORT void MediaMempool_purgePool();

#endif//_MEDIA_MEMORY_POOL_H_
