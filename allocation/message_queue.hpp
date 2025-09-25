#include <boost/interprocess/allocators/allocator.hpp> 
#include <string> 
#include <boost/containers/string> 
#include <boost/interprocess/managed_shared_memory.hpp> 
#include <memory> 
#include <iterator>
#include <mutex> 


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

        [[nodiscard]] constexpr Meta get_range_header() const noexcept 
        {
            return _header; 
        }

        [[nodiscard]] constexpr auto is_empty() const noexcept 
        {
            return current_index == write_index; 
        }

        constexpr size_type get_window_size() const noexcept
        {
            return write_index - current_index; 
        }
        
        FixedMessageQueue(const Alloc& alloc, const Meta& h, std::size_t Cap) : 
            alloc_inst(alloc), 
            _header(h),
            _capacity(Cap),
            current_index{0}, 
            write_index{0},  
            anchor_index{0}
        {
            // constexpr const std::size_t alignment = alignof(T); 

            _data = alloc_inst.allocate(_capacity); 
        }

        ~FixedMessageQueue()
        {

        }


        FixedMessageQueue(const FixedMessageQueue& rhs) = delete; 
        FixedMessageQueue& operator=(const FixedMessageQueue& rhs) = delete; 
        FixedMessageQueue(FixedMessageQueue&& rhs) = delete; 
        FixedMessageQueue(FixedMessageQueue&& lhs) = delete; 

        void destroy_last_range()
        {

        }

        bool try_pushing_range(const Meta& h, const std::vector<P>& vals)
        {

        }

        iterator search_window_for(auto&& Callable)
        {

        }

    private:

        allocator_type alloc_inst; 
        Meta _header; 
        pointer _data; 
        std::size_t _capacity; 
        size_type current_index; 
        size_type anchor_index; 
        size_type write_index; 
        mutable std::mutex m; 
       
}; 

