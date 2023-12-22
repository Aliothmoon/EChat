

#ifndef ECHAT_SYNCMAP_H
#define ECHAT_SYNCMAP_H

#include <vector>
#include <map>
#include <mutex>
#include <vector>
#include <initializer_list>

template<typename K, typename V>
class SyncMap {
    SyncMap();

    SyncMap(std::initializer_list<std::pair<K, V>> init_list);

    V &operator[](const K &key);

    std::vector<K> keys() const;

    size_t size() const;

    bool contains(const K &key) const;

    bool insert(const K &key, const V &value);

    bool erase(const K &key);

};


#endif //ECHAT_SYNCMAP_H
