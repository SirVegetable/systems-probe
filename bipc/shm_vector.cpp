#include <boost/interprocess/managed_shared_memory.hpp> 
#include <boost/interprocess/containers/vector.hpp> 
#include <boost/interprocess/allocators/allocator.hpp> 

using namespace boost::interprocess; 

/* 
* Define an STL comptaible allocator ints that allocates from the managed_shared_memory. 
* This allocator will allow placing containers in the segment
*/

using ShmemAllocator = allocator<int, managed_shared_memory::segment_manager>; 

using MyVector = vector<int,ShmemAllocator>; 

int main(int argc, char* argv)
{
    if(argc == 1)
    {
        struct shm_remove
        {
            shm_remover(){ shared_memory_object::remove("MySharedMemory"); }
            ~shm_remover(){shared_memory_object::remove("MySharedMemory"); }
        }remover;
        
        managed_shared_memory segment(create_only_t, "MySharedMemory", 65536); 
        
    }
}