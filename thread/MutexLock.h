#ifndef __MUTEXLOCK__HH
#define __MUTEXLOCK__HH
#include "Mutex.h"

namespace IrvCS
{
  /**
   * Utility class to lock a mutex and automaticlly unlock it when the 
   * class is destructed.  This makes sure we don't accidentally forget to 
   * unlock the mutex.
   * @see Mutex
   **/
  class MutexLock
  {
  public:
    /**
     * Constructor with Mutex to lock
     * @param mutex the mutex to lock
     **/
    MutexLock(Mutex &mutex):mutex_(mutex)
      {
        mutex.lock();
      }

    /**
     * Unlock the mutex when destroying
     **/
    virtual ~MutexLock()
      {
        mutex_.unlock();
      }
  private:

    /**
     * The mutex
     **/
    Mutex& mutex_;
  };
  
}

#endif
