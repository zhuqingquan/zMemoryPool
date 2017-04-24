#include "FragmentBlockPool.h"

using namespace zTools;

DWORD FragmentBlockPool::s_BlockFlag = 0xf83e65a3;

FragmentBlockPool::FragmentBlockPool(unsigned int uiBlockSize) 
: m_uiBlockSize(uiBlockSize)
, m_uiNewSizeOfBlock(0)
, m_uiNewDWORDCountOfBlock(0)
, m_llLastUseTime(0)
{
	m_uiNewDWORDCountOfBlock = m_uiBlockSize / sizeof(DWORD) + 1;
	m_uiNewSizeOfBlock = m_uiNewDWORDCountOfBlock * sizeof(DWORD) + sizeof(FragBlockHead);
}

FragmentBlockPool::~FragmentBlockPool(void)
{	
	clear();
}

int FragmentBlockPool::releaseAllBlock()
{
	DWORD *pDW = NULL;
	int bytescount = 0;
	for(vector<DWORD*>::iterator iter = m_pMemory.begin();
		iter != m_pMemory.end();
		++iter)
	{
		pDW = *iter;
        FragBlockHead* pHead = (FragBlockHead*)pDW;
        pHead--;
        bytescount += pHead->size;
		free(pHead);
	}
	m_pMemory.clear();
	return bytescount;
}
/*
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
*/
DWORD *FragmentBlockPool::newBlock()
{
	//DWORD *pRet = (DWORD*)malloc(m_uiNewDWORDCountOfBlock*sizeof(DWORD));//new DWORD[m_uiNewDWORDCountOfBlock];
	//*pRet++ = s_BlockFlag;
	//*pRet++ = m_uiBlockSize;
	FragBlockHead *pRet = (FragBlockHead*)malloc(m_uiNewSizeOfBlock);
    pRet->flag = s_BlockFlag;
    pRet->size = m_uiBlockSize;
    pRet->ts = 0;
    pRet++;
	return (DWORD*)pRet;
}

DWORD *FragmentBlockPool::getBlock(unsigned int &uiSizeOfNew)
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

unsigned int FragmentBlockPool::timeToRelease(int llTime, int delta)
{
	int countOfRelease = 0;
	m_lock.lock();
    for (size_t i = 0; i < m_pMemory.size(); i++) 
    {
        DWORD* pBlock = m_pMemory[i];
        FragBlockHead* pHead = (FragBlockHead*)pBlock;
        pHead--;
        int realdelta = llTime - pHead->ts;
        if(realdelta<delta)
            continue;
        countOfRelease += pHead->size;
        free(pHead);
        m_pMemory[i] = NULL;
    }
    size_t h = 0;
    size_t t = m_pMemory.size() <= 0 ? 0 : m_pMemory.size() - 1;
    while(h<t)
    {
        while(m_pMemory[h]!=NULL)
        {
            h++;
        }
        while(m_pMemory[t]==NULL)
        {
            t--;
        }
        if(h<t)
        {
            DWORD* tmp = m_pMemory[h];
            m_pMemory[h] = m_pMemory[t];
            m_pMemory[t] = tmp;
        }
    }
    while(m_pMemory.size()>0 && m_pMemory.back()==NULL)
    {
        m_pMemory.pop_back();
    }
	m_lock.unlock();
	unsigned int size = (unsigned int)countOfRelease * m_uiNewSizeOfBlock;
	return countOfRelease;
}

int zTools::FragmentBlockPool::clear()
{
    int ret = 0;
	m_lock.lock();
	ret = releaseAllBlock();
	m_lock.unlock();
    return ret;
}
