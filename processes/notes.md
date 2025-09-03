# Overview 
- Every process has a unique id, a non-negative integer, because the process ID is always unique it is used for identifying. 
- Pids are reused but only after termination, with some delay. 
- There are special PIDS, but these differ from implementation to implementation, usually PID 0 is the scheduler(swapper process).
- PID 1, usually corresponds to init process and is unvoked by the kernel at the end of the booststrap procedure, the program file for this process is /sbin/init. 
This process is responsible for bringing up the unix system after bootstrap, and usually reads the system-dependent initialization files like: /etc/rc* files, or
/etc/init.d and brings the system to a certain state such as multiuser. 
- The init process never dies, it is a normal user process, not a system process in the kernel like the swapper although it does have run with superuser privelieges. 
- The kernel will have a set of processes that provide OS/kernel services for example PID 2 is sometimes used as the pagedaemon and is responsible for supporting the 
virtual memory system. 
- pid related functionality resides in unistd.h and example usage is in basic_pids.cpp


## Process Guts 

## Process Groups

## Process Sessions 

## Fork()


## setpgid(pid_t pid, pid_t pgid)
- this function shall either join an existing process group or create a new processe group within the session of the calling process. 
- the process group ID of a session leader shall not change. 
- upon success, the pgid of the process with the pid that matches the pid in args shall be set to the pgid arg. 
- a special case, if pid is 0, the process ID of the calling process shall be used. 
- also if the pgid is 0, the process ID of the indicated process shall be used
- returns 0 on success and -1 on failure. 
