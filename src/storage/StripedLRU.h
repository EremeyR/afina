#ifndef AFINA_STRIPEDLRU_H
#define AFINA_STRIPEDLRU_H

#include "SimpleLRU.h"
namespace Afina {
namespace Backend {

template <int stripe_count>
class StripedLRU : public Afina::Storage {
public:
    StripedLRU(size_t memory_limit = stripe_count * 1024, const int a = 4);
    ~StripedLRU() {}

    // see SimpleLRU.h
    bool Put(const std::string &key, const std::string &value) override;

    // see SimpleLRU.h
    bool PutIfAbsent(const std::string &key, const std::string &value) override;

    // see SimpleLRU.h
    bool Set(const std::string &key, const std::string &value) override;

    // see SimpleLRU.h
    bool Delete(const std::string &key) override;

    // see SimpleLRU.h
    bool Get(const std::string &key, std::string &value) override;

private:
    std::array<std::pair<SimpleLRU, std::mutex>,stripe_count> _stripes;
};

} // namespace Backend
} // namespace Afina

#endif // AFINA_STRIPEDLRU_H
