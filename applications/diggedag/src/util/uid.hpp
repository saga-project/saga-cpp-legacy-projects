
#ifndef DIGGEDAG_UTIL_UID_HPP
#define DIGGEDAG_UTIL_UID_HPP

namespace diggedag 
{
  namespace util
  {
    // This class provides a trivial implementation for a unique ID.  
    // That ID is not globally unique (!uuid), but simply a INT which is
    // increased after every query.
    //
    // NOTE that inheriting classes need to explicitely invoke the copy c'tor of
    // the uid base class, like this
    //
    // class child : public uid
    // {
    //   ...
    //   child (const child & src)
    //     : uid (src)
    //   {
    //     this->state_ = src.state_;
    //     ...
    //   }
    // };
    //
    // Well, this needs to be done if copies point to the same underlying
    // resource, as in the diggedag use case...
    class uid
    {
      private:
        unsigned int id_;


      public:
        uid (void);
        uid (const uid & src);
       ~uid (void) { };

        unsigned int uid_get (void) const { return id_; }
    };

  } // namespace util

} // namespace diggedag

#endif // DIGGEDAG_UTIL_UID_HPP

