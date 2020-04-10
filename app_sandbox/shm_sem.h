#pragma once

#include <fcntl.h> /* For O_* constants */
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */

#include <string>

/**
 *   Exception class for shared memory
 */
class ShmException : public std::exception {
 public:
  /**
   *   Construct a SharedMemoryException with a explanatory message.
   *   @param message explanatory message
   *   @param bSysMsg true if system message (from strerror(errno))
   *   should be postfixed to the user provided message
   */
  ShmException(const std::string& message, bool bSysMsg = false) throw();

  /**
   * Destructor.
   * Virtual to allow for subclassing.
   */
  virtual ~ShmException() throw();

  /**
   * Returns a pointer to the (constant) error description.
   *  @return A pointer to a \c const \c char*. The underlying memory
   *          is in posession of the \c Exception object. Callers \a must
   *          not attempt to free the memory.
   */
  virtual const char* what() const throw() { return msg_.c_str(); }

 protected:
  /** Error message.*/
  std::string msg_;
};

class ShmSemaphore {
 public:
  enum {
    C_READ_ONLY = O_RDONLY,
    C_READ_WRITE = O_RDWR,
  } CREATE_MODE;

  enum {
    A_READ = PROT_READ,
    A_WRITE = PROT_WRITE,
  } ATTACH_MODE;

  static const std::string sLockSemaphoreName;

 public:
  ShmSemaphore(const std::string& sName);
  ~ShmSemaphore();

  bool Create(size_t nSize, int mode = C_READ_WRITE);
  bool Attach(int mode = A_READ | A_WRITE);
  bool Detach();
  bool Lock();
  bool TryLock();
  bool UnLock();
  int GetID() { return shm_id_; }
  void* GetData() { return ptr_; };
  const void* GetData() const { return ptr_; }

 private:
  void Clear();

 private:
  std::string name_;
  int shm_id_;
  sem_t* sem_id_;
  size_t m_nSize;
  void* ptr_;
};