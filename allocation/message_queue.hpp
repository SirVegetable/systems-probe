#ifndef MESSAGE_QUEUE_HPP
#define MESSAGE_QUEUE_HPP

#include <string> 
#include <memory> 
#include <iterator>
#include <mutex> 
#include <type_traits>
#include <cstdint> 

template<typename T, typename Meta, typename Alloc> 
class FixedMessageQueue
{
    public: 

        static_assert(std::is_same_v<T,typename std::allocator_traits<Alloc>::value_type>, "must be the same type"); 
        using value_type = T; 
        using allocator_type = Alloc; 
        using pointer = typename std::allocator_traits<Alloc>::pointer; 
        using const_pointer = typename std::allocator_traits<Alloc>::pointer; 
        using reference = value_type&; 
        using const_reference = const value_type&; 
        using size_type = typename std::allocator_traits<Alloc>::size_type; 
        using difference_type = typename std::allocator_traits<Alloc>::difference_type; 
        using iterator = pointer; 
        using const_iterator = const_pointer; 
        using reverse_iterator = std::reverse_iterator<iterator>; 
        using const_reverse_iterator = const reverse_iterator; 

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

        [[nodiscard]]constexpr auto range_fits(std::size_t requested) noexcept 
        {
            return (_capacity - write_index) > requested; 
        }
        
        FixedMessageQueue(const Alloc& alloc, const Meta& h, std::size_t cap) : 
            alloc_inst{alloc}, 
            _header(h),
            _capacity(cap),
            current_index{0}, 
            write_index{0}
        {
            if(cap < 1)
            {
                _capacity = 100; 
            }
            _data = alloc_inst.allocate(_capacity);     
        }

        ~FixedMessageQueue()
        {
            for(auto i = 0; i < _capacity; i++)
            {
                alloc_inst.destroy(_data[i]); 
            }
            alloc_inst.deallocate(_capacity); 
            _data = nullptr; 
        }


        FixedMessageQueue(const FixedMessageQueue& rhs) = delete; 
        FixedMessageQueue& operator=(const FixedMessageQueue& rhs) = delete; 
        FixedMessageQueue(FixedMessageQueue&& rhs) = delete; 
        FixedMessageQueue& operator=(FixedMessageQueue&& lhs) = delete; 

        bool try_pushing_range(const Meta& hdr, const std::vector<T>& vals)
        {
            if(!m.try_lock())
            {
                return false; 
            }

            current_index = write_index;

            if(range_fits(vals.size()))
            {

                for(const auto& v : vals)
                {
                    _data[write_index++] = v;
                }
            }
            else 
            {

                for(const auto& v : vals) 
                {
                    _data[write_index] = v; 
                    next_index(write_index); 
                }
            }
            m.unlock(); 

            _header = hdr; 
            return true; 
        }

        bool search_window_for_item(auto&& Callable)
        {
            if(!m.try_lock())
            {
                return false; 
            }

            for(auto iter = begin(); iter != end(); iter++)
            {
                Callable(iter); 
            }

            m.unlock(); 
            return true; 
        }


    private:
        
        constexpr iterator begin() noexcept { return _data + current_index; }
        constexpr iterator end() noexcept { return  _data + write_index; }
        constexpr const_iterator begin() const noexcept { return _data + current_index; }
        constexpr const_iterator end() const noexcept { return _data + write_index; }
        constexpr reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
        constexpr reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
        constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
        constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
        constexpr const_iterator cbegin() const noexcept { return _data + current_index; }
        constexpr const_iterator cend() const noexcept { return _data + write_index; }
        constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
        constexpr const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }

        [[nodiscard]] constexpr auto next_index(size_type idx) const noexcept {
            return (idx + 1) % _capacity;
        }

        allocator_type alloc_inst; 
        Meta _header; 
        pointer _data; 
        std::size_t _capacity; 
        size_type current_index; 
        size_type write_index; 
        mutable std::mutex m; 
       
}; 

#endif 

