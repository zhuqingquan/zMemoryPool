/*
 * @Author: zhuqingquan
 * @Date: 2021-08-08 19:36:47
 * @LastEditTime: 2021-08-09 00:30:47
 * @Description:  Template for different type Object poll.
 */
#ifndef _Z_OBJECT_POOL_H_
#define _Z_OBJECT_POOL_H_

#include <deque>
#include <mutex>

namespace zTools
{

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
            mallocObjs(STEP);
        if(m_queue.empty())
        {
            return NULL;
        }
        T* result = m_queue.front();
        m_queue.pop_front();
        return result;
    }

    void put(T* obj)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push_front(obj);
    }

private:
    void mallocObjs(size_t count)
    {
        for (size_t i = 0; i < count; i++)
        {
            m_queue.push_back(new T());
        }
    }

private:
    std::deque<T*> m_queue;
    std::mutex m_mutex;
    const int STEP = 8;
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