#ifndef SHM_TRANSACTION_HPP
#define SHM_TRANSACTION_HPP

#include <boost/interprocess/allocators/allocator.hpp> 
#include <cstdint> 
#include "common.hpp"
#include <cstring> 
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

}

#endif 