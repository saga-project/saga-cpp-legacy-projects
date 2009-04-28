
#ifndef DIGGEDAG_EDGE_HPP
#define DIGGEDAG_EDGE_HPP

#include <saga/saga.hpp>

#include "util/shared_ptr.hpp"

#include "enum.hpp"


namespace diggedag
{
  namespace impl
  {
    class dag;
    class edge;
  }

  class dag;
  class node;
  class scheduler;
  class edge
  {
    protected:
      my_shared_ptr <impl::edge> impl_;
   // my_shared_ptr <impl::edge> get_impl (void) const { return impl_; } 

    private:
      bool  has_impl_;
      void  check_ (void) const;

    protected:
      void set_dag (dag & d);
      friend class diggedag::impl::dag;


    public:
      edge (void);
      edge (const saga::url & src, 
            const saga::url & tgt = "");
      edge (const edge      & src);
      ~edge (void);

      void            fire         (void);
      void            add_src_node (const node & src);
      void            add_tgt_node (const node & tgt);
      void            erase_src    (void);
      void            erase_tgt    (void);
      diggedag::state get_state    (void) const;
      saga::url       get_src      (void) const;
      saga::url       get_tgt      (void) const;
      diggedag::node  get_src_node (void) const;
      diggedag::node  get_tgt_node (void) const;
  };

} // namespace diggedag

#endif // DIGGEDAG_EDGE_HPP

