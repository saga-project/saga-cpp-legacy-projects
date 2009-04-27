
#include "edge.hpp"
#include "edge_impl.hpp"

namespace diggedag
{
  edge::edge (const saga::url & src, 
              const saga::url & tgt) 
    : impl_ (new impl::edge (src, tgt)) 
  {
  }

  edge::edge (const edge & src) 
    : impl_ (src.get_impl ())
  {
  }

  edge::~edge (void)
  {
  }

  void edge::fire (void)
  {
    impl_->fire ();
  }

  void edge::add_src_node (const node & src)
  {
    impl_->add_src_node (src);
  }

  void edge::add_tgt_node (const node & tgt)
  {
    impl_->add_tgt_node (tgt);
  }

  void edge::erase_src (void)
  {
    impl_->erase_src ();
  }

  void edge::erase_tgt (void)
  {
    impl_->erase_tgt ();
  }

  diggedag::state edge::get_state (void) const
  {
    return impl_->get_state ();
  }

  saga::url edge::get_src (void) const
  {
    return impl_->get_src ();
  }

  saga::url edge::get_tgt (void) const
  {
    return impl_->get_tgt (); 
  }

  diggedag::node edge::get_src_node (void) const 
  {
    return impl_->get_src_node ();
  }

  diggedag::node edge::get_tgt_node (void) const 
  { 
    return impl_->get_tgt_node ();
  }

  void edge::set_dag (dag & d)
  {
    impl_->set_dag (d);
  }

} // namespace diggedag

