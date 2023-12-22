//
// Created by Heimweh on 2023/12/22.
//

#ifndef ECHAT_POOL_H
#define ECHAT_POOL_H

#include <future>
#include <functional>
#include "../queue/SyncQueue.h"

class ThreadPool {
private:
    class ThreadWorker {

    private:
        int m_id; // 工作id

        ThreadPool *m_pool; // 所属线程池


    public:
        ThreadWorker(ThreadPool *pool, int id);

        void operator()();
    };

    bool m_shutdown; // 线程池是否关闭

    SyncQueue<std::function<void()>> m_queue; // 执行函数安全队列，即任务队列

    std::vector<std::thread> m_threads; // 工作线程队列

    std::mutex m_conditional_mutex; // 线程休眠锁互斥变量

    std::condition_variable m_conditional_lock; // 线程环境锁，可以让线程处于休眠或者唤醒状态
public:
    explicit ThreadPool(int n_threads = 4);

    ThreadPool(const ThreadPool &) = delete;

    ThreadPool(ThreadPool &&) = delete;

    ThreadPool &operator=(const ThreadPool &) = delete;

    ThreadPool &operator=(ThreadPool &&) = delete;

    void init();

    void shutdown();

    template<typename F, typename... Args>
    auto submit(F &&f, Args &&...args) -> std::future<decltype(f(args...))>;
};

template<typename F, typename... Args>
auto ThreadPool::submit(F &&f, Args &&... args) -> std::future<decltype(f(args...))> {
    // Create a function with bounded parameter ready to execute
    std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f),
                                                           std::forward<Args>(args)...);
    // 连接函数和参数定义，特殊函数类型，避免左右值错误

    // Encapsulate it into a shared pointer in order to be able to copy construct
    auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

    // Warp packaged task into void function
    std::function<void()> warpper_func = [task_ptr]() {
        (*task_ptr)();
    };

    // 队列通用安全封包函数，并压入安全队列
    m_queue.enqueue(warpper_func);

    // 唤醒一个等待中的线程
    m_conditional_lock.notify_one();

    // 返回先前注册的任务指针
    return task_ptr->get_future();
}

#endif //ECHAT_POOL_H
