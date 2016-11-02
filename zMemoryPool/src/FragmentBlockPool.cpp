#include "FragmentBlockPool.h"

using namespace zTools;

DWORD FragmentBlockPool::s_BlockFlag = 0xf83e65a3;

FragmentBlockPool::FragmentBlockPool(unsigned int uiBlockSize) :
				m_uiBlockSize(uiBlockSize)
				,m_uiNewSizeOfBlock(0)
				,m_uiNewDWORDCountOfBlock(0)
				,m_llLastUseTime(0)
{
	m_uiNewDWORDCountOfBlock = m_uiBlockSize / sizeof(DWORD) + 1 + 2;
	m_uiNewSizeOfBlock = m_uiNewDWORDCountOfBlock * sizeof(DWORD);
}

FragmentBlockPool::~FragmentBlockPool(void)
{	
	clear();
}

int FragmentBlockPool::releaseAllBlock()
{
	DWORD *pDW = NULL;
	int count = 0;
	for(vector<DWORD*>::iterator iter = m_pMemory.begin();
		iter != m_pMemory.end();
		++iter)
	{
		pDW = *iter;
		pDW -= 2;
		//delete pDW;
		free(pDW);
		count ++;
	}
	m_pMemory.clear();
	return count;
}

int FragmentBlockPool::releaseBlockToHalf()
{
	int count = m_pMemory.size();
	count /= 2;
	int retCount = 0;
	vector<DWORD*>::reference ref = m_pMemory.back();
	DWORD *pDW = NULL;
	while(0 < count && 0 < m_pMemory.size())
	{
		ref = m_pMemory.back();
		pDW = ref;
		pDW -= 2;
		delete pDW;
		m_pMemory.pop_back();
		count --;
		retCount ++;
	}
	return retCount;
}

int FragmentBlockPool::releaseBlockToOne()
{
	int count = m_pMemory.size();
	count -= 1;
	int retCount = 0;
	vector<DWORD*>::reference ref = m_pMemory.back();
	DWORD *pDW = NULL;
	while(0 < count && 0 < m_pMemory.size())
	{
		ref = m_pMemory.back();
		pDW = ref;
		pDW -= 2;
		delete pDW;
		m_pMemory.pop_back();
		count --;
		retCount ++;
	}
	return retCount;
}

int FragmentBlockPool::releaseByCount(int count)
{
	int retCount = 0;
	vector<DWORD*>::reference ref = m_pMemory.back();
	DWORD *pDW = NULL;
	while(0 < count && 0 < m_pMemory.size())
	{
		ref = m_pMemory.back();
		pDW = ref;
		pDW -= 2;
		delete pDW;
		m_pMemory.pop_back();
		count --;
		retCount ++;
	}
	return retCount;
}

DWORD *FragmentBlockPool::newBlock()
{
	//unsigned int size = m_uiBlockSize / sizeof(DWORD) + 1 + 2;
	DWORD *pRet = (DWORD*)malloc(m_uiNewDWORDCountOfBlock*sizeof(DWORD));//new DWORD[m_uiNewDWORDCountOfBlock];
	*pRet++ = s_BlockFlag;
	*pRet++ = m_uiBlockSize;
	return pRet;
}

DWORD *FragmentBlockPool::getBlock(LONGLONG llTime,unsigned int &uiSizeOfNew)
{
	DWORD *pRet = NULL;
	uiSizeOfNew = 0;
	m_lock.lock();
	do 
	{
		if(0 >= m_pMemory.size())
		{
			pRet = newBlock();
			uiSizeOfNew = m_uiNewSizeOfBlock;
			break;
		}
		vector<DWORD*>::reference ref = m_pMemory.back();
		pRet = ref;
		m_pMemory.pop_back();
	} while (false);
	m_llLastUseTime = llTime;
	m_lock.unlock();
	return pRet;
}

int FragmentBlockPool::releaseBlock(DWORD *pBlock)
{
	m_lock.lock();
	m_pMemory.push_back(pBlock);
	m_lock.unlock();
	return 0;
}

unsigned int FragmentBlockPool::timeToRelease(LONGLONG llTime,LONGLONG llFeqQuart)
{
	int ret;
	int countOfRelease = 0;
	m_lock.lock();
	ret = m_pMemory.size();
	if(20 < ret)
	{
		ret = ret - 20;
		ret = ret > 5 ? 5: ret;
		countOfRelease += releaseByCount(ret);
	}
	ret = (llTime - m_llLastUseTime) / llFeqQuart;//秒
	if(0 < m_pMemory.size())
	{
		if(2*1024*1024 + 100 >= m_uiBlockSize)
		{
			//2M以内的处理
			if(10 < ret)
			{
				countOfRelease += releaseAllBlock();
			}
		}
		else
		{
			if(5 < ret)
			{
				countOfRelease += releaseAllBlock();
			}
			else if(3 < ret)
			{
				countOfRelease += releaseBlockToOne();
			}
			else if(1 < ret)
			{
				countOfRelease += releaseBlockToHalf();
			}
		}

	}

	m_lock.unlock();
	unsigned int size = (unsigned int)countOfRelease * m_uiNewSizeOfBlock;
	return size;
}

void zTools::FragmentBlockPool::clear()
{
	m_lock.lock();
	releaseAllBlock();
	m_lock.unlock();
}
