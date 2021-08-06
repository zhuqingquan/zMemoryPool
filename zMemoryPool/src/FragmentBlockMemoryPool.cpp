
#include "FragmentBlockMemoryPool.h"
#include <stdio.h>
#include <algorithm>
#ifdef _WINDOWS
#pragma comment(lib, "Winmm.lib")
#else
#include <sys/time.h>
#include <unistd.h>
#endif

using namespace zTools;

uint64_t getTimeInMillsec()
{
#ifdef _WINDOWS
	return timeGetTime();
#else
	struct timeval now;
	gettimeofday(&now, nullptr);
	time_t nowInMillsec = now.tv_sec * 1000 + now.tv_usec / 1000;
	return (uint64_t)nowInMillsec;
#endif
}

FragmentBlockMemoryPool::FragmentBlockMemoryPool(void) 
: m_hThread(NULL)
, m_bRuning(true)
, m_hit(0), m_loss(0)
{
	m_startTs = getTimeInMillsec();
    m_hThread = new thread(bind(&FragmentBlockMemoryPool::timeToFreeThreadCallback, this));
}

FragmentBlockMemoryPool::~FragmentBlockMemoryPool(void)
{
	m_bRuning = false;
    if(m_hThread)
    {
        m_hThread->join();
        delete m_hThread;
        m_hThread = NULL;
    }
	purgePool();
}

unsigned int calsize(unsigned int need, const std::vector<unsigned int>& sortedLine)
{
	unsigned int result = ((need + 32 - 1) & (~(32 - 1)));
	for (size_t i=0; i<sortedLine.size(); i++)
	{
		if(sortedLine[i]>=need)
		{
			return sortedLine[i];
		}
	}
	return result;
}

int FragmentBlockMemoryPool::calculateSize( int size )
{
	int ret = 0;
	if(1024 >= size)
		ret = 1024;
	else if(5*1024 >= size)
		ret = 5*1024;
	else if(10 * 1024 >= size)
		ret = 10 * 1024;
	else if(100 * 1024 >= size)
		ret = 100 * 1024;
	else if(200 * 1024 >= size)
		ret = 200 * 1024;
	else if(500 * 1024 >= size)
		ret = 500 * 1024;
	else if(1*1024*1024 >= size)
		ret = 1*1024*1024;
	else if((704*576*4+1) >= size)    //D1���������  Լ 1.58M
		ret = 704*576*4+8;  //���ֵ����Ϊ 8��������������BoostΪ������8�ֽڶ������4�ֽڶ�����������ֵ�������ٳ�8������4������ͬ
	else if(2*1024*1024 >= size)
		ret = 2*1024*1024;
	else if((1024*768*4+1) >= size)		//DVD��Ƶ��������� 3M ��1024*768��
		ret = 1024*768*4+8;				
	else if((1280*720*4+1) >= size)		//720P��Ƶ��������� 3.5M ��1280*720��
		ret = 1280*720*4+8;
	else if(4*1024*1024 >= size)
		ret = 4*1024*1024;
	else if(8*1024*1024 >= size)
		ret = 8*1024*1024;
	else if(16*1024*1024 >= size)
		ret = 16*1024*1024;
	else if(32*1024*1024 >= size)
		ret = 32*1024*1024;
	else if(64*1024*1024 >= size)
		ret = 64*1024*1024;
	else if(128*1024*1024 >= size)
		ret = 128*1024*1024;
	else if(256*1024*1024 >= size)
		ret = 256*1024*1024;
	else if(500*1024*1024 >= size)
		ret = 500*1024*1024;
	else if(1024*1024*1024 >= size)
		ret = 1024*1024*1024;
	else
		ret = size;
	if(ret > 0)
		ret += (8 - (ret % 8));
	return ret;
}

bool sizeCompare(unsigned int l, unsigned int r)
{
	return l < r;
}

bool FragmentBlockMemoryPool::init(MemPoolInitParam* param)
{
	FragmentBlockMemPoolInitParam* poolParam = dynamic_cast<FragmentBlockMemPoolInitParam*>(param);
	if (!poolParam)	return false;
	m_initparam = *poolParam;//copy the params
	//sort the block size by ascending order
	std::sort(m_initparam.blockSizes.begin(), m_initparam.blockSizes.end(), sizeCompare);
	return true;
}

void* FragmentBlockMemoryPool::malloc( std::size_t memorySize)
{
	if(0 >= memorySize)
		return NULL;
	//unsigned int size = calculateSize(memorySize);
	unsigned int size = calsize(memorySize, m_initparam.blockSizes);
// 	char logMsg[512] = {0};
// 	sprintf(logMsg, "MemoryPool: malloc %u byte = %u KB, real malloc %u KB", memorySize, memorySize / 1024, size / 1024);
// 	OutputDebugString(logMsg);
	map<unsigned int, FragmentBlockPool*>::iterator iter;
	FragmentBlockPool *pFragmentBlockPool = NULL;
	m_lock.lock_shared();
	iter = m_blockMap.find(size);
	if(iter != m_blockMap.end())
	{
		pFragmentBlockPool = iter->second;
	}
	m_lock.unlock_shared();
	if(NULL == pFragmentBlockPool)
	{
		m_lock.lock();
		iter = m_blockMap.find(size);
		if(iter != m_blockMap.end())
		{
			pFragmentBlockPool = iter->second;
		}
		else
		{
			pFragmentBlockPool = new FragmentBlockPool(size);
			m_blockMap[size] = pFragmentBlockPool;
			//pair<unsigned int, FragmentBlockPool*>key_value;
			//key_value.first = size;
			//key_value.second = pFragmentBlockPool;
			//m_blockMap.insert(key_value);
		}
		m_lock.unlock();

		if(NULL == pFragmentBlockPool)
			return NULL;
	}

	unsigned int uiSizeOfNew;
	DWORD *pDW = pFragmentBlockPool->getBlock(uiSizeOfNew);
	if(0 < uiSizeOfNew)
	{
		m_lock.lock();
		this->m_uiAllMemorySize += uiSizeOfNew;
		m_lock.unlock();
		//InterlockedIncrement64(&m_loss);
        m_loss++;
// 		char logMsg[512] = {0};
// 		sprintf(logMsg, "MemoryPool: real alloc %u KB", uiSizeOfNew / 1024);
// 		OutputDebugString(logMsg);
	}
	else
	{
		//InterlockedIncrement64(&m_hit);
        m_hit++;
	}
	return pDW;
}

/**
 * Method		freeMemory
 * @brief		�ͷŷ����ȥ��ָ��
 * @param[in]	void * ptr
 * @return		void
 */
void FragmentBlockMemoryPool::free( void *ptr )
{
	if(NULL == ptr)
		return;
	FragmentBlockPool::FragBlockHead* pBlockHead = (FragmentBlockPool::FragBlockHead*)ptr;
	pBlockHead--;
	if(pBlockHead->flag!= FragmentBlockPool::s_BlockFlag)
		return ;
	unsigned int size = (unsigned int)(pBlockHead->size);
	map<unsigned int,FragmentBlockPool*>::iterator iter;
	FragmentBlockPool *pFragmentBlockPool = NULL;
	m_lock.lock_shared();
	iter = m_blockMap.find(size);
	if(iter != m_blockMap.end())
	{
		pFragmentBlockPool = iter->second;
	}
	m_lock.unlock_shared();
    //refresh the ts of the block that malloced before.
    // boost::posix_time::ptime nowTs = boost::posix_time::microsec_clock::local_time();
    // boost::posix_time::time_duration td = nowTs - m_startTs;
    // int durationToStart = td.total_milliseconds();
	uint64_t nowTs = getTimeInMillsec();
	uint64_t durationToStart = nowTs - m_startTs;
    pBlockHead->ts = (unsigned int)durationToStart;
    pBlockHead++;
	pFragmentBlockPool->releaseBlock((DWORD*)pBlockHead);
}

/**
 * Method		purgePool
 * @brief		����ڴ�أ�������ϵͳ
 * @return		void
 */
void FragmentBlockMemoryPool::purgePool()
{
    //even thought this func called, there are still many memory not free that useing by user.
    //so m_uiAllMemorySize is not equal to 0 after call this func.
	map<unsigned int, FragmentBlockPool*>::iterator iter;
	FragmentBlockPool *pFragmentBlockPool = NULL;
	m_lock.lock();
	for (iter = m_blockMap.begin();
		iter != m_blockMap.end();
		++iter)
	{
		pFragmentBlockPool = iter->second;
		int freedBytes = pFragmentBlockPool->clear();
        m_uiAllMemorySize -= freedBytes;
        printf("Release %d bytes in BlockPool %u Total now=%u\n", freedBytes, pFragmentBlockPool, m_uiAllMemorySize);
	}
	//m_blockMap.clear(); //Can not clear the blockpool map item
	m_lock.unlock();
}

/*
 * ��ȡ��ϵͳ������ڴ���ܺ͡�
 */
unsigned int FragmentBlockMemoryPool::getTotalMemorySize()
{
	return 0;
}

unsigned int FragmentBlockMemoryPool::getSizeOfBlock(void *ptr)
{
	if(NULL == ptr)
		return 0;
	DWORD *p = (DWORD*)ptr;
	p -= 2;
	if(FragmentBlockPool::s_BlockFlag != *p++)
		return 0;
	return *p;
}

//static 
DWORD FragmentBlockMemoryPool::timeToFreeThread(void *pParam)
{
	FragmentBlockMemoryPool *pThis = (FragmentBlockMemoryPool*)pParam;
	pThis->timeToFreeThreadCallback();
	return 0;
}

void FragmentBlockMemoryPool::timeToFreeThreadCallback()
{
	int count = 0;
	map<unsigned int,FragmentBlockPool*>::iterator iter;
	FragmentBlockPool *pFragmentBlockPool = NULL;
	//LARGE_INTEGER systemTime;
	unsigned int uiReleaseSize = 0;
	//unsigned int uiHalfMaxMemorySize = m_uiMaxMemorySize / 2;
	while(true == m_bRuning)
	{
#ifdef _WINDOWS
		Sleep(10);
#else
		usleep(10 * 1000);
#endif
		count ++;
		if(100 > count)
			continue;
		count = 0;
		uiReleaseSize = 0;
		m_lock.lock_shared();
		if(m_uiAllMemorySize < m_uiMaxMemorySize)
		{
			m_lock.unlock_shared();
			continue;
		}
		uint64_t nowTs = getTimeInMillsec();
		int llToStart = (int)(nowTs - m_startTs);
		for(iter = m_blockMap.begin();
			iter != m_blockMap.end();
			++iter)
		{
			pFragmentBlockPool = iter->second;
            int delta = 1000 * 60 * 30;//30min
			uiReleaseSize += pFragmentBlockPool->timeToRelease(llToStart, delta);
		}
		m_lock.unlock_shared();
		if(0 < uiReleaseSize)
		{
			m_lock.lock();
			this->m_uiAllMemorySize -= uiReleaseSize;
			m_lock.unlock();
		}
	}
}
