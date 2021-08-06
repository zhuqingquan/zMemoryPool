/*
 * @file	MemoryPool.h
 * @author	lei jian hui
 * @date	2011-11-21  10:46
 * @brief	�ڴ��ʵ����
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
	* @brief		�ڴ��ʵ����
	*				ԭ�� ���ö���boost::Pool����������״��С���ڴ��
	*						�����򿴳���һ��ͳһ���ڴ�أ�ÿ�η�����ʺϴ�С���ڴ��
	*				�ŵ㣺
	*					1. ���뷽�㡣���ڴ棬ָ�������������boost���
	*					2. ���ܿɿ����ų�׼��׼���boost...
	*				ȱ�㣺
	*					1. ��������ڴ������˷ѡ�
	*					2. ̫����ڴ����������ռ���ڴ档boost ����ʼ32����С��ϵͳ���루�״������ڴ�ʱ������
	*					3. �ڴ�ռ��ֻ��Ӵ󣬲�����٣�����ʹ���ͷ�purge_memory�ͷŻ�ϵͳ��
	* @ThreadSafe	��
	*/
	class MEMORYPOOL_EXPORT_IMPORT MemoryPool 
	{
	public:	
		/**
		* Method		CreateMemoryPool
		* @brief		����typeName������Ӧ��MemoryPool����
		* @param[in]	const char* typename MemoryPool����������
		* @return		������MemoryPool����
		*/
		static MemoryPool* CreateMemoryPool(const char* typeName);

		/**
		* Method		ReleaseMemoryPool
		* @brief		release MemoryPool obj
		*/
		static void ReleaseMemoryPool(MemoryPool** mempool);

		/**
		* Method		init
		* @brief		���MemoryPool��ʹ��֮ǰ��Ҫ���г�ʼ��������ø÷������г�ʼ��
		*				��������ʼ����Ҳ�ɲ����øú�����MemoryPool�ӿڵľ���ʵ������Դ˽����ر�˵��
		* @param[in]	MemPoolInitParam* param ���ڳ�ʼ��MemoryPool�Ĳ���������MemoryPool���͵Ĳ�ͬ����ͬ
		* @return		true--��ʼ���ɹ�  false--��ʼ��ʧ��
		*/
		virtual bool init(MemPoolInitParam* param) = 0;

		/**
			* Method		~MemoryPool
			* @brief		��������
			*/
		virtual ~MemoryPool() = 0;

		/**
			* Method		getMemory
			* @brief		��ȡһ��С�ʺϵ��ڴ��
			* @param[in]	const std::size_t & sMemorySize
			* @return		MemoryStreamPtr
			*/
		virtual void* malloc( std::size_t memorySize) = 0;

		/**
			* Method		freeMemory
			* @brief		�ͷŷ����ȥ��ָ��
			* @param[in]	void * ptr
			*/
		virtual void free( void *ptr ) = 0;

		/**
			* Method		purgePool
			* @brief		����ڴ�أ�������ϵͳ
			*/
		virtual void purgePool() = 0;

		/*
			* ��ȡ��ϵͳ������ڴ���ܺ͡�
			*/
		virtual unsigned int getTotalMemorySize() = 0;
	protected:
		/**
		* Method		MemoryPool
		* @brief		���캯��
		* @return
		*/
		MemoryPool();

		unsigned int m_uiAllMemorySize;//��ϵͳ����ģ������ڴ���ܺ͡�
		unsigned int m_uiMaxMemorySize;//����������ϵͳ������ڴ档
	};

	/**
		* Method		mallocMemory_MemoryPool
		* @brief		��MemoryPool�������ڴ�ռ�
		* @param[in]	unsigned long nSize ������ڴ�ռ�Ĵ�С
		* @return		void* ���뵽���ڴ�ռ���׵�ַ��NULL--ʧ��
		*/
	//MEMORYPOOL_EXPORT_IMPORT void* mallocMemory_MemoryPool(unsigned long nSize);
	/**
		* Method		freeMemory_MemoryPool
		* @brief		����MemoryPool��������ڴ�ռ�黹��MemoryPool��
		* @param[in]	void * memory ������ڴ�ռ��׵�ַ
		*/
	//MEMORYPOOL_EXPORT_IMPORT void  freeMemory_MemoryPool(void* memory);
}//namespace zTools
#endif // _ZTOOLS_MemoryPool_H__
