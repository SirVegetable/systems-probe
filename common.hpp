#ifndef COMMON_HPP 
#define COMMON_HPP

#include <cstdlib> 
#include <iostream> 

inline void sys_err(const char* what)
{
    std::cout << "error occurred: " << what << '\n';
    exit(1); 
}

#endif 