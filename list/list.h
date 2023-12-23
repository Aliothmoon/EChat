#include <vector>
#include <mutex>
#include <functional>
#include <unordered_set>

#ifndef ECHAT_LIST_H
#define ECHAT_LIST_H

template<typename T>
class SyncList {

public:
    std::vector<T> data;
    mutable std::mutex mtx;

    void push_back(int val);

    size_t size() const;

    auto erase(decltype(data.begin()) it) -> decltype(data.begin());

    void lock();

    void unlock();

};

template<typename T>
void SyncList<T>::unlock() {
    mtx.unlock();
}

template<typename T>
void SyncList<T>::lock() {
    mtx.lock();
}


template<typename T>
void SyncList<T>::push_back(int val) {
    std::lock_guard<std::mutex> lock{mtx};
    data.push_back(val);
}

template<typename T>
size_t SyncList<T>::size() const {
    std::lock_guard<std::mutex> lock{mtx};
    return data.size();
}

//template<typename T>
//void SyncList<T>::traverse(const std::function<void(typename std::vector<T>::iterator &)> &callback) const {
//    std::lock_guard<std::mutex> lock(mtx);
//    for (auto it = data.begin(); it != data.end(); ++it) {
//        callback(it);
//    }
//}

template<typename T>
auto SyncList<T>::erase(decltype(data.begin()) it) -> decltype(data.begin()) {
    std::lock_guard<std::mutex> lock(mtx);
    return data.erase(it);
}

#endif //ECHAT_LIST_H