#pragma once
#ifndef _ZTOOLS_FRAGMENT_BLOCK_MEMORY_POOL_H_
#define _ZTOOLS_FRAGMENT_BLOCK_MEMORY_POOL_H_

#include "MemoryPool.h"
#include <map>
#include "FragmentBlockPool.h"
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#ifdef _WINDOWS
#include <windows.h>
#endif
#include <inttypes.h>

namespace zTools
{

using namespace std;

/**
 * @name	FragmentBlockMemPoolInitParam
 * @brief	Param use to init FragmentBlockMemoryPool
 *			Example: blockSizes = {4, 100};
 *			whatever call malloc(1) or malloc(3), the actural malloced size is 4
 *			whatever call malloc(16) or malloc(96), the actural malloced size is 100
 *			when malloc(n) n>100, then the actural malloced size is align4byte(n)
 **/
class FragmentBlockMemPoolInitParam : public zTools::MemPoolInitParam
{
public:
	std::vector<unsigned int> blockSizes;
};//class FragmentBlockMemPoolInitParam

class FragmentBlockMemoryPool : public zTools::MemoryPool
{
public:
	FragmentBlockMemoryPool(void);
	virtual ~FragmentBlockMemoryPool(void);

	virtual bool init(MemPoolInitParam* param);

	virtual void* malloc( std::size_t memorySize);

	/**
	 * Method		freeMemory
	 * @brief		释放分配出去的指针
	 * @param[in]	void * ptr
	 * @return		void
	 */
	virtual void free( void *ptr );

	/**
	 * Method		purgePool
	 * @brief		清空内存池，返还给系统
	 * @return		void
	 */
	virtual void purgePool();

	/*
	 * 获取向系统申请的内存的总和。
	 */
	virtual unsigned int getTotalMemorySize();

	/*
	 * 获取块的大小。
	 */
	virtual unsigned int getSizeOfBlock(void *ptr);

public:
	void timeToFreeThreadCallback();
private:
	int calculateSize( int size );
	static DWORD timeToFreeThread(void *pParam);
private:
	FragmentBlockMemPoolInitParam m_initparam;
	map<unsigned int, FragmentBlockPool*> m_blockMap;
	boost::shared_mutex m_lock;
    boost::posix_time::ptime m_startTs;
	//LONGLONG m_freqQuart;
    //HANDLE m_hThread;
    boost::thread* m_hThread;
    bool m_bRuning;
	LONGLONG m_hit;
	LONGLONG m_loss;
};

}//namespace zTools

#endif //_ZTOOLS_FRAGMENT_BLOCK_MEMORY_POOL_H_
