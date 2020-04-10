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

  /**
   * @brief Creates a shared memory segment with the size given
   * @param[in] nSize Size of memory in bytes
   * @param[in] mode Access mode of the shared memory, defaulted as read/write
   * mode
   */
  bool Create(size_t nSize, int mode = C_READ_WRITE);

  /**
   * @brief Attaches the shared memory segment to memory
   * @param[in] mode, defaulted as (A_READ | A_WRITE)
   */
  bool Attach(int mode = A_READ | A_WRITE);

  /**
   * @brief Detaches the shared memory segment from the memory
   */
  bool Detach();

  /**
   * @brief Locks the semaphore using sem_wait
   */
  bool Lock();

  /**
   * @brief Try to lock the semaphore using sem_trywait
   */
  bool Trylock();

  /**
   * @brief Unlocks the semaphore using sem_post
   */
  bool Unlock();

  /**
   * @brief Unlocks the semaphore using sem_post only if the current sem value
   * is 0, indicating that it was locked by a user process
   */
  bool UnlockSingle();

  /**
   * @brief Returns the id of the shared memory
   */
  int ID() const { return shm_id_; }

  /**
   * @brief Returns the data of the shared memory
   */
  void* Data() { return ptr_; };

  /**
   * @brief Returns the immutable data of the shared memory
   */
  const void* Data() const { return ptr_; }

 private:
  /**
   * @brief Clears and unlinks the shared memory
   */
  void Clear();

 private:
  /** name of the shared memory */
  std::string name_;

  /** id of the shared memory */
  int shm_id_;

  /** id of the semaphore */
  sem_t* sem_id_;

  /** size of the shared memory */
  size_t size_;

  /** pointer of the shared memory */
  void* ptr_;
};