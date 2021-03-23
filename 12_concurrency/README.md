# Home work: Concurrency and synchronization

# Task - Implement kernel module which prints
 numbers 1 to N to kernel ring buffer from N
 different kthreads. For synchronization select
 one or few methods, like:
 - mutexes
 - spinlocks
 - semaphores
 - barriers
 - atomic values
 - seqlocks

 The result for N=2 should be present as dmesg output:
1
1
2
2
