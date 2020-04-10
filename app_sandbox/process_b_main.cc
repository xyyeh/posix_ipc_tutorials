#include <fcntl.h>
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <chrono>
#include <iostream>

#include "shm_sem.h"

bool kill_program = false;

void ExitCallback(int signum) {
  std::cout << "Terminating program ..." << std::endl;
  kill_program = true;
}

struct ShmData {
  int data[10];
};

int main() {
  ShmSemaphore shared_memory("/testSharedmemory1");

  shared_memory.Create(100);
  shared_memory.Attach();
  ShmData* ptr = (ShmData*)shared_memory.GetData();

  // thread properties
  struct sched_param params;
  params.sched_priority = sched_get_priority_max(SCHED_FIFO);
  auto ret = pthread_setschedparam(pthread_self(), SCHED_FIFO, &params);
  if (ret != 0) {
    // Print the error
    std::cout << "Unsuccessful in setting thread realtime prio" << std::endl;
    return -1;
  }

  // Now verify the change in thread priority
  int policy = 0;
  ret = pthread_getschedparam(pthread_self(), &policy, &params);
  if (ret != 0) {
    std::cout << "Couldn't retrieve real-time scheduling paramers" << std::endl;
    return -1;
  }

  // Check the correct policy was applied
  if (policy != SCHED_FIFO) {
    std::cout << "Scheduling is NOT SCHED_FIFO!" << std::endl;
  } else {
    std::cout << "SCHED_FIFO OK" << std::endl;
  }

  // Print thread scheduling priority
  std::cout << "Thread priority is " << params.sched_priority << std::endl;

  // Set affinity
  cpu_set_t my_set;
  CPU_ZERO(&my_set);   /* Initialize it all to 0, i.e. no CPUs selected. */
  CPU_SET(1, &my_set); /* set the bit that represents core 1. */
  sched_setaffinity(0, sizeof(cpu_set_t), &my_set);

  // control c callback
  signal(SIGINT, ExitCallback);

  // loop
  std::cout << "adsfsdfadsfadsfa" << std::endl;
  const unsigned int kSecToNanosec = 1e9;
  const double kFixedTimeStep = 1.0 / 1000.0;  // 1hz
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  t.tv_sec++;
  while (1) {
    // try waiting on client and execute client commands
    shared_memory.Lock();
    std::cout << ptr->data[0] << std::endl;
    // update feedback
    // capture last commands to send out
    shared_memory.UnLockLimited();

    // kill program on control c
    if (kill_program) {
      break;
    }
  }

  return 0;
}