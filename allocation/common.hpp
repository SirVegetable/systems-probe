#ifndef COMMON_HPP
#define COMMON_HPP

#include <boost/interprocess/allocators/allocator.hpp> 
#include <boost/interprocess/containers/string.hpp> 
#include <boost/interprocess/managed_shared_memory.hpp> 
#include <string> 
#include <string_view>
#include <cstdint> 
namespace bipc = boost::interprocess; 

namespace shm
{
    template<typename T> using allocator = bipc::allocator<T, bipc::managed_shared_memory::segment_manager>; 
    using string = bipc::basic_string<char, std::char_traits<char>, shm::allocator<char>>; 
}


constexpr int from_hex_digit(char c)
{
    if (c >= 'a' && c <= 'f')
        return c - ('a' - 10);
    if (c >= 'A' && c <= 'F')
        return c - ('A' - 10);

    return 0; 
}

constexpr std::array<std::uint64_t, 4> convert_hex_to_le_uint256(std::string_view hex_value)
{
    std::array<std::uint64_t, 4> out{0, 0, 0, }; 
    if(hex_value.size() < 3)
    {
        return out; 
    }
    
    auto hv = hex_value.substr(2); /* remove prefix */
  
    int num_digits = 0;
    
    for(const auto c : hv)
    {
        if(++num_digits > int{sizeof(out) * 2}) { return std::array<std::uint64_t,4>{0,0,0,0};  }
        int x = 0; 
        size_t i = num_digits % 64;
        out[i] = ( x << std::uint64_t{4}) | from_hex_digit(c);
    }
    return out;
}

#endif 