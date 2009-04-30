
#ifndef DIGGEDAG_NODE_HPP
#define DIGGEDAG_NODE_HPP

#include <boost/thread.hpp>


namespace diggedag
{
  class node 
  {
    private:
      boost::mutex   mtx_;
      boost::thread  thread_;

    public:
      node  (void) { }
      ~node (void)                   
      {
        thread_.join ();
      }

      void            fire            (void);
      void            thread_work     (void);
  };

} // namespace diggedag

#endif // DIGGEDAG_NODE_HPP

