// MemoryPoolTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "MemoryPool.h"
#include "..\MemoryPool\include_pri\FragmentBlockMemoryPool.h"
#include <list>
#include <time.h>

unsigned int calsize(unsigned int need, const std::vector<unsigned int>& sortedLine)
{
	unsigned int result = (need + 32) ^ 31;
	for (size_t i=0; i<sortedLine.size(); i++)
	{
		if(sortedLine[i]>need)
		{
			return sortedLine[i];
		}
	}
	return result;
}

int _tmain(int argc, _TCHAR* argv[])
{
	zTools::MemoryPool* pool = zTools::MemoryPool::CreateMemoryPool("FragmentBlock");
	zTools::FragmentBlockMemPoolInitParam* initparam = new zTools::FragmentBlockMemPoolInitParam();
	initparam->blockSizes.push_back(4);
	initparam->blockSizes.push_back(1920*1080*3);
	initparam->blockSizes.push_back(1920*1080*2);
	initparam->blockSizes.push_back(1920*1080*4);
	pool->init(initparam);

	std::list<char*> allocatedBufs;

	char* buf = (char*)pool->malloc(1);
	assert(buf);
	allocatedBufs.push_back(buf);

	buf = (char*)pool->malloc(100);
	assert(buf);
	allocatedBufs.push_back(buf);

	buf = (char*)pool->malloc(17);
	assert(buf);
	allocatedBufs.push_back(buf);

	buf = (char*)pool->malloc(1920*1080*3+1);
	assert(buf);
	allocatedBufs.push_back(buf);

	buf = (char*)pool->malloc(1920*1080*4+1);
	assert(buf);
	allocatedBufs.push_back(buf);

	srand(time(NULL));

	unsigned int min = 0;
	unsigned int max = 1920*2*1080*2*4;
	while(true)
	{
		for (size_t i=0; i<100; i++)
		{
			unsigned int rnum = rand();
			rnum = (rnum % (max-min)) + min;
			buf = (char*)pool->malloc(rnum);
			allocatedBufs.push_back(buf);
			Sleep(100);
		}
		
		for (size_t i=0; i<100; i++)
		{
			pool->free(allocatedBufs.front());
			allocatedBufs.pop_front();
			Sleep(100);
		}
		Sleep(1);
	}
	return 0;
}

