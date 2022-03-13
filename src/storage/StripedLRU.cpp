#include "StripedLRU.h"
namespace Afina {
namespace Backend {

StripedLRU::StripedLRU(size_t memory_limit, size_t number_of_strips)
    : _stripes(number_of_strips) {
    size_t stripe_size = memory_limit / number_of_strips;
    if (stripe_size < 2 || stripe_size >= 1048576) {
        throw(std::length_error("incorrect memory_limit"));
    }

    for (auto& stripe : _stripes) {
        stripe.first.SetSize(stripe_size);
    }
}

bool StripedLRU::Put(const std::string &key, const std::string &value) {
    auto& stripe_pair = _stripes[std::hash<std::string>{}(key) % _stripes.size()];
    const std::lock_guard<std::mutex> _guard(stripe_pair.second);
    return stripe_pair.first.Put(key, value);
}

bool StripedLRU::PutIfAbsent(const std::string &key, const std::string &value) {
    auto& stripe_pair = _stripes[std::hash<std::string>{}(key) % _stripes.size()];
    const std::lock_guard<std::mutex> _guard(stripe_pair.second);
    return stripe_pair.first.PutIfAbsent(key, value);
}

bool StripedLRU::Set(const std::string &key, const std::string &value) {
    auto& stripe_pair = _stripes[std::hash<std::string>{}(key) % _stripes.size()];
    const std::lock_guard<std::mutex> _guard(stripe_pair.second);
    return stripe_pair.first.Set(key, value);
}

bool StripedLRU::Delete(const std::string &key) {
    auto& stripe_pair = _stripes[std::hash<std::string>{}(key) % _stripes.size()];
    const std::lock_guard<std::mutex> _guard(stripe_pair.second);
    return stripe_pair.first.Delete(key);
}

bool StripedLRU::Get(const std::string &key, std::string &value) {
    auto& stripe_pair = _stripes[std::hash<std::string>{}(key) % _stripes.size()];
    const std::lock_guard<std::mutex> _guard(stripe_pair.second);
    return stripe_pair.first.Get(key, value);
}
} // namespace Backend
} // namespace Afina