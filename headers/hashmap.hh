#ifndef HASHMAP_HH
#define HASHMAP_HH

#include "common.hh"

template<typename Key, typename Value>
class ConcurrentHashMap {
private:
    std::unordered_map<Key, Value> map_;
    std::unordered_map<Key, std::shared_mutex> mutexes_;
    std::shared_mutex map_mutex_;  // protects access to mutexes_

public:
    void set(const Key& key, const Value& value) {
        std::unique_lock<std::shared_mutex> map_lock(map_mutex_);
        auto& mutex = mutexes_[key];
        map_lock.unlock();

        std::unique_lock<std::shared_mutex> lock(mutex);
        map_[key] = value;
    }

    Value get(const Key& key) {
        std::shared_lock<std::shared_mutex> map_lock(map_mutex_);
        auto it = mutexes_.find(key);
        if (it == mutexes_.end()) {
            throw std::runtime_error("Key not found");
        }
        std::shared_mutex& mutex = it->second;
        map_lock.unlock();

        std::shared_lock<std::shared_mutex> lock(mutex);
        return map_.at(key);
    }

    void remove(const Key& key) {
        std::unique_lock<std::shared_mutex> map_lock(map_mutex_);
        auto it = mutexes_.find(key);
        if (it == mutexes_.end()) {
            return;
        }
        std::shared_mutex& mutex = it->second;
        map_lock.unlock();

        std::unique_lock<std::shared_mutex> lock(mutex);
        map_.erase(key);
        lock.unlock();

        std::unique_lock<std::shared_mutex> map_remove_lock(map_mutex_);
        mutexes_.erase(key);
    }
};

#endif