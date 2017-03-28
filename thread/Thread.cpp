#include "Thread.h"

namespace IrvCS
{
  /**
   * Thread for performing DSA operations.
   **/
  Thread::Thread():interrupted_(false), tid_(0)
  {
    int status=pthread_attr_init(&attr_);
    if (0 != status)
    {
      syslog(LOG_ERR, "Error initializing thread attribute:  %s (%d)",
             strerror(status), status);
    }
  }

  /**
   * clean up the thread
   **/
  Thread::~Thread()
  {
    int status=pthread_attr_destroy(&attr_);
    if (0 != status)
    {
      syslog(LOG_ERR, "Error initializing thread attribute:  %s (%d)",
             strerror(status), status);
    }
  }

  /**
   * Start the thread
   * @return 0 for success
   * @return errno if failed
   **/
  int Thread::start()
  {
    return pthread_create(&tid_, &attr_, startThread, this);
  }

  /**
   * Kill the thread
   **/
  int Thread::interrupt()
  {
    syslog(LOG_NOTICE, "Interrupting Thread %u", tid_);
    interrupted_=true;
    return 0;
  }

  /**
   * Wait for the thread to exit
   **/
  int Thread::join(void **arg)
  {
    int status=pthread_join(tid_, arg);
    return status;
  }

  /**
   *
   */
  bool Thread::isInterrupted()
  {
    return interrupted_;
  }

  void *Thread::startThread(void *arg)
  {
    Thread *thread=static_cast<Thread *>(arg);
    syslog(LOG_DEBUG, "Starting Thread %u", thread->tid_);
    void *retVal=thread->run();
    syslog(LOG_DEBUG, "Ending Thread %u", thread->tid_);
    return retVal;
  }    
}
