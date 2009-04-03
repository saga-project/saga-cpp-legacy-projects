
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

    // copy c'tor
    //
    // NOTE that this is a shallow copy: all actions on the new instance work on
    // the same thread from the source instance.  However, this implementation
    // does NOT ensure that the state of the two instances does not diverge
    // after copy.  In particular, only one instance will ever be able to call
    // thread_wait() successfully.  The user is responsible to ensure that all
    // state changing actions occure on one copy of this class instance.
    //
    // We provide this copy c'tor to allow this class to be used in standard
    // containers.
    thread::thread (const thread & t)
    {
      this->thread_state_  = t.thread_state_;
      this->thread_        = t.thread_;
      this->mtx_           = t.mtx_;
    }

    // the d'tor cancels the thread forcefully, to avoid dangling threads which
    // bever finish.  Thus, if one wants to have long running threads, then one
    // needs to keep this instance alive for that amount of time.
    thread::~thread () 
    {
      thread_cancel ();
    }


    // called by the inheriting classes, or by other consumers, to get the
    // thread going.  If the state is not 'New', this method does nothing.
    // Otherwise, the external "C" thread_startup_ routine is called, and state
    // is changed to Running.
    void thread::thread_run (void)
    {
      scoped_lock l (mtx_);

      if ( thread_state_ != ThreadNew )
        return;

      thread_state_ = ThreadRunning;

      if ( 0 != pthread_create (&thread_, NULL, diggedag::util::thread_startup_, this) )
      {
        thread_state_ = ThreadFailed;
      }
    }


    // thread_start is called by the external "C" function 'thread_startup_'
    // defined above.  It represents the running thread, and calls thread_work,
    // which holds the threads workload.
    void * thread::thread_start (void)
    {
      // First, try get the mutex.  run() is locking the mutex to set the state
      // to Running - we wait 'til this is done, to ensure correct state.
      {
        scoped_lock l (mtx_);
      }

      // now startup is completed - call the (custom) workload
      this->thread_work ();

      // the thread workload is done - update state
      {
        scoped_lock l (mtx_);
        
        thread_state_ = ThreadDone;
      }

      // only if the thread finishes on its own, i.e. if it enters the Done
      // state, the callback is called to signal thus to the consumer.  All
      // other state transitions are triggered explicitely by the consumer,
      // and thus do not require notification.
      this->thread_cb ();

      // nothing more to do: close thread
      pthread_exit (NULL);
    }


    // thread_wait allows to simply wait 'til the thread finishes on its own.
    // All state setting etc is done by the thread.  Note that the
    // notification callback is invoked when the thread finishes, before
    // wait() returns
    void thread::thread_wait (void)
    {
      pthread_join (thread_, NULL);
    }


    // thread_cancel() cn be called by the inheriting classes, or by other 
    // consumers, to cancel the (running) thread.  If thread is not in Running 
    // state, this call does nothing.
    void thread::thread_cancel (void)
    {
      scoped_lock l (mtx_);

      if ( thread_state_ != ThreadRunning )
        return;

      pthread_cancel (thread_);

      thread_state_ = ThreadCancelled;
    }

    // allow the consumer to do some explicit locking
    void thread::thread_lock (void)
    {
      mtx_.lock ();
    }

    void thread::thread_unlock (void)
    {
      mtx_.unlock ();
    }

    scoped_lock thread::thread_scoped_lock (void)
    {
      // FIXME!!!!
      return scoped_lock (mtx_);
    }

  } // namespace util

} // namespace diggedag

