#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp> 
#include <cstdint> 
#include <functional> 
#include <boost/unordered_map.hpp>
#include <boost/container_hash/hash.hpp> 

int main()
{
    using namespace boost::interprocess; 
    struct shm_remover
    {
        shm_remover() { shared_memory_object::remove("MyName"); }
        ~shm_remover() { shared_memory_object::remove("MyName"); }
    } remover; 

    managed_shared_memory segment(create_only, "MyName", 65536); 

    using KeyType = int; 
    using MappedType = float; 
    using ValueType = std::pair<const int, float>; 
    using ShmemAllocator = allocator<ValueType, managed_shared_memory::segment_manager>; 
    using MyHashMap = boost::unordered_map<KeyType, MappedType, boost::hash<KeyType>, std::equal_to<KeyType>, ShmemAllocator>; 

    /* 
    * construct a shared memory hash map. Note that the first parameter is the initial bucket count and after that,
    * the hash function, the equality function and the allocator. 
    */
    MyHashMap* mymap = segment.construct<MyHashMap>("MyHashMap")(3u, boost::hash<int>(), std::equal_to<int>(), segment.get_allocator<ValueType>());

    /* insert data in the hash map */
    for(std::size_t i = 0; i < 100u; ++i)
    {
        mymap->insert(ValueType((int)i, (float)i)); 
    }
    return 0; 
}

