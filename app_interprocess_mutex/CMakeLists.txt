cmake_minimum_required(VERSION 3.1)
project(ipc_mutex)

set(PROCESS_A "${PROJECT_NAME}_a")
set(PROCESS_B "${PROJECT_NAME}_b")

# Search for libraries
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

# Two executables to handle shared memory synchronization
add_executable(${PROCESS_A} process_a_main.cc shared_mutex.cc)
target_link_libraries(${PROCESS_A}
    PRIVATE Threads::Threads rt)

add_executable(${PROCESS_B} process_a_main.cc shared_mutex.cc)
target_link_libraries(${PROCESS_B}
    PRIVATE Threads::Threads rt)