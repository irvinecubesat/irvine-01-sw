#include <gtest/gtest.h>
#include <Thread.h>
#include <Mutex.h>
#include <MutexLock.h>
#include <list>

using namespace IrvCS;

class MyThread:public Thread
{
public:
  MyThread(Mutex &mutex):Thread(),mutex_(mutex), blocked_(false)
  {
  }
  ~MyThread(){}

  bool isBlocked()
  {
    return blocked_;
  }

protected:
  
  void *run()
  {
    syslog(LOG_INFO, "%d Blocking on mutex", pthread_self());
    blocked_=true;
    MutexLock lock(mutex_);
    syslog(LOG_INFO, "%d Got the mutex", pthread_self());
    blocked_=false;
    return NULL;
  }
private:
  bool blocked_;
  Mutex &mutex_;
};

class MutexLockFixture: public::testing::Test
{
public:
  MutexLockFixture()
    {
      openlog("MutexLock", LOG_PERROR, LOG_USER);
    }

  virtual void SetUp()
    {
    }

  virtual void TearDown()
    {
    }
  
  ~MutexLockFixture()
    {
      // kill and stop the threads when done
      for (auto ci=threadList_.begin(); ci != threadList_.end(); ci++)
      {
        (*ci)->interrupt();
        void *arg;
        (*ci)->join(&arg);  // wait for thread to complete before destroying
        delete *ci;
      }
    }

  MyThread *createAndStartThread(Mutex &mutex)
    {
      MyThread *thread = new MyThread(mutex);
      thread->start();
      threadList_.push_back(thread);
      return thread;
    }
private:
  std::list<Thread *> threadList_;

};
/**
 * Test Mutex locking and unlocking
 **/
TEST_F(MutexLockFixture, LockTest)
{
  MyThread *thread[3]={0};
  Mutex mutex;
  {
    MutexLock lock(mutex);  // all threads will block on this mutex.

    thread[0]=createAndStartThread(mutex);
    thread[1]=createAndStartThread(mutex);
    thread[2]=createAndStartThread(mutex);
    
    sleep(1); // make sure threads are up and running
    
    ASSERT_EQ(true, thread[0]->isBlocked());
    ASSERT_EQ(true, thread[1]->isBlocked());
    ASSERT_EQ(true, thread[2]->isBlocked());
  }
  // when MutexLock goes out of scope, mutex is unlocked.

  for (int i = 0; i < 3; i++)
  {
    void *arg=NULL;
    thread[i]->join(&arg);
  }
  ASSERT_EQ(false, thread[0]->isBlocked());
  ASSERT_EQ(false, thread[1]->isBlocked());
  ASSERT_EQ(false, thread[2]->isBlocked());
  
}
