
#ifndef DIGGEDAG_UTIL_MUTEX_HPP
#define DIGGEDAG_UTIL_MUTEX_HPP

#include <pthread.h>

// FIXME: check mutex call return values

namespace diggedag 
{
  namespace util
  {
    // This class provides a wrapper around a pthread mutex's, and allows to
    // lock/unlock them.  The class should be exception save, i.e. the mutex
    // gets destroyed if the classes destructor is called.
    class mutex 
    {
      private:
        pthread_mutex_t * mtx_;
         

      public:
        mutex (void)
        {
          mtx_    = new pthread_mutex_t;
          pthread_mutex_init (mtx_, NULL);
        }

        mutex (const mutex & src)
        {
          this->mtx_ = src.mtx_;
        }

        // dtor
        ~mutex (void)
        {
          pthread_mutex_destroy (mtx_);
        }

        void lock (void) 
        {
          pthread_mutex_lock (mtx_);
        }

        void unlock (void) 
        {
          pthread_mutex_unlock (mtx_);
        } 
    };

  } // namespace util

} // namespace diggedag

#endif // DIGGEDAG_UTIL_MUTEX_HPP

