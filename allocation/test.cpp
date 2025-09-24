#include <cstdint> 
#include <iostream> 
#include "shm_transaction.hpp"

int main(int argc, char* argv[])
{
    using namespace boost::interprocess; 
    if(argc == 1)
    {
        struct shm_remove
        {
            shm_remove(){ shared_memory_object::remove("TESTSHM"); }
            shm_remove(){shared_memory_object::remove("TESTSHM"); }
        }remover; 

        managed_shared_memory segment(create_only, "TESTSHM", 65536); 

        
    }
    #endif
}