#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/inreprocess/allocators/allocator.hpp> 
#include <boost/interprocess/containers/map.hpp> 
#include <boost/interprocess/containers/string.hpp> 
#include <boost/interprocess/containers/vector.hpp> 

using namespace boost::interprocess; 

/* alloctor and and container typedefs */
using segment_manager_t = managed_shared_memory::segment_manager; 
using void_allocator = allocator<void, segment_manager_t>; 
using int_allocator = allocator<int, segment_manager_t>; 
using int_vector = vector<int, int_allocator>; 
using int_vector_allocator = allocator<int_vector, segment_manager_t>; 
using vector_of_int_vectors = vector<int_vector, int_vector_allocator>; 
using char_allocator = allocator<char, segment_manager_t>; 
using char_string = basic_string<char, std::char_traits<char>, char_allocator>; 


class ComplexData
{

    public: 
        /* 
        * since void allocator is convertible to any other allocator<T>, we can
        * simplify the initialization taking just one allocator for all inner containers
        */

        ComplexData(int id, const char* name, const void_allocator& void_alloc) : 
            id_(id), 
            string_data(name, void_alloc), 
            vecs(void_alloc)
        {}

    private:
        int id_;  
        char_string string_data; 
        vector_of_int_vectors vecs; 

}; 


/* definition of the map holding a string as a key and a ComplexData as mapped type */

using MapValueType = std::pair<const char_string, ComplexData>; 
using MovableToMapValue = std::pair<char_string, ComplexData>;
using MapValueTypeAllocator = allocator<MapValueType, segment_manager_t>; 
using ComplexMapType = map<char_string, ComplexData, std::less<char_string>>; 

int main()
{
    struct Remover
    {
        shm_remover() { shared_memory_object::remove("MySharedMemory"); }
        ~shm_remover() { shared_memory_object::remove("MySharedMemory"); }
    } remover;

    managed_shared_memory segment(create_only, "MySharedMemory", 65536); 
    void_alloc alloc_inst(segment.get_segment_manager()); 

    /* construct the shared memory map and fill it */
    ComplexMapType* my_map = segment.construct<ComplexMapType>
                                                ("MyMap")
                                                (std::less<char_string>(), alloc_inst); 

    for(int i = 0; i < 100; i++)
    {
        /* both key(char_string) and value(ComplexData) need an allocator instance */
        char_string key_object(alloc_inst); 
        ComplexData mapped_obj(i, "default name", alloc_inst); 
        MapValueType value(key_object, mapped_obj); 

        /* modify values and insert them in the map */
        my_map->insert(value); 
    }

    return 0; 
}