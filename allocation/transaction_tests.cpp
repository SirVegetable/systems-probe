#include "common.hpp"
#include "shm_transaction.hpp"
#include <iostream> 
#include <vector> 
#include <cstdlib> 

#define SHM_NAME "TXSHM"

static constexpr const char* input1 = "0x604080408786abcde9900088aacc"; 
static constexpr const char* input2 = "0x";
static constexpr const char* input3 = "0x80808080775566"; 

std::array<std::string, 6> values = {
    "0x", "0xFFFFFFFFFFFFFFFF", "0x10000000000000000", "0x1234567890ABCDEF1234567890ABCDEF", 
    "0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", "0x2E05" }; 

static constexpr char addr1[20] = {'0','0','0','0','1','a','c','c','c','0','0','7','1','e','d','c','b','b','a','a'}; 
static constexpr char addr2[20] = {'a','a','b','b','d','d','1','1','0','0','0','0','f','f','a','b','d','d','a','0'}; 

static constexpr std::uint64_t tnonce = 100; 

using TestVector = shm::vector<shm::TransactionV4>; 

int main(int argc, char* argv[])
{
    
    using namespace boost::interprocess; 
    if(argc == 1)
    {
        struct shm_remove
        {
            shm_remove() { shared_memory_object::remove(SHM_NAME); }
            ~shm_remove() { shared_memory_object::remove(SHM_NAME); }
        } remover; 

        managed_shared_memory segment(create_only, SHM_NAME, 1049000); 
        auto* shm_manager = segment.get_segment_manager(); 
        const auto init_memory = shm_manager->get_free_memory(); 

        shm::allocator<void> void_alloc(segment.get_segment_manager()); 

        TestVector* test_vec = segment.construct<TestVector>("TxVector")(void_alloc); 

        std::cout << "the size of Transaction V4 is: " << sizeof(shm::TransactionV4) << "\n"; 
        std::string child_args(argv[0]); 
        for(const auto& v : values)
        {
            test_vec->push_back(shm::TransactionV4(addr1, addr2, v, tnonce, input1, void_alloc)); 
            test_vec->push_back(shm::TransactionV4(addr1, addr2, v, tnonce, input2, void_alloc)); 
            test_vec->push_back(shm::TransactionV4(addr1, addr2, v, tnonce, input3, void_alloc)); 
        }

        child_args += " child checking vec"; 

        std::system(child_args.c_str()); 

        if(segment.find<TestVector>("TxVector").first)
        {
            std::cerr << "error the child failed to destroy\n"; 
            return 1; 
        }
        const auto memory_after = shm_manager->get_free_memory(); 
        std::cout << "THE DIFFERENCE BETWEEN MEM INIT AND AFTER IS: " << init_memory - memory_after << "\n"; 
    }
    else 
    {
        managed_shared_memory segment(open_only, SHM_NAME); 
        TestVector* test_vec = segment.find<TestVector>("TxVector").first; 

        if(test_vec->size() != 18)
        {
            segment.destroy<TestVector>("TxVector"); 
            exit(1); 
        }

        for(std::size_t i = 0; i < test_vec->size(); i++)
        {
            std::cout << "*************** TRANSACTION INDEX: " << i << " ***************\n"; 
            auto& tx = (*test_vec)[i]; 
            std::cout << "the recipient address is: " << std::string(tx.recipient,sizeof(tx.recipient)) << '\n'; 
            std::cout << "the sender address is: " << std::string(tx.sender, sizeof(tx.recipient)) << '\n'; 
            std::cout << "the input is: " << tx.input << '\n'; 
            std::cout << "the prefix is: " << std::string(tx.prefix, sizeof(tx.prefix)) << '\n'; 
            std::cout << "the nonce is: " << tx.nonce << '\n'; 
            std::cout << "\n"; 
        }

        segment.destroy<TestVector>("TxVector"); 
    }

    return 0; 
}
