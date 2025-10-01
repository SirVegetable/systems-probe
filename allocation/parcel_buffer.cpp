#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <memory>
#include <iostream>
#include <scoped_allocator>
#include <utility> 

namespace bip = boost::interprocess;

// -----------------------------
// Shared memory typedefs
// -----------------------------
using segment_manager_t = bip::managed_shared_memory::segment_manager;
template <typename T>
using shm_allocator = bip::allocator<T, segment_manager_t>;

template <typename T>
using scoped_alloc = std::scoped_allocator_adaptor<shm_allocator<T>>;

using shm_string = bip::basic_string<char, std::char_traits<char>, scoped_alloc<char>>;

// -----------------------------
// Parcel type
// -----------------------------
struct Parcel
{
    using allocator_type = scoped_alloc<char>;

    int id{};
    shm_string data;

    // Normal constructor
    Parcel(int i, const char* s, const allocator_type& alloc)
        : id(i), data(s, alloc) {}

    // // Allocator-aware constructor (called by scoped_allocator_adaptor)
    // Parcel(std::allocator_arg_t, const allocator_type& alloc,
    //        int i, const char* s)
    //     : id(i), data(s, alloc) {}

    // // Default allocator-aware constructor
    // Parcel(std::allocator_arg_t, const allocator_type& alloc)
    //     : id{}, data(alloc) {}
};

// -----------------------------
// Fixed-size vector in shared memory
// -----------------------------
// template <typename T>
// class ParcelBuffer
// {
// public:
//     using allocator_type = scoped_alloc<T>;
//     using pointer = typename std::allocator_traits<allocator_type>::pointer; 

//     ParcelBuffer(const allocator_type& alloc, std::size_t capacity)
//         : _alloc(alloc), _capacity(capacity)
//     {
//         _data = std::allocator_traits<allocator_type>::allocate(_alloc, _capacity);
//     }

//     ~ParcelBuffer()
//     {
//         clear();
//         std::allocator_traits<allocator_type>::deallocate(_alloc, std::to_address(_data), _capacity);
//     }

//     template<typename... Args>
//     bool emplace_back(Args&&... args)
//     {
//         if (_size == _capacity) return false;
//         T* p = std::to_address(_data + _size);
//         std::allocator_traits<allocator_type>::construct(_alloc, p,
//             std::allocator_arg, _alloc.inner_allocator(), std::forward<Args>(args)...);
//         ++_size;
//         return true;
//     }

//     void clear()
//     {
//         while (_size > 0)
//         {
//             std::allocator_traits<allocator_type>::destroy(_alloc, std::to_address(_data + _size - 1));
//             --_size;
//         }
//     }

//     constexpr auto capacity() 
//     {
//         return _capacity; 
//     }

// private:
//     allocator_type _alloc;
//     std::size_t _capacity{0};
//     std::size_t _size{0};
//     pointer _data{};
// };


int main()
{
    struct shm_remover {
        shm_remover() { bip::shared_memory_object::remove("MyShm"); }
        ~shm_remover() { bip::shared_memory_object::remove("MyShm"); }
    } remover;

    bip::managed_shared_memory segment(bip::create_only, "MyShm", 65536);
    auto* mgr = segment.get_segment_manager();
    
    const auto init_free = mgr->get_free_memory();
    scoped_alloc<Parcel> alloc(mgr); 

    using BufVec =  std::vector<Parcel, scoped_alloc<Parcel>>; 
   
    // using Buf = ParcelBuffer<Parcel>;
    BufVec* buf = segment.construct<BufVec>("Buffer")(alloc);
  
    for (int i = 0; i < 5; ++i)
        buf->push_back(Parcel(i, "Hello Shared Memory!", shm_allocator<void>(mgr))); 
    
    segment.destroy<BufVec>("Buffer");
    const auto after_free = mgr->get_free_memory();

    std::cout << "Memory leak check: "
              << (init_free - after_free) << " bytes difference\n";
    return 0; 
}
