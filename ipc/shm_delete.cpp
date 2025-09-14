#include <iostream> 
#include <sys/mman.h> 

int main(int argc, char* argv[])
{
    
    if(argc != 2)
    {
        std::cerr << "usage: shm_delete <name> \n"; 
        exit(1); 
    }
    
    auto unlink_status = (shm_unlink(argv[1])) != -1; 
    std::cout << "the unlinking status: " << unlink_status << "\n";  
    return 0; 
}