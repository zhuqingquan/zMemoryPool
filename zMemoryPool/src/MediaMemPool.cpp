#include "MediaMemPool.h"
#include "FragmentBlockMemoryPool.h"
// #include <boost/thread/mutex.hpp>

using namespace zTools;

static zTools::MemoryPool* MediaMemPoolInstance = zTools::MemoryPool::CreateMemoryPool("FragmentBlock");
/*
_MediaMempoolInitParam::_MediaMempoolInitParam()
: blockSizeArray(NULL)
, sizeArrayCount(0)
{}

_MediaMempoolInitParam::~_MediaMempoolInitParam()
{
}
*/
zTools::MemoryPool* getMemoryPoolInstance()
{
	return MediaMemPoolInstance;
}

bool MediaMempool_init(MediaMempoolInitParam* initparam)
{	
	if(initparam==NULL || initparam->sizeArrayCount<=0 || initparam->blockSizeArray==NULL)
		return false;
	zTools::MemoryPool* pool = getMemoryPoolInstance();
	zTools::FragmentBlockMemPoolInitParam* fbinitparam = new zTools::FragmentBlockMemPoolInitParam();
	for (unsigned long i=0; i<initparam->sizeArrayCount; i++)
	{
		fbinitparam->blockSizes.push_back(initparam->blockSizeArray[i]);
	}
	return pool->init(fbinitparam);
}

void MediaMempool_deinit()
{	
	zTools::MemoryPool* pool = getMemoryPoolInstance();
	if(pool!=NULL)
	{
		zTools::MemoryPool::ReleaseMemoryPool(&pool);
		MediaMemPoolInstance = NULL;
	}
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
