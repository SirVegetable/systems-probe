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

# Processes 

## Process Guts 

## Process Groups
- process groups are a collection of related processes which can all be signalled at once.

## Process Sessions 
- process sessions are a collection of process groups, which are either attached to a single terminal device (known as the controlling terminal ) or not attached to any terminal. 
- sessions are used for job control: one of the process groups in the session is the foreground process group, and can be sent signals by terminating control characters. 
- You can think of a session with a controlling terminal as corresponding to a "login" on that terminal. (Daemons normally disassociate themselves from any controlling terminal by creating a new session without one.)
- defined in the posix standard. 

## Fork()


## setpgid(pid_t pid, pid_t pgid)
- this function shall either join an existing process group or create a new processe group within the session of the calling process. 
- the process group ID of a session leader shall not change. 
- upon success, the pgid of the process with the pid that matches the pid in args shall be set to the pgid arg. 
- a special case, if pid is 0, the process ID of the calling process shall be used. 
- also if the pgid is 0, the process ID of the indicated process shall be used
- returns 0 on success and -1 on failure. 

# Shared Memory

## mmap()
- the mmap function maps either a file or a posix shared memory object into the address space of a process.
- usually used for three purposes: 
1) with a regular file to provide memory-mapped I/O.
2) with special files to procide anonymous memory mappings.
3) with shm_open to provide posix shared memory between unrelated processes. 
- function signature: void* mmap(void* addr, size_t len, int prot, int flags, int fd, off_t offset); and returns tthe starting address of mapped region if OK, else MAP_FAILED on error. 
- parameter addr: can specify the starting address within the process of where the descriptor fd should be mapped. 
- Normally, the addr is a nullptr, telling the kernel to choose the starting address. 
- parameter len: is the number of bytes to map into the address of the process, starting at the offset bytes from the beginning of the file. 
- Normally offset is set to 0. 
- parameter prot: this specifies the protection of the memory-mapped region using constants. The common value for this argument is PROT_READ | PROT_WRITE for read-write access. 
- flags are specified by constants either MAP_FIXED or MAP_SHARED 
- on way to share memory between a parent and child is to call mmap with MAP_SHARED before calling fork(), then posix gurantees that memory mappings in the parent are retained in the child. Furthermore, changes made by the parent are visible to child and vice versa.
- After mmap() returns successfully, the fd argument can be closed. This has no effect on the mapping established. 

## munmap()
- The munmap function removes a mapping from the address space of the process. 
- function signature: int munmap(void* addr, size_t len); 
- the addr argument is the address that was returned by mmap(), and th elen is the size of that mapped region. 
- Any reference to these regions after munmap() results in the generation of a SIGSEGV signal to the process
- If the mapped region was mapped using MAP_PRIVATE, the changes made are discarded. 

## mysync()
- The kernel's virtual memory algorithm keeps the memory-mapped file(typically on disk)