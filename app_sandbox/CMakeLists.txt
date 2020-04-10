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

add_executable(${PROJECT_NAME}_loop_a shm_sem.cc process_a_main.cc)
target_link_libraries(${PROJECT_NAME}_loop_a
    PRIVATE Threads::Threads rt)

add_executable(${PROJECT_NAME}_loop_b shm_sem.cc process_b_main.cc)
target_link_libraries(${PROJECT_NAME}_loop_b
    PRIVATE Threads::Threads rt)