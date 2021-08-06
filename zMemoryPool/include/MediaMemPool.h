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
 *	@brief			��ʼ���ڴ��
 *	@param[in]		MediaMempoolInitParam * initparam ��ʼ������
 *	@return			MEMORYPOOL_EXPORT_IMPORT bool true--�ɹ� false--ʧ��
 **/
MEMORYPOOL_EXPORT_IMPORT bool MediaMempool_init(MediaMempoolInitParam* initparam);

/**
 *	@name	MediaMempool_deinit
 *	@brief	deinit memory pool. after deinit, all allocate will failed.
 **/
MEMORYPOOL_EXPORT_IMPORT void MediaMempool_deinit();

/**
 * Method		MediaMempool_malloc
 * @brief		��MemoryPool�������ڴ�ռ�
 * @param[in]	unsigned long nSize ������ڴ�ռ�Ĵ�С
 * @return		void* ���뵽���ڴ�ռ���׵�ַ��NULL--ʧ��
 */
MEMORYPOOL_EXPORT_IMPORT void* MediaMempool_malloc(unsigned long nSize);

/**
 * Method		MediaMempool_free
 * @brief		����MemoryPool��������ڴ�ռ�黹��MemoryPool��
 * @param[in]	void * memory ������ڴ�ռ��׵�ַ
 */
MEMORYPOOL_EXPORT_IMPORT void  MediaMempool_free(void* memory);

/**
 * Method		MediaMempool_purgePool
 * @brief		����MemoryPool�еĿ����ڴ�黹��ϵͳ��
 *				����黹�Ĳ���δ����ȫ�������ڴ涼�黹��Ҳ���ܱ�������
 */
MEMORYPOOL_EXPORT_IMPORT void MediaMempool_purgePool();

#endif//_MEDIA_MEMORY_POOL_H_
