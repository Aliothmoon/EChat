#include <queue>
#include <mutex>

template<typename T>
class SyncQueue {
private:
    std::queue<T> m_queue; //利用模板函数构造队列

    std::mutex m_mutex; // 访问互斥信号量

public:
    SyncQueue() = default;

    SyncQueue(SyncQueue &&other) noexcept {}

    ~SyncQueue() = default;

    bool empty() {
        std::lock_guard<std::mutex> lock(m_mutex); // 互斥信号变量加锁，防止m_queue被改变

        return m_queue.empty();
    }

    int size() {
        std::lock_guard<std::mutex> lock(m_mutex); // 互斥信号变量加锁，防止m_queue被改变

        return m_queue.size();
    }

    // Push
    void enqueue(T &t) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.emplace(t);
    }

    // Pop
    bool dequeue(T &t) {
        std::lock_guard<std::mutex> lock(m_mutex); // 队列加锁

        if (m_queue.empty())
            return false;
        t = std::move(m_queue.front()); // 取出队首元素，返回队首元素值，并进行右值引用
        m_queue.pop(); // 弹出入队的第一个元素
        return true;
    }
};