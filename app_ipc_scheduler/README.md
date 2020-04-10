## Semaphore and shared memory
This app consists of methods to create a semaphore to synchronize between two processes, A and B. The data to be synchronized is stored in a posix shared memory. To run the program, start with process_a and then process_b. Note that process_b needs to terminate first since it is triggered by a semaphore from A. It will be in a deadlock state if A stops posting semaphores.

## Additional functionalities
Additional functionalities are provided, including:
1. timed loop using clock_nanosleep
2. setting process scheduling parameters such as policy and priority
3. setting cpu affinity
