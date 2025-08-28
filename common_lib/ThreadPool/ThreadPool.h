#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include <list>
#include <cstdio>
#include <exception>
#include <thread>
#include <mutex>
#include "../Sem/Sem.h"
#include "../CGImysql/sql_connection_pool.h"

template <typename T>
class threadpool
{
public:
    // thread_number是线程池中线程的数量，max_requests是请求队列中最多允许的、等待处理的请求的数量*/
    threadpool(int actor_model, connection_pool *connPool, int thread_number = 8, int max_request = 10000);
    ~threadpool();
    bool append(T *request, int state);
    bool append_p(T *request);

private:
    void run();

private:
    int m_thread_number;        // 线程池中的线程数
    int m_max_requests;         // 请求队列中允许的最大请求数
    std::vector<std::thread> m_threads;       // 描述线程池的数组，其大小为m_thread_number
    std::list<T *> m_workqueue; // 请求队列
    std::mutex queuelocker;       // 保护请求队列的互斥锁
    sem m_queuestat;            // 是否有任务需要处理
    connection_pool *m_connPool;  // 数据库
    int m_actor_model;          // 模型切换
};

template <typename T>
threadpool<T>::threadpool(int actor_model, connection_pool *connPool, int thread_number, int max_requests) : 
m_actor_model(actor_model), m_thread_number(thread_number), m_max_requests(max_requests), m_threads(NULL), m_connPool(connPool){
    if (thread_number == 0 || max_requests <= 0){
        throw std::exception();
    }

    m_thread.reverse(m_thread_number);

    for (int i = 0; i < thread_number; ++i){
        m_threads.emplace_back([this]{
            this->run(); // this指向的是当前threadpool类的对象
        })
    }
};

template <typename T>
threadpool<T>::~threadpool(){
    for (auto& thread :: m_threads){
        if (thread.joinable()){
            thread.join();
        }
    }
    std::lock_guard<std::mutex> lock(queuelocker);
    m_workqueue.clear();  
}

template <typename T>
bool threadpool<T>::append(T *request, int state){ // 入队操作
    std::lock_guard<std::mutex> lock<queuelocker>;
    if (m_workqueue.size() >= m_max_requests){
        queuelocker.unlock();
        return false;
    }
    request->m_state = state;
    m_workqueue.push_back(request);
    queuelocker.unlock();
    m_queuestat.post();
}

template <typename T>
bool threadpool<T>::append_p(T *request){
    std::lock_guard<std::mutex> lock<queuelocker>;
    if (m_workqueue.size() >= m_max_requests){
        queuelocker.unlock();
        return false;
    }
    m_workqueue.push_back(request);
    queuelocker.unlock();
    m_queuestat.post();
}

template <typename T>
void threadpool<T>::run(){
    while(true){
        m_queuestat.wait();
        std::lock_guard<std::mutex> lock<queuelocker>;
        if (m_workqueue.empty()){
            queuelocker.unlock();
            continue; // 等待非空
        }
        T *request = m_workqueue.front(); // 取出队列头事件
        m_workqueue.pop_front(); // 去除头事件

        if (!request){
            continue; // 若空指针则返回
        }
        
        // 业务实现
        if (m_actor_model == 1){ // reactor模式
            if (request->read_once()){
                request->improve = 1;
                connectionRAII mysqlcon(&request->mysql, m_connPool);
                request->process();
            }
        }

        else {
            if (request->write()){
                request->improv = 1;
            }
        }

    }
}

#endif _THREADPOOL_H_