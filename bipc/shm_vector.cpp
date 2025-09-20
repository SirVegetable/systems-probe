#include <boost/interprocess/managed_shared_memory.hpp> 
#include <boost/interprocess/containers/vector.hpp> 
#include <boost/interprocess/allocators/allocator.hpp> 
#include <cstdlib> 
#include <string> 
using namespace boost::interprocess; 

/* 
* Define an STL comptaible allocator ints that allocates from the managed_shared_memory. 
* This allocator will allow placing containers in the segment
*/

using ShmemAllocator = allocator<int, managed_shared_memory::segment_manager>; 

using MyVector = vector<int,ShmemAllocator>; 

int main(int argc, char* argv[])
{
    if(argc == 1)
    {
        struct shm_remover
        {
            shm_remover(){ shared_memory_object::remove("MySharedMemory"); }
            ~shm_remover(){shared_memory_object::remove("MySharedMemory"); }
        } remover;
        
        managed_shared_memory segment(create_only, "MySharedMemory", 65536); 
        const ShmemAllocator alloc(segment.get_segment_manager()); 

        MyVector* vec = segment.construct<MyVector>("MyVector")(alloc); 
        for(int i = 0; i < 100; i++)
        {
            vec->push_back(i); 
        }

        std::string s(argv[0]); 
        s += " child"; 
        if(0 != std::system(s.c_str()))
        {
            return 1; 
        }

        /* check if child process destroyed vector*/
        if(segment.find<MyVector>("MyVector").first)
        {
            return 1; 
        }
    }
    else 
    {
        managed_shared_memory segment(open_only, "MySharedMemory"); 
        
        /* find the vector */
        MyVector* shm_vec = segment.find<MyVector>("MyVector").first;
        /* use vector in reverse order */
        std::sort(shm_vec->rbegin(), shm_vec->rend()); 
        segment.destroy<MyVector>("MyVector"); 
    }

    return 0; 
}