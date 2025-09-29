#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp> 
#include "common.hpp"
#include "shm_transaction.hpp"
#include <iostream> 

#define SHM_NAME "TXSHM"

static constexpr const char* input1 = "0x604080408786abcde9900088aacc"; 
static constexpr const char* input2 = "";
static constexpr const char* input3 = "0x80808080775566"; 

std::array<std::string, 6> values = {
    "0x", "0xFFFFFFFFFFFFFFFF", "0x10000000000000000", "0x1234567890ABCDEF1234567890ABCDEF", 
    "0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", "0x2E05" }; 

static constexpr char addr1[20] = {'0','0','0','0','1','a','c','c','c','0','0','7','1','e','d','c','b','b','a','a'}; 
static constexpr char addr2[20] = {'a','a','b','b','d','d','1','1','0','0','0','0','f','f','a','b','d','d','a','0'}; 
static constexpr std::uint64_t tnonce = 100; 

using namespace boost::interprocess; 

int main(int argc, char* argv[])
{
    if(argc == 1)
    {

        struct shm_remove
        {
            shm_remove() { shared_memory_object::remove(SHM_NAME); }
            ~shm_remove() { shared_memory_object::remove(SHM_NAME); }
        } remover; 

        managed_shared_memory segment(create_only, SHM_NAME, 1049000); 

        auto* shm_manager = segment.get_segment_manager(); 
        
        std::cout << "the size returned by the shm manager after initialization: " << shm_manager->get_size() << '\n'; 
        std::cout << "the remaining free bytes is: " << shm_manager->get_free_memory() << '\n'; 

        shm::allocator<void> void_alloc(segment.get_segment_manager()); 
        
        shm::vector<shm::TransactionV4>* writer_vec = segment.construct<shm::vector<shm::TransactionV4>>("SzVec")(void_alloc); 
        std::cout << "the size after vec is put into the shared memory is: " << shm_manager->get_size() << '\n'; 
        std::cout << "the remaining free bytes after vector is placed into shared memory is: " << shm_manager->get_free_memory() << '\n'; 
        std::cout << "the vector capacity is: " << writer_vec->capacity() << "\n"; 
        std::cout << "the size of the vector class is: " << sizeof(shm::vector<shm::TransactionV4>) << "\n"; 

        auto init_cap = shm_manager->get_free_memory(); 
        writer_vec->push_back(shm::TransactionV4(addr1, addr2, "0x1", tnonce, input2, void_alloc)); 
        std::cout << "after the first transaction the capacity is: " << writer_vec->capacity() << '\n'; 
        auto after_empty_input = shm_manager->get_free_memory(); 
        std::cout << "after pushing an empty input value the amount of memory taken up was: " << init_cap - after_empty_input <<'\n'; 
        std::cout << "the amount of memory taken that was not part of the transaction class was: " << init_cap - after_empty_input - sizeof(shm::TransactionV4) << '\n'; 
        writer_vec->pop_back(); 
        const auto after_popback = shm_manager->get_free_memory(); 
        std::cout << "after popping the empty tx, was memory returned? " << ((init_cap - after_popback) == 0) << '\n'; 
        auto input_size = constructor_input.size(); 
        writer_vec->push_back(shm::TransactionV4(addr1, addr2, "0x1", tnonce, constructor_input.c_str(), void_alloc)); 
        const auto after_constructor_input = shm_manager->get_free_memory(); 
        const auto memory_after = init_cap - after_constructor_input - sizeof(shm::TransactionV4); 
        std::cout << "after pushing the constructor input transaction the amount of memory taken was: " << memory_after << "\n"; 
        std::cout << "besides the constructor data the additional overhead was: " << memory_after - input_size << "\n"; 
        for(const auto& v : values)
        {
            writer_vec->push_back(shm::TransactionV4(addr1, addr2, v, tnonce, input1, void_alloc)); 
            writer_vec->push_back(shm::TransactionV4(addr1, addr2, v, tnonce, input2, void_alloc)); 
            writer_vec->push_back(shm::TransactionV4(addr1, addr2, v, tnonce, input3, void_alloc)); 
        }

        std::string child_args(argv[0]); 
        child_args += " reader is starting now... "; 
        std::system(child_args.c_str()); 

        if(segment.find<shm::vector<shm::TransactionV4>>("SzVec").first)
        {
            std::cerr << "error the child failed to destroy\n"; 
            return 1; 
        }
    }
    else 
    {
        
        managed_shared_memory segment(open_only, SHM_NAME); 
        auto* shm_manager = segment.get_segment_manager(); 
        auto* reader_vec = segment.find<shm::vector<shm::TransactionV4>>("SzVec").first; 
        for(auto iter = reader_vec->begin(); iter != reader_vec->end(); iter++)
        {
            std::cout << "the nonce is: " << iter->nonce << "\n"; 
        }

        std::cout << "The vectors capacity is: " << reader_vec->capacity() << "\n"; 
        std::cout <<"\n";

        std::cout << "REMAINING FREE MEMORY IS: " << shm_manager->get_free_memory() << "\n"; 
        
        std::cout << "THE SIZE OF THE CONSTRUCTOR INPUT IS: " << constructor_input.size() << '\n'; 
        segment.destroy<shm::vector<shm::TransactionV4>>("SzVec"); 

    }
    return 0; 
}