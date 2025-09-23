#include <boost/interprocess/allocators/allocator.hpp> 
#include <string> 
#include <boost/containers/string> 
#include <boost/interprocess/managed_shared_memory.hpp> 
#include <memory> 

namespace bipc = boost::interprocess; 

namespace shm
{
    template<typename T> using allocator = bipc::allocator<T, bipc::segment_manager>; 
    using string = boost::container::basic_string<char, std::char_traits<char>, shm::allocator<char>>; 
}

template<typename T, typename Alloc> 
class MessageQueue
{
    public: 

        static_assert(std::is_same_v<T, std::allocator_traits<Alloc>::value_type>); 
        using value_type = T; 
        using allocator_type = Alloc; 
        using pointer = typename std::allocator_traits<Alloc>::pointer; 
        using const_pointer = typename std::allocator_traits<Alloc>::pointer
        using reference = value_type&; 
        using const_reference = const value_type&; 
        using size_type = typename std::allocator_traits<Alloc>::size_type; 
        using difference_type = typename std::allocator_traits<Alloc>::difference_type; 
        using iterator = pointer; 
        using const_iterator = const_pointer; 
        /* maybe add reverse iterators */

        

    private: 
        allocator_type alloc_inst; 
        pointer begin; 
        size_type capacity; 
        size_type size; 
        
}; 

