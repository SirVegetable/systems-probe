#include <cstdlib> 
#include <sys/stat.h> 
#include <iostream> 
#include <unistd.h> 
#include <semaphore.h> 
#include <stdio.h> 
#include <fcntl.h> 
#include <sys/mman.h> 

/* 
* Program Description: 
* unlike the previous program increment_counter.cpp, this program will use a 
* shared semaphore instead of named semaphore. 
*/

static constexpr const mode_t FILE_MODE = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; 

struct Shared
{
    sem_t mutex;
    int counter;  
}; 

Shared shared{}; 

int main(int argc, char* argv[])
{

    #if defined(__APPLE__)
        std::cerr << "the file mode is: "<< FILE_MODE << "the args are: " << argv[1] << "\n"; 
        return argc;
    #else 

    int fd, i, nloop = 0;
    Shared* ptr; 

    if(argc < 3)
    {
        std::cerr << "insufficient args provided: must be <pathname> <# of loops> \n"; 
        exit(1); 
    }

    nloop = std::atoi(argv[2]);

    fd = open(argv[1], O_RDWR | O_CREAT, FILE_MODE); 
    if(fd == -1)
    {
        std::cerr << "failed to open file or create file: " << argv[1] << '\n'; 
        exit(1); 
    }
    write(fd, &shared, sizeof(shared)); 
    ptr = static_cast<Shared*>(mmap(nullptr, sizeof(shared), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)); 
    close(fd); 

    /* initialize semaphore that is shared between processes */
    sem_init(&ptr->mutex, 1, 1); 
    setbuf(stdout, nullptr); 

    if(fork() == 0)
    {
        for(i = 0; i < nloop; i++)
        {
            sem_wait(&ptr->mutex); 
            std::cout << "child: " << ptr->counter++ << '\n'; 
            sem_post(&ptr->mutex); 
        }
        exit(0); 
    }
    for(i = 0; i < nloop; i++)
    {
        sem_wait(&ptr->mutex); 
        std::cout << "parent: " << ptr->counter++ << '\n'; 
        sem_post(&ptr->mutex); 
    }
    return 0; 

    #endif 
}
