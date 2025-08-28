#ifndef _LOCKER_H_
#define _LOCKER_H_

#include <mutex>
#include <condition_variable>
#include <chrono>

class sem {
public:
    sem(int count = 0) : m_count(count) {}
    ~sem() = default;

    bool wait() {
        std::unique_lock<std::mutex> lock(mtx); // 先加锁，安全访问m_count
        cv.wait(lock, [this]() { return m_count > 0; }); // 被唤醒后，会先重新获取锁并检查条件是否成立
        --m_count;
        return true;
    }

    bool post() {
        std::lock_guard<std::mutex> lock(mtx);
        ++m_count;
        cv.notify_one(); // 唤醒一个条件变量
        return true;
    }

private:
    std::mutex mtx;
    std::condition_variable cv;
    int m_count;
};


#endif _LOCKER_H_