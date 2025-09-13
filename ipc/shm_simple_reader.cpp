#include <sys/stat.h> 
#include <iostream> 
#include <unistd.h> 
#include <sys/mman.h> 
#include <fcntl.h> 
#include <cstdlib>

struct ReaderRole
{
    mode_t mode = O_RDONLY; 
    mode_t permissions = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
}; 


int main(int argc, char* argv[])
{
    int fd, err; 
    size_t i; 
    struct stat mem_stats; 
    unsigned char* ptr; 
    if(argc != 2)
    {
        std::cerr << "incorrect usage: <name> \n"; 
        exit(1); 
    }
    constexpr auto[m, p] = ReaderRole{}; 
    fd = shm_open(argv[1], m, p); 
    if(fd == -1)
    {
        std::cerr << "could not open ipc name: " << argv[1] << "\n"; 
        exit(1); 
    }
    err = fstat(fd, &mem_stats); 
    if(err == -1)
    {
        std::cerr << "failed to retrieve shared memory object\n";
        exit(1);  
    }
    ptr = static_cast<unsigned char*>(mmap(nullptr, mem_stats.st_size, PROT_READ, MAP_SHARED, fd, 0)); 
    close(fd); 
    unsigned char c; 
    for(i = 0; i < mem_stats.st_size; i++)
    {
        if((c = *ptr++) != (i % 256))
        {
            std::cerr << "ptr at index: " << i << " is erroneous and equals: " << c << "\n";  
            exit(1); 
        }
    }
    return 0; 
}