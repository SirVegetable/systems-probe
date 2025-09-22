
template<typename T, std::size_t Size, typename Alloc> 
class MessageQueue : public Alloc
{
    public: 
        using value_type = T; 
        using pointer = Alloc::pointer; 
        using const_pointer = Alloc::const_pointer; 
        using reference = value_type&; 
        using const_reference = const value_type&; 
        using size_type = Alloc::size_type; 
        using difference_type = Alloc::difference_type; 
    private: 
        pointer begin; 
        pointer end; 
        size_type capacity; 
        
}; 


