#ifndef DIGEDAG_UTIL_SCOPEDLOCK_HPP
#define DIGEDAG_UTIL_SCOPEDLOCK_HPP

#include "util/mutex.hpp"


namespace digedag 
{
  namespace util
  {
    // This class provides a simple scoped lock, based on the
    // util::mutex class.
    class scoped_lock 
    {
      private:
        util::mutex mtx_;

      public:
        scoped_lock (void)
        {
          mtx_.lock ();
        }

        // sometimes, the user provides its own mutex for locking
        scoped_lock (util::mutex & mtx)
        {
          mtx_ = mtx;
          mtx_.lock ();
        }

        ~scoped_lock () 
        {
          mtx_.unlock ();
        }
    };

  } // namespace util

} // namespace digedag


#endif // DIGEDAG_UTIL_SCOPEDLOCK_HPP

