#include <boost/interprocess/managed_shared_memory.hpp> 
#include <cstdlib> /* std::system */
#include <cstddef> 
#include <utility> 
#include <cassert> 


int main(int argc, char* argv[])
{
    using namespace boost::interprocess; 
    using MyType = std::pair<double, int>; 
    
    /* parent */
    if(argc == 1)
    {
        /* removes shared memory on construction and destructor */
        struct shm_remove
        {
            shm_remove(){ shared_memory_object::remove("MySharedMemory"); }
            ~shm_remove(){ shared_memory_object::remove("MySharedMemory"); }
        } remover; 

        /* construct managed shared memory */
        managed_shared_memory segment(create_only, "MySharedMemory", 65536); 

        /* create an object of MyType initialized to {0.0, 0} */
        MyType* instance = segment.construct<MyType>("MyTypeInstance")/* name of the object */
                                                    (0.0, 0); /* ctor first argument */
        
        /* create an array of 10 elements of MyType initialized to {0.0, 0} */
        MyType* arr = segment.construct<MyType>
                                            ("MyTypeArray")/* name of the object */
                                            [10] /* number of elements */
                                            (0.0, 0); /* same two ctor arguments for all arguments*/
        
        /* create an two arrays of 3 elements of MyType initializing each one */
        float float_inits[3] = {0.0, 1.1, 2.2}; 
        int int_inits[3] = {0, 1, 2}; 
        
        /* MyType array initialization in shm */
        MyType* array_it = segment.construct_it<MyType>
                                            ("MyTypeArrayFromIt") /* name */
                                            [3] /* size */
                                            (&float_inits[0], &int_inits[0]); /* iterators for the first and second ctor args*/
        
        if(array_it == nullptr || instance == nullptr || arr == nullptr)
        {
            return 1; 
        }
        /* 
        * launch child process which executes the main again this time with args
        * which means the child process executes the else statement. 
        */
        std::string s{argv[0]}; 
        s += " child"; 
        if(0 != std::system(s.c_str()))
        {
            return 1; 
        }

        /* check if child has destroyed all objects, err if it hasn't */
        if(segment.find<MyType>("MyTypeArray").first ||
            segment.find<MyType>("MyTypeInstance").first ||
            segment.find<MyType>("MyTypeArrayFromIt").first)
        {
            return 1; 
        }
    }
    else
    {

        /* open the managed shared memory */
        managed_shared_memory segment(open_only, "MySharedMemory"); 
        
        std::pair<MyType*,managed_shared_memory::size_type> res; 

        /* find the array */
        res = segment.find<MyType>("MyTypeArray"); 
        /* length should be 10*/
        if(res.second != 10)
        {
            return 1; 
        }
        /* find the object instance */
        res = segment.find<MyType>("MyTypeInstance"); 
        if(res.second != 1)
        {
            return 1; 
        }

        /* find the iterator instantiated array */
        res = segment.find<MyType>("MyTypeArrayFromIt"); 
        if(res.second != 3)
        {
            return 1; 
        }

        /* were done now, delete all the objects */
        segment.destroy<MyType>("MyTypeArray"); 
        segment.destroy<MyType>("MyTypeInstance"); 
        segment.destroy<MyType>("MyTypeArrayFromIter"); 
    }
    return 0; 
}