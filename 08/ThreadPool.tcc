#ifndef THREAD_POOL_TCC
#define THREAD_POOL_TCC

#include <utility>
#include <memory>
#include <algorithm>

#include "ThreadPool.h"

ThreadPool::ThreadPool(unsigned size) {
    size = std::min(size, std::thread::hardware_concurrency());
    for(size_t i = 0; i < size; i++) {
        threads_.emplace_back([this] {
            wait();
        });
    }
}

template <class Func, class... Args>
auto ThreadPool::exec(Func func, Args&&... args) -> std::future<decltype(func(args...))> {
    if (threads_.size() == 0)
        throw std::runtime_error("Invalid threadpool size");
    auto task = std::make_shared<std::packaged_task<decltype(func(args...))()>>(
        std::bind(std::forward<Func>(func), std::forward<Args>(args)...)
    );

    std::future f = task->get_future();
    {
        std::lock_guard lk(tasks_mutex_);
        tasks_.emplace([task] {
            (*task)();
        });
    }
    cond_.notify_one();
    return f;
}

void ThreadPool::wait() {
    std::function<void()> task;
    while (true) {
        {
            std::unique_lock lk(tasks_mutex_);
            cond_.wait(lk, [this] { 
                return stop_ || !tasks_.empty();
            });
            if (stop_ && tasks_.empty())
                return;
            task = std::move(tasks_.front());
            tasks_.pop();
        }
        task();
    }
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard lk(tasks_mutex_);
        stop_ = true;
    }
    cond_.notify_all();
    for (auto&& thread : threads_)
        thread.join();
}

#endif  // THREAD_POOL_TCC
