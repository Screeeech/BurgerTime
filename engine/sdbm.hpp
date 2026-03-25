#ifndef ENGINE_SDBM_HPP
#define ENGINE_SDBM_HPP

#include <cstddef>

namespace sdbm
{

consteval unsigned int sdbm_hash(const char* str, size_t len)
{
    unsigned int hash = 0;
    for(size_t i = 0; i < len; ++i)
    {
        hash = static_cast<unsigned int>(str[i]) + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}

consteval unsigned int operator""_h(const char* str, size_t len)
{
    return sdbm_hash(str, len);
}

}

#endif  // ENGINE_SDBM_HPP
