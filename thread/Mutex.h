#ifndef __MUTEX_HH__
#define __MUTEX_HH__

#include <pthread.h>
#include <syslog.h>
#include <string.h>

namespace IrvCS
{
  /**
   * Mutex protects a segment of code from multiple threads accessing it 
   * concurrently.
   * @see MutexLock
   **/
  class Mutex
  {
  public:
    /**
     * Default Mutex initialization.  If we need to specify any special
     * attributes, subclass
     **/
    Mutex()
    {
      int err=pthread_mutex_init(&mutex_, NULL);
      if (0 != err)
      {
        syslog(LOG_ERR, "%s:  Error initializing:  %s (%d)",
               __FILENAME__, strerror(err), err);
      }
    }

    virtual ~Mutex()
    {
      pthread_mutex_destroy(&mutex_) ;
    }

    /**
     * Lock the mutex
     * @return 0 if success
     * @return errno if not successful
     **/
    virtual int lock()
    {
      int status=pthread_mutex_lock(&mutex_);
      if (status != 0)
      {
        syslog(LOG_ERR, "Error locking mutex:  %s (%d)",
               strerror(status), status);
      }
      return status;
    }

    /**
     * Unlock the mutex
     * @return 0 if success
     * @return errno if not successful
     **/
    virtual int unlock()
    {
      int status=pthread_mutex_unlock(&mutex_);
      if (status != 0)
      {
        syslog(LOG_ERR, "Error locking mutex:  %s (%d)",
               strerror(status), status);
      }

      return status;
    }

    /**
     * Check the mutex to see if it's locked
     * @return 0 if success
     * @return errno if not successful
     **/
    virtual int trylock()
    {
      return pthread_mutex_trylock(&mutex_);
    }
   
  private:
    pthread_mutex_t mutex_;

  };
    
}
#endif
