
#ifndef DIGGEDAG_EDGE_HPP
#define DIGGEDAG_EDGE_HPP

#include "edge_impl.hpp"

#include "util/shared_ptr.hpp"

namespace diggedag
{
  class edge
  {
    private:
      util::shared_ptr <impl::edge> impl_;
      util::shared_ptr <impl::edge> get_impl (void) const { return impl_; } 

    public:
      edge (const saga::url & src, 
            const saga::url & tgt) 
        : impl_ (new impl::edge (src, tgt)) 
      {
      }

      edge (const edge & src) 
        : impl_ (src.get_impl ())
      {
      }

      ~edge (void)
      {
      }

      void fire (void)
      {
        impl_->fire ();
      }

      void add_src_node (const node & src)
      {
        impl_->add_src_node (src);
      }

      void add_tgt_node (const node & tgt)
      {
        impl_->add_tgt_node (tgt);
      }

      void erase_src (void)
      {
        impl_->erase_src ();
      }

      void erase_tgt (void)
      {
        impl_->erase_tgt ();
      }

      diggedag::state get_state (void) const
      {
        return impl_->get_state ();
      }

      saga::url get_src (void) const
      {
        return impl_->get_src ();
      }

      saga::url get_tgt (void) const
      {
        return impl_->get_tgt (); 
      }

      diggedag::node get_src_node (void) const 
      {
        return impl_->get_src_node ();
      }
      
      diggedag::node get_tgt_node (void) const 
      { 
        return impl_->get_tgt_node ();
      }
  };

} // namespace diggedag

#endif // DIGGEDAG_EDGE_HPP

