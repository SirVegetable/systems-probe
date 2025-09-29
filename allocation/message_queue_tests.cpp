#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp> 
#include "common.hpp"
#include "shm_transaction.hpp"
#include <iostream> 
#include <cstdint> 
#include "message_queue.hpp"

#define SHM_NAME "TXQUEUE"

static constexpr std::size_t MiB = 1 << 20; 
static constexpr std::size_t NUM_TXs = 1000; 

struct BlockHeader
{
    std::uint64_t number{0}; 
    std::uint64_t timestamp{0}; 
}; 

static constexpr BlockHeader header{100, 23012380123802UL}; 
using TestTxQueue = FixedMessageQueue<shm::TransactionV4,BlockHeader, shm::allocator<shm::TransactionV4>>; 

int main()
{
    using namespace boost::interprocess; 
    struct shm_remover
    {
        shm_remover(){ shared_memory_object::remove(SHM_NAME); }
        ~shm_remover(){ shared_memory_object::remove(SHM_NAME); } 
    } remover; 
    
    managed_shared_memory segment(create_only, SHM_NAME, MiB); 
    auto shm_manager = segment.get_segment_manager(); 

    if(MiB == 1048576)
    {
        std::cout << "true" << "\n"; 
    }
    else 
    {
        std::cout << "false"; 
    }
   
    const auto initial_capacity = shm_manager->get_free_memory(); 

    shm::allocator<void> alloc(shm_manager); 

    auto* tx_queue = segment.construct<TestTxQueue>("QUEUE")(alloc, header, NUM_TXs); 

    const auto allocated_tx_queue_size = (sizeof(shm::TransactionV4) * NUM_TXs) + sizeof(TestTxQueue); 

    const auto remaining = shm_manager->get_free_memory(); 
    auto [n, ts] = tx_queue->get_range_header();  
    std::cout << "the queues block number is: " << n << " and the timestamp is: " << ts << '\n'; 
    std::cout << "there is: " << remaining << " remaining free bytes \n"; 
    std::cout << "the queue and associated memory took up: " << initial_capacity - remaining << '\n'; 
    std::cout << "the allocator cost to this is: " << initial_capacity - remaining - allocated_tx_queue_size << "\n"; 

    segment.destroy<TestTxQueue>("QUEUE"); 
    return 0; 
}