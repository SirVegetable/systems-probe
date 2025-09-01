#include <unistd.h> 
#include <iostream> 
#include <sys/wait.h> 

int main()
{
    std::cout << "parent pid is: " << getpid() << "\n";
    std::cout << "parent group pid is: " << getpgrp() << "\n"; 
    
    auto child1 = fork();
    if(child1 == 0 )
    {
        std::cout << "child1 pid is: " << getpid() << '\n'; 
        std::cout << "child1 group pid is: " << getpgrp() << '\n'; 
        exit(0); 
    } 

    auto child2 = fork(); 
    if(child2 == 0)
    {
        std::cout << "child2 pid is: " << getpid() << '\n';
        std::cout << "child2 group pid is: " << getpgrp() << '\n'; 
    }
    
    waitpid(child1, nullptr, 0); 
    waitpid(child2, nullptr, 0); 
    return 0; 
}