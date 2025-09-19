#include <boost/interprocess/managed_shared_memory.hpp> 
#include <cstdlib> /* std::system */
#include <sstream> 


/* 
* this program allocates a shared memory segment, and copies the message to that buffer, 
* then sends the offset of that shared_memory to another process. 
*/

int main(int argc, char* argv[])
{
    using namespace boost::interprocess; 
    
    /* in parent right now */
    if(argc == 1)
    {
        /* remove shared memory on construction and destruction*/
        struct shm_remove
        {
            shm_remove(){ shared_memory_object::remove("MySharedMemory"); }
            ~shm_remove(){ shared_memory_object::remove("MySharedMemory"); }
        } remover; 

        /* create a managed shared memory segment */
        managed_shared_memory segment(create_only, "MySharedMemory", 65536); 

        /* allocate a portion of the segments memory */
        managed_shared_memory::size_type free_memory = segment.get_free_memory(); 
        void *shptr = segment.allocate(1024 /* bytes to allocate */);
        
        // check invariant 
        if(free_memory <= segment.get_free_memory())
        {
            return 1; 
        }

        /* 
        * A handle from the base address can identify any byte of the shared
        * even if it is mapped in a different address space. 
        */
        managed_shared_memory::handle_t handle = segment.get_handle_from_address(shptr); 
        std::stringstream s; 
        s << argv[0] << " " << handle; 
        s << std::ends; 

        /* launch the child process */
        if(0 != std::system(s.str().c_str()))
        {
            return 1; 
        }

        /* check the memory has been freed */
        if(free_memory != segment.get_free_memory())
        {
            return 1; 
        }
    }
    else 
    {
        /* open managed segment */
        managed_shared_memory segment(open_only, "MySharedMemory"); 

        managed_shared_memory::handle_t handle = 0; 

        /* obtain handle value */
        std::stringstream s; 
        s << argv[1]; s >> handle; 

        /* get buffer local handle from handle */
        void* msg = segment.get_address_from_handle(handle); 
        // deallocate previously allocate memory
        segment.deallocate(msg); 
    }
    return 0; 
}