/*
 * @Author: zhuqingquan
 * @Date: 2021-08-08 19:36:47
 * @LastEditTime: 2021-08-12 00:30:18
 * @Description:  Template for different type Object poll.
 */
#ifndef _Z_OBJECT_POOL_H_
#define _Z_OBJECT_POOL_H_

#include <deque>
#include <mutex>

namespace zTools
{

/**
 * @name PoolStrategy
 * @brief   内存池或者对象池资源管理的策略。解决如下问题：
 *          1. 当池中无数据时，一次应该申请多少的数据（比如多少个对象，或者多大的内存块）
 *          2. 是否需要释放一些对象还回给系统，释放多少？
 **/
class PoolStrategy
{
public:
    /**
     * @brief 当用户从ObjectPool中申请一个Object时，调用此方法通知策略对象
     **/
    void onGetObj()
    {
    }

    /**
     * @brief 当用户将一个Object放回ObjectPool时，调用此方法通知策略对象
     **/
    void onPutObj()
    {

    }

    /**
     * @brief   当从系统申请了count个对象或者内存块时，调用此方法通知策略对象
     **/
    void onMalloc(size_t count)
    {

    }

    /**
     * @brief   当将count个对象或者内存块释放还回给系统时，调用此方法通知策略对象
     **/
    void onFree(size_t count)
    {
        
    }

    /**
     * @brief 当池中没有对象或者内存时，内存池先调用此方法确定需要一次性从系统中申请多少的资源放入池中
     **/
    int getMallocCount()
    {
        return m_initCount;
    }

    /**
     * @brief 返回是否需要将池中的对象或者内存还回一部分给系统，还多少。
     **/
    int getFreeCount()
    {
       return 0; 
    }

private:
    int m_initCount = 2;
    int m_curObjCount = 2;
};//class poolStrategy

/**
 * @name    PoolImpl_Dequeue
 * @brief   A Object pool implemente with std::dequeue to manage the objects.
 **/
template<typename T>
class PoolImpl_Dequeue
{
public:
    PoolImpl_Dequeue(size_t initCount = 0)
    {
        mallocObjs(initCount);
    }

    ~PoolImpl_Dequeue()
    {
        // 这样简单的释放策略可能是有问题的。
        // 如果还有些被使用中的对象没有通过put返回m_queue队列内，则此处将释放不了那些对象。那些对象将变成野指针。
        std::deque<T*> tmpQueue;
        m_mutex.lock();
        m_queue.swap(tmpQueue);
        m_mutex.unlock();
        while(!tmpQueue.empty())
        {
            delete tmpQueue.front();
            tmpQueue.pop_front();
        }
    }

    T* get()
    {
        // 当队列中为空时，一次性申请STEP个对象放入队列
        std::lock_guard<std::mutex> lock(m_mutex);
        if(m_queue.empty())
        {
            int count = m_strategy.getMallocCount();
            if(count>0)
                mallocObjs(count);
        }
        if(m_queue.empty())
        {
            return NULL;
        }
        T* result = m_queue.front();
        m_queue.pop_front();
        m_strategy.onGetObj();
        return result;
    }

    void put(T* obj)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push_front(obj);
        m_strategy.onPutObj();
        int count = m_strategy.getFreeCount();
        if(count>0)
            freeObjs(count);
    }

private:
    void mallocObjs(size_t count)
    {
        for (size_t i = 0; i < count; i++)
        {
            m_queue.push_back(new T());
        }
        m_strategy.onMalloc(count);
    }

    void freeObjs(size_t count)
    {
        while (m_queue.size() > 1 && (count--) > 0)
        {
            T* obj = m_queue.back();
            m_queue.pop_back();
            delete obj;
        }
        
    }

private:
    std::deque<T*> m_queue;
    std::mutex m_mutex;
    const int STEP = 8;
    PoolStrategy m_strategy;
};//class PoolImpl_Dequeue;

/**
 * @name    ObjectPool
 * @brief   A template definition for Object Pool
 **/
template<typename T>
class ObjectPool
{
public:
    ObjectPool(size_t initCount = 0)
        : m_impl(initCount)
    {

    }
    
    ~ObjectPool()
    {

    }

    T* get()
    {
        return m_impl.get();
    }

    void put(T* obj)
    {
        m_impl.put(obj);
    }

private:
    // disable copy constructor and assignd
    ObjectPool& operator=(const ObjectPool&);
    ObjectPool(const ObjectPool&);

    PoolImpl_Dequeue<T> m_impl;
};//class ObjectPool

}

#endif//_Z_OBJECT_POOL_H_