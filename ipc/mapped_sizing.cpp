#include <iostream> 
#include <cstdlib> 
#include <sys/stat.h> 
#include <unistd.h> 
#include <fcntl.h>
#include <sys/mman.h> 
#include <algorithm> 

static constexpr mode_t FILE_MODE = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; 

/* 
* when a regular file is memory mapped, the size of the mapping in memory normally equals
* the size of file. Previous programs (incrementing) the file size is set to the size of
* the memory mapping. But these two sizes can differ. 
*
* 1) take command line args to specify the file path, size to set the file to, and the
* size of the memory mapping. 
* 2) the file being open needs to be created if it does not exist, or truncate to a size
* of 0 if it already exists. The size of the file is then set to the specified size by 
* seeking to that size minus 1 byte and 1 writing byte. 
* 3) memory map the file using the specified cmline arg. Then close the file. 
* 4) print the page size using sysconf 
*/

int main(int argc, char* argv[])
{
    int fd; 
    char* ptr; 
    size_t file_size, mmap_size, page_size, i; 

    if(argc < 4)
    {
        std::cerr << "insufficient cmdline args: <pathname> <file size> <mapping size> \n"; 
        exit(1); 
    }
    file_size = std::atoi(argv[2]); 
    mmap_size = std::atoi(argv[3]); 

    fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, FILE_MODE); 
    if(fd == -1)
    {
        std::cerr << "failed file open: " << argv[1] << '\n'; 
        exit(1);
    }

    auto lserr = lseek(fd, file_size - 1, SEEK_SET); 
    if(lserr == (off_t) - 1)
    {
        std::cerr << "lseek err on: " << argv[1] << '\n'; 
        exit(1);
    }

    auto werr = write(fd, "", 1); 
    if(werr == -1)
    {
        std::cerr << "write error on: " << argv[1] << '\n'; 
        exit(1);
    }

    ptr = static_cast<char*>(mmap(nullptr, mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)); 
    close(fd); 

    page_size = sysconf(_SC_PAGESIZE); 
    if(page_size == 0)
    {
        std::cerr << "error getting page size \n"; 
        exit(1);
    }

    std::cout << "PAGESIZE IS: " << page_size << '\n'; 
    for(i = 0; i < std::max(file_size, mmap_size); i += page_size)
    {
        std::cout << "ptr[" << i << "] = " << static_cast<int>(ptr[i]) << " \n"; 
        ptr[i] = 1; 
        std::cout << "ptr[" << i + page_size - 1 << "] = " << static_cast<int>(ptr[i + page_size - 1]) << '\n'; 
        ptr[i + page_size -11] = 1; 
    }
    std::cout << "ptr[" << i << "] = " << static_cast<int>(ptr[i]) << '\n'; 
    return 0; 
}