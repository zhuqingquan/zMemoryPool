// MemoryPoolTest.cpp : 定义控制台应用程序的入口点。
//
#ifdef _WINDOWS
#include "stdafx.h"
#endif
#include <list>
#include <time.h>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include "MemoryPool.h"
#include "../zMemoryPool/include_pri/FragmentBlockMemoryPool.h"
#include <stdio.h>

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

int main(int argc, char* argv[])
{
	zTools::MemoryPool* pool = zTools::MemoryPool::CreateMemoryPool("FragmentBlock");
	zTools::FragmentBlockMemPoolInitParam* initparam = new zTools::FragmentBlockMemPoolInitParam();
	initparam->blockSizes.push_back(4);
	initparam->blockSizes.push_back(1920*1080*3);
	initparam->blockSizes.push_back(1920*1080*2);
	initparam->blockSizes.push_back(1920*1080*4);
	initparam->blockSizes.push_back(1920*2*1080*4);
	initparam->blockSizes.push_back(1920*2*1080*2*4);
	pool->init(initparam);

    printf("Pool inited.\n");

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

    printf("befor while loop...\n");

	unsigned int min = 0;
	unsigned int max = 1920*2*1080*2*4;
	while(true)
	{
        size_t tcount = 100;
		for (size_t i=0; i<tcount; i++)
		{
			unsigned int rnum = rand();
			rnum = (rnum % (max-min)) + min;
			buf = (char*)pool->malloc(rnum);
			allocatedBufs.push_back(buf);
			//Sleep(100);
            boost::this_thread::sleep(boost::posix_time::millisec(10));
		}
		
		for (size_t i=0; i<tcount; i++)
		{
			pool->free(allocatedBufs.front());
			allocatedBufs.pop_front();
			//Sleep(100);
            boost::this_thread::sleep(boost::posix_time::millisec(10));
		}
        printf("now to purge pool.\n");
        pool->purgePool();
		//Sleep(1);
        boost::this_thread::sleep(boost::posix_time::millisec(1));
	}
	return 0;
}

