#ifndef ENGINE_SDBM_HPP
#define ENGINE_SDBM_HPP

namespace Utils
{

consteval unsigned int sdbm_hash(const char* str, std::size_t len)
{
    unsigned int hash = 0;
    for(std::size_t i = 0; i < len; ++i)
    {
        hash = static_cast<unsigned int>(str[i]) + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}


}

consteval unsigned int operator""_h(const char* str, std::size_t len)
{
    return Utils::sdbm_hash(str, len);
}

#endif  // ENGINE_SDBM_HPP
