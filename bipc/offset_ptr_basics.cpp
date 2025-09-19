#include <boost/interprocess/managed_shared_memory.hpp> 
#include <boost/interprocess/offset_ptr.hpp> 

using namespace boost::interprocess; 
struct list_node
{
    offset_ptr<list_node> next; 
    int value; 
};



int main(void)
{

    struct shm_remove
    {
        shm_remove(){ shared_memory_object::remove("MySharedMemory"); }
        ~shm_remove(){ shared_memory_object::remove("MySharedMemory"); }
    } remover; 

    managed_shared_memory segment(create_only, "MySharedMmeory", 65536);

    /* create linked list with 10 nodes in shared memory */
    offset_ptr<list_node> prev = 0, current, first; 
    int i; 
    for(i = 0; i < 10; ++i, prev = current)
    {
        current = static_cast<list_node*>(segment.allocate(sizeof(list_node))); 
        current->value = i; 
        current->next = 0; 
        if(!prev)
        {
            first = current; 
        }
        else 
        {
            prev->next = current; 
        }
    }
    // communicate list to other processes...
    // ... 
    // child/other processes finished now destroy
    for(current = first; current; )
    {
        prev = current; 
        current = current->next; 
        segment.deallocate(prev.get()); 
    }

    return 0; 
}