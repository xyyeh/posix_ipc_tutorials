cmake_minimum_required(VERSION 3.1)
project(test_proj)


# Search for libraries
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

# Two executables to handle shared memory synchronization
add_executable(${PROJECT_NAME} main.cc shm_sem.cc)
target_link_libraries(${PROJECT_NAME}
    PRIVATE Threads::Threads rt)