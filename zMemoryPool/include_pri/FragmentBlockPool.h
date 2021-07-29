#pragma once
#ifndef _ZTOOLS_FRAGMEN_BLOCK_POOL_H_
#define _ZTOOLS_FRAGMEN_BLOCK_POOL_H_

#ifdef _WINDOWS
#include <windows.h>
#else
typedef unsigned int DWORD;
typedef long long LONGLONG;
#endif


#include <inttypes.h>
#include <vector>

#if _WINDOWS && !HAS_CPP11
#include <boost/thread/mutex.hpp>
using namespace boost;
#else
#include <mutex>
using namespace std;
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
