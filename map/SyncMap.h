

#ifndef ECHAT_SYNCMAP_H
#define ECHAT_SYNCMAP_H

#include <map>
#include <mutex>
#include <vector>
#include <initializer_list>

template<typename K, typename V>
class SyncMap {
public:
    SyncMap() = default;

    SyncMap(std::initializer_list<std::pair<K, V>> init_list) {
        for (auto &pair: init_list) {
            map_[pair.first] = pair.second;
        }
    }

    V &operator[](const K &key) {
        std::lock_guard<std::mutex> lock(mutex_);
        return map_[key];
    }

    bool insert(const K &key, const V &value) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto result = map_.insert(std::make_pair(key, value));
        return result.second;
    }

    bool erase(const K &key) {
        std::lock_guard<std::mutex> lock(mutex_);
        return map_.erase(key);
    }

    bool contains(const K &key) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return map_.find(key) != map_.end();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return map_.size();
    }

    std::vector<K> keys() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<K> result;
        for (auto &pair: map_) {
            result.push_back(pair.first);
        }
        return result;
    }

private:
    std::map<K, V> map_;
    mutable std::mutex mutex_;
};


#endif //ECHAT_SYNCMAP_H
