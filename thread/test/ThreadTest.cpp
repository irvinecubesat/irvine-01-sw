#include <gtest/gtest.h>
#include <Thread.h>
#include <unistd.h>
using namespace IrvCS;

/**
 * Test starting and interrupt a thread
 **/
TEST(Threads, StartInterruptJoinThread)
{
  //openlog("Thread", LOG_PERROR, LOG_USER);
  class MyThread:public Thread
  {
  public:
    MyThread():Thread(),counter_(0)
      {
      }
    ~MyThread()
      {
      }
    
    int getCounter()
      {
        return counter_;
      }
    
  protected:
    void *run()
      {
        while (!isInterrupted())
        {
          counter_++;
          usleep(1000);
        }
        return NULL;
      }
  private:
    int counter_;
  };

  MyThread* thread = new MyThread();

  int status=thread->start();
  ASSERT_EQ(0, status);
  sleep(1);

  status=thread->interrupt();
  ASSERT_EQ(0, status);

  void *arg;
  status=thread->join(&arg);

  ASSERT_EQ(0, status);

  ASSERT_GT(thread->getCounter(),0);
}

/**
 * Test starting then joining a thread and waiting for it to exit at 
 * counter==10000
 **/
TEST(Threads, StartThenJoinThread)
{
  //openlog("Thread", LOG_PERROR, LOG_USER);
  class MyThread:public Thread
  {
  public:
    MyThread():Thread(),counter_(0)
      {
      }
    ~MyThread()
      {
      }
    
    int getCounter()
      {
        return counter_;
      }
    
  protected:
    void *run()
      {
        while (!isInterrupted() && counter_<1000)
        {
          counter_++;
          usleep(100);
        }
      }
  private:
    int counter_;
  };

  MyThread* thread = new MyThread();

  int status=thread->start();
  ASSERT_EQ(0, status);
  sleep(1);

  void *arg;
  status=thread->join(&arg);

  ASSERT_EQ(0, status);

  ASSERT_EQ(thread->getCounter(),1000);
}

/**
 * Test starting multiple threads then joining each thread and waiting for it to exit at different counter intervals
 **/
TEST(Threads, StartMultiThenJoinThread)
{
  openlog("Thread", LOG_PERROR, LOG_USER);
  class MyThread:public Thread
  {
  public:
    MyThread(int limit):Thread(),counter_(0), limit_(limit)
      {
      }
    ~MyThread()
      {
      }
    
    int getCounter()
      {
        return counter_;
      }
    
  protected:
    void *run()
      {
        while (!isInterrupted() && counter_<limit_)
        {
          counter_++;
          usleep(100);
        }
      }
  private:
    int counter_;
    int limit_;
  };

  MyThread* thread1 = new MyThread(3000);
  MyThread* thread2 = new MyThread(2000);
  MyThread* thread3=  new MyThread(1000);

  int status=thread1->start();
  ASSERT_EQ(0, status);
  status=thread2->start();
  ASSERT_EQ(0, status);
  status=thread3->start();
  
  ASSERT_EQ(0, status);
  sleep(1);

  void *arg;
  status=thread1->join(&arg);
  ASSERT_EQ(0, status);
  status=thread2->join(&arg);
  ASSERT_EQ(0, status);
  status=thread3->join(&arg);
  ASSERT_EQ(0, status);

  ASSERT_EQ(thread1->getCounter(),3000);
  ASSERT_EQ(thread2->getCounter(),2000);
  ASSERT_EQ(thread3->getCounter(),1000);
}
