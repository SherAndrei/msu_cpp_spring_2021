#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <functional>

#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <atomic>

class ThreadPool {
 public:
    explicit ThreadPool(unsigned size);

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    ~ThreadPool();

 public:
    template <class Func, class... Args>
    auto exec(Func func, Args&&... args) -> std::future<decltype(func(args...))>;

 private:
    void wait();

 private:
    std::vector<std::thread> threads_;
    std::condition_variable cond_;

    std::mutex tasks_mutex_;
    std::queue<std::function<void()>> tasks_;

    bool stop_ = false;
};

#include "ThreadPool.tcc"

#endif  // THREAD_POOL_H
