#include "MemoryPool.h"
#include "FragmentBlockMemoryPool.h"

using namespace zTools;

MemPoolInitParam::~MemPoolInitParam()
{
}

MemoryPool::MemoryPool() 
: m_uiAllMemorySize(0)
, m_uiMaxMemorySize(0)
{
#ifdef _WIN64
	MEMORYSTATUS status;
	GlobalMemoryStatus(&status);
	m_uiMaxMemorySize = status.dwAvailPhys * 0.7;
#elif WIN32
	//32位程序中最大分配3G
	m_uiMaxMemorySize = 3 * 1024 * 1024 * 1024;
#endif
}

MemoryPool::~MemoryPool()
{

}

MemoryPool* MemoryPool::CreateMemoryPool(const char* typeName)
{
	return new FragmentBlockMemoryPool();//new SOA::BoostMemoryPool::BoostMemoryPool();
}

void MemoryPool::ReleaseMemoryPool(MemoryPool** mempool)
{
	if(mempool!=NULL && *mempool!=NULL)
	{
		delete (*mempool);
		*mempool = NULL;
	}
}
