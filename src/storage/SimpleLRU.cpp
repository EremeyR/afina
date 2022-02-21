#include "SimpleLRU.h"

namespace Afina {
namespace Backend {

// See MapBasedGlobalLockImpl.h
bool SimpleLRU::Put(const std::string &key, const std::string &value) {
    if (key.size() + value.size() > _max_size) {
        throw;
    }

    auto current_pair = _lru_index.find(key);
    if(current_pair == _lru_index.end()){
        if (_real_size + key.size() + value.size() <= _max_size) {
            MakeNewHead(key, value);
        } else {
            FreeLast(key.size() + value.size());
            MakeNewHead(key, value);
        }
    } else {
        if (value.size() > current_pair->second.get().value.size()) {
            if (_real_size + (value.size() - current_pair->second.get().value.size()) > _max_size) {
                FreeLast(value.size() - (_max_size - (_real_size - current_pair->second.get().value.size())));
            }
        }
        _real_size -= (current_pair->second.get().key.size() + current_pair->second.get().value.size());

        if (current_pair->second.get().prev == nullptr) {  //  if it's a head
            current_pair->second.get().value = value;
        } else if(current_pair->second.get().next == nullptr) {  //  if it's a tail
            current_pair->second.get().prev->next.reset(nullptr);
            _lru_tail = current_pair->second.get().prev;
            MakeNewHead(key, value);
        } else {
            current_pair->second.get().next->prev = current_pair->second.get().prev;
            current_pair->second.get().prev->next = std::move(current_pair->second.get().next);
            MakeNewHead(key, value);
        }
    }
    return true;
}

// See MapBasedGlobalLockImpl.h
bool SimpleLRU::PutIfAbsent(const std::string &key, const std::string &value) {
    if (key.size() + value.size() > _max_size) {
        throw;
    }

    auto current_pair = _lru_index.find(key);
    if(current_pair != _lru_index.end()){
        return false;
    } else {
        if (_real_size + key.size() + value.size() <= _max_size) {
            MakeNewHead(key, value);
        } else {
            FreeLast(key.size() + value.size());
            MakeNewHead(key, value);
        }
    }
    return true;
}

// See MapBasedGlobalLockImpl.h
bool SimpleLRU::Set(const std::string &key, const std::string &value) {
    if (key.size() + value.size() > _max_size) {
        throw;
    }

    auto current_pair = _lru_index.find(key);
    if(current_pair == _lru_index.end()){
        return false;
    } else {
        if (value.size() > current_pair->second.get().value.size()) {
            if (_real_size + (value.size() - current_pair->second.get().value.size()) > _max_size) {
                FreeLast(value.size() - (_max_size - (_real_size - current_pair->second.get().value.size())));
            }
        }
        _real_size -= (current_pair->second.get().key.size() + current_pair->second.get().value.size());

        if (current_pair->second.get().prev == nullptr) {  //  if it's a head
            current_pair->second.get().value = value;
        } else if(current_pair->second.get().next == nullptr) {  //  if it's a tail
            current_pair->second.get().prev->next.reset(nullptr);
            _lru_tail = current_pair->second.get().prev;
            MakeNewHead(key, value);
        } else {
            current_pair->second.get().next->prev = current_pair->second.get().prev;
            current_pair->second.get().prev->next = std::move(current_pair->second.get().next);
            MakeNewHead(key, value);
        }
    }
    return true;
}

// See MapBasedGlobalLockImpl.h
bool SimpleLRU::Delete(const std::string &key) {
    auto current_pair = _lru_index.find(key);
    if(current_pair == _lru_index.end()) {
        return false;
    } else {
        _lru_index.erase(_lru_tail->key);
        _real_size -= (current_pair->second.get().key.size() + current_pair->second.get().value.size());

        if (current_pair->second.get().prev == nullptr) {  //  if it's a head
            _lru_head = std::move(_lru_head->next);
        } else if(current_pair->second.get().next == nullptr) {  //  if it's a tail
            current_pair->second.get().prev->next.reset(nullptr);
            _lru_tail = current_pair->second.get().prev;
        } else {
            current_pair->second.get().next->prev = current_pair->second.get().prev;
            current_pair->second.get().prev->next = std::move(current_pair->second.get().next);
        }
    }
    return true;
}

// See MapBasedGlobalLockImpl.h
bool SimpleLRU::Get(const std::string &key, std::string &value) { return false; }

bool SimpleLRU::FreeLast(size_t size) {
    if (size > _max_size) {
        return false;
    }
    while (_real_size > _max_size - size) {
        _real_size -= (_lru_tail->key.size() + _lru_tail->value.size());
        _lru_index.erase(_lru_tail->key);
        _lru_tail = _lru_tail->prev;
        _lru_tail->next.reset(nullptr);
    }
    return true;
}
void SimpleLRU::MakeNewHead(const std::string &key, const std::string &value) {
    if (_lru_index.empty()) {
        _lru_head = std::make_unique<lru_node>(key, value, nullptr);
        _lru_tail = _lru_head.get();
    } else {
        _lru_head = std::make_unique<lru_node>(lru_node{key, value, std::move(_lru_head)});
        _lru_head->next->prev = _lru_head.get();
    }
    _lru_index.emplace((*_lru_head).key, *_lru_head);

    _real_size +=  key.size() + value.size();
}

} // namespace Backend
} // namespace Afina
