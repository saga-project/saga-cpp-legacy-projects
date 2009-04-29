
#ifndef DIGGEDAG_UTIL_THREAD_HPP
#define DIGGEDAG_UTIL_THREAD_HPP

#include <iostream>

#ifdef USE_BOOST
# include "boost/thread.hpp"
# define PREFIX boost::mutex
#else
# include <pthread.h>
# include "util/scoped_lock.hpp"
# define PREFIX util
#endif



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
      void * thread_startup_ (void * arg);
    }

    // This class provides simple threading ability.  The consumer inherits
    // this class, and overloads the following methods
    //   void thread_main (void)  // the thread workload
    //   void thread_cb   (void)  // callback on thread termination
    //
    class thread
    {
      public:
        // thread states
        enum thread_state
        {
          ThreadNew       = 0,
          ThreadRunning   = 1,
          ThreadDone      = 2,
          ThreadFailed    = 3, 
          ThreadCancelled = 4
        };

      private:
        // state management
        thread::thread_state thread_state_;
#ifdef USE_BOOST
        boost::thread thread_;
        boost::mutex  mtx_;
#else
        pthread_t     thread_;
        util::mutex   mtx_;
#endif
        

      public:
        thread (void);
      //thread (thread & t);
        virtual ~thread (void);


      private:
        // the external "C" startup helper is a friend, so that it can
        // run thread_start_
        friend void * thread_startup_ (void * arg);

      protected:
        // thread startup method, which manages the workload
        void * thread_start  (void);

        // workload methods, to be overloaded by consumer
        virtual void thread_work (void) { };

        // notification callback - does not need to be defined by consumer
        virtual void thread_cb   (void) { };


      public:
        // public thread management calls
        void          thread_run         (void);
        void          thread_wait        (void);
        void          thread_lock        (void);
        void          thread_unlock      (void);
        PREFIX::scoped_lock thread_scoped_lock (void);
        thread_state  thread_state       (void) const
        {
          return thread_state_; 
        }
    };

  } // namespace util

} // namespace diggedag


#endif // DIGGEDAG_UTIL_THREAD_HPP

