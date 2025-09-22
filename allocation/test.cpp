#include <cstdint> 
#include <iostream> 

int get_array_depending(std::size_t size, std::size_t after)
{
    return size * 2 - after; 
}

void fill_array_with(int n, std::size_t size, int* ptr)
{
    for(std::size_t i = 0; i < size; i++)
    {
        ptr[i] = n; 
    }
}

void print_array(const int* ptr, std::size_t size)
{
    for(std::size_t i = 0; i < size; i++)
    {
        std::cout << "the value at index: " << i << " is: " << ptr[i] << "\n"; 
    }
}

int main()
{
    #if defined(__APPLE__)
        return 0;
    #else 

    int arr[get_array_depending(10, 2)]; 
    const auto arr_size = sizeof(arr) / sizeof(int); 
    std::cout << "the size of the array is: " << arr_size << "\n"; 

    fill_array_with(1, arr_size, arr); 
    print_array(arr, arr_size); 

    #endif
}