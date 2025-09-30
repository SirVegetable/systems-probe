#ifndef ADAPTIVE_ALLOCATOR_QUEUE_HPP
#define ADAPTIVE_ALLOCATOR_QUEUE_HPP

#include <boost/interprocess/allocators/allocator.hpp> 
#include <boost/interprocess/managed_shared_memory.hpp> 
#include <array> 
#include <cstdint> 
#include <string_view> 
#include <cstring> 

namespace bip = boost::interprocess; 


namespace shmv2
{
    template<typename T> using allocator = std::scoped_allocator_adaptor<bip::allocator<T, bip::managed_shared_memory::segment_manager>>; 
    using string = bip::string<char, std::char_traits<char>, shmv2::allocator<char>>; 


    struct Tx
    {
        Tx(const char r[20], const char s[20], std::string_view hex_v, std::uint64_t n, const char* str, const shmv2::allocator<char>& alloc) : 
                nonce(n), 
                input(str, alloc)
        {
            std::memcpy(recipient, r, 20); 
            std::memcpy(sender, s, 20); 
            value = convert_hex_to_le_uint256(hex_v); 
            std::memset(prefix, 0, 8); 
            
            if(std::strlen(str) > 10)
            {
                std::memcpy(prefix, str + 2, 8); 
            }
        }

        char recipient[20];
        char sender[20]; 
        char prefix[8]; 
        std::uint64_t nonce; 
        std::array<std::uint64_t, 4> value; 
        shmv2::string input; 
    }; 

}



constexpr std::array<std::uint64_t, 4> convert_hex_to_le_uint256(std::string_view hex_value)
{
    std::array<std::uint64_t, 4> out{0, 0, 0, 0};

    // require "0x" prefix
    if (hex_value.size() <= 2 || hex_value[0] != '0' || (hex_value[1] != 'x' && hex_value[1] != 'X'))
        return out;

    auto hv = hex_value.substr(2); // remove prefix
    if (hv.empty()) return out;

    int word = 0;
    int shift = 0; // bits filled in current word

    // parse right-to-left (LSB first)
    for (int i = int(hv.size()) - 1; i >= 0 && word < 4; --i) {
        int nibble = from_hex_digit(hv[i]);
        if (nibble < 0) return {0,0,0,0}; // invalid char

        out[word] |= (std::uint64_t(nibble) << shift);
        shift += 4;

        if (shift == 64) {
            shift = 0;
            word++;
        }
    }

    return out;
}



int main()
{
    return 0; 
}


#endif 