
#include <boost/interprocess/allocators/allocator.hpp> 
#include <boost/interprocess/managed_shared_memory.hpp> 
#include <boost/interprocess/containers/string.hpp> 
#include <array> 
#include <cstdint> 
#include <string_view> 
#include <cstring> 
#include <scoped_allocator>
#include <iostream> 
#include <memory> 
#include <utility> 


#define SHM_NAME "TXQUEUE"
#define MiB_SIZE 1 << 20

static constexpr char addr1[20] = {'0','0','0','0','1','a','c','c','c','0','0','7','1','e','d','c','b','b','a','a'}; 
static constexpr char addr2[20] = {'a','a','b','b','d','d','1','1','0','0','0','0','f','f','a','b','d','d','a','0'}; 
static constexpr std::uint64_t tnonce = 100; 
// static constexpr const char* input1 = ""; 

static constexpr const char* input2 = "0x604080408786abcde9900088aacc8786abcde9900088aacc8786abcde9900088aacc"; 

const std::string v = "0x01"; 

namespace bip = boost::interprocess; 


constexpr int from_hex_digit(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
    if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
    return -1;
}

constexpr std::array<std::uint64_t, 4> convert_hex_to_le_uint256(std::string_view hex_value)
{
    std::array<std::uint64_t, 4> out{0, 0, 0, 0};

    // require "0x" prefix
    if (hex_value.size() <= 2 || hex_value[0] != '0' || (hex_value[1] != 'x' && hex_value[1] != 'X'))
        return out;

    auto hv = hex_value.substr(2); // remove prefix
    if (hv.empty()) return out;

    int word = 0;
    int shift = 0; // bits filled in current word

    // parse right-to-left (LSB first)
    for (int i = int(hv.size()) - 1; i >= 0 && word < 4; --i) {
        int nibble = from_hex_digit(hv[i]);
        if (nibble < 0) return {0,0,0,0}; // invalid char

        out[word] |= (std::uint64_t(nibble) << shift);
        shift += 4;

        if (shift == 64) {
            shift = 0;
            word++;
        }
    }

    return out;
}



namespace shmv2
{
    template<typename T> using allocator = std::scoped_allocator_adaptor<bip::allocator<T, bip::managed_shared_memory::segment_manager>>; 
    using string = bip::basic_string<char, std::char_traits<char>, bip::allocator<char, bip::managed_shared_memory::segment_manager>>; 


    struct Tx
    {
        Tx(const char r[20], const char s[20], std::string_view hex_v, std::uint64_t n, const char* str, const shmv2::allocator<void>& alloc) : 
                nonce(n), 
                input(str, alloc)
        {
            std::memcpy(recipient, r, 20); 
            std::memcpy(sender, s, 20); 
            value = convert_hex_to_le_uint256(hex_v); 
            std::memset(prefix, 0, 8); 
            
            if(std::strlen(str) > 10)
            {
                std::memcpy(prefix, str + 2, 8); 
            }
        }

        char recipient[20];
        char sender[20]; 
        char prefix[8]; 
        std::uint64_t nonce; 
        std::array<std::uint64_t, 4> value; 
        shmv2::string input; 
    }; 

}

template<typename Alloc, typename IIt> 
void destroy_with_allocator(Alloc& alloc, IIt bsrc, IIt esrc) noexcept 
{
    while(bsrc != esrc)
    {
        --esrc; 
        std::allocator_traits<Alloc>::destroy(alloc, std::to_address(esrc)); 
    }
}

template<typename T> 
class MessageBuffer
{
    public:

        using value_type = T; 
        using allocator_type = shmv2::allocator<T>; 
        using pointer = typename std::allocator_traits<allocator_type>::pointer; 
        using const_pointer = typename std::allocator_traits<allocator_type>::pointer; 
        using reference = value_type&; 
        using const_reference = const value_type&; 
        using size_type = typename std::allocator_traits<allocator_type>::size_type; 
        using difference_type = typename std::allocator_traits<allocator_type>::difference_type; 
        using iterator = pointer; 
        using const_iterator = const_pointer; 
        using reverse_iterator = std::reverse_iterator<iterator>; 
        using const_reverse_iterator = const reverse_iterator; 

        MessageBuffer(const allocator_type& a, std::size_t cap) : 
            _alloc(a), 
            _capacity(cap), 
            _current(0)
        {
            _data = std::allocator_traits<allocator_type>::allocate(_alloc, _capacity); 
        }
        
        MessageBuffer(const MessageBuffer& rhs) = delete;
        MessageBuffer& operator=(const MessageBuffer& rhs) = delete;  
        MessageBuffer(MessageBuffer&& rhs) = delete; 
        MessageBuffer& operator =(MessageBuffer&& rhs) = delete; 

        ~MessageBuffer()
        {
            if(!is_empty()) {destroy_with_allocator(_alloc, begin(), end()); }; 
            std::allocator_traits<allocator_type>::deallocate(_alloc, std::to_address(_data), _capacity); 
            _data = nullptr; 
        }

        bool try_push_back(value_type&& val)
        {
            if(size() == capacity())
            {
                return false; 
            }

            std::allocator_traits<allocator_type>::construct(_alloc, std::to_address(end()), std::move(val)); 
            _current++; 
            return true; 
        }

        constexpr auto size() noexcept
        {
            return _current; 
        }

        constexpr auto capacity() noexcept
        {
            return _capacity; 
        }

        constexpr auto is_empty() noexcept
        {
            return _current > 0; 
        }

        void print_nonces_in_buffer()
        {
            auto bsrc = begin(); 
            auto esrc = end(); 
            while(bsrc != esrc)
            {
                --esrc; 
                std::cout << esrc->nonce << "\n"; 
            }
        }
    private: 

        constexpr iterator begin() noexcept { return _data; }
        constexpr iterator end() noexcept {return _data + _current; } 
        
        allocator_type _alloc; 
        size_type _capacity; 
        size_type _current; 
        pointer _data; 


}; 



int main()
{
    struct shm_remover
    {
        shm_remover(){ bip::shared_memory_object::remove(SHM_NAME); }
        ~shm_remover(){ bip::shared_memory_object::remove(SHM_NAME); }
    } remover; 

    bip::managed_shared_memory segment(bip::create_only, SHM_NAME, MiB_SIZE); 
    
    auto* shm_manager = segment.get_segment_manager(); 

    const auto init_free_mem = shm_manager->get_free_memory(); 
    std::cout << "the initial free memory is: " << init_free_mem <<"\n"; 
    
    shmv2::allocator<void> void_alloc(shm_manager); 
    
    MessageBuffer<shmv2::Tx>* mbuf = segment.construct<MessageBuffer<shmv2::Tx>>("MBUF")(void_alloc, 10); 
    std::cout << "the size of a TX is: " << sizeof(shmv2::Tx) << '\n'; 
    std::cout << "the capacity is: " << mbuf->capacity() << "\n"; 

    const auto free_mem_after_queue_init = shm_manager->get_free_memory();

    auto status = mbuf->try_push_back(shmv2::Tx(addr1, addr2, v, tnonce, input2, void_alloc)); 
    std::cout << "the tx was successfully pushed? " << status << '\n'; 
    status = mbuf->try_push_back(shmv2::Tx(addr1, addr2, v, tnonce, input2, void_alloc)); 
    const auto free_mem_after_p1 = shm_manager->get_free_memory(); 
    std::cout << "the tx used: " << free_mem_after_queue_init - free_mem_after_p1 << " bytes \n"; 

    mbuf->print_nonces_in_buffer(); 

    segment.destroy<MessageBuffer<shmv2::Tx>>("MBUF"); 
    
    const auto mem_after_q_dtor = shm_manager->get_free_memory(); 
    std::cout << "the difference between init and final mem is: " << init_free_mem - mem_after_q_dtor << '\n'; 
    return 0; 
}


