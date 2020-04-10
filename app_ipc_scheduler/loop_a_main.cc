#include <signal.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

#include "shm_sem.h"

struct SyncData {
  unsigned int info;
};

bool kill_program = false;

void ExitCallback(int signum) {
  std::cout << "Terminating program ..." << std::endl;
  kill_program = true;
}

int main(int argc, char* argv[]) {
  try {
    ShmSemaphore shared_memory("/SyncShm");
    shared_memory.Create(sizeof(struct SyncData));
    shared_memory.Attach();
    SyncData* ptr = (SyncData*)shared_memory.Data();

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
      std::cout << "Couldn't retrieve real-time scheduling paramers"
                << std::endl;
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
    const unsigned int kSecToNanosec = 1e9;
    const double kFixedTimeStep = 1.0 / 1000.0;  // 1hz
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    t.tv_sec++;
    while (1) {
      if (shared_memory.UnlockSingle() == false) {
        std::cout << "Missed ticks" << std::endl;
      }

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
  } catch (std::exception& ex) {
    std::cout << "Exception:" << ex.what();
  }
}