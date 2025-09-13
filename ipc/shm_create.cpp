#include <iostream> 
#include <unistd.h> 

static constexpr int def_flags = O_RDWR | O_CREAT; 
static constexpr int e_flags = O_RDWR | O_CREAT | O_EXCL; 

int main(int argc, char* argv[])
{
    int c, fd, flag;  
    off_t length; 
    
    char* ptr; 
    if(argc != 2)
    {
        std::cerr << " incorrect usage: <opt> <filename> \n"; 
        exit(1); 
    }
    
    
    return 0; 
}