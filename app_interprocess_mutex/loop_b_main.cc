#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "shared_mutex.h"

bool kill_program = false;

void ExitCallback(int signum) {
  std::cout << "Terminating program ..." << std::endl;
  kill_program = true;
  exit(1);
}

int main() {
  // control c callback
  signal(SIGINT, ExitCallback);

  // Init shared mutex by a name, which can be used by
  // any other process to access the mutex.
  // This function both creates new and opens an existing mutex.
  shared_mutex_t mutex = shared_mutex_init("/my-mutex");
  if (mutex.ptr == nullptr) {
    return 1;
  }

  if (mutex.created) {
    std::cout << "The mutex was just created" << std::endl;
  } else {
    std::cout << "The mutex was not created" << std::endl;
  }

  // loop
  bool process_flag = false;
  while (1) {
    // std::cout << "afds" << std::endl;

    // use pthread calls for locking and unlocking.
    pthread_mutex_lock(mutex.ptr);
    // process_flag = mutex.has_data;
    // if (mutex.has_data) {
    //   std::cout << "true" << std::endl;
    //   mutex.has_data = false;
    // }

    std::cout << mutex.has_data << std::endl;

    pthread_mutex_unlock(mutex.ptr);
    usleep(10000);
    // if (process_flag) {
    //   // run computations

    //   pthread_mutex_lock(mutex.ptr);
    //   mutex.has_data = false;
    //   pthread_mutex_unlock(mutex.ptr);
    // } else {
    //   usleep(10);
    // }
    // kill program on control c
    if (kill_program) {
      break;
    }
  }

  // Closing is used to release local resources, used by a mutex.
  // It's still available to any other process.
  if (shared_mutex_close(mutex)) {
    return 1;
  }
  return 0;
}