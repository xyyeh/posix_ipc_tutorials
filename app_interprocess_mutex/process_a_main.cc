#include <stdio.h>

#include <iostream>
#include <string>

#include "shared_mutex.h"

int main() {
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
  }

  // Use pthread calls for locking and unlocking.
  pthread_mutex_lock(mutex.ptr);
  std::cout << "Press eny key to unlock the mutex" << std::endl;
  char dummy;
  std::cin >> dummy;
  pthread_mutex_unlock(mutex.ptr);

  // Closing is used to release local resources, used by a mutex.
  // It's still available to any other process.
  if (shared_mutex_close(mutex)) {
    return 1;
  }
  return 0;
}