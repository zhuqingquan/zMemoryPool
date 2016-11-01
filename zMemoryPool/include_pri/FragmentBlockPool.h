#pragma once
#ifndef _ZTOOLS_FRAGMEN_BLOCK_POOL_H_
#define _ZTOOLS_FRAGMEN_BLOCK_POOL_H_

#include <windows.h>
#include <vector>
#include <boost/thread/mutex.hpp>

namespace zTools
{

using namespace std;
class FragmentBlockPool
{
public:
	FragmentBlockPool(unsigned int uiBlockSize);
	~FragmentBlockPool(void);

	DWORD *getBlock(LONGLONG llTime,unsigned int &uiSizeOfNew);
	int releaseBlock(DWORD *pBlock);

	unsigned int timeToRelease(LONGLONG llTime,LONGLONG llFeqQuart);

public:
	static DWORD s_BlockFlag;//±êÖ¾Î»¡£
private:
	DWORD *newBlock();
	int releaseAllBlock();
	int releaseBlockToHalf();
	int releaseBlockToOne();
	int releaseByCount(int count);
private:
	
	unsigned int m_uiBlockSize;
	unsigned int m_uiNewSizeOfBlock;
	unsigned int m_uiNewDWORDCountOfBlock;
	vector<DWORD*> m_pMemory;
	boost::mutex m_lock;
	LONGLONG m_llLastUseTime;
};

}//namespace zTools

#endif //_ZTOOLS_FRAGMEN_BLOCK_POOL_H_