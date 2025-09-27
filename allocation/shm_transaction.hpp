#ifndef SHM_TRANSACTION_HPP
#define SHM_TRANSACTION_HPP

#include <boost/interprocess/allocators/allocator.hpp> 
#include <cstdint> 
#include "common.hpp"
#include <cstring> 
#include <string_view> 
#include <array> 
namespace shm 
{

struct TransactionV1
{
    
    explicit TransactionV1(const uint8_t r[20], const uint8_t v[32], const uint8_t s[20], std::uint64_t n, const char* str, const shm::allocator<char>& alloc) : 
        nonce(n), 
        input(str, alloc)
    {
        std::memcpy(recipient, r, 20); 
        std::memcpy(value, v, 32);  
        std::memcpy(sender, s, 20);  
    }
    
    uint8_t recipient[20]; 
    uint8_t value[32]; 
    uint8_t sender[20]; 
    std::uint64_t nonce; 
    shm::string input; 
};  


struct TransactionV2
{
    
    explicit TransactionV2(const uint8_t v[32], const uint8_t r[20], const uint8_t s[20], std::uint64_t n, const char* str, const shm::allocator<char>& alloc) : 
        nonce(n), 
        input(str, alloc)
    {
        std::memcpy(value, v, 32);  
        std::memcpy(recipient, r, 20); 
        std::memcpy(sender, s, 20);  
    }
    
    uint8_t value[32];
    uint8_t recipient[20]; 
    uint8_t sender[20]; 
    std::uint64_t nonce; 
    shm::string input; 
};  


struct TransactionV3 
{
    uint8_t value[32];
    uint8_t recipient[20]; 
    uint8_t sender[20]; 
    std::uint64_t nonce; 
};

template<typename UInt256Type = std::array<std::uint64_t, 4>> 
struct TransactionV4
{
    TransactionV4(const char r[20], const char s[20], std::string_view hex_v, std::uint64_t n, const char* str, const shm::allocator<char>& alloc) : 
            nonce(n), 
            input(str, alloc)
    {
        std::memcpy(recipient, r, 20); 
        std::memcpy(sender, s, 20); 
        std::memset(value.data(), 0, value.size()); 
        if(hex_v.size() > 3)
        {
            value[0] = static_cast<std::uint64_t>(hex_v[1] + hex_v[2]); 
        }
        std::memset(prefix, 0, 8); 
        if(std::strlen(str) > 10)
        {
            std::memcpy(prefix, str + 2, 9); 
        }
    }   
    char recipient[20];
    char sender[20]; 
    char prefix[8]; 
    std::uint64_t nonce; 
    UInt256Type value; 
    shm::string input; 
}; 


}

#endif 