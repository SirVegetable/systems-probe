#include <unistd.h> 
#include <iostream> 
#include <sys/wait.h> 
#include <sys/types.h> 
#include <cassert> 


inline int assign_child_to_group(pid_t pgid)
{
    return setpgid(0, pgid); 
}

int main()
{
    std::cout << "parent pid is: " << getpid() << "\n";
    std::cout << "parent group pid is: " << getpgrp() << "\n"; 
    
    auto child1 = fork();
    if(child1 == 0 )
    {
        setpgid(0, 0); /* set to myself */
        std::cout << "child1 pid is: " << getpid() << '\n'; 
        std::cout << "child1 group pid is: " << getpgrp() << '\n'; 
        exit(0); 
    } 
    /* race condition here or onwards to the waitpids, a 1 microsecond sleep prevents it */
    auto child2 = fork(); 
    if(child2 == 0)
    {
        int ret = assign_child_to_group(child1); 
        assert(ret == 0); 
        std::cout << "child2 pid is: " << getpid() << '\n';
        std::cout << "child2 group pid is: " << getpgrp() << '\n'; 
        exit(1); 
    }
    
    // waitpid(child1, nullptr, 0); 
    // waitpid(child2, nullptr, 0); 

    /* 
    * now instead of waiting for both child pids, I can wait on the childrens 
    * group pid, according to the documentation if the passed pid is a negative
    * then its absolute value is treated as the group pid. The result is pid of
    * the child that died/quit.
    */
    for(size_t i = 0; i < 2; i++)
    {
        auto pid = waitpid(-child1, nullptr, 0); 
        std::cout << "the child that exited was: " << pid << '\n'; 
    }

    return 0; 
}