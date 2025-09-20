#include <boost/interprocess/managed_shared_memory.hpp> 
#include <boost/interprocess/containers/map.hpp> 
#include <boost/interprocess/allocators/allocator.hpp> 
#include <utility> 
#include <functional> 

int main()
{
    using namespace boost::interprocess; 

    struct shm_remover
    {
        shm_remover(){ managed_shared_memory::remove("MySharedMemory"); }
        ~shm_remover(){ managed_shared_memory::remove("MySharedMemory"); }
    } remover; 

    managed_shared_memory segment(create_only, "MySharedMemory", 65536); 

    /* 
    * note that map<Key, MappedType>'s value_type is std::pair<const Key, MappedType>, 
    * this allocator will allow to place containers in managed shared memory segments 
    */
    using Keytype = int; 
    using MappedType = float; 
    using ValueType = std::pair<const int, float>; 

    /* 
    * STL Compatible allocator for the map. This allocator will allow to place the 
    * map in the in the shared memory segment. 
    */
    using ShmemAllocator = allocator<ValueType, managed_shared_memory::segment_manager>;
    
    /* 
    * Alias a map of ints that uses the previous STL-like allocator. Note that the 
    * third parameter argument is the ordering function of the map, just like with 
    * std::map, used to compare keys. 
    */
    using MyMap = map<KeyType, MappedType, std::less<KeyType>; 

    /* initialize the shared memory STL-comaptible allocator */
    ShmemAllocator alloc_inst(segment.get_segment_manager()); 

    /* 
    * construct a shared memory map. Note that the first parameter is the comparison 
    * function and the second one allocator. This is the same signature is std::map's
    * constructror taking an allocator. 
    */
    MyMap* mymap = segment.construct<MyMap>("MyMap")
                                            (std::less<int>(), alloc_inst); 
    
    /* isnert data in the map */
    for(int i = 0; i < 100; i++)
    {
        mymap->insert(std::pair<const int, float>(i, float(i))); 
    }
    return 0; 
}