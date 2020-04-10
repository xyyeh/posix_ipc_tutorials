cmake_minimum_required(VERSION 3.1)
project(ipc_scheduler)

set(LOOP_PROCESS_A "${PROJECT_NAME}_loop_a")
set(LOOP_PROCESS_B "${PROJECT_NAME}_loop_b")

# Search for libraries
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

# Two executables to handle shared memory synchronization
add_executable(${LOOP_PROCESS_A} loop_a_main.cc shm_sem.cc)
target_link_libraries(${LOOP_PROCESS_A}
    PRIVATE Threads::Threads rt)

add_executable(${LOOP_PROCESS_B} loop_b_main.cc shm_sem.cc)
target_link_libraries(${LOOP_PROCESS_B}
    PRIVATE Threads::Threads rt)