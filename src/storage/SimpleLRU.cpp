#include "SimpleLRU.h"

namespace Afina {
namespace Backend {

// See MapBasedGlobalLockImpl.h
bool SimpleLRU::Put(const std::string &key, const std::string &value) {
    if (_lru_index.empty()) {
        if (key.size() + value.size() > _max_size) {
            return false;
        }


        _lru_head = std::make_unique<lru_node>(lru_node({key, value, nullptr, nullptr}));
        _lru_tail = _lru_head.get();
        _lru_index.emplace(key, *_lru_head);

        _real_size =  key.size() + value.size();
        return true;
    } else {
        auto current_pair = _lru_index.find(key);
        if(current_pair == _lru_index.end()){
            if (_real_size + key.size() + value.size() <= _max_size) {
                _lru_head = std::make_unique<lru_node>(lru_node{key, value, nullptr, std::move(_lru_head)});
            }
        }

    }


    return false; }

// See MapBasedGlobalLockImpl.h
bool SimpleLRU::PutIfAbsent(const std::string &key, const std::string &value) { return false; }

// See MapBasedGlobalLockImpl.h
bool SimpleLRU::Set(const std::string &key, const std::string &value) { return false; }

// See MapBasedGlobalLockImpl.h
bool SimpleLRU::Delete(const std::string &key) { return false; }

// See MapBasedGlobalLockImpl.h
bool SimpleLRU::Get(const std::string &key, std::string &value) { return false; }

} // namespace Backend
} // namespace Afina
