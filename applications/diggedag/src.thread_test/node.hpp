
#ifndef DIGGEDAG_NODE_HPP
#define DIGGEDAG_NODE_HPP

#include <saga/saga.hpp>

#include <boost/thread.hpp>


namespace diggedag
{
  enum state
  {
    Incomplete = 0,
    Pending    = 1,
    Running    = 2, 
    Ready      = 3,
    Failed     = 4
  };


  class node 
  {
    private:
      saga::job::description         jd_;
      diggedag::state                state_;
      boost::mutex                   mtx_;
      boost::thread                  thread_;

    public:
      node  (saga::job::description & jd)
        : jd_    (jd),
          state_ (diggedag::Pending) 
      { 
      }

      ~node (void)                   
      {
        thread_.join ();
      }

      void            fire            (void);
      void            thread_work     (void);
      diggedag::state get_state       (void);
  };

} // namespace diggedag

#endif // DIGGEDAG_NODE_HPP

