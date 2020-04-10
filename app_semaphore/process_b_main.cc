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

#include <array>
#include <chrono>
#include <iostream>
#include <numeric>

#define SEM_SERVER_MUTEX_NAME "/server-sem-mutex"
#define SEM_CLIENT_MUTEX_NAME "/client-sem-mutex"
#define SHARED_MEM_NAME "/posix-shared-mem-example"

bool kill_program = false;
std::array<double, 100> loop_time;
unsigned idx = 0;

void SignalCallback(int signum) {
  std::cout << "Terminating program ..." << std::endl;
  kill_program = true;
}

struct shared_memory {
  int buffer[10];
};

// Print system error and exit
void error(const std::string &msg) {
  perror(msg.c_str());
  exit(1);
}

int main() {
  struct shared_memory *shared_mem_ptr;
  sem_t *server_mutex_sem;
  sem_t *client_mutex_sem;
  int fd_shm;

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
  CPU_SET(2, &my_set); /* set the bit that represents core 1. */
  sched_setaffinity(0, sizeof(cpu_set_t), &my_set);

  // control c callback
  signal(SIGINT, SignalCallback);

  // mutual exclusion semaphore, mutex_sem with an initial value 0.
  if ((server_mutex_sem = sem_open(SEM_SERVER_MUTEX_NAME, O_CREAT, 0660, 0)) ==
      SEM_FAILED) {
    sem_unlink(SEM_SERVER_MUTEX_NAME);
    error("Unlinks server semaphore and exiting");
  } else {
    int a;
    if (sem_getvalue(server_mutex_sem, &a) == EINVAL) {
      error("Cannot get server semaphore value");
    } else {
      std::cout << "server sem value = " << a << std::endl;
    }
  }

  //  mutual exclusion semaphore, mutex_sem with an initial value 0.
  if ((client_mutex_sem = sem_open(SEM_CLIENT_MUTEX_NAME, O_CREAT, 0660, 0)) ==
      SEM_FAILED) {
    sem_unlink(SEM_CLIENT_MUTEX_NAME);
    error("Unlinks client semaphore and exiting");
  } else {
    int a;
    if (sem_getvalue(client_mutex_sem, &a) == EINVAL) {
      return -1;
    } else {
      std::cout << "client sem value = " << a << std::endl;
    }
  }

  // opens shared memory
  if ((fd_shm = shm_open(SHARED_MEM_NAME, O_RDWR | O_CREAT, 0660)) == -1) {
    error("shm_open");
  }

  // truncates shared memory
  if (ftruncate(fd_shm, sizeof(struct shared_memory)) == -1) {
    error("ftruncate");
  }

  // shared memory pointer
  if ((shared_mem_ptr = (struct shared_memory *)mmap(
           NULL, sizeof(struct shared_memory), PROT_READ | PROT_WRITE,
           MAP_SHARED, fd_shm, 0)) == MAP_FAILED) {
    error("mmap");
  }

  // loop
  int duration_last;
  auto tic = std::chrono::system_clock::now();
  while (1) {
    // waiting on server
    sem_wait(server_mutex_sem);
    auto toc = std::chrono::system_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(toc - tic)
            .count();
    loop_time[idx] = duration - duration_last;
    idx = (idx + 1) % loop_time.size();
    duration_last = duration;

    // if (idx == 0) {
    //   std::cout << "Mean loop time = "
    //             << std::accumulate(loop_time.begin(), loop_time.end(), 0) /
    //                    ((double)loop_time.size())
    //             << std::endl;
    // }

    // process data from server, run computations and update shared memory
    usleep(200);

    // post to server only the value is zero
    // int sem_val;
    // if (sem_getvalue(client_mutex_sem, &sem_val) == EINVAL) {
    //   return -1;
    // } else {
    //   if (sem_val == 0) {
    if (sem_post(client_mutex_sem) != 0) {
      error("sem_post");
    }
    //   }
    // }

    // kill program on control c
    if (kill_program) {
      break;
    }
  }

  return 0;
}