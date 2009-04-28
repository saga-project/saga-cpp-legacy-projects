
#ifndef DIGGEDAG_UTIL_SHAREDPTR_HPP
#define DIGGEDAG_UTIL_SHAREDPTR_HPP

#ifdef USE_BOOST
# define  my_shared_ptr boost::shared_ptr
#else
# define  my_shared_ptr util::shared_ptr
#endif

#ifdef USE_BOOST

#include <boost/shared_ptr.hpp>

#else

#include "util/scoped_lock.hpp"

namespace diggedag
{
  namespace util
  {

    // the shared_ptr class is a pointer container which
    // performes reference counting for the pointer it holds.
    // The pointer is deleted whn the reference count reaches
    // zero, i.e. when the last copy of the shared pointer is
    // deleted.
    //
    // NOTES:
    //   - the  application must ensure that the pointer is 
    //     not in use outside that reference counting scheme, 
    //     at time of deletion.
    //
    // LIMITATIONS:
    //   - refcounter is of long type, that obviously limits 
    //     the number of allowed copies
    //
    //   - to obtain correct deletion semantics on casting of
    //     shared pointers, the classes involved SHOULD have
    //     virtual destructors.
    //
    template <class T> class shared_ptr 
    {
      // allow for dynamic casts for other shared ptr types
      template <class U> friend class shared_ptr;


      private:
        T     * ptr_;  // holds the pointer
        long  * cnt_;  // reference count


        // Increment the reference count.  
        // The counter is locked with a scoped lock.
        void inc_ (void)
        {
          util::scoped_lock lck;
          (*cnt_) += 1;
        }


        // Decrement the reference count.   If ref count reaches zero, delete
        // the pointer.
        // The counter is locked with a scoped lock.
        void dec_ (void)
        {
          util::scoped_lock lck;

          (*cnt_) -= 1;

          if ( 0 == *cnt_ )
          {
            delete (cnt_);
            delete (ptr_);
          }
        }


      public:
        // default ctor
        // marked explicit, so we never automatically convert to a shared pointer
        explicit shared_ptr ( T * p = 0 )
          : ptr_ (p)
        {
          try 
          {
            // get new counter, and increment
            cnt_ = new long;
            inc_ ();
          }
          catch ( ... )
          {
            // can't do much if we don't even get a counter allocated
            delete (p);
            throw;
          }
        }

        // copy ctor, increments ref count
        shared_ptr (const shared_ptr <T> & p)
          : ptr_ (p.ptr_), 
            cnt_ (p.cnt_)
        {
          inc_ ();
        }

        // casting copy ctor, increments ref count
        template <class U> 
        shared_ptr (const shared_ptr <U> & p)
          : ptr_ (dynamic_cast <T*> (p.ptr_)), 
            cnt_ (p.cnt_)
        {
          inc_ ();
        }


        // dtor, decrements ref count
        ~shared_ptr () 
        {
          dec_ ();
        }


        // assignment
        shared_ptr <T> & operator= (const shared_ptr <T> & p) 
        {
          // got really new pointer
          if ( this != & p ) 
          {
            // decrease refcount for the old ptr, and delete if needed
            dec_ ();
          }

          // init 'this' just as in the copy c'tor
          ptr_ = p.ptr_;
          cnt_ = p.cnt_;

          // increment ref count for new pointer
          inc_ ();

          return *this;
        }


        // accessors
        T & operator * ()  const 
        {
          return *ptr_;
        }
        
        T * operator->() const
        {
          return ptr_;
        }

        T * get ()  const 
        {
          return ptr_;
        }

        // allow to compare shared pointers, by comparing the contained pointer
        bool compare (const shared_ptr <T> & p) const
        {
          return (ptr_ == p.ptr_);
        }

        template <class U> U * dynamic_ptr_cast (void)
        {
          return dynamic_cast <U *> (ptr_);
        }

        template <class U> shared_ptr <U> dynamic_shared_ptr_cast (void)
        {
          U * tmp = dynamic_cast <U *> (ptr_);
          return shared_ptr <U> (tmp);
        }
    };


    // == operator as shortcut for a.compare (b)
    template <class T, class U> inline bool operator== (shared_ptr <T> const & a, 
                                                        shared_ptr <U> const & b)
    {
      return a.compare (b);
    }
    

  } // namespace util

} // namespace diggedag

#endif // USE_BOOST

#endif // DIGGEDAG_UTIL_SHAREDPTR_HPP

