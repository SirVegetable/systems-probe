
#include <unistd.h> /* contains read, write, sleep and fork */
#include <sys/types.h> /* contains pid_t */
#include <iostream>
#include "../common.hpp"

/* extern variables in the initialized data segment */
int globvar = 6; 
char buff[] = "a write to stdout \n"; 

int main(void)
{
    int var; /* automatic variable on the stack */
    pid_t pid; 

    var = 88; 
    if(write(STDOUT_FILENO, buff, sizeof(buff) -1) != sizeof(buff) - 1)
    {
        sys_err("write error"); 
        std::cout << "before the fork\n"; 
    }

    if(((pid = fork())  < 0 ))
    {
        sys_err("forked error"); 
    }
    /* child modifies variables */
    else if(pid == 0)
    {
        globvar++; 
        var++; 
    }
    else {
        /* parent sleeps for 2 seconds */
        sleep(2); 
    }

    std::cout << "pid is: " << getpid() << " globals are: " << globvar << " and " << var << '\n'; 
    exit(0); 
}
