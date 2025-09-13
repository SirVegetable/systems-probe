#include <sys/stat.h> 
#include <iostream> 
#include <unistd.h> 
#include <sys/mman.h> 
#include <fcntl.h> 
#include <cstdlib>

struct WriterRole
{
    mode_t mode = O_RDWR; 
    mode_t permissions = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
}; 


int main(int argc, char* argv[])
{
    int fd, err; 
    int i; 
    struct stat mem_stats; 
    unsigned char* ptr; 
    if(argc != 2)
    {
        std::cerr << "incorrect usage: <file name> \n"; 
        exit(1); 
    }
    constexpr WriterRole role{}; 

    fd = shm_open(argv[1], role.mode, role.permissions); 
    if(fd == -1)
    {
        std::cerr << "could not open ipc name: " << argv[1] << '\n'; 
        exit(1); 
    }
    err = fstat(fd, &mem_stats); 
    if(err == -1)
    {
        std::cerr << "failed to retrieve shared memory object\n";
        exit(1);  
    }
    ptr = static_cast<unsigned char*>(mmap(nullptr, mem_stats.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)); 
    close(fd); 

    for(i = 0; i < mem_stats.st_size; i++)
    {
        *ptr++ = i % 256; 
    }
    return 0; 
}