
#include "FragmentBlockMemoryPool.h"
#include <stdio.h>

using namespace zTools;

FragmentBlockMemoryPool::FragmentBlockMemoryPool(void) 
: m_hThread(NULL)
, m_bRuning(true)
, m_hit(0), m_loss(0)
{
	//LARGE_INTEGER freq;
	//int ret = ::QueryPerformanceFrequency(&freq);
	//m_freqQuart = freq.QuadPart;
    m_startTs = boost::posix_time::microsec_clock::local_time();//get start ts when pool create.

	//m_hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)timeToFreeThread,this,0,NULL);
    m_hThread = new boost::thread(boost::bind(&FragmentBlockMemoryPool::timeToFreeThreadCallback, this));
}

FragmentBlockMemoryPool::~FragmentBlockMemoryPool(void)
{
	m_bRuning = false;
	//WaitForSingleObject(m_hThread, INFINITE);
	//CloseHandle(m_hThread);
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
		if(sortedLine[i]>need)
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
	else if((704*576*4+1) >= size)    //D1解码后数据  约 1.58M
		ret = 704*576*4+8;  //这个值必须为 8的整数倍，否则Boost为了满足8字节对齐或者4字节对齐可能在这个值基础上再乘8（或者4），下同
	else if(2*1024*1024 >= size)
		ret = 2*1024*1024;
	else if((1024*768*4+1) >= size)		//DVD视频解码后数据 3M （1024*768）
		ret = 1024*768*4+8;				
	else if((1280*720*4+1) >= size)		//720P视频解码后数据 3.5M （1280*720）
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
 * @brief		释放分配出去的指针
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
    boost::posix_time::ptime nowTs = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration td = nowTs - m_startTs;
    int durationToStart = td.total_milliseconds();
    pBlockHead->ts = durationToStart;
    pBlockHead++;
	pFragmentBlockPool->releaseBlock((DWORD*)pBlockHead);
}

/**
 * Method		purgePool
 * @brief		清空内存池，返还给系统
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
 * 获取向系统申请的内存的总和。
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
		//Sleep(10);
        boost::this_thread::sleep(boost::posix_time::millisec(10));
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
		//::QueryPerformanceCounter(&systemTime);
        boost::posix_time::ptime nowTs = boost::posix_time::microsec_clock::local_time();
        boost::posix_time::time_duration toStart = nowTs - m_startTs;
        int llToStart = toStart.total_milliseconds();
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
