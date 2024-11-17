#include "MediaMemPool.h"
#include "FragmentBlockMemoryPool.h"

using namespace zTools;

static zTools::MemoryPool* MediaMemPoolInstance = nullptr;		//全局对象，全局只创建一个MemoryPool对象
zTools::MemoryPool* getMemoryPoolInstance()
{
	if(MediaMemPoolInstance==nullptr)
	{
		MediaMemPoolInstance = zTools::MemoryPool::CreateMemoryPool("FragmentBlock");
	}
	return MediaMemPoolInstance;
}

void releaseMediaMempoolInstance()
{
	if(MediaMemPoolInstance!=nullptr)
	{
		zTools::MemoryPool::ReleaseMemoryPool(&MediaMemPoolInstance);
		MediaMemPoolInstance = nullptr;
	}
}

bool MediaMempool_init(MediaMempoolInitParam* initparam)
{	
	if(initparam==NULL || initparam->sizeArrayCount<=0 || initparam->blockSizeArray==NULL)
		return false;
	zTools::MemoryPool* pool = getMemoryPoolInstance();
	zTools::FragmentBlockMemPoolInitParam fbinitparam;
	for (unsigned long i=0; i<initparam->sizeArrayCount; i++)
	{
		fbinitparam.blockSizes.push_back(initparam->blockSizeArray[i]);
	}
	return pool->init(&fbinitparam);
}

void MediaMempool_deinit()
{	
	releaseMediaMempoolInstance();
}

void* MediaMempool_malloc(unsigned long nSize)
{
	zTools::MemoryPool* pool = getMemoryPoolInstance();
	return pool!=NULL ? pool->malloc(nSize) : NULL;
}

void  MediaMempool_free(void* memory)
{
	zTools::MemoryPool* pool = getMemoryPoolInstance();
	if(pool!=NULL) pool->free(memory);
}

void MediaMempool_purgePool()
{
	zTools::MemoryPool* pool = getMemoryPoolInstance();
	if(pool!=NULL) pool->purgePool();
}
