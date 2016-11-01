#include "MemoryPool.h"
#include "FragmentBlockMemoryPool.h"

using namespace zTools;

MemoryPool::MemoryPool() : m_uiAllMemorySize(0)
											,m_uiMaxMemorySize(0)
{
	MEMORYSTATUS status;
	GlobalMemoryStatus(&status);
	m_uiMaxMemorySize = status.dwAvailPhys;
}

MemoryPool::~MemoryPool()
{

}

MemoryPool* MemoryPool::CreateMemoryPool(const char* typeName)
{
	return new FragmentBlockMemoryPool();//new SOA::BoostMemoryPool::BoostMemoryPool();
}

//void* mallocMemory_MemoryPool( unsigned long nSize )
//{
//	return MemoryPool::getInstance()->getMemory(nSize);
//}
//
//void freeMemory_MemoryPool( void* memory )
//{
//	MemoryPool::getInstance()->freeMemory(memory);
//}
