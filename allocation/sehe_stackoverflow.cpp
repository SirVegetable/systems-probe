#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <exception> 
#include <string> 
#include <cstdint> 
#include <random> 
#include <iostream> 
#include <utility>
#include <algorithm>
#include <iomanip>

namespace bip = boost::interprocess; 
struct BadSharedMemoryAccess final : std::runtime_error
{
    BadSharedMemoryAccess(std::string msg) : std::runtime_error{std::move(msg)}
    {}
};


using Segment = bip::managed_shared_memory; 
template<typename U> using Alloc = bip::allocator<U, Segment::segment_manager>; 

using Message = bip::string; 
using Feed = bip::vector<Message>; 
using SharedMessage = bip::basic_string<char, std::char_traits<char>, Alloc<char>>; 
using SharedFeed = bip::vector<SharedMessage, Alloc<SharedMessage>>; 

class CustomData final 
{
    public: 
        using allocator_type = SharedFeed::allocator_type; 

        CustomData(std::size_t capacity, allocator_type alloc) : 
            _messages(capacity, SharedMessage(alloc), alloc) /* no brace initialization */
        {}

        auto& messages() { return _messages; }
        const auto& messages() const { return _messages; }

    private: 
        SharedFeed _messages; 
        
}; 


template<typename T>
class ShmSegment final 
{
    public: 
        template<typename... Args> 
        ShmSegment(const std::string& shm_segment_name, std::size_t segment_size, 
                std::string const& obj_name, Args&&... args) 
                : 
                _shm{bip::open_or_create, shm_segment_name.c_str(), segment_size}
        {
            _data = _shm.find_or_construct<T>(shm_segment_name.c_str())
                    (std::forward<Args>(args)...,_shm.get_segment_manager()); 

            if(!_data) throw BadSharedMemoryAccess{"cannot access " + shm_segment_name + "/" + obj_name}; 
        }

        T const& get() const {return *_data; }
        T& get() { return *_data; }
        auto free_memory() const {return _shm.get_free_memory(); }

    protected: 
        T* _data; 
    
    private: 
        Segment _shm; 

}; 


Feed generate_heap_feed(std::size_t n)
{
    Feed feed; 
    feed.reserve(n); 
    for(std::size_t i = 0; i < n; i++)
    {
        feed.emplace_back("blablabla11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111" 
            + std::to_string(i));
    }
    return feed; 
}

int main()
{
    static constexpr std::size_t capacity = {100}; 
    static constexpr auto estimate = 30ull << 10; 

    using SharedData = ShmSegment<CustomData>; 
    SharedData shm_data("SHM_SEGMENT", estimate, "SHM_CONTAINER", capacity); 
    std::cout << "Free Memory: " << shm_data.free_memory() << '\n'; 

    const Feed feed = generate_heap_feed(capacity); 
    SharedFeed& shm_feed = shm_data.get().messages(); 
    
    /* copy feed from heap to shm */
    auto const n = std::min(feed.size(), shm_feed.size()); 
    std::copy_n(feed.begin(), n, shm_feed.begin()); 
    std::cout << "Copied: " << n << '\n'; 
    std::cout << "Free Memory: " << shm_data.free_memory() << '\n'; 

    {
        // check some random samples
        std::default_random_engine prng{std::random_device{}()};
        auto pick = [&] { return std::uniform_int_distribution<>(0, n-1)(prng); };

        for (auto index : {pick(), pick(), pick(), pick()}) {
            std::string_view a = feed.at(index);
            std::string_view b = shm_feed.at(index);
            std::cout << "Message #" << index
                << (a == b? " OK":" FAIL")
                << " " << std::quoted(b) << std::endl;
        }
    }
    return 0; 

}