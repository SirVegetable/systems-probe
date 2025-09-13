#include <iostream> 
#include <sys/mman.h> 
#include <unistd.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <cstdlib> 
/* 
* This program demonstrates a common technique for handling a file that is growing: 
* we specify a memory-map size that is larger than the file, keep track of the file's 
* current size(making sure not to reference beyond the current end-of-file), and then
* just let the file's size increase as more data is written to the file. 
* 
* Steps: 
* 1) open a file or creating it if it does not exist or truncating it if it does exist.
* the file is memory mapped with a size of 32768, even the file's current size is 0. 
* 2) increase the size of the file, 4096 bytes at a time, by calling ftruncate, and then
* fetch the byte that is now the final byte of the file. 
*/

#define FILE "test.data"
static constexpr mode_t FILE_MODE = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; 
static constexpr size_t SIZE = 32768; 
static constexpr size_t PAGESIZE = 4096; 

int main(void)
{
    int fd;
    size_t i;  
    char* ptr; 

    fd = open(FILE, O_RDWR | O_CREAT | O_TRUNC, FILE_MODE); 
    if(fd == -1)
    {
        std::cerr << "failed file open: " << FILE << '\n'; 
        exit(1);
    }
    ptr = static_cast<char*>(mmap(nullptr, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)); 

    /* note we don't close the file, since we need to grow it */
    for(i = PAGESIZE; i <= SIZE; i += PAGESIZE)
    {
        std::cout << "setting the size to: " << i << '\n'; 
        if(ftruncate(fd, i) == -1 )
        {
            std::cerr << "TRUNCATE ERROR \n"; 
            exit(1); 
        } 
        std::cout << "ptr[" << i - 1 << "] = " << static_cast<int>(ptr[i - 11]) << '\n'; 
    }

    return 0; 
}