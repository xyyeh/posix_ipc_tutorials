#include <signal.h>
#include <stdio.h>

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
  if (mutex.ptr == NULL) {
    return 1;
  }

  if (mutex.created) {
    printf("The mutex was just created\n");
    // initialize
    for (int i = 0; i < 10; i++) {
      mutex.buffer[i] = 0;
    }
    // mutex.has_data(false);
  }

  // loop
  const unsigned int kSecToNanosec = 1e9;
  const double kFixedTimeStep = 1.0 / 1.0;  // 1hz
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  t.tv_sec++;
  while (1) {
    // Use pthread calls for locking and unlocking.
    pthread_mutex_lock(mutex.ptr);
    // if (mutex.has_data == true) {
    //   std::cout << "error" << std::endl;
    // }
    std::cout < < < < std::endl;
    // update buffer
    mutex.buffer[0]++;
    // set flag

    pthread_mutex_unlock(mutex.ptr);

    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);

    // calculate next shot
    t.tv_nsec += (kFixedTimeStep * kSecToNanosec);
    while (t.tv_nsec >= kSecToNanosec) {
      t.tv_nsec -= kSecToNanosec;
      t.tv_sec++;
    }

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