#pragma once
#ifndef _ZTOOLS_FRAGMENT_BLOCK_MEMORY_POOL_H_
#define _ZTOOLS_FRAGMENT_BLOCK_MEMORY_POOL_H_

#include <map>
#include "MemoryPool.h"
#include "FragmentBlockPool.h"

#if _WINDOWS 
#include <windows.h>

#if !HAS_CPP11
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread.hpp>

using namespace boost;
#else
#include <inttypes.h>
#include <shared_mutex>
#include <thread>
#include <functional>
using namespace std;
#endif//!HAS_CPP11

#else//NO _WINDOWS
#include <shared_mutex>
#include <thread>
#include <functional>
using namespace std;
#endif//#if _WINDOWS && !HAS_CPP11

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

	virtual bool init(MemPoolInitParam* param) override;
	virtual void* malloc( std::size_t memorySize) override;
	virtual void free( void *ptr ) override;

	/**
	 * Method		purgePool
	 * @brief		清空内存池，返还给系统
	 * @return		void
	 */
	virtual void purgePool();

	virtual unsigned int getTotalMemorySize();

	virtual unsigned int getSizeOfBlock(void *ptr);

public:
	void timeToFreeThreadCallback();
private:
	void releasePool();
	int calculateSize( int size );
	static DWORD timeToFreeThread(void *pParam);
private:
	FragmentBlockMemPoolInitParam m_initparam;
	map<unsigned int, FragmentBlockPool*> m_blockMap;
	shared_mutex m_lock;
	uint64_t m_startTs;
    //boost::posix_time::ptime m_startTs;
    thread* m_hThread;
    bool m_bRuning;
	std::atomic<uint64_t> m_hit;
	std::atomic<uint64_t> m_loss;
};

}//namespace zTools

#endif //_ZTOOLS_FRAGMENT_BLOCK_MEMORY_POOL_H_
