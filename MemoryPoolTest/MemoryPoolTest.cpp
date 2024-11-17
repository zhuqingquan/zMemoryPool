// MemoryPoolTest.cpp : �������̨Ӧ�ó������ڵ㡣
//
#ifdef _WINDOWS
#include "stdafx.h"
#endif
#include <list>
#include <time.h>
#include "MemoryPoolHeader.h"

#if !HAS_CPP11
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#else
#include <thread>
#endif

#include "MemoryPool.h"
#include "../zMemoryPool/include_pri/FragmentBlockMemoryPool.h"
#include <stdio.h>
#include <assert.h>
#include "ObjectPool.h"
#include "MediaMemPool.h"

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

struct MediaFrame
{
	uint64_t pts;
	uint64_t dts;
	uint32_t length;
};//struct MediaFrame

zTools::ObjectPool<MediaFrame> gObjpoolFrame(2);
void objpool_malloc_free_random()
{
	size_t count = 1000;
	const size_t MALLOC_TIMES_MAX = 90;
	std::list<MediaFrame*> frames;
	srand((uint32_t)time(nullptr));
	while(count--)
	{
		int mallocTimes = (rand() % MALLOC_TIMES_MAX) + 1;
		for (int i = 0; i < mallocTimes; i++)
		{
			frames.push_back(gObjpoolFrame.get());
		}

		int sleepMs = (rand() % 50);
		Sleep(sleepMs);

		int freeTimes = (rand() % MALLOC_TIMES_MAX) + 1;
		for (int i = 0; i < freeTimes; i++)
		{
			if(frames.size()<=0)
				break;
			gObjpoolFrame.put(frames.front());

			frames.pop_front();
		}
		while (frames.size() > 2 * MALLOC_TIMES_MAX)
		{
			gObjpoolFrame.put(frames.front());
			frames.pop_front();
		}
	}
	std::cout << "Thread done. frames.count=" << frames.size() << " " << __FUNCTION__ << std::endl;
}

void simulate_objectPool_malloc_free()
{
	srand((unsigned int)time(nullptr));
	const size_t threadCount = 1;
	std::vector<thread*> threadList;
	for (size_t i = 0; i < threadCount; i++)
	{
		//thread* th = new thread(objpool_malloc_free_random);
		//threadList.push_back(th);
		thread th(objpool_malloc_free_random);
		th.detach();
	}
	
}

void test_MediaMempoolInitParam()
{
	MediaMempoolInitParam param;
	param.sizeArrayCount = 3;
	param.blockSizeArray = (unsigned long*)malloc(param.sizeArrayCount * sizeof(unsigned long));
	param.blockSizeArray[0] = 1024;
	param.blockSizeArray[0] = 1024 * 4;
	param.blockSizeArray[0] = 1024 * 1024;

	MediaMempool_init(&param);

	void* p = MediaMempool_malloc(128);

	MediaMempool_free(p);

	MediaMempool_purgePool();

	MediaMempool_deinit();

	p = MediaMempool_malloc(1024 * 128);

	MediaMempool_free(p);

}
int main(int argc, char* argv[])
{
	test_MediaMempoolInitParam();
	/*
	zTools::ObjectPool<MediaFrame> objpool_int(2);

	MediaFrame* f1 = objpool_int.get();
	f1->pts = 1;
	MediaFrame* f2 = objpool_int.get();
	f2->pts = 2;
	MediaFrame* f3 = objpool_int.get();
	f3->pts = 3;
	objpool_int.put(f2);
	objpool_int.put(f3);
	MediaFrame* f4 = objpool_int.get();
	if(f4->pts != 3)
		assert(true);

	*/
	simulate_objectPool_malloc_free();

	system("pause");
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
			Sleep(10);
		}
		
		for (size_t i=0; i<tcount; i++)
		{
			pool->free(allocatedBufs.front());
			allocatedBufs.pop_front();
			Sleep(10);
		}
        printf("now to purge pool.\n");
        pool->purgePool();
		Sleep(1);
	}
	return 0;
}

