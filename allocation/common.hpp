#ifndef COMMON_HPP
#define COMMON_HPP

#include <boost/interprocess/allocators/allocator.hpp> 
#include <boost/interprocess/containers/string.hpp> 
#include <boost/interprocess/managed_shared_memory.hpp> 
#include <string> 

namespace bipc = boost::interprocess; 

namespace shm
{
    template<typename T> using allocator = bipc::allocator<T, bipc::managed_shared_memory::segment_manager>; 
    using string = bipc::basic_string<char, std::char_traits<char>, shm::allocator<char>>; 
}

#endif 