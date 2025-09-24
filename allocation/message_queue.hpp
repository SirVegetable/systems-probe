#include <boost/interprocess/allocators/allocator.hpp> 
#include <string> 
#include <boost/containers/string> 
#include <boost/interprocess/managed_shared_memory.hpp> 
#include <memory> 
#include <iterator>



template<typename T, typename Meta, typename Alloc> 
class FixedMessageQueue
{
    public: 

        static_assert(std::is_same_v<T, std::allocator_traits<Alloc>::value_type>); 
        using value_type = T; 
        using allocator_type = Alloc; 
        using pointer = typename std::allocator_traits<Alloc>::pointer; 
        using const_pointer = typename std::allocator_traits<Alloc>::pointer
        using reference = value_type&; 
        using const_reference = const value_type&; 
        using size_type = typename std::allocator_traits<Alloc>::size_type; 
        using difference_type = typename std::allocator_traits<Alloc>::difference_type; 
        using iterator = pointer; 
        using const_iterator = const_pointer; 
        using reverse_iterator = std::reverse_iterator<iterator>; 
        using const_reverse_iterator = const reverse_iterator; 

        constexpr iterator begin() return { _begin; }
        constexpr iterator end() return { _begin + _size; }
        constexpr const_iterator begin() const noexcept { return _begin; }
        constexpr const_iterator end() const noexcept { return _begin + _size; }
        constexpr reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
        constexpr reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
        constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
        constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
        constexpr const_iterator cbegin() const noexcept { return _begin; }
        constexpr const_iterator cend() const noexcept { return _begin + _size; }
        constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
        constexpr const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }

        [[nodiscard]] constexpr auto get_range_header() const noexcept 
        {
            return _header; 
        }

        [[nodiscard]] constexpr auto is_empty() const noexcept 
        {
            return read_index == write_index; 
        }

        constexpr size_type size() const noexcept
        {
            return _size; 
        }
        
        FixedMessageQueue(const Alloc& alloc, const Meta& h, std::size_t Cap) : 
            alloc_inst(alloc), 
            _header(h),
            
        {}

        FixedMessageQueue(const FixedMessageQueue& rhs) = delete; 
        FixedMessageQueue& operator=(const FixedMessageQueue& rhs) = delete; 
        FixedMessageQueue(FixedMessageQueue&& rhs) = delete; 
        FixedMessageQueue(FixedMessageQueue&& lhs) = delete; 

        // void constexpr destroy_range()
        // {

        // }

        // bool try_pushing_one(const value_type& val) noexcept(noexcept)
        // {
            
        // }

        // bool try_pushing_range(); 

    
    private:
        allocator_type alloc_inst; 
        Meta _header; 
        pointer _data; 
        std::size_t _capacity; 
        size_type read_index; 
        size_type write_index; 
       
}; 

