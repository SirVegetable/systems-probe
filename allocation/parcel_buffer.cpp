#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <memory>
#include <iostream>
#include <scoped_allocator>
#include <utility> 
#include <iterator> 



template<typename A, typename IIt> 
void destroy_with_allocator(A& alloc, IIt bsrc, IIt esrc)
{
    for(; bsrc != esrc; ++bsrc)
    {
        std::allocator_traits<A>::destroy(alloc, std::addressof(*bsrc)); 
    }
}

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
    Parcel(int i, const char* s, void_allocator alloc)
        : id(i), data(s, alloc) {}

};

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
        using reverse_iterator = std::reverse_iterator<iterator>; 
        using const_reverse_iterator = std::reverse_iterator<const_iterator>; 
        static_assert(std::is_same_v<T, typename std::allocator_traits<allocator_type>::value_type>); 

        ParcelBuffer(allocator_type alloc, size_type cap) : 
            _alloc(alloc),
             _size(0),  
            _capacity(cap)
        {
            _data = std::allocator_traits<allocator_type>::allocate(_alloc, _capacity); 
        }

        ~ParcelBuffer()
        {
            clear(); 
            std::allocator_traits<allocator_type>::deallocate(_alloc, _data, _capacity); 
        }

        template<typename... Args> 
        void unchecked_emplace_back(Args&&... args)
        {
            auto p = _data + _size; 
            std::allocator_traits<allocator_type>::construct(_alloc, std::addressof(*p), static_cast<Args&&>(args)...); 
            _size++; 
        }

        void clear()
        {
            destroy_with_allocator(_alloc, begin(), end()); 
            _size = 0; 
        }
        
        iterator begin() { return _data; }
        const_iterator begin() const {return _data; }
        iterator end() { return _data + _size; }
        const_iterator end() const { return _data + _size; }

    private: 
        allocator_type _alloc; 
        pointer _data; 
        size_type _size; 
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
   
    using Buf = ParcelBuffer<Parcel, scoped_alloc<Parcel>>;
    Buf* buf = segment.construct<Buf>("Buffer")(alloc, 10);
  
    for (int i = 0; i < 5; ++i)
    {
        buf->unchecked_emplace_back(i, "Hello Shared Memory! This longer string will ensure no SSO happens", shm_allocator<void>(mgr)); 
    }
    std::cout << "Memory Used: " << init_free - mgr->get_free_memory() << '\n'; 
    segment.destroy<Buf>("Buffer");
    const auto after_free = mgr->get_free_memory();

    std::cout << "Memory leak check: "
              << (init_free - after_free) << " bytes difference\n";
    return 0; 
}
