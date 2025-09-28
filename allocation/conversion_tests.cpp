#include "common.hpp"
#include <string> 
#include <array> 
#include <iostream> 

using Answer = std::array<std::uint64_t, 4>; 

int main()
{
    std::string t1 = "0x"; 
    std::string t2 = "0xFFFFFFFFFFFFFFFF"; 
    std::string t3 = "0x10000000000000000"; 
    std::string t4 = "0x1234567890ABCDEF1234567890ABCDEF"; 
    std::string t5 = "0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"; 
    std::string t6 = "0x2E05"; 

    Answer a1 = {0,0,0,0}; 
    Answer a2 =  { 0xFFFFFFFFFFFFFFFFULL, 0, 0, 0 }; 
    Answer a3 = { 0, 0x1, 0, 0 }; 
    Answer a4 = { 0x1234567890ABCDEFULL, 0x1234567890ABCDEFULL, 0, 0}; 
    Answer a5 =  { 0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL}; 
    Answer a6 = {0x2E05, 0, 0, 0}; 

    bool test1 = convert_hex_to_le_uint256(t1) == a1; 
    bool test2 = convert_hex_to_le_uint256(t2) == a2;
    bool test3 = convert_hex_to_le_uint256(t3) == a3; 
    bool test4 = convert_hex_to_le_uint256(t4) == a4;
    bool test5 = convert_hex_to_le_uint256(t5) == a5; 
    bool test6 = convert_hex_to_le_uint256(t6) == a6;   


    std::cout << "The result of test one is: " << test1 << '\n'; 
    std::cout << "The result of test two is: " << test2 << '\n'; 
    std::cout << "The result of test three is: " << test3 << '\n'; 
    std::cout << "The result of test four is: " << test4 << '\n'; 
    std::cout << "The result of test five is: " << test5 << '\n'; 
    std::cout << "The result of test six is: " << test6 << '\n'; 

}