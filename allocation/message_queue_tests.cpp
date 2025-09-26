#include <boost/interprocess/managed_shared_memory.hpp> 
#include <boost/interprocess/allocators/allocator.hpp> 
#include <iostream> 
#include <cstdint> 
#include "message_queue.hpp"

static constexpr std::size_t PS = 4096; 

using namespace boost::interprocess; 
using ShmStringAllocator = allocator<char, managed_shared_memory::segment_manager>; 

int main()
{
    struct shm_remover
    {
        shm_remover(){ shared_memory_object::remove("SHMQUEUE"); }
        ~shm_remover(){ shared_memory_object::remove("SHMQUEUE"); } 
    } remover; 

    managed_shared_memory segment(create_only, "SHMQUEUE", PS * 2); 
    ShmStringAllocator str_alloc(segment.get_segment_manager()); 
    std::cout << "THE SIZE OF THE ALLOCATOR IS: " << sizeof(str_alloc) << "\n"; 
    return 0; 
}