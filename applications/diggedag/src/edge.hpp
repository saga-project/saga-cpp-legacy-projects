
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

      void add_node (const node & n)
      {
        impl_->add_node (n);
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
  };

} // namespace diggedag

#endif // DIGGEDAG_EDGE_HPP

