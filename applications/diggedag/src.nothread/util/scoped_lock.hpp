#ifndef DIGGEDAG_UTIL_SCOPEDLOCK_HPP
#define DIGGEDAG_UTIL_SCOPEDLOCK_HPP

#include "util/mutex.hpp"


#ifdef USE_BOOST
# include <boost/thread.hpp>
# define my_scoped_lock boost::mutex::scoped_lock
#else

namespace diggedag 
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

        // sometimes, the user provides its own mutex for lokking
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

} // namespace diggedag

#endif // USE_BOOST

#endif // DIGGEDAG_UTIL_SCOPEDLOCK_HPP

