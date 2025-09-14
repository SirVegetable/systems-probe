#include <iostream> 
#include <unistd.h> 
#include <cstdlib> 
#include <sys/mman.h> 
#include <sys/stat.h> 
#include <fcntl.h>


static constexpr mode_t FILE_MODE = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

int main(int argc, char* argv[])
{
    int c, fd, flags;   
    off_t length; 

    flags = O_RDWR | O_CREAT; 
    while( (c = getopt(argc, argv, "e")) != -1)
    {
        switch(c)
        {
            case 'e': 
                flags |= O_EXCL; 
                break; 
        }
    }
    if(optind != argc -2)
    {
        std::cerr << "usage: shm_create [ -e ] <name> <length> \n"; 
        exit(1); 
    }
    length = std::atoi(argv[optind + 1]); 
    fd = shm_open(argv[optind], flags, FILE_MODE); 
    if(fd == -1)
    {
        std::cerr << "failed to open file descriptor\n"; 
        exit(1); 
    }

    int err = ftruncate(fd, length); 
    if(err == -1)
    {
        std::cerr << "sizing failed\n"; 
        exit(1);  
    }
    auto ptr = mmap(nullptr, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); 
    if(ptr == MAP_FAILED)
    {
        std::cerr << "failed to mmap the file \n"; 
        exit(1); 
    }
    std::cout << "succesfully created shared mem object\n";
    return 0; 
}