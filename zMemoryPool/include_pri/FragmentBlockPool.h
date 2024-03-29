#pragma once
#ifndef _ZTOOLS_FRAGMEN_BLOCK_POOL_H_
#define _ZTOOLS_FRAGMEN_BLOCK_POOL_H_

#include <vector>
#include "MemoryPoolHeader.h"

#if _WINDOWS 
#include <windows.h>

#if !HAS_CPP11
#include <boost/thread/mutex.hpp>
typedef boost::mutex mutex;
#else
#include <inttypes.h>
#include <mutex>
#endif // !HAS_CPP11

#else
#include <mutex>
typedef unsigned int DWORD;
typedef long long LONGLONG;
#endif

namespace zTools
{

using namespace std;
class FragmentBlockPool
{
public:
	FragmentBlockPool(unsigned int uiBlockSize);
	~FragmentBlockPool(void);

	DWORD* getBlock(unsigned int &uiSizeOfNew);
	int releaseBlock(DWORD *pBlock);

    /**
     * @name    timeToRelease
     * @brief   Free the block that not use in delta milliseconds to the last time call releaseBlock.
     * @param[in] time of now
     * @param[in] int delta dalta milliseconds
     * @return unsigned int Bytes count freed.
     **/
	unsigned int timeToRelease(int llTime, int delta);

	int clear();
public:
    struct FragBlockHead
    {
        unsigned int flag;
        unsigned int size;
        unsigned int ts;    //timestamp of free
    };
	static DWORD s_BlockFlag;//��־λ��
private:
	DWORD *newBlock();
	int releaseAllBlock();
	//int releaseBlockToHalf();
	//int releaseBlockToOne();
	//int releaseByCount(int count);
private:
	
	unsigned int m_uiBlockSize;
	unsigned int m_uiNewSizeOfBlock;
	unsigned int m_uiNewDWORDCountOfBlock;
	vector<DWORD*> m_pMemory;
	mutex m_lock;
	int m_llLastUseTime;
};

}//namespace zTools

#endif //_ZTOOLS_FRAGMEN_BLOCK_POOL_H_
