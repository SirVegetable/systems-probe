#include <sys/mman.h> 
#include <semaphore.h>
#include <iostream> 
#include <cstdlib> 
#include <fcntl.h> 
#include <unistd.h> 
#include <sys/stat.h> 
#include <stdio.h> 

/* 
* Program Description: 
* We want the parent and child to share a piece of memory in which the counter is stored.
* To do so we use a memory mapped file: a file we open that map to our address space, which
* we get from the commandline. 
* 
* Program Steps:
* 1) get file name from the cmdline, so we open the file for reading and writing, creating the
* file if it does not exist. 
* 2) write the integer value 0 to it for the counter. 
* 3) call mmap to the file just opened to map to this process, the first arg is nullptr, so the
* system chooses the start address for us. The length is the size of the integer, and we specify
* the read and write permissions, and MAP_SHARED argument so child can access and parent can see. 
* 4) store the start address from mmap()
* 5) set the standard ouput unbuffered and call fork. 
* 6) then have both child and parent increment the counter. 
* 7) print the counter
* 8) cleanup 
*/

static constexpr const char* SEM_NAME = "mysem"; 
static constexpr const mode_t FILE_MODE = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; 

int main(int argc, char* argv[])
{
    int fd, i, nloop, zero = 0; 
    int* start_addr; 
    sem_t *mutex; 

    if(argc < 3)
    {
        std::cerr << "insufficient arguments provided: args should be: filename and number of loops to do \n"; 
        exit(1); 
    }
    nloop = std::atoi(argv[2]); 
    fd = open(argv[1], O_RDWR | O_CREAT, FILE_MODE); 
    if(fd == -1) 
    { 
        std::cerr << "failed to open file: " << argv[1] << '\n'; 
        exit(1); 
    }
    auto did_write = write(fd, &zero, sizeof(int)); 
    if(did_write == -1)
    {
        std::cerr << "failed to write to file: " << argv[1] << '\n';  
    }

    start_addr = static_cast<int*>(mmap(nullptr, sizeof(int),  PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)); 
    close(fd); 

    /* create, initialize, and unlink semaphore */
    mutex = sem_open(SEM_NAME, O_CREAT | O_EXCL, FILE_MODE, 1); 
    
    /* 
    * This creates a named semaphore in the system. On Linux, this lives under /dev/shm/ as something like 
    * /dev/shm/sem.mysem and any process can open it later (using the same SEM_NAME) as long as it still exists.
    * So we unlink it immediately which means I don't want any other processes to be able to open this semaphoe 
    * by name but processes that already have it open(this process and its children) can keep using it. 
    */
    sem_unlink(SEM_NAME); 

    /* set to unbuffered */
    setbuf(stdout, nullptr); 
    if(fork() == 0)
    {
        for(i = 0; i < nloop; i++ )
        {
            sem_wait(mutex); 
            std::cout << "child: " << (*start_addr)++ << '\n'; 
            sem_post(mutex); 
        }
        exit(0); 
    }
    /* parent */
    for(i = 0; i < nloop; i++)
    {
        sem_wait(mutex); 
        std::cout << "parent: " << (*start_addr)++ << '\n'; 
        sem_post(mutex); 
    }

    return 0; 
}