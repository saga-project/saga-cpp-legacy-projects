
#include "edge.hpp"
#include "edge_impl.hpp"

namespace diggedag
{
  edge::edge (void)
    : has_impl_ (false)
  {
  }

  edge::edge (const saga::url & src, 
              const saga::url & tgt) 
    : impl_     (new impl::edge (src, tgt)) 
    , has_impl_ (true)
  {
    impl_->set_edge (*this);
  }

  edge::edge (const edge & src) 
    : impl_     (src.impl_)
    , has_impl_ (true)
  {
    impl_->set_edge (*this);
  }

  edge::~edge (void)
  {
  }

  void edge::check_ (void) const
  {
    if ( ! has_impl_ )
    {
      throw "Cannot call methods on an empty edge";
    }
  }

  void edge::fire (void)
  {
    check_ ();
    impl_->fire ();
  }

  void edge::add_src_node (const node & src)
  {
    check_ ();
    impl_->add_src_node (src);
  }

  void edge::add_tgt_node (const node & tgt)
  {
    check_ ();
    impl_->add_tgt_node (tgt);
  }

  void edge::erase_src (void)
  {
    check_ ();
    impl_->erase_src ();
  }

  void edge::erase_tgt (void)
  {
    check_ ();
    impl_->erase_tgt ();
  }

  diggedag::state edge::get_state (void) const
  {
    check_ ();
    return impl_->get_state ();
  }

  saga::url edge::get_src (void) const
  {
    check_ ();
    return impl_->get_src ();
  }

  saga::url edge::get_tgt (void) const
  {
    check_ ();
    return impl_->get_tgt (); 
  }

  diggedag::node edge::get_src_node (void) const 
  {
    check_ ();
    return impl_->get_src_node ();
  }

  diggedag::node edge::get_tgt_node (void) const 
  { 
    check_ ();
    return impl_->get_tgt_node ();
  }

  void edge::set_dag (dag & d)
  {
    check_ ();
    impl_->set_dag (d);
  }

} // namespace diggedag

