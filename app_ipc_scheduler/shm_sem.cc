#include "shm_sem.h"

#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>

const std::string ShmSemaphore::sLockSemaphoreName = "/semaphoreInit";

ShmSemaphore::ShmSemaphore(const std::string& sName)
    : name_(sName), ptr_(nullptr), shm_id_(-1), sem_id_(nullptr), size_(0) {
  /**
   * Semaphore open
   */
  sem_id_ = sem_open(sLockSemaphoreName.c_str(), O_CREAT, S_IRUSR | S_IWUSR, 1);
}

bool ShmSemaphore::Create(size_t nSize, int mode /*= READ_WRITE*/) {
  size_ = nSize;
  shm_id_ = shm_open(name_.c_str(), O_CREAT | mode, 0666);
  if (shm_id_ < 0) {
    switch (errno) {
      case EACCES:
        throw ShmException("Permission Exception ");
        break;
      case EEXIST:
        throw ShmException(
            "Shared memory object specified by name already exists.");
        break;
      case EINVAL:
        throw ShmException("Invalid shared memory name passed.");
        break;
      case EMFILE:
        throw ShmException(
            "The process already has the maximum number of files open.");
        break;
      case ENAMETOOLONG:
        throw ShmException("The length of name exceeds PATH_MAX.");
        break;
      case ENFILE:
        throw ShmException(
            "The limit on the total number of files open on the system has "
            "been reached");
        break;
      default:
        throw ShmException(
            "Invalid exception occurred in shared memory creation");
        break;
    }
  }
  /* adjusting mapped file size (make room for the whole segment to map)      --
   * ftruncate() */
  ftruncate(shm_id_, size_);

  return true;
}

bool ShmSemaphore::Attach(int mode /*= A_READ | A_WRITE*/) {
  /* requesting the shared segment    --  mmap() */
  ptr_ = mmap(NULL, size_, mode, MAP_SHARED, shm_id_, 0);
  if (ptr_ == nullptr) {
    throw ShmException("Exception in attaching the shared memory region");
  }
  return true;
}

bool ShmSemaphore::Detach() { munmap(ptr_, size_); }

bool ShmSemaphore::Lock() { sem_wait(sem_id_); }

bool ShmSemaphore::Trylock() { return (sem_trywait(sem_id_) == 0); }

bool ShmSemaphore::Unlock() { return (sem_post(sem_id_) == 0); }

bool ShmSemaphore::UnlockSingle() {
  int sem_val;
  if (sem_getvalue(sem_id_, &sem_val) == 0) {
    if (sem_val == 0) {
      return (sem_post(sem_id_) == 0);
    } else {
      return false;
    }
  }
  return false;
}

ShmSemaphore::~ShmSemaphore() { Clear(); }

void ShmSemaphore::Clear() {
  if (shm_id_ != -1) {
    if (shm_unlink(name_.c_str()) < 0) {
      perror("shm_unlink");
    }
  }
  /**
   * Semaphore unlink: Remove a named semaphore  from the system.
   */
  if (sem_id_ != NULL) {
    /**
     * Semaphore Close: Close a named semaphore
     */
    if (sem_close(sem_id_) < 0) {
      perror("sem_close");
    }
    /**
     * Semaphore unlink: Remove a named semaphore  from the system.
     */
    if (sem_unlink(sLockSemaphoreName.c_str()) < 0) {
      perror("sem_unlink");
    }
  }
}

ShmException::ShmException(const std::string& message,
                           bool bSysMsg /*= false*/) throw() {}

ShmException::~ShmException() throw() {}