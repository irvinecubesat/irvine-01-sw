#ifndef __THREAD_HH__
#define __THREAD_HH__

#include <pthread.h>
#include <syslog.h>
#include <string.h>

namespace IrvCS
{
  /**
   * Run a method in a thread.  Subclass this class and override the run() method.
   **/
  class Thread
  {
  public:
    /**
     * Default constructor for a thread.
     **/
    Thread();

    /**
     * clean up the thread
     **/
    virtual ~Thread();

    /**
     * Start the thread
     * @return 0 for success
     * @return errno if failed
     **/
    int start();

    /**
     * Interrupt the thread
     **/
    int interrupt();

    /**
     * Wait for the thread to exit
     * @param arg the pointer returned by the run function.
     **/
    int join(void **arg);

    /**
     * Is this thread interrupted?  Subclasses should check this
     * at convenient intervals to properly terminate the run function.
     */
    bool isInterrupted();
    
  protected:
    /**
     * Abstract method that subclasses must implement
     * @return pointer to custom data
     **/
    virtual void *run() = 0;
    
  private:

    /**
     * This is the pthread function that gets run when
     * the thread is started
     * @param arg is the data passed to the thread, which is the
     *        class object in this case.
     **/
    static void *startThread(void *arg);

    bool interrupted_;

    pthread_t tid_;
    pthread_attr_t attr_;

  };
}
#endif
