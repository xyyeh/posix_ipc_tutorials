## Shared Memory with Embedded Semaphore
This app consist of a generic shared memory class that contains an embedded semaphore, thus incorporating native semaphore functionalities within the class. The semaphore can be used to synchronize both data and time (see the app for more details).

## Limitations
Since the process to be synchronized (loop_b_main) is only able to lock instead of unlock for time synchronization, it needs to be run last (after loop_a_main) and exited first (before loop_a_main).
