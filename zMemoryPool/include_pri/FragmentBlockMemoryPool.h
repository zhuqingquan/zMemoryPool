#pragma once
#ifndef _ZTOOLS_FRAGMENT_BLOCK_MEMORY_POOL_H_
#define _ZTOOLS_FRAGMENT_BLOCK_MEMORY_POOL_H_

#include "MemoryPool.h"
#include <map>
#include "FragmentBlockPool.h"

#if _WINDOWS && !HAS_CPP11
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread.hpp>
using namespace boost;
#else
#include <shared_mutex>
#include <thread>
#include <functional>
using namespace std;
#endif//#if _WINDOWS && !HAS_CPP11

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
	 * @brief		�ͷŷ����ȥ��ָ��
	 * @param[in]	void * ptr
	 * @return		void
	 */
	virtual void free( void *ptr );

	/**
	 * Method		purgePool
	 * @brief		����ڴ�أ�������ϵͳ
	 * @return		void
	 */
	virtual void purgePool();

	/*
	 * ��ȡ��ϵͳ������ڴ���ܺ͡�
	 */
	virtual unsigned int getTotalMemorySize();

	/*
	 * ��ȡ��Ĵ�С��
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
	shared_mutex m_lock;
	uint64_t m_startTs;
    //boost::posix_time::ptime m_startTs;
	//LONGLONG m_freqQuart;
    //HANDLE m_hThread;
    thread* m_hThread;
    bool m_bRuning;
	LONGLONG m_hit;
	LONGLONG m_loss;
};

}//namespace zTools

#endif //_ZTOOLS_FRAGMENT_BLOCK_MEMORY_POOL_H_
