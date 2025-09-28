#ifndef COMMON_HPP
#define COMMON_HPP

#include <boost/interprocess/allocators/allocator.hpp> 
#include <boost/interprocess/containers/string.hpp> 
#include <boost/interprocess/managed_shared_memory.hpp> 
#include <boost/interprocess/containers/vector.hpp> 
#include <string> 
#include <string_view>
#include <cstdint> 
namespace bipc = boost::interprocess; 

namespace shm
{
    template<typename T> using allocator = bipc::allocator<T, bipc::managed_shared_memory::segment_manager>; 
    using string = bipc::basic_string<char, std::char_traits<char>, shm::allocator<char>>; 

    template<typename T> using vector = bipc::vector<T, allocator<T>>; 
}


constexpr int from_hex_digit(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
    if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
    return -1; // invalid
}

constexpr std::array<std::uint64_t, 4>
convert_hex_to_le_uint256(std::string_view hex_value)
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
#endif 