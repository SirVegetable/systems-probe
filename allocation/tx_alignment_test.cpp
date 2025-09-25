#include <cstdint> 
#include <iostream> 
#include "shm_transaction.hpp"
#include "common.hpp"
#include <iostream> 

inline constexpr double page_size = 4096.0;
inline constexpr double MiB_size = 1048576.0;  
static constexpr uint8_t addr1[20] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19}; 
static constexpr uint8_t addr2[20] = {20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39};

static constexpr uint8_t v[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9,10}; 
const char* str = "abcdefghijk"; 

int main(void)
{
    using namespace boost::interprocess; 
    struct shm_remove
    {
        shm_remove(){ shared_memory_object::remove("TESTSHM"); }
        ~shm_remove(){shared_memory_object::remove("TESTSHM"); }
    }remover; 

    managed_shared_memory segment(create_only, "TESTSHM", 65536); 

    auto alloc_inst = shm::allocator<char>(segment.get_segment_manager()); 

    shm::TransactionV1 t1(addr1, addr2, v,1000, str, alloc_inst); 
    std::cout << "The size of TranactionV1 is: " << sizeof(t1) << "\n"; 
    std::cout << "The alignment of TransactionV1 is: " << alignof(shm::TransactionV2) << '\n'; 

    shm::TransactionV2 t2(v, addr1, addr2, 1000, str, alloc_inst); 
    std::cout << "The size of TransactionV2 is: " << sizeof(t2) << '\n'; 
    std::cout << "The alignment of Transaction V2 is: " << alignof(shm::TransactionV2) << '\n'; 
    
    std::cout << "The size of Transaction V3 is: " << sizeof(shm::TransactionV3) << '\n'; 
    std::cout << "The alignment of Transaction V3 is: " << alignof(shm::TransactionV3) << "\n"; 
    
   
    std::cout << "The number of pages in a MiB is: " << MiB_size / page_size << '\n'; 
    std::cout << "A MiB can hold: " << MiB_size / 112 << " shm transactions \n"; 
    std::cout << "\nif a transaction has an input size of 1024 bytes then the 2 MiB can only hold: "
                << 2 * MiB_size / (112 + 1024) << " transactions, excluding the allocators. \n"; 
    return 0; 
}