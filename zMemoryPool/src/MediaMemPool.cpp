#include "MediaMemPool.h"
#include "FragmentBlockMemoryPool.h"
#include <boost/thread/mutex.hpp>

using namespace zTools;

static zTools::MemoryPool* MediaMemPoolInstance = zTools::MemoryPool::CreateMemoryPool("FragmentBlock");

zTools::MemoryPool* getMemoryPoolInstance()
{
	return MediaMemPoolInstance;
}

bool MediaMempool_init(MediaMempoolInitParam* initparam)
{	
	if(initparam==NULL || initparam->sizeArrayCount<=0 || initparam->blockSizeArray)
		return false;
	zTools::MemoryPool* pool = getMemoryPoolInstance();
	zTools::FragmentBlockMemPoolInitParam* fbinitparam = new zTools::FragmentBlockMemPoolInitParam();
	for (unsigned long i=0; i<initparam->sizeArrayCount; i++)
	{
		fbinitparam->blockSizes.push_back(initparam->blockSizeArray[i]);
	}
	return pool->init(fbinitparam);
}

void* MediaMempool_malloc(unsigned long nSize)
{
	zTools::MemoryPool* pool = getMemoryPoolInstance();
	return pool->malloc(nSize);
}

void  MediaMempool_free(void* memory)
{
	zTools::MemoryPool* pool = getMemoryPoolInstance();
	pool->free(memory);
}

void MediaMempool_purgePool()
{
	zTools::MemoryPool* pool = getMemoryPoolInstance();
	pool->purgePool();
}
