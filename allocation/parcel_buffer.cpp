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

using void_allocator = shm_allocator<void>; 
// -----------------------------
// Parcel type
// -----------------------------
struct Parcel
{

    int id{};
    shm_string data;

    // Normal constructor
    Parcel(int i, const char* s, const void_allocator& alloc)
        : id(i), data(s, alloc) {}

    // // Allocator-aware constructor (called by scoped_allocator_adaptor)
    // Parcel(std::allocator_arg_t, const allocator_type& alloc,
    //        int i, const char* s)
    //     : id(i), data(s, alloc) {}

    // // Default allocator-aware constructor
    // Parcel(std::allocator_arg_t, const allocator_type& alloc)
    //     : id{}, data(alloc) {}
};

/*
-----------------------------
Fixed-size vector in shared memory
-----------------------------
*/

template<typename T, typename Allocator>
class ParcelBuffer
{
    public: 
        using value_type = T; 
        using allocator_type = Allocator; 
        using reference = value_type&; 
        using const_reference = const value_type&; 
        using size_type = typename std::allocator_traits<allocator_type>::size_type; 
        using difference_type = typename std::allocator_traits<allocator_type>::difference_type; 
        using pointer = typename std::allocator_traits<allocator_type>::pointer; 
        using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer; 
        using iterator = pointer; 
        using const_iterator = const_pointer; 
        using reverse_iterator = std::reverse_iterator(iterator); 
        using const_reverse_iterator = std::reverse_iterator(const_iterator); 
        static_assert(std::is_same_v<T, typename std::allocator_traits<allocator_type>::value_type>); 

        ParcelBuffer(size_type cap, allocator_type alloc) : 
            _capacity(cap), 
            _alloc(alloc)
        {
            _data = std::allocator_traits<allocator_type>::allocate(_alloc, cap); 
        }
    private: 
        allocator_type _alloc; 
        pointer _data; 
        size_type _end; 
        size_type _capacity; 

}; 

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
