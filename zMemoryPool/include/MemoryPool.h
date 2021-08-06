/*
 * @file	MemoryPool.h
 * @author	lei jian hui
 * @date	2011-11-21  10:46
 * @brief	内存池实现类
 * @Copyright (C) 2011, by Ragile Corporation
 */
#pragma once

#ifndef _ZTOOLS_MemoryPool_H__
#define _ZTOOLS_MemoryPool_H__

#include <string>
#include "MemoryPoolHeader.h"

namespace zTools
{
	/**
	 * @name	MemPoolInitParam
	 * @brief	Params use to init memory pool
	 *			Specify memory pool have specify init param
	 **/
	class MEMORYPOOL_EXPORT_IMPORT MemPoolInitParam
	{
	public:
		virtual ~MemPoolInitParam() = 0;
	};

	/**
	* @name		MemoryPool
	* @brief		内存池实现类
	*				原理： 利用定长boost::Pool，创建阶梯状大小的内存池
	*						对外则看成是一个统一的内存池，每次分配出适合大小的内存块
	*				优点：
	*					1. 编码方便。无内存，指针管理，基本都由boost解决
	*					2. 性能可靠。号称准标准库的boost...
	*				缺点：
	*					1. 分配出的内存块会有浪费。
	*					2. 太大的内存数据申请会占用内存。boost 按初始32倍大小向系统申请（首次申请内存时创建）
	*					3. 内存占用只会加大，不会减少，除非使用释放purge_memory释放回系统。
	* @ThreadSafe	是
	*/
	class MEMORYPOOL_EXPORT_IMPORT MemoryPool 
	{
	public:	
		/**
		* Method		CreateMemoryPool
		* @brief		根据typeName创建对应的MemoryPool对象
		* @param[in]	const char* typename MemoryPool的类型名称
		* @return		创建的MemoryPool对象
		*/
		static MemoryPool* CreateMemoryPool(const char* typeName);

		/**
		* Method		ReleaseMemoryPool
		* @brief		release MemoryPool obj
		*/
		static void ReleaseMemoryPool(MemoryPool** mempool);

		/**
		* Method		init
		* @brief		如果MemoryPool在使用之前需要进行初始化，则调用该方法进行初始化
		*				如果不需初始化则也可不调用该函数，MemoryPool接口的具体实现类因对此进行特别说明
		* @param[in]	MemPoolInitParam* param 用于初始化MemoryPool的参数，根据MemoryPool类型的不同而不同
		* @return		true--初始化成功  false--初始化失败
		*/
		virtual bool init(MemPoolInitParam* param) = 0;

		/**
			* Method		~MemoryPool
			* @brief		析构函数
			*/
		virtual ~MemoryPool() = 0;

		/**
			* Method		getMemory
			* @brief		获取一大小适合的内存块
			* @param[in]	const std::size_t & sMemorySize
			* @return		MemoryStreamPtr
			*/
		virtual void* malloc( std::size_t memorySize) = 0;

		/**
			* Method		freeMemory
			* @brief		释放分配出去的指针
			* @param[in]	void * ptr
			*/
		virtual void free( void *ptr ) = 0;

		/**
			* Method		purgePool
			* @brief		清空内存池，返还给系统
			*/
		virtual void purgePool() = 0;

		/*
			* 获取向系统申请的内存的总和。
			*/
		virtual unsigned int getTotalMemorySize() = 0;
	protected:
		/**
		* Method		MemoryPool
		* @brief		构造函数
		* @return
		*/
		MemoryPool();

		unsigned int m_uiAllMemorySize;//向系统申请的，所有内存的总和。
		unsigned int m_uiMaxMemorySize;//大概最大能向系统申请的内存。
	};

	/**
		* Method		mallocMemory_MemoryPool
		* @brief		在MemoryPool中申请内存空间
		* @param[in]	unsigned long nSize 申请的内存空间的大小
		* @return		void* 申请到的内存空间的首地址，NULL--失败
		*/
	//MEMORYPOOL_EXPORT_IMPORT void* mallocMemory_MemoryPool(unsigned long nSize);
	/**
		* Method		freeMemory_MemoryPool
		* @brief		将从MemoryPool中申请的内存空间归还到MemoryPool中
		* @param[in]	void * memory 申请的内存空间首地址
		*/
	//MEMORYPOOL_EXPORT_IMPORT void  freeMemory_MemoryPool(void* memory);
}//namespace zTools
#endif // _ZTOOLS_MemoryPool_H__
