
#include "util/thread.hpp"


namespace diggedag 
{
  namespace util
  {
    extern "C"
    {
      // the thread main routine, as argument to pthread_create, needs to be
      // a C function, not a C++ member method.  Thus, we define an "C" external
      // function here, which takes a pointer to an instance of our thread class,
      // and calls its thread_start method.
      void * thread_startup_ (void * arg)
      {
        diggedag::util::thread * t = (diggedag::util::thread *) arg;
        return t->thread_start ();
      }
    }


    // the c'tor does nothing than setting the state to 'New'
    thread::thread (void)
      : thread_state_ (ThreadNew)
    {
    }

    // Note that the d'tor does not wait for the thread to finish.  Thus, we
    // expect the worker thread to continue as long as the application is alive
    // FIXME: a clean thread_cancel should be used.  pthread_kill has trouble if
    // the thread is owning locks.  boost.thread.kill only works for often
    // irrelevant cancelation points.
    thread::~thread () 
    {
#ifdef DO_THREADS
      pthread_join (thread_, NULL);
#endif
    }


    // called by the inheriting classes, or by other consumers, to get the
    // thread going.  If the state is not 'New', this method does nothing.
    // Otherwise, the external "C" thread_startup_ routine is called, and state
    // is changed to Running.
    void thread::thread_run (void)
    {
      util::scoped_lock l (mtx_);

      if ( thread_state_ != ThreadNew )
        return;

      thread_state_ = ThreadRunning;

#ifdef DO_THREADS
      if ( 0 != pthread_create (&thread_, NULL, diggedag::util::thread_startup_, this) )
      {
        thread_state_ = ThreadFailed;
      }
#else
      thread_start ();
#endif
    }


    // thread_start is called by the external "C" function 'thread_startup_'
    // defined above.  It represents the running thread, and calls thread_work,
    // which holds the threads workload.
    void * thread::thread_start (void)
    {
      // First, try get the mutex.  run() is locking the mutex to set the state
      // to Running - we wait 'til this is done, to ensure correct state.
      {
        util::scoped_lock l (mtx_);
      }

      // now startup is completed - call the (custom) workload
      this->thread_work ();

      // the thread workload is done - update state
      {
        util::scoped_lock l (mtx_);
        
        thread_state_ = ThreadDone;
      }

      // nothing more to do: close thread
#ifdef DO_THREADS
      pthread_exit (NULL);
#endif

      return NULL;
    }


    // thread_join allows to simply wait 'til the thread finishes on its own.
    // All state setting etc is done by the thread.  
    void thread::thread_wait (void)
    {
#ifdef DO_THREADS
      pthread_join (thread_, NULL);
#endif
    }

    // allow the consumer to wait for thread completion
    void thread::thread_join (void)
    {
#ifdef DO_THREADS
      pthread_join (thread_, NULL);
#endif
    }

  } // namespace util

} // namespace diggedag

