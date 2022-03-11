#include "StripedLRU.h"
namespace Afina {
namespace Backend {


template <int stripe_count>
StripedLRU<stripe_count>::StripedLRU(size_t memory_limit, const int a) {
    size_t stripe_size = memory_limit / stripe_count;
    if (stripe_size < 2 || stripe_size >= 1048576) {
        throw(std::length_error("incorrect memory_limit"));
    }

    for (auto& stripe : _stripes) {
        stripe.first.SetSize(stripe_size);
    }
}

template <int stripe_count>
bool StripedLRU<stripe_count>::Put(const std::string &key, const std::string &value) {
    auto& stripe_pair = _stripes[std::hash<std::string>{}(key) % stripe_count];
    const std::lock_guard<std::mutex> _guard(stripe_pair.second);
    return stripe_pair.first.Put(key, value);
}

template <int stripe_count>
bool StripedLRU<stripe_count>::PutIfAbsent(const std::string &key, const std::string &value) {
    auto& stripe_pair = _stripes[std::hash<std::string>{}(key) % stripe_count];
    const std::lock_guard<std::mutex> _guard(stripe_pair.second);
    return stripe_pair.first.PutIfAbsent(key, value);
}

template <int stripe_count>
bool StripedLRU<stripe_count>::Set(const std::string &key, const std::string &value) {
    auto& stripe_pair = _stripes[std::hash<std::string>{}(key) % stripe_count];
    const std::lock_guard<std::mutex> _guard(stripe_pair.second);
    return stripe_pair.first.Set(key, value);
}

template <int stripe_count>
bool StripedLRU<stripe_count>::Delete(const std::string &key) {
    auto& stripe_pair = _stripes[std::hash<std::string>{}(key) % stripe_count];
    const std::lock_guard<std::mutex> _guard(stripe_pair.second);
    return stripe_pair.first.Delete(key);
}
template <int stripe_count> bool
StripedLRU<stripe_count>::Get(const std::string &key, std::string &value) {
    auto& stripe_pair = _stripes[std::hash<std::string>{}(key) % stripe_count];
    const std::lock_guard<std::mutex> _guard(stripe_pair.second);
    return stripe_pair.first.Get(key);
}
} // namespace Backend
} // namespace Afina