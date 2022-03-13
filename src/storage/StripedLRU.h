#ifndef AFINA_STRIPEDLRU_H
#define AFINA_STRIPEDLRU_H

#define DEFAULT_STRIPS_NUMB 5

#include <vector>

#include "SimpleLRU.h"
namespace Afina {
namespace Backend {

class StripedLRU : public Afina::Storage {
public:
    StripedLRU(size_t memory_limit = DEFAULT_STRIPS_NUMB * 1024, size_t number_of_strips = DEFAULT_STRIPS_NUMB);
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
    std::vector <std::pair<SimpleLRU, std::mutex>> _stripes;
};

} // namespace Backend
} // namespace Afina

#endif // AFINA_STRIPEDLRU_H
