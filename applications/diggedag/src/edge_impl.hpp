
#ifndef DIGGEDAG_EDGE_IMPL_HPP
#define DIGGEDAG_EDGE_IMPL_HPP

#include <vector>

#include <saga/saga.hpp>

#include "enum.hpp"
#include "util/uid.hpp"
#include "util/thread.hpp"

namespace diggedag
{
  class node;

  namespace impl
  {
    // FIXME: think about async
    // FIXME: think about data renaming
    class edge : public diggedag::util::uid, 
                 public diggedag::util::thread
    {
      private:
        saga::url       src_;   // src location of data
        saga::url       tgt_;   // tgt location of data
        diggedag::state state_; // state of instance

        std::vector <diggedag::node> nodes_;


      public:
        edge  (const saga::url & src, 
               const saga::url & tgt);
        ~edge (void);

        void            fire        (void);
        void            thread_work (void);
        void            erase_src   (void);
        void            erase_tgt   (void);
        void            add_node    (const diggedag::node & n);
        diggedag::state get_state   (void) const;

        saga::url       get_src     (void) const { return src_; }
        saga::url       get_tgt     (void) const { return tgt_; }
    };

  } // namespace impl

} // namespace diggedag

#endif // DIGGEDAG_EDGE_IMPL_HPP

